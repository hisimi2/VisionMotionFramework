# DVH_VAT 기능명세서 템플릿
(프로젝트: DVH_VAT)  
작성자:  
버전: 0.1  
작성일: YYYY-MM-DD

---

## 1. 문서 목적
이 문서는 `DVH_VAT` 프로젝트의 기능 요구사항(Functional Requirements)과 수락기준을 정리하기 위한 템플릿입니다. 각 기능 항목은 구현, 테스트, 검증에 필요한 최소한의 정보를 포함하도록 구성되어 있습니다.

---

## 2. 범위(Scope)
- 포함: 비전 기반 검사 시퀀스 실행, 하드웨어 제어(모션/조명), 결과 저장(이미지/파라미터), UI 통지
- 제외: 외부 ERP/기술지원 시스템 통합(명시 시 별도 문서화)

---

## 3. 용어 정리(Glossary)
- `VAT_Context` : 런타임 공유 상태(Blackboard)
- `VatCorrectionEngine` : 시퀀스 엔진 진입점
- `IVatSequence` / `ITaskStep` : 시퀀스/태스크 인터페이스
- `IAsyncVisionProcessor` / `IVisionEventHandler` : 비전 통신 인터페이스
- `IDataRepository` : 영구 저장소 인터페이스
- `AsyncSequenceRunner` : 시퀀스 비동기 실행기

---

## 4. 기능 템플릿 (Feature Template)
각 기능은 아래 템플릿에 따라 기술하세요.

- 기능 ID: F-XXX
- 제목(Title): 한 줄 요약
- 분류(Category): 인터페이스 / 컴포넌트 / 시퀀스 / 저장소 등
- 우선순위(Priority): High / Medium / Low
- 담당(Owner):
- 설명(Description): 기능의 목적과 동작 개요(한두 문단)
- 전제 조건(Preconditions): 실행 전에 만족해야 할 조건
- 입력(Input): 입력 파라미터 / 트리거 이벤트
- 출력(Output): 반환값 / 부수효과(파일, DB, 메시지)
- 관련 인터페이스(Interfaces):
  - 예: `IAsyncVisionProcessor::RequestMeasureAsync(const StringMap&)`
  - 예: `IDataRepository::SaveImage(const std::string&, const std::vector<uint8_t>&, std::string&)`
- 주요 시나리오(Main Flow): 단계별 흐름(간단한 순서)
- 예외/오류 처리(Error Handling): 실패 시 행동, 복구 절차
- 동시성 고려(Concurrency): 락/스레드/타임아웃 관련 요구
- 보안/권한(Security): 접근 제어 필요 여부
- 수락 기준(Acceptance Criteria): 테스트로 검증 가능한 조건(정량적 포함)
- 테스트 케이스(Test Cases): 단위/통합/회귀 테스트 항목
- 리스크 및 의존성(Risks & Dependencies)
- 참고(Notes)

---

## 5. 인터페이스 계약(Interface Contracts)
이 섹션에는 각 인터페이스의 요약과 필수 시그니처, 호출 규약을 기입합니다.

### 5.1 `IAsyncVisionProcessor` / `IVisionEventHandler`
- 목적: 비전 장치와 비동기 통신
- 필수 메서드(예)
  - `VisionCom::VisionStatus Initialize(const VisionConnectionConfig& config)`
  - `void Disconnect()`
  - `bool RequestMeasureAsync(const StringMap& params)`
  - `void OnMeasure(const ByteArray& body)` (콜백 형태)
- 호출 규약: 모든 `Request*Async`는 비동기이며, 결과는 `On*` 콜백으로 전달된다.

### 5.2 `IDataRepository`
- 목적: 파라미터/이미지/검사결과 영구 저장
- 필수 메서드(예)
  - `StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value)`
  - `StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue)`
  - `StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath)`
- 트랜잭션 규약: `SqliteDataRepository`는 트랜잭션을 사용하며, 실패 시 롤백이 보장되어야 함.

### 5.3 `IVatSequence` / `ITaskStep`
- 목적: 시퀀스/태스크 실행 모델
- 필수 시그니처(예)
  - `void Start(VAT_ContextPtr ctx)`
  - `bool Poll()` / `StepState GetState()`
  - `void Stop()`
- 동작 규약: `ITaskStep`은 `NonBlockingStepBase`를 상속하여 비동기 상태 머신으로 동작.

(필요한 추가 인터페이스를 동일 형식으로 기술)

---

## 6. 데이터 모델 / 스토리지
- `VatRunParams` : `visionParams`(StringMap), `seqParams`(StringMap), `visionPositions`(vector<`VisionPosition`>)
- `VisionPosition` : `pos: vector<double>`, `locateId: int`, `visionRequestId: int`
- DB 스키마(요약): `params`, `images`, `sequence_runs`, `z_focus_points`, `InspInitPos`, 등
- 이미지 저장 전략(예): temp 파일 -> DB INSERT(temp path) -> rename -> DB UPDATE(final path)

---

## 7. 동작 시나리오(대표 Use Case)
(각 시나리오를 상세화해서 작성)
### 7.1 시퀀스 실행 (RunSequence)
- 트리거: UI에서 사용자가 시퀀스 실행 요청
- 흐름:
  1. `VatCorrectionEngine::RunSequence(sequenceName)` 호출
  2. `SequenceBuilder`가 `IVatSequence` 생성
  3. `AsyncSequenceRunner`가 워커 스레드에서 시퀀스 실행
  4. 각 `ITaskStep`는 `VAT_Context`로 비전/액추에이터 호출
  5. 결과는 `IDataRepository`에 저장; UI는 `IVatObserver`로 통지
- 수락기준: 시퀀스가 모든 Task를 성공적으로 수행하고 `sequence_runs`에 완료 상태 기록

---

## 8. 상태 및 에러 모델
- `VatCorrectionEngine` 상태머신: `Idle`, `Running`, `Paused`, `Stopped`, `Completed`, `Error`
- 에러 등급: Warning / Recoverable / Fatal
- 에러 전파: Task 내부 에러 → 시퀀스 중단 또는 지정 보정 루틴 → `VAT_Context::SetLastError` 및 `IVatObserver` 통지

---

## 9. 동시성·성능 요구
- `VAT_Context`의 공유 데이터는 반드시 `MutexType`으로 보호
- 비전 응답 대기 타임아웃: default 5,000 ms(조정 가능)
- 이미지 저장은 비동기(가능 시 `AsyncDataRepository`) 권장

---

## 10. 테스트 항목
- 단위 테스트
  - `VisionMemoryProcessor`로 `ITaskStep` 단위 동작 검증
  - `SqliteDataRepository`의 transaction/SaveImage 경로 테스트
- 통합 테스트
  - `RunSequence` 전체 플로우 (모킹 하드웨어 + 실제 DB)
- 스트레스/동시성 테스트
  - 다중 시퀀스(동시 실행 시나리오), DB 동시접근

---

## 11. 수락 기준(예시)
- 기능 F-001(`RunSequence`) : 정상 입력으로 시퀀스가 100% 완료되면 `sequence_runs`에 `status=finished` 및 `finished_at`이 기록되어야 함.
- 기능 F-002(`SaveImage`) : `SaveImage` 호출 시 임시 파일이 정상 생성되고 최종 경로로 rename 후 DB에 업데이트 되어야 함.

---

## 12. 보안·운영·배포 노트
- DB 파일과 이미지 저장 경로에 대한 파일 권한을 확인할 것.
- 마이그레이션 SQL(`migration_v1.sql`)은 배포 시점에 함께 제공.
- 로깅 수준은 운영 시 INFO, 디버깅 시 DEBUG로 변경 가능.

---

## 13. 추적성(Traceability)
- 기능 항목에 대해 Git 이슈/PR 번호와 연결할 것.
- 각 구현 파일과 테스트 케이스를 기능 ID에 매핑.

---

## 14. 템플릿 예시 (완성된 한 항목)
- 기능 ID: F-001
- 제목: 시퀀스 실행 API (`RunSequence`)
- 분류: 엔진 / 서비스
- 우선순위: High
- 담당: 팀A
- 설명: UI 요청으로 시퀀스를 실행하고 결과를 저장/통지한다.
- 전제조건: `VAT_Context`에 유효한 `IDataRepository` 및 `IAsyncVisionProcessor`가 주입되어 있어야 함.
- 입력: `sequenceName: std::string`
- 출력: `bool`(성공/실패), 내부적으로 `sequence_runs` row 생성 및 상태 업데이트
- 인터페이스: `VatCorrectionEngine::RunSequence(std::string)`
- 주요 시나리오: (위 7.1 참고)
- 수락기준: 실행 완료 시 `sequence_runs`에 `status=finished`.
- 테스트케이스: 모킹 비전 + `SqliteDataRepository`로 end-to-end 테스트
- 리스크: 비전 응답 지연 시 타임아웃 정책 필요
- 메모: 로그 레벨 INFO 이상에서 상태 기록

---

## 부록
- 변경 이력 표(Version, Date, Author, Summary)
- 참고문서: 코드 위치(예: `DVH_Library/include/VAT_Context.h`, `DVH_Library/include/IVisionProcessor.h`, `DVH_Library/include/IDataRepository.h`)

---
