# F-001: 시퀀스 실행 API (`RunSequence`)

- 기능 ID: F-001  
- 제목: 시퀀스 실행 API (`VatCorrectionEngine::RunSequence`)  
- 분류: 엔진 / 서비스  
- 우선순위: High  
- 담당(Owner): (작성 시 기입)  
- 참조 파일: `DVH_Library/include/VatCorrectionEngine.h`, `DVH_Library/src/VatCorrectionEngine.cpp`, `DVH_Library/include/SequenceBuilderBase.h`, `DVH_Library/include/AsyncSequenceRunner.h`, `DVH_Library/include/VAT_Context.h`

## 설명
UI 또는 상위 시스템에서 전달된 시퀀스 이름으로 검사 시퀀스를 생성·실행한다. 엔진은 `SequenceBuilder`로 `IVatSequence`를 생성하고 `AsyncSequenceRunner`에 위임하여 워커 스레드에서 비동기 실행을 시작한다. 실행 중 상태·이벤트는 `VAT_Context`와 `IVatObserver`를 통해 노티파이 된다.

## 전제 조건
- `VatCorrectionEngine`이 초기화되어 있으며 `m_pCtx`(유효한 `VAT_ContextPtr`)가 설정됨.
- `m_pBuilder`(SequenceBuilder)가 설정되어 있음.
- `VAT_Context`에 적절한 `IDataRepository` 및 `IAsyncVisionProcessor`가 주입되어 초기화 상태(필요시)임.
- 현재 엔진 상태가 `Idle` 또는 `Stopped` 여야 함(동시 실행 미허용 시).

## 입력
- `sequenceName: std::string` — 실행할 시퀀스 타입/이름.

## 출력
- 반환값: `bool` — 시퀀스 실행을 정상적으로 시작했는지 여부(시작 실패 시 false).
- 부수효과: `sequence_runs`에 run row 생성(또는 `CreateSequenceRun` 호출), `AsyncSequenceRunner`에서 비동기 실행 시작, UI/Observer에 시작 이벤트 발행.

## 관련 인터페이스
- `SequenceBuilderBase::BuildSequence(const std::string&, VAT_ContextPtr, VatActuatorPtr) -> IVatSequencePtr`
- `AsyncSequenceRunner::Start(IVatSequencePtr, VAT_ContextPtr, VatActuatorPtr)`
- `IDataRepository::CreateSequenceRun(...)`, `IDataRepository::UpdateSequenceRunStatus(...)`
- `IVatObserver` 콜백(시작/진행/완료/에러)

## 주요 시나리오 (Main Flow)
1. 엔진 상태 확인(이미 실행 중이면 실패 반환).
2. `m_pBuilder->BuildSequence(sequenceName, m_pCtx, m_actuator)` 호출하여 `IVatSequence` 생성.
3. `IDataRepository::CreateSequenceRun`으로 run 레코드 생성(가능하면 runId 획득).
4. `m_pRunner` 존재 여부 확인. 없으면 기본 `AsyncSequenceRunner` 생성 및 설정.
5. `m_pRunner->Start(seq, m_pCtx, m_actuator)` 호출로 비동기 실행 시작.
6. 엔진 상태를 `Running`으로 전환, `IVatObserver`에 시작 이벤트 전송.
7. 즉시 true 반환(단, 시작 실패 시 false).

## 예외/오류 처리
- 빌더 누락 또는 `BuildSequence` 실패 → false 반환, `VAT_Context::SetLastError`와 `IVatObserver`에 오류 통지.
- `CreateSequenceRun` 실패 → 로그 및 UI 경고, 실행 여부는 정책에 따라 진행(권장: 실행 시작 전 필수 실패 시 중단).
- `m_pRunner->Start` 실패 → rollback(생성된 run row 상태를 실패로 갱신) 및 false 반환.
- 모든 실패 경로는 적절한 로그와 `SetLastError`로 기록.

## 동시성 고려
- `RunSequence` 진입부에서 엔진 상태 변경은 뮤텍스/상태머신으로 원자적 처리.
- `VAT_Context`와 `IDataRepository` 호출은 스레드 안전 규약을 준수해야 함.
- 동시 다중 `RunSequence` 호출은 기본적으로 거부(또는 큐잉 정책 명시).

## 수락 기준 (Acceptance Criteria)
- 정상 환경: `RunSequence("Load1")` 호출 시 즉시 true 반환하고 `sequence_runs`에 신규 레코드가 생성되며 `AsyncSequenceRunner`가 워커 스레드에서 시퀀스를 실행 시작한다.
- 실패 환경: 빌더 없거나 빌드 실패 시 false 반환, `sequence_runs`에 미등록, `VAT_Context::GetLastError()`로 오류 확인 가능.
- 시작 실패(런너 실패) 시 DB에 `status="failed"` 또는 동등한 상태로 업데이트 됨.

## 테스트 케이스 (권장)
1. 단위: `m_pBuilder`를 모킹하여 정상 시나리오에서 `RunSequence`가 true를 반환하고 `m_pRunner->Start`가 호출되는지 검증.
2. 단위: 빌더가 nullptr일 때 false 반환 및 에러 세팅 검증.
3. 통합: `SqliteDataRepository` + `VisionMemoryProcessor`로 end-to-end 시나리오 실행, `sequence_runs`의 상태 변화 확인.
4. 오류: `CreateSequenceRun` 실패 시 `RunSequence` 거부 동작/로그 검증.
5. 동시성: 다중 스레드에서 연속 호출 시 두 번째 호출이 거부되는지 확인.

## 리스크 및 의존성
- `SequenceBuilder` 구현의 다양성: 빌드 실패 시 엔진 동작 정책 필요(중단 vs 경고).
- DB IO 지연/실패가 실행 경로에 영향을 줄 수 있음 — 트랜잭션 및 타임아웃 정책 필요.
- 비전 초기화 미완료 상태에서 시퀀스가 시작되지 않도록 검사 필요.

## 구현 권장사항 / 메모
- `RunSequence`는 "시작 요청 수락"을 의미하며 실제 작업은 `AsyncSequenceRunner`가 수행. 즉시 반환하도록 설계.
- 생성된 runId는 `VAT_Context` 또는 시퀀스에 주입하여 후속 Task들이 결과를 기록할 수 있게 함.
- 엔진 상태 전이는 반드시 내부 상태머신(예: `Idle`→`Running` 등)을 통해 수행하고 외부 동시 접근을 방지.
