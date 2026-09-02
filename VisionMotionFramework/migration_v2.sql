-- migration_v2.sql
-- VMF Vision Motion Framework 데이터베이스 스키마 확장
-- 신규 테이블: inspection_runs, inspection_results, inspection_locations, socket_masters
-- 작성일: 2026-05-14
-- 버전: 2.0

-- ============================================================
-- inspection_runs: 검사 실행 이력 테이블
-- ============================================================
CREATE TABLE IF NOT EXISTS inspection_runs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    type_code TEXT NOT NULL,          -- 검사 유형 코드 (PLVI, Orientation, COK_ID, VAT, Piggyback, BallInspection, SetCok 등)
    run_id TEXT NOT NULL,             -- 프로토콜 nDataID 또는 시퀀스 run_id
    params_json TEXT,                 -- 요청 파라미터 전체 (JSON 형태, cData 등)
    status TEXT NOT NULL DEFAULT 'running',  -- 상태 (running, finished, error 등)
    result_json TEXT,                 -- 결과 데이터 전체 (JSON 형태, cData 결과 등)
    error_code INTEGER DEFAULT -1,    -- 프로토콜 Error Description 코드 (-1이면 없음)
    created_at TEXT NOT NULL,         -- 생성 일시 (ISO 8601 형식)
    finished_at TEXT                  -- 종료 일시 (ISO 8601 형식, 상태가 finished일 때 설정)
);

CREATE INDEX IF NOT EXISTS idx_inspection_runs_type_code ON inspection_runs(type_code);
CREATE INDEX IF NOT EXISTS idx_inspection_runs_run_id ON inspection_runs(r  un_id);
CREATE INDEX IF NOT EXISTS idx_inspection_runs_status ON inspection_runs(status);
CREATE INDEX IF NOT EXISTS idx_inspection_runs_created_at ON inspection_runs(created_at);

-- ============================================================
-- inspection_results: 검사 결과 상세 항목 테이블
-- ============================================================
CREATE TABLE IF NOT EXISTS inspection_results (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    run_id INTEGER NOT NULL,          -- inspection_runs.id (FK)
    result_type TEXT NOT NULL,        -- 결과 유형 (예: device_result, socket_result, offset_data 등)
    result_index INTEGER NOT NULL DEFAULT -1,  -- 배열 인덱스 (Device 0, Socket 1 등, 인덱스 없으면 -1)
    result_json TEXT NOT NULL,        -- 세부 결과 데이터 (JSON 형태)
    created_at TEXT NOT NULL,         -- 생성 일시 (ISO 8601 형식)
    FOREIGN KEY (run_id) REFERENCES inspection_runs(id) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_inspection_results_run_id ON inspection_results(run_id);
CREATE INDEX IF NOT EXISTS idx_inspection_results_result_type ON inspection_results(result_type);
CREATE INDEX IF NOT EXISTS idx_inspection_results_result_index ON inspection_results(result_index);

-- ============================================================
-- inspection_locations: 검사 위치 마스터 테이블
-- ============================================================
CREATE TABLE IF NOT EXISTS inspection_locations (
    location_code INTEGER PRIMARY KEY,  -- 위치 코드 (0=Set-Plate1, 1=Set-Plate2, ..., 11=Shuttle Table1, ...)
    location_name TEXT NOT NULL,        -- 위치명
    location_type TEXT NOT NULL,        -- 위치 유형 ("SetPlate", "ShuttleTable", "RetestBuffer" 등)
    is_active INTEGER NOT NULL DEFAULT 1  -- 활성화 여부 (1=활성, 0=비활성)
);

-- 초기 데이터: Set-Plate 위치 (0~10)
INSERT OR IGNORE INTO inspection_locations (location_code, location_name, location_type, is_active) VALUES
(0, 'Set-Plate1', 'SetPlate', 1),
(1, 'Set-Plate2', 'SetPlate', 1),
(2, 'Set-Plate3', 'SetPlate', 1),
(3, 'Set-Plate4', 'SetPlate', 1),
(4, 'Set-Plate5', 'SetPlate', 1),
(5, 'Set-Plate6', 'SetPlate', 1),
(6, 'Set-Plate7', 'SetPlate', 1),
(7, 'Set-Plate8', 'SetPlate', 1),
(8, 'Set-Plate9', 'SetPlate', 1),
(9, 'Set-Plate10', 'SetPlate', 1),
(10, 'Set-Plate11', 'SetPlate', 1);

-- 초기 데이터: Shuttle Table 위치 (11~21)
INSERT OR IGNORE INTO inspection_locations (location_code, location_name, location_type, is_active) VALUES
(11, 'Shuttle Table1', 'ShuttleTable', 1),
(12, 'Shuttle Table2', 'ShuttleTable', 1),
(13, 'Shuttle Table3', 'ShuttleTable', 1),
(14, 'Shuttle Table4', 'ShuttleTable', 1),
(15, 'Shuttle Table5', 'ShuttleTable', 1),
(16, 'Shuttle Table6', 'ShuttleTable', 1),
(17, 'Shuttle Table7', 'ShuttleTable', 1),
(18, 'Shuttle Table8', 'ShuttleTable', 1),
(19, 'Shuttle Table9', 'ShuttleTable', 1),
(20, 'Shuttle Table10', 'ShuttleTable', 1),
(21, 'Shuttle Table11', 'ShuttleTable', 1);

-- 초기 데이터: Retest Buffer 위치 (22~32)
INSERT OR IGNORE INTO inspection_locations (location_code, location_name, location_type, is_active) VALUES
(22, 'Retest Buffer1', 'RetestBuffer', 1),
(23, 'Retest Buffer2', 'RetestBuffer', 1),
(24, 'Retest Buffer3', 'RetestBuffer', 1),
(25, 'Retest Buffer4', 'RetestBuffer', 1),
(26, 'Retest Buffer5', 'RetestBuffer', 1),
(27, 'Retest Buffer6', 'RetestBuffer', 1),
(28, 'Retest Buffer7', 'RetestBuffer', 1),
(29, 'Retest Buffer8', 'RetestBuffer', 1),
(30, 'Retest Buffer9', 'RetestBuffer', 1),
(31, 'Retest Buffer10', 'RetestBuffer', 1),
(32, 'Retest Buffer11', 'RetestBuffer', 1);

CREATE INDEX IF NOT EXISTS idx_inspection_locations_code ON inspection_locations(location_code);
CREATE INDEX IF NOT EXISTS idx_inspection_locations_type ON inspection_locations(location_type);

-- ============================================================
-- socket_masters: Socket 마스터 테이블
-- ============================================================
CREATE TABLE IF NOT EXISTS socket_masters (
    socket_no INTEGER PRIMARY KEY,     -- Socket 번호 (1~32)
    socket_type INTEGER NOT NULL,      -- Socket 유형 (0=Single, 1=Dual-H, 2=Dual-V)
    station_no INTEGER NOT NULL        -- Station 번호
);

-- 초기 데이터: Socket 1~32 (기본값: Single, Station 1)
INSERT OR IGNORE INTO socket_masters (socket_no, socket_type, station_no) VALUES
(1, 0, 1), (2, 0, 1), (3, 0, 1), (4, 0, 1), (5, 0, 1), (6, 0, 1), (7, 0, 1), (8, 0, 1),
(9, 0, 1), (10, 0, 1), (11, 0, 1), (12, 0, 1), (13, 0, 1), (14, 0, 1), (15, 0, 1), (16, 0, 1),
(17, 0, 1), (18, 0, 1), (19, 0, 1), (20, 0, 1), (21, 0, 1), (22, 0, 1), (23, 0, 1), (24, 0, 1),
(25, 0, 1), (26, 0, 1), (27, 0, 1), (28, 0, 1), (29, 0, 1), (30, 0, 1), (31, 0, 1), (32, 0, 1);

CREATE INDEX IF NOT EXISTS idx_socket_masters_socket_no ON socket_masters(socket_no);
CREATE INDEX IF NOT EXISTS idx_socket_masters_type ON socket_masters(socket_type);
CREATE INDEX IF NOT EXISTS idx_socket_masters_station ON socket_masters(station_no);

-- ============================================================
-- 마이그레이션 완료
-- ============================================================
-- 참고: 기존 테이블 (params, images, sequence_runs, z_focus_points, z_focus_results, 
--        InspInitPos, PickerCamDistance, HandPitch, UpperCamTeachingInspection, TeachingPos,
--        Hand_Cam_Group, Cam_Location_Group, Location)은 migration_v1.sql에서 생성됩니다.
--        이 파일은 신규 테이블만 추가합니다.
