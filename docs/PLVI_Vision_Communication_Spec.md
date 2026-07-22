# PLVI Vision Communication Specification

## 1. 개요 및 연결 정보 (Header Info)

| 항목 | 내용 |
|------|------|
| **대상 시스템** | Handler (H) ↔ Vision (V) |
| **Server / IP** | PC1 ~ PC2 |
| **Port Number** | 8000 ~ 8001 |
| **서비스 ID / Func ID** | Service: 107 (공통) |

---

## 2. 메시지 유형 및 서비스 ID 요약

| 구분 | Direction | Ass'y / 기능 | Message | S | F | 설명 |
|------|-----------|--------------|---------|---|---|------|
| 1 | H → V | PLVI | REQ_MEASURE | 107 | 5 | PLVI 검사 요청 / 검사 취소 |
| 2 | V → H | PLVI | REQ_RESULT | 107 | 6 | PLVI 검사 결과 응답 (Req ID + 1000) |
| 3 | H → V | Orientation Check | REQ_MEASURE | 107 | 7 | Orientation (A1 index mark) 검사 요청 |
| 4 | H → V | Piggyback | REQ_MEASURE | 107 | 9 | Piggyback 검사 전 위치 정보 전달 |
| 5 | H → V | SET_COK | REQ_MEASURE | 107 | 11 | SET_COK 요청 (nCmd = 1000) |

---

## 3. 상세 메시지 패킷 규격 (Message Specifications)

### 3.1. PLVI (Plate & Device Inspection)

#### [REQ_MEASURE] PLVI 검사 요청 (H → V)

**S / F: 107 / 5**

**Action:** PLVI 검사를 요청하거나 취소할 때 사용.

| # | Parameter | Type / Format | Description / Value | 비고 (Remark) |
|---|-----------|---------------|---------------------|---------------|
| 1 | nDataID | INT | ID 값 사용 | |
| 2 | nStatus | INT | 상태 정보 | |
| 3 | cData[0] | INT (in String) | PLVI 위치 | 0: Set-Plate1, 1: Set-Plate2<br>11: Shuttle Table1, 12: Shuttle Table2<br>21: Retest Buffer1, 22: Retest Buffer2 |
|   | cData[1] | String | PKG Name (현재 검사하는 PKG) | |
|   | cData[2] | String/Array | C-Tray 정보 | [2][0]: C-Tray X 수량<br>[2][1]: C-Tray Y 수량<br>X, Y 수량 일치 여부로 동일 PKG 확인 |
|   | cData[3] | String/Array | 개별 Device 정보 | [3][0]: 0 (없음) 또는 99 (존재)<br>[3][1] ~: Device 존재 유무 배열<br>수량은 C-Tray X × Y로 계산 |

> **Note:** Retest Buffer 번호 추가됨

#### [REQ_RESULT] PLVI 검사 결과 (V → H)

**S / F: 107 / 6**

**Action:** REQ_MEASURE 요청에 대한 결과 응답.

| # | Parameter | Type / Format | Description / Value | 비고 (Remark) |
|---|-----------|---------------|---------------------|---------------|
| 1 | nDataID | INT | 요청의 Req ID + 1000 | |
| 2 | nStatus | INT | 0: STATUS_ERROR<br>1: STATUS_SUCCESS | |
| 3 | cData[0] | INT (in String) | Error Description | 0: BUSY<br>1: CAM_DISCONNECT<br>2: CAM_TIMEOUT<br>3: CAM_STATUS_FAIL<br>4: LIGHT_ERROR<br>5: NO_TEACHING_DATA (Teaching Data가 없는 경우 5 리턴) |
|   | cData[1] | String | 전체 결과 ('0': OK, '1': NG) | PLVI 전체 판정 |
|   | cData[2] | INT (in String) | PLVI 위치 (요청 시와 동일한 위치 값) | |
|   | cData[3] | String/Array | 개별 Device 검사 결과 | 0: Device 없음<br>99: Device 존재 (정상)<br>1: Pocket Leave<br>2: Pocket Double<br>11: Device Missing (Handler에는 있으나 비전 결과 없음)<br>12: Device Mismatch (Handler에는 없으나 비전 결과 있음) |

---

### 3.2. Orientation Check (A1 Index Mark)

#### [REQ_MEASURE] Orientation 검사 요청 (H → V)

**S / F: 107 / 7**

**Action:** Orientation (A1 index mark) 방향 및 위치 검사를 요청.

| # | Parameter | Type / Format | Description / Value | 비고 (Remark) |
|---|-----------|---------------|---------------------|---------------|
| 1 | nDataID | INT | ID 값 사용 | |
| 2 | nStatus | INT | 상태 정보 | |
| 3 | cData[0] | INT (in String) | Position Info (방향) | 0: UP_LEFT<br>1: UP_RIGHT<br>2: DOWN_RIGHT<br>3: DOWN_LEFT |
|   | cData[1] | INT (in String) | Setplate Position Info (검사 위치) | 0: Set-Plate1<br>1: Set-Plate2<br>11: Shuttle Table1<br>12: Shuttle Table2 |

---

### 3.3. Piggyback Inspection Ready

#### [REQ_MEASURE] Piggyback 검사 위치 전달 (H → V)

**S / F: 107 / 9**

**Action:** Piggyback 검사 시작 전, 위치 정보를 전달.

| # | Parameter | Type / Format | Description / Value | 비고 (Remark) |
|---|-----------|---------------|---------------------|---------------|
| 1 | nDataID | INT | ID 값 사용 | |
| 2 | nStatus | INT | 상태 정보 | |
| 3 | cData[0] | INT (in String) | 검사 위치 정보 | 1: Station No.1<br>2: Station No.2<br>... |

---

### 3.4. SET_COK

#### [REQ_MEASURE] COK 설정 요청 (H → V)

**S / F: 107 / 11**

**Action:** COK 파라미터 정보 및 PKG 정보 설정.

| # | Parameter | Type / Format | Description / Value | 비고 (Remark) |
|---|-----------|---------------|---------------------|---------------|
| 1 | nCmd | INT | 1000 (SET_COK) | |
| 2 | nStatus | INT | 상태 정보 | |
| 3 | cData[0] | INT (in String) | PKG 수량 (1: 1PKG, 2: 2PKG) | |
|   | cData[1] | String | PKG1 Name | |
|   | cData[2] | String/Array | PKG1 C-Tray 규격 정보 | [2][0]: PKG1 C-Tray X<br>[2][1]: PKG1 C-Tray Y |

---

## Revision History

| Version | Date | Author | Description |
|---------|------|--------|-------------|
| 1.0 | 2024 | - | Initial specification document |