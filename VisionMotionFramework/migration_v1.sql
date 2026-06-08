-- migration_v1.sql
-- VMF SQLite Database Schema v1.0
-- SqliteDataRepository::Initialize()에서 자동 로드됩니다.

CREATE TABLE IF NOT EXISTS params (
    recipe      TEXT NOT NULL,
    name        TEXT NOT NULL,
    value       TEXT,
    PRIMARY KEY (recipe, name)
);

CREATE TABLE IF NOT EXISTS images (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    tag         TEXT NOT NULL,
    path        TEXT NOT NULL,
    created_at  TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS sequence_runs (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    type            TEXT NOT NULL,
    params_json     TEXT,
    status          TEXT DEFAULT 'running',
    result_summary  TEXT,
    created_at      TEXT NOT NULL,
    finished_at     TEXT
);

CREATE TABLE IF NOT EXISTS z_focus_points (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    run_id          INTEGER NOT NULL,
    z_position      REAL NOT NULL,
    score           REAL,
    sample_count    INTEGER DEFAULT 0,
    extra_json      TEXT,
    created_at      TEXT NOT NULL,
    FOREIGN KEY (run_id) REFERENCES sequence_runs(id)
);

CREATE TABLE IF NOT EXISTS InspInitPos (
    cam_index   INTEGER NOT NULL,
    location_id INTEGER NOT NULL,
    pkg_id      INTEGER NOT NULL,
    pos_x       REAL DEFAULT 0,
    pos_y       REAL DEFAULT 0,
    focus       REAL DEFAULT 0,
    PRIMARY KEY (cam_index, location_id, pkg_id)
);

CREATE TABLE IF NOT EXISTS PickerCamDistance (
    cam_index   INTEGER NOT NULL,
    pkg_id      INTEGER NOT NULL,
    N_offset_x  REAL DEFAULT 0,
    N_offset_y  REAL DEFAULT 0,
    W_offset_x  REAL DEFAULT 0,
    W_offset_y  REAL DEFAULT 0,
    PRIMARY KEY (cam_index, pkg_id)
);

CREATE TABLE IF NOT EXISTS HandPitch (
    hand_id     INTEGER NOT NULL,
    pkg_id      INTEGER NOT NULL,
    row         INTEGER NOT NULL,
    col         INTEGER NOT NULL,
    N_offset_x  REAL DEFAULT 0,
    N_offset_y  REAL DEFAULT 0,
    W_offset_x  REAL DEFAULT 0,
    W_offset_y  REAL DEFAULT 0,
    PRIMARY KEY (hand_id, pkg_id, row, col)
);

CREATE TABLE IF NOT EXISTS UpperCamTeachingInspection (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    hand_id     INTEGER NOT NULL,
    location_id INTEGER NOT NULL,
    pkg_id      INTEGER NOT NULL,
    insp_date   TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS TeachingPos (
    id       INTEGER PRIMARY KEY AUTOINCREMENT,
    insp_id  INTEGER NOT NULL,
    pos_x    REAL NOT NULL,
    pos_y    REAL NOT NULL,
    pos_z    REAL NOT NULL,
    FOREIGN KEY (insp_id) REFERENCES UpperCamTeachingInspection(id)
);

CREATE TABLE IF NOT EXISTS Hand_Cam_Group (
    hand_id       INTEGER NOT NULL,
    cam_index_id  INTEGER NOT NULL,
    PRIMARY KEY (hand_id, cam_index_id)
);

CREATE TABLE IF NOT EXISTS Cam_Location_Group (
    cam_index_id INTEGER NOT NULL,
    location_id  INTEGER NOT NULL,
    PRIMARY KEY (cam_index_id, location_id)
);

CREATE TABLE IF NOT EXISTS Location (
    id      INTEGER PRIMARY KEY AUTOINCREMENT,
    locate  TEXT NOT NULL UNIQUE
);
