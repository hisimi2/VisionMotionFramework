PLVI Vision Communication Specification

1. 개요 및 연결 정보 (Header Info)

항목	내용
대상 시스템	Handler (H) ↔ Vision (V)
Server / IP	PC1 ~ PC2
Port Number	8000 ~ 8001
서비스 ID / Func ID	Service: 107 (공통)


2. 메시지 유형 및 서비스 ID 요약

구분	Direction	Ass'y / 기능	Message	S	F	설명
1	H → V	PLVI	REQ_MEASURE	107	5	PLVI 검사 요청 / 검사 취소
2	V → H	PLVI	REQ_RESULT	107	6	PLVI 검사 결과 응답 (Req ID + 1000)
3	H → V	Orientation Check	REQ_MEASURE	107	7	Orientation (A1 index mark) 검사 요청
4	H → V	Piggyback	REQ_MEASURE	107	9	Piggyback 검사 전 위치 정보 전달
5	H → V	SET_COK	REQ_MEASURE	107	11	SET_COK 요청 (nCmd = 1000)


3. 상세 메시지 패킷 규격 (Message Specifications)

3.1. PLVI (Plate & Device Inspection)

[REQ_MEASURE] PLVI 검사 요청 (H → V)

S / F: 107 / 5
Action: PLVI 검사를 요청하거나 취소할 때 사용.  


#	Parameter	Type / Format	Description / Value	비고 (Remark)
1	nDataID	INT	ID 값 사용	
2	nStatus	INT	상태 정보	
3	cData[0]	INT (in String)	PLVI 위치	0: Set-Plate1, 1: Set-Plate2
11: Shuttle Table1, 12: Shuttle Table2
21: Retest Buffer1, 22: Retest Buffer2
cData[1]	String	PKG Name (현재 검사하는 PKG)	
cData[2]	String/Array	C-Tray 정보	[2][0]: C-Tray X 수량
[2][1]: C-Tray Y 수량
X, Y 수량 일치 여부로 동일 PKG 확인
cData[3]	String/Array	개별 Device 정보	[3][0]: 0 (없음) 또는 99 (존재br>[3][1] ~: Device 존재 유무 배열
수량은 C-Tray X × Y로 계산
> Note: Retest Buffer 번호 추가됨				

[REQ_RESULT] PLVI 검사 결과 (V → H)

S / F: 107 / 6
Action: REQ_MEASURE 요청에 대한 결과 응답.  


#	Parameter	Type / Format	Description / Value	비고 (Remark)
1	nDataID	INT	요청의 Req ID + 1000	
2	nStatus	INT	0: STATUS_ERROR
1: STATUS_SUCCESS	
3	cData[0]	INT (in String)	Error Description	0: BUSY
1: CAM_DISCONNECT
2: CAM_TIMEOUT
3: CAM_STATUS_FAIL
4: LIGHT_ERROR
5: NO_TEACHING_DATA (Teaching Data가 없는 경우 5 리턴)
cData[1]	String	전체 결과 ('0': OK, '1': NG)	PLVI 전체 판정
cData[2]	INT (in String)	PLVI 위치 (요청 시와 동일한 위치 값)	
cData[3]	String/Array	개별 Device 검사 결과	0: Device 없음
99: Device 존재 (정상br>1: Pocket Leave
2: Pocket Double
11: Device Missing (Handler에는 있으나 비전 결과 없음br>12: Device Mismatch (Handler에는 없으나 비전 결과 있음)


3.2. Orientation Check (A1 Index Mark)

[REQ_MEASURE] Orientation 검사 요청 (H → V)

S / F: 107 / 7
Action: Orientation (A1 index mark) 방향 및 위치 검사를 요청.  


#	Parameter	Type / Format	Description / Value	비고 (Remark)
1	nDataID	INT	ID 값 사용	
2	nStatus	INT	상태 정보	
3	cData[0]	INT (in String)	Position Info (방향)	0: UP_LEFT
1: UP_RIGHT
2: DOWN_RIGHT
3: DOWN_LEFT
cData[1]	INT (in String)	Setplate Position Info (검사 위치)	0: Set-Plate1
1: Set-Plate2
11: Shuttle Table1
12: Shuttle Table2


3.3. Piggyback Inspection Ready

[REQ_MEASURE] Piggyback 검사 위치 전달 (H → V)

S / F: 107 / 9
Action: Piggyback 검사 시작 전, 위치 정보를 전달.  


#	Parameter	Type / Format	Description / Value	비고 (Remark)
1	nDataID	INT	ID 값 사용	
2	nStatus	INT	상태 정보	
3	cData[0]	INT (in String)	검사 위치 정보	1: Station No.1
2: Station No.2
...


3.4. SET_COK

[REQ_MEASURE] COK 설정 요청 (H → V)

S / F: 107 / 11
Action: COK 파라미터 정보 및 PKG 정보 설정.  


#	Parameter	Type / Format	Description / Value	비고 (Remark)
1	nCmd	INT	1000 (SET_COK)	
2	nStatus	INT	상태 정보	
3	cData[0]	INT (in String)	PKG 수량 (1: 1PKG, 2: 2PKG)	
cData[1]	String	PKG1 Name	
cData[2]	String/Array	PKG1 C-Tray 규격 정보	[2][0]: PKG1 C-Tray X
[2][1]: PKG1 C-Tray Y


Revision History

Version	Date	Author	Description
1.0	2024	-	Initial specification document


📌 개요 및 목적

PLVI (Plate & Device Inspection) 영역에서 S107F5(REQ_MEASURE, 요청) 및 S107F6(REQ_RESULT, 응답) 메시지 쌍은 목적 및 동작 방식에 따라 크게 3가지 용도로 구분되어 사용되고 있습니다.  


PLVI 검사 요청 및 실행

방향: Handler → Vision (S107F5) / Vision → Handler (S107F6)
용도: Handler가 Vision PC로 대상 위치 및 PKG 정보를 전달하여 실제 PLVI 검사를 시작하도록 요청하는 기본 기능입니다.
주요 파라미터 (S107F5):
cData[0]: PLVI 검사 위치 (0: Set-Plate1, 1: Set-Plate2, 11: Shuttle Table1 등)
cData[1]: PKG Name
cData[2]: C-Tray 규격 정보 (X, Y 수량)
cData[3]: 개별 Device 존재 유무 정보 (0: 없음, 99: 존재)
응답 내용 (S107F6):
Vision에서 검사 수행 후 전체 결과(OK/NG) 및 개별 Pocket/Device별 상태(Pocket Leave, Double, Missing, Mismatch 등)를 반환합니다.
PLVI 검사 결과 재요청 (결과 조회)

방향: Handler → Vision (S107F5) / Vision → Handler (S107F6)
용도: 이미 실행되었거나 진행된 PLVI 검사에 대해 검사 결과 값만을 다시 요청하여 가져오는 폴링/조회 용도입니다.
차이점: 1번(검사 시작)과 달리 파라미터에 별도의 위치나 PKG 상세 정보를 실어 보내지 않고, 기존 nDataID 기준 결과 조회 목적으로 요청합니다.
응답 내용 (S107F6):
동일한 S107F6 규격으로 해당 요청 ID에 대한 검사 결과 및 에러 상태(cData[0]), 전체 판정(cData[1]), 개별 Device 결과(cData[3])를 응답합니다.
PLVI 검사 취소 (Inspection Cancel)

방향: Handler → Vision (S107F5) / Vision → Handler (S107F6)
용도: 시퀀스 중단, 에러 발생 또는 작업 변경 등으로 인해 대기 중이거나 진행 예정인 PLVI 검사 명령을 취소할 때 사용됩니다.
주요 파라미터 (S107F5):
cData[0]: 취소하려는 대상 PLVI 검사 위치 정보
응답 내용 (S107F6):
Vision이 취소 요청을 접수하고 에러 코드 또는 상태값(cData[0])을 통해 취소 처리 완료 여부를 응답합니다.


💡 요약 비교

구분	S107F5 요청 목적	S107F5 주요 파라미터	S107F6 응답 주요 내용
① 검사 요청	PLVI 검사 수행 위치, PKG명, C-Tray 정보, Device 유무	cData[0]: 위치
cData[1]: PKG
cData[2]: C-Tray
cData[3]: Device 유무	전체/개별 Device 검사 판정 결과
② 결과 요청	검사 결과 조회	nDataID 중심	cData[0]: 에러 코드
cData[1]: 전체 판정
cData[3]: 개별 Device 결과
③ 검사 취소	해당 위치 검사 취소	cData[0]: 취소 대상 위치	취소 처리 결과 (Status/Error)


📊 S107F7 / S107F8 활용 사례

1. Orientation Check (A1 Index Mark 검사)

목적: PKG/Device가 올바른 방향으로 놓여 있는지(A1 Index Mark 위치) 검사합니다.
요청 (S107F7, H → V):
cData[0]: 기대하는 Index Mark 방향 (0: UP_LEFT, 1: UP_RIGHT, 2: DOWN_RIGHT, 3: DOWN_LEFT)
cData[1]: 검사 대상 위치 (0: Set-Plate1, 1: Set-Plate2, 11: Shuttle Table1 등)
응답 (S107F8, V → H):
cData[0]: 에러 상태 (카메라 연결, 타임아웃, 티칭 데이터 등)
cData[1]: 최종 판정 결과 ('0': OK / '1': NG)
cData[2]: 상세 내용 (0: Missing, 1: Ball Inspection 등 예비 기능)

2. COK ID (Change of Kit ID / 모델 정보 확인)

목적: 장비의 기종 변경(Change Over Kit) 시 사용되는 COK 인덱스 또는 PKG 사양(ID)이 Vision과 Handler 간에 일치하는지 확인 및 전달합니다.
요청 (S107F11, H → V):
nCmd: 1000 (SET_COK)
cData[0]: PKG 수량 (1: 1PKG, 2: 2PKG)
cData[1]: PKG1 Name (모델명/ID)
cData[2]: PKG1 C-Tray 규격 (X/Y 크기)
응답 (S107F12, V → H):
cData[0]: 에러 설명 (0: BUSY, 1: CAM_DISCONNECT, 5: NO_TEACHING_DATA)
cData[1]: 설정 처리 결과 ('0': OK / '1': NG)

3. Piggyback Inspection Ready

목적: Piggyback 검사 수행 전, Vision 카메라가 검사할 위치(Station) 정보를 사전 전달합니다.
요청 (S107F9, H → V):
cData[0]: 검사 위치 정보 (1: Station No.1, 2: Station No.2 …)
응답 (S107F10, V → H):
cData[0]: 에러 설명 (0: BUSY, 1: CAM_DISCONNECT 등)
cData[1]: 준비/검사 결과 판정 ('0': OK / '1': NG)

4. VAT (Vision Alignment / Teaching 또는 검사 예비/부가 검사)

목적: Vision Alignment, Teaching, 또는 추가 옵션 검사(Ball Inspection/Missing 검사 등)의 처리 및 동기화에 활용됩니다.
특징: S107F8 응답 파라미터 중 cData[2] 영역에 'Ball Inspection 추가 기능용 예비' 또는 'Missing' 항목이 지정되어 있어, Alignment/Ball 관련 데이터를 주고받을 수 있도록 확장 구조로 설계되었습니다.


📝 종합 분석 요약

이 프로토콜 구조는 동일/유사한 Message Frame (S107 계열)을 유지하면서, 내부에 들어가는 명령어 코드는 물론 위치/상태 파라미터(cData)의 정의를 유연하게 변경하여 [방향 검사(Orientation)], [모델 변경(COK ID)], [위치 지정(Piggyback)], [부가 보정/검사(VAT/Ball)] 등 다양한 목적에 맞게 재사용(Polymorphic)하도록 다변화된 사양입니다.  
