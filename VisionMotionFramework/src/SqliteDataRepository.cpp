// SqliteDataRepository.cpp
// C++14 적용 및 Boost (LockGuardType 등) 의존성 제거

#include "StdAfx.h"
#include "SqliteDataRepository.h"
#include "FileUtils.h"
#include "IDataRepository.h" // StorageError(WriteError 등) 정의 보장
#include <sqlite3.h>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <cstdio> // remove, rename
#include <cstdint>
#include <mutex>  // std::lock_guard

static uint64_t g_imageCounter = 0;

namespace VMF {

    // 파일을 문자열로 읽는 헬퍼
    static bool readFileToString(const std::string& path, std::string& out)
    {
        std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary);
        if (!ifs) return false;
        std::ostringstream ss;
        ss << ifs.rdbuf();
        out = ss.str();
        return true;
    }

    static std::string formatIsoTime(std::time_t t)
    {
        char buf[64] = { 0 };
        std::tm tm_storage;
    #if defined(_MSC_VER) || defined(__MINGW32__)
        // MSVC / MinGW: use localtime_s (thread-safe)
        std::tm* tm = nullptr;
        if (localtime_s(&tm_storage, &t) == 0) {
            tm = &tm_storage;
        }
    #elif defined(__unix__) || defined(__APPLE__) || defined(_POSIX_VERSION)
        // POSIX: use localtime_r (thread-safe)
        std::tm* tm = localtime_r(&t, &tm_storage);
    #else
        // Fallback
        std::tm* tm = std::localtime(&t);
    #endif
        if (tm) {
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
            return std::string(buf);
        }
        return std::string();
    }

    static bool isInTransaction(sqlite3* db)
    {
        if (!db) return false;
        return (sqlite3_get_autocommit(db) == 0);
    }

    static bool beginTransaction(sqlite3* db)
    {
        if (!db) return false;
        char* err = nullptr;
        int rc = sqlite3_exec(db, "BEGIN;", nullptr, nullptr, &err);
        if (rc != SQLITE_OK) {
            if (err) sqlite3_free(err);
            return false;
        }
        return true;
    }

    static bool commitTransaction(sqlite3* db)
    {
        if (!db) return false;
        char* err = nullptr;
        int rc = sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &err);
        if (rc != SQLITE_OK) {
            if (err) sqlite3_free(err);
            return false;
        }
        return true;
    }

    static void rollbackTransaction(sqlite3* db)
    {
        if (!db) return;
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
    }

    SqliteDataRepository::SqliteDataRepository(const std::string& dbFilePath, const std::string& imageBasePath)
        : dbPath_(dbFilePath), imageBasePath_(imageBasePath), db_(nullptr), initialized_(false)
    {
    }

    SqliteDataRepository::~SqliteDataRepository()
    {
        Shutdown();
    }

    StorageError SqliteDataRepository::Initialize()
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (initialized_) return StorageSuccess;

        int rc = sqlite3_open(dbPath_.c_str(), &db_);
        if (rc != SQLITE_OK) {
            std::cerr << "[SqliteDataRepository] Failed to open DB '" << dbPath_ << "': "
                << (db_ ? sqlite3_errmsg(db_) : "unknown") << std::endl;
            if (db_) { sqlite3_close(db_); db_ = nullptr; }
            return StorageWriteFailed;
        }

        {
            char* err = nullptr;
            rc = sqlite3_exec(db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &err);
            if (rc != SQLITE_OK) {
                std::cerr << "[SqliteDataRepository] PRAGMA foreign_keys failed: " << (err ? err : "unknown") << std::endl;
                if (err) sqlite3_free(err);
                sqlite3_close(db_); db_ = nullptr;
                return StorageWriteFailed;
            }
            rc = sqlite3_exec(db_, "PRAGMA journal_mode = WAL;", nullptr, nullptr, &err);
            if (rc != SQLITE_OK) {
                if (err) sqlite3_free(err);
                std::cerr << "[SqliteDataRepository] PRAGMA journal_mode=WAL failed or not supported." << std::endl;
            }
            else {
                if (err) sqlite3_free(err);
            }
            rc = sqlite3_exec(db_, "PRAGMA synchronous = NORMAL;", nullptr, nullptr, &err);
            if (err) sqlite3_free(err);
        }

        // 마이그레이션 SQL 로드
        std::string migrationPath;
        auto pos = dbPath_.find_last_of("/\\");
        if (pos != std::string::npos) {
            migrationPath = dbPath_.substr(0, pos) + "/migration_v1.sql";
        }
        else {
            migrationPath = "migration_v1.sql";
        }

        std::string migrationSql;
        bool loaded = readFileToString(migrationPath, migrationSql);
        if (!loaded) {
            if (migrationPath != "migration_v1.sql") {
                loaded = readFileToString("migration_v1.sql", migrationSql);
            }
        }

        if (loaded && !migrationSql.empty()) {
            char* err = nullptr;
            rc = sqlite3_exec(db_, "BEGIN TRANSACTION;", nullptr, nullptr, &err);
            if (rc != SQLITE_OK) {
                std::cerr << "[SqliteDataRepository] Failed to BEGIN transaction for migration: " << (err ? err : "unknown") << std::endl;
                if (err) sqlite3_free(err);
                sqlite3_close(db_); db_ = nullptr;
                return StorageWriteFailed;
            }

            rc = sqlite3_exec(db_, migrationSql.c_str(), nullptr, nullptr, &err);
            if (rc != SQLITE_OK) {
                std::cerr << "[SqliteDataRepository] Failed to execute migration SQL: " << (err ? err : "unknown") << std::endl;
                if (err) sqlite3_free(err);
                sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);
                sqlite3_close(db_); db_ = nullptr;
                return StorageWriteFailed;
            }

            rc = sqlite3_exec(db_, "COMMIT;", nullptr, nullptr, &err);
            if (rc != SQLITE_OK) {
                std::cerr << "[SqliteDataRepository] Failed to COMMIT migration transaction: " << (err ? err : "unknown") << std::endl;
                if (err) sqlite3_free(err);
                sqlite3_exec(db_, "ROLLBACK;", nullptr, nullptr, nullptr);               
                sqlite3_close(db_); db_ = nullptr;
                return StorageWriteFailed;
            }

            std::cout << "[SqliteDataRepository] Migration applied from: " << migrationPath << std::endl;
        }
        else {
            std::cout << "[SqliteDataRepository] No migration file found at: " << migrationPath
                << " (or file empty). Skipping migration." << std::endl;
        }

        initialized_ = true;
        return StorageSuccess;
    }

    StorageError SqliteDataRepository::Shutdown()
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (db_) {
            sqlite3_close(db_);
            db_ = nullptr;
        }
        initialized_ = false;
        return StorageSuccess;
    }

    StorageError SqliteDataRepository::executeSimple(const char* sql)
    {
        if (!initialized_) return StorageWriteFailed;
        char* err = nullptr;
        int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &err);
        if (rc != SQLITE_OK) {
            if (err) {
                std::cerr << "[SqliteDataRepository] executeSimple error: " << err << std::endl;
                sqlite3_free(err);
            }
            return StorageWriteFailed;
        }
        return StorageSuccess;
    }

    StorageError SqliteDataRepository::SaveParam(const std::string& recipe, const std::string& name, const std::string& value)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (!initialized_) return StorageWriteFailed;

        const char* sql = "INSERT OR REPLACE INTO params(recipe,name,value) VALUES(?,?,?);";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            return StorageWriteFailed;
        }
        sqlite3_bind_text(stmt, 1, recipe.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, value.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) return StorageWriteFailed;

        return StorageSuccess;
    }

    StorageError SqliteDataRepository::LoadParam(const std::string& recipe, const std::string& name, std::string& outValue)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (!initialized_) return StorageFileNotFound;

        const char* sql = "SELECT value FROM params WHERE recipe=? AND name=? LIMIT 1;";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            return StorageWriteFailed;
        }
        sqlite3_bind_text(stmt, 1, recipe.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            const unsigned char* text = sqlite3_column_text(stmt, 0);
            outValue = text ? reinterpret_cast<const char*>(text) : std::string();
            sqlite3_finalize(stmt);
            return StorageSuccess;
        }

        sqlite3_finalize(stmt);
        return StorageFileNotFound;
    }

    std::string SqliteDataRepository::makeImageFilename(const std::string& tag)
    {
        std::time_t t = std::time(nullptr);
        uint64_t cnt = ++g_imageCounter;
        std::ostringstream oss;
        oss << tag << "_" << t << "_" << cnt << ".bin";
        return FileUtils::JoinPath(imageBasePath_, oss.str());
    }

    StorageError SqliteDataRepository::SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (!initialized_) return StorageWriteFailed;

        // final path and temp path
        std::string finalPath = makeImageFilename(contextTag);
        std::string tempPath = finalPath + ".tmp";

        // write temp file
        {
            std::ofstream ofs(tempPath.c_str(), std::ios::binary);
            if (!ofs) {
                std::cerr << "[SqliteDataRepository] Failed to open temp image file: " << tempPath << std::endl;
                return StorageWriteFailed;
            }
            if (!imageData.empty()) {
                ofs.write(reinterpret_cast<const char*>(imageData.data()), static_cast<std::streamsize>(imageData.size()));
            }
            ofs.close();
        }

        // A 방식: DB에 tempPath로 INSERT -> commit -> rename(temp->final) -> UPDATE path
        if (!beginTransaction(db_)) {
            std::remove(tempPath.c_str());
            return StorageWriteFailed;
        }

        const char* insertSql = "INSERT INTO images(tag, path, created_at) VALUES(?,?,?);";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, insertSql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            rollbackTransaction(db_);
            std::remove(tempPath.c_str());
            return StorageWriteFailed;
        }

        sqlite3_bind_text(stmt, 1, contextTag.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, tempPath.c_str(), -1, SQLITE_TRANSIENT);
        std::time_t t = std::time(nullptr);
        std::string ts = formatIsoTime(t);
        sqlite3_bind_text(stmt, 3, ts.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            rollbackTransaction(db_);
            std::remove(tempPath.c_str());
            return StorageWriteFailed;
        }

        // get inserted row id
        sqlite3_int64 rowid = sqlite3_last_insert_rowid(db_);

        if (!commitTransaction(db_)) {
            rollbackTransaction(db_);
            std::remove(tempPath.c_str());
            return StorageWriteFailed;
        }

        // 2) rename temp -> final
        if (std::rename(tempPath.c_str(), finalPath.c_str()) != 0) {
            std::cerr << "[SqliteDataRepository] Failed to rename temp image to final path: " << tempPath << " -> " << finalPath << std::endl;
            std::remove(tempPath.c_str());
            if (!beginTransaction(db_)) {
                return StorageWriteFailed;
            }
            const char* delSql = "DELETE FROM images WHERE id = ?;";
            sqlite3_stmt* delStmt = nullptr;
            rc = sqlite3_prepare_v2(db_, delSql, -1, &delStmt, nullptr);
            if (rc == SQLITE_OK) {
                sqlite3_bind_int64(delStmt, 1, rowid);
                rc = sqlite3_step(delStmt);
                sqlite3_finalize(delStmt);
                if (rc == SQLITE_DONE) {
                    commitTransaction(db_);
                }
                else {
                    rollbackTransaction(db_);
                    std::cerr << "[SqliteDataRepository] Failed to delete image record for rowid=" << rowid << std::endl;
                    return StorageWriteFailed;
                }
            }
            else {
                rollbackTransaction(db_);
                return StorageWriteFailed;
            }
            return StorageWriteFailed;
        }

        // 3) UPDATE images.path -> finalPath WHERE id = rowid
        if (!beginTransaction(db_)) {
            std::remove(finalPath.c_str());
            return StorageWriteFailed;
        }

        const char* updateSql = "UPDATE images SET path = ? WHERE id = ?;";
        sqlite3_stmt* updStmt = nullptr;
        rc = sqlite3_prepare_v2(db_, updateSql, -1, &updStmt, nullptr);
        if (rc != SQLITE_OK) {
            if (updStmt) sqlite3_finalize(updStmt);
            rollbackTransaction(db_);
            std::remove(finalPath.c_str());
            return StorageWriteFailed;
        }

        sqlite3_bind_text(updStmt, 1, finalPath.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(updStmt, 2, rowid);

        rc = sqlite3_step(updStmt);
        sqlite3_finalize(updStmt);
        if (rc != SQLITE_DONE) {
            rollbackTransaction(db_);
            std::remove(finalPath.c_str());
            return StorageWriteFailed;
        }

        if (!commitTransaction(db_)) {
            rollbackTransaction(db_);
            std::remove(finalPath.c_str());
            return StorageWriteFailed;
        }

        outPath = finalPath;
        return StorageSuccess;
    }

    StorageError SqliteDataRepository::SaveSequenceRun(const std::string& sequenceName, const std::string& summary)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (!initialized_) return StorageWriteFailed;

        if (!beginTransaction(db_)) return StorageWriteFailed;

        const char* sql = "INSERT INTO sequence_runs(name, result_summary, created_at, status) VALUES(?,?,?,?);";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        std::time_t t = std::time(nullptr);
        std::string ts = formatIsoTime(t);

        sqlite3_bind_text(stmt, 1, sequenceName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, summary.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, ts.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, "finished", -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        if (!commitTransaction(db_)) {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        return StorageSuccess;
    }

    StorageError SqliteDataRepository::CreateSequenceRun(const std::string& sequenceName, const std::string& paramsJson, int& outRunId)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        outRunId = -1;
        if (!initialized_) return StorageWriteFailed;

        if (!beginTransaction(db_)) return StorageWriteFailed;

        const char* sql = "INSERT INTO sequence_runs(type, params_json, status, created_at) VALUES(?,?,?,?);";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        std::time_t t = std::time(nullptr);
        std::string ts = formatIsoTime(t);

        sqlite3_bind_text(stmt, 1, sequenceName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, paramsJson.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, "running", -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, ts.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        sqlite3_int64 rowid = sqlite3_last_insert_rowid(db_);
        if (!commitTransaction(db_)) {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        outRunId = static_cast<int>(rowid);
        return StorageSuccess;
    }

    StorageError SqliteDataRepository::SaveZFocusPoint(int runId, double zPosition, double score, int sampleCount, const std::string& extraJson)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (!initialized_) return StorageWriteFailed;

        if (!beginTransaction(db_)) return StorageWriteFailed;

        const char* sql = "INSERT INTO z_focus_points(run_id, z_position, score, sample_count, extra_json, created_at) VALUES(?,?,?,?,?,?);";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            return StorageWriteFailed;
        }

        std::time_t t = std::time(nullptr);
        std::string ts = formatIsoTime(t);

        sqlite3_bind_int(stmt, 1, runId);
        sqlite3_bind_double(stmt, 2, zPosition);
        sqlite3_bind_double(stmt, 3, score);
        sqlite3_bind_int(stmt, 4, sampleCount);
        sqlite3_bind_text(stmt, 5, extraJson.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, ts.c_str(), -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        if (rc != SQLITE_DONE) {
             rollbackTransaction(db_);
             return StorageWriteFailed;
        }

        if (!commitTransaction(db_)) {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        return StorageSuccess;
    }


    //////////검사 결과 저장//////////////

    StorageError SqliteDataRepository::SaveZFocusResult(int camIndex, int locationId, int pkgId, double newFocus)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (!initialized_) return StorageWriteFailed;

        if (!beginTransaction(db_))
            return StorageWriteFailed;

		const char* sql =
			"INSERT INTO InspInitPos "
			"(cam_index, location_id, pkg_id, pos_x, pos_y, focus) "
			"VALUES(?, ?, ?, 0, 0, ?) "
			"ON CONFLICT(cam_index, location_id, pkg_id) "
			"DO UPDATE SET focus = excluded.focus;";

        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
		{
			if (stmt) sqlite3_finalize(stmt);
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

        sqlite3_bind_int(stmt, 1, camIndex);
        sqlite3_bind_int(stmt, 2, locationId);
        sqlite3_bind_int(stmt, 3, pkgId);
        sqlite3_bind_double(stmt, 4, newFocus);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        if (rc != SQLITE_DONE)
        {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        if (!commitTransaction(db_))
        {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        return StorageSuccess;
    }


   // Picker-Cam 거리 값 저장
	StorageError SqliteDataRepository::SavePickerCamDistanceResult(
		int camIndex,
		int pkgId,
        double narrowX,
        double narrowY,
        double wideX,
        double wideY)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageWriteFailed;

		if (!beginTransaction(db_))
			return StorageWriteFailed;

		const char* sql =
			"INSERT INTO PickerCamDistance "
			"(cam_index, pkg_id, N_offset_x, N_offset_y, W_offset_x, W_offset_y) "
			"VALUES(?, ?, ?, ?, ?, ?) "
			"ON CONFLICT(cam_index, pkg_id) "
			"DO UPDATE SET "
            "N_offset_x = excluded.N_offset_x, "
            "N_offset_y = excluded.N_offset_y, "
            "W_offset_x  = excluded.W_offset_x, "
            "W_offset_y = excluded.W_offset_y;";

		sqlite3_stmt* stmt = nullptr;

		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
		{
			if (stmt) sqlite3_finalize(stmt);
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		sqlite3_bind_int(stmt, 1, camIndex);
		sqlite3_bind_int(stmt, 2, pkgId);
		sqlite3_bind_double(stmt, 3, narrowX);
		sqlite3_bind_double(stmt, 4, narrowY);
		sqlite3_bind_double(stmt, 5, wideX);
		sqlite3_bind_double(stmt, 6, wideY);

		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		if (rc != SQLITE_DONE)
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		if (!commitTransaction(db_))
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		return StorageSuccess;
	}

	// 검사 위치 정보 저장
	StorageError SqliteDataRepository::SaveCalibrationPosResult(
		int camIndex,
		int locationId,
		int pkgId,
		double posX,
		double posY)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageWriteFailed;
		
		if (!beginTransaction(db_))
			return StorageWriteFailed;

		const char* sql =
			"INSERT INTO InspInitPos "
			"(cam_index, location_id, pkg_id, pos_x, pos_y, focus) "
			"VALUES (?, ?, ?, ?, ?, 0) "
			"ON CONFLICT(cam_index, location_id, pkg_id) "
			"DO UPDATE SET "
			"pos_x = excluded.pos_x, "
			"pos_y = excluded.pos_y;";

		sqlite3_stmt* stmt = nullptr;

		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
		{
			if (stmt) sqlite3_finalize(stmt);
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		sqlite3_bind_int(stmt, 1, camIndex);
		sqlite3_bind_int(stmt, 2, locationId);
		sqlite3_bind_int(stmt, 3, pkgId);
		sqlite3_bind_double(stmt, 4, posX);
		sqlite3_bind_double(stmt, 5, posY);

		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		if (rc != SQLITE_DONE)
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		if (!commitTransaction(db_))
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		return StorageSuccess;
	}

	// 핸드 피치 정보 저장
	StorageError SqliteDataRepository::SaveHandPitchResult(
		int handId,
		int pkgId,
		int col,
		int row,
		double narrowX,
		double narrowY,
		double wideX,
		double wideY)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageWriteFailed;
		
		if (!beginTransaction(db_))
			return StorageWriteFailed;

		const char* sql =
			"INSERT INTO HandPitch "
			"(hand_id, pkg_id, row, col, "
			"N_offset_x, N_offset_y, W_offset_x, W_offset_y) "
			"VALUES (?, ?, ?, ?, ?, ?, ?, ?) "
			"ON CONFLICT(hand_id, pkg_id, row, col) "
			"DO UPDATE SET "
			"N_offset_x = excluded.N_offset_x, "
			"N_offset_y = excluded.N_offset_y, "
			"W_offset_x = excluded.W_offset_x, "
			"W_offset_y = excluded.W_offset_y;";

		sqlite3_stmt* stmt = nullptr;

		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
		{
			if (stmt) sqlite3_finalize(stmt);
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		sqlite3_bind_int(stmt, 1, handId);
		sqlite3_bind_int(stmt, 2, pkgId);
		sqlite3_bind_int(stmt, 3, row);
		sqlite3_bind_int(stmt, 4, col);
		sqlite3_bind_double(stmt, 5, narrowX);
		sqlite3_bind_double(stmt, 6, narrowY);
		sqlite3_bind_double(stmt, 7, wideX);
		sqlite3_bind_double(stmt, 8, wideY);

		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		if (rc != SQLITE_DONE)
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		if (!commitTransaction(db_))
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		return StorageSuccess;
	}

	StorageError SqliteDataRepository::SaveTeachingResult(
		int handId,
		int locationId,
		int pkgId,
		double posX,
		double posY,
		double posZ)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageWriteFailed;
		
		if (!beginTransaction(db_))
			return StorageWriteFailed;

		sqlite3_stmt* stmt = nullptr;
		int rc;

		const char* insertInspectionSql =
			"INSERT INTO UpperCamTeachingInspection "
			"(hand_id, location_id, pkg_id, insp_date) "
			"VALUES (?, ?, ?, datetime('now'));";

		rc = sqlite3_prepare_v2(db_, insertInspectionSql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		sqlite3_bind_int(stmt, 1, handId);
		sqlite3_bind_int(stmt, 2, locationId);
		sqlite3_bind_int(stmt, 3, pkgId);

		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		if (rc != SQLITE_DONE)
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		sqlite3_int64 inspId = sqlite3_last_insert_rowid(db_);

		const char* insertPosSql =
			"INSERT INTO TeachingPos "
			"(insp_id, pos_x, pos_y, pos_z) "
			"VALUES (?, ?, ?, ?);";

		rc = sqlite3_prepare_v2(db_, insertPosSql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		sqlite3_bind_int64(stmt, 1, inspId);
		sqlite3_bind_double(stmt, 2, posX);
		sqlite3_bind_double(stmt, 3, posY);
		sqlite3_bind_double(stmt, 4, posZ);

		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		if (rc != SQLITE_DONE)
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		if (!commitTransaction(db_))
		{
			rollbackTransaction(db_);
			return StorageWriteFailed;
		}

		return StorageSuccess;
	}

    //////////검사 결과 로드//////////////
	StorageError SqliteDataRepository::LoadInspInitPos(
		int camIndex,
		int locationId,
		int pkgId,
		double& posX,
		double& posY,
		double& focus)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageFileNotFound;
		
		const char* sql =
			"SELECT pos_x, pos_y, focus "
			"FROM InspInitPos "
			"WHERE cam_index = ? AND location_id = ? AND pkg_id = ?;";

		sqlite3_stmt* stmt = nullptr;
		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
			return StorageFileNotFound;

		sqlite3_bind_int(stmt, 1, camIndex);
		sqlite3_bind_int(stmt, 2, locationId);
		sqlite3_bind_int(stmt, 3, pkgId);

		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW)
		{
			posX = sqlite3_column_double(stmt, 0);
			posY = sqlite3_column_double(stmt, 1);
			focus = sqlite3_column_double(stmt, 2);
			sqlite3_finalize(stmt);
			return StorageSuccess;
		}

		sqlite3_finalize(stmt);
		return StorageNotFound;
	}

	StorageError SqliteDataRepository::LoadPickerCamDistance(
		int camIndex,
		int pkgId,
        double& narrowX,
        double& narrowY,
        double& wideX,
        double& wideY)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageFileNotFound;
		
		const char* sql =
			"SELECT N_offset_x, N_offset_y, W_offset_x, W_offset_y "
			"FROM PickerCamDistance "
			"WHERE cam_index = ? AND pkg_id = ?;";

		sqlite3_stmt* stmt = nullptr;
		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
			return StorageFileNotFound;

		sqlite3_bind_int(stmt, 1, camIndex);
		sqlite3_bind_int(stmt, 2, pkgId);

		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW)
		{
            narrowX = sqlite3_column_double(stmt, 0);
            narrowY = sqlite3_column_double(stmt, 1);
            wideX = sqlite3_column_double(stmt, 2);
            wideY = sqlite3_column_double(stmt, 3);
			sqlite3_finalize(stmt);
			return StorageSuccess;
		}

		sqlite3_finalize(stmt);
		return StorageNotFound;
	}

	StorageError SqliteDataRepository::LoadHandPitch(
		int handId,
		int pkgId,
		int row,
		int col,
		double& narrowX,
		double& narrowY,
		double& wideX,
		double& wideY)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageFileNotFound;
		
		const char* sql =
			"SELECT N_offset_x, N_offset_y, W_offset_x, W_offset_y "
			"FROM HandPitch "
			"WHERE hand_id = ? AND pkg_id = ? AND row = ? AND col = ?;";

		sqlite3_stmt* stmt = nullptr;
		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
			return StorageFileNotFound;

		sqlite3_bind_int(stmt, 1, handId);
		sqlite3_bind_int(stmt, 2, pkgId);
		sqlite3_bind_int(stmt, 3, row);
		sqlite3_bind_int(stmt, 4, col);

		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW)
		{
			narrowX = sqlite3_column_double(stmt, 0);
			narrowY = sqlite3_column_double(stmt, 1);
			wideX = sqlite3_column_double(stmt, 2);
			wideY = sqlite3_column_double(stmt, 3);
			sqlite3_finalize(stmt);
			return StorageSuccess;
		}

		sqlite3_finalize(stmt);
		return StorageNotFound;
	}

	StorageError SqliteDataRepository::LoadTeachingResult(
		int handId,
		int locationId,
		int pkgId,
		int dateoffset,
		double& posX,
		double& posY,
		double& posZ)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageFileNotFound;
		
		const char* sql =
			"SELECT t.pos_x, t.pos_y, t.pos_z "
			"FROM UpperCamTeachingInspection u "
			"JOIN TeachingPos t ON u.id = t.insp_id "
			"WHERE u.hand_id = ? AND u.location_id = ? AND u.pkg_id = ? "
			"ORDER BY u.insp_date DESC "
			"LIMIT 1 OFFSET ?;";

		sqlite3_stmt* stmt = nullptr;
		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
			return StorageFileNotFound;

		sqlite3_bind_int(stmt, 1, handId);
		sqlite3_bind_int(stmt, 2, locationId);
		sqlite3_bind_int(stmt, 3, pkgId);
		sqlite3_bind_int(stmt, 4, dateoffset);

		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW)
		{
			posX = sqlite3_column_double(stmt, 0);
			posY = sqlite3_column_double(stmt, 1);
			posZ = sqlite3_column_double(stmt, 2);
			sqlite3_finalize(stmt);
			return StorageSuccess;
		}

		sqlite3_finalize(stmt);
		return StorageNotFound;
	}

	StorageError SqliteDataRepository::LoadHandCamGroup(
		int handId,
		std::vector<int>& camIds)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageFileNotFound;
		
		camIds.clear();

		const char* sql =
			"SELECT cam_index_id "
			"FROM Hand_Cam_Group "
			"WHERE hand_id = ?;";

		sqlite3_stmt* stmt = nullptr;

		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
			return StorageFileNotFound;

		sqlite3_bind_int(stmt, 1, handId);

		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
		{
			int camId = sqlite3_column_int(stmt, 0);
			camIds.push_back(camId);
		}

		sqlite3_finalize(stmt);

		if (camIds.empty())
			return StorageNotFound;

		return StorageSuccess;
	}

    StorageError SqliteDataRepository::LoadCamLocationGroup(
        int camIndex,
        std::vector<int>& locateIds)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (!initialized_) return StorageFileNotFound;

        locateIds.clear();

        const char* sql =
            "SELECT location_id "
            "FROM Cam_Location_Group "
            "WHERE cam_index_id = ?;";

        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
            return StorageFileNotFound;

        sqlite3_bind_int(stmt, 1, camIndex);

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            int locateId = sqlite3_column_int(stmt, 0);
            locateIds.push_back(locateId);
        }

        sqlite3_finalize(stmt);

        if (locateIds.empty())
            return StorageNotFound;

        return StorageSuccess;
    }

	StorageError SqliteDataRepository::LoadLocationIdByName(
		const std::string& locateName,
		int& locationId)
	{
		std::lock_guard<std::mutex> lg(mutex_);
		if (!initialized_) return StorageFileNotFound;
		
		const char* sql =
			"SELECT id "
			"FROM Location "
			"WHERE locate = ?;";

		sqlite3_stmt* stmt = nullptr;

		int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK)
			return StorageFileNotFound;

		sqlite3_bind_text(stmt, 1, locateName.c_str(), -1, SQLITE_TRANSIENT);

		rc = sqlite3_step(stmt);
		if (rc == SQLITE_ROW)
		{
			locationId = sqlite3_column_int(stmt, 0);
			sqlite3_finalize(stmt);
			return StorageSuccess;
		}

		sqlite3_finalize(stmt);
		return StorageNotFound;
	}

    StorageError SqliteDataRepository::UpdateSequenceRunStatus(int runId, const std::string& status, const std::string& resultSummaryJson)
    {
        std::lock_guard<std::mutex> lg(mutex_);
        if (!initialized_) return StorageWriteFailed;

        if (!beginTransaction(db_)) return StorageWriteFailed;

        const char* sql = "UPDATE sequence_runs SET status = ?, result_summary = ?, finished_at = ? WHERE id = ?;";
        sqlite3_stmt* stmt = nullptr;
        int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            if (stmt) sqlite3_finalize(stmt);
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        std::time_t t = std::time(nullptr);
        std::string ts = formatIsoTime(t);

        sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, resultSummaryJson.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, ts.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, runId);

        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        if (!commitTransaction(db_)) {
            rollbackTransaction(db_);
            return StorageWriteFailed;
        }

        return StorageSuccess;
    }

} // namespace VMF
