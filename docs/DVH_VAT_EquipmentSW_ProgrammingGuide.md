# `DVH_VAT.dll` 연동 프로그래밍 가이드 for `EquipmentSW`

## 1. 목적

이 문서는 `EquipmentSW` 프로젝트에서 `DVH_VAT.dll` 기반 VAT 시퀀스를 시작, 중지, 확장하는 방법을 설명합니다.  
대상은 다음과 같습니다.

- UI에서 VAT 시퀀스를 실행하려는 개발자
- `Load1` 전용 시퀀스/Task를 추가하려는 개발자
- Vision / Repository / Actuator 연동 지점을 파악하려는 개발자

---

## 2. 현재 구조 요약

현재 구조는 크게 아래 흐름으로 동작합니다.

1. UI(`CEquipmentSWDlg`)가 엔진 어댑터를 소유합니다.
2. UI가 Strategy 타입을 지정하여 시퀀스를 시작합니다.
3. Strategy가 Builder / Repository / Vision Processor / 파라미터를 준비합니다.
4. Builder가 `VatSequence`를 생성하고 Task들을 순서대로 추가합니다.
5. 각 Task는 `NonBlockingStepBase`를 상속하여 상태 기반으로 동작합니다.
6. Task는 `VAT_Context`를 통해 Vision / DB / 파라미터에 접근하고, `IVatActuator`를 통해 장비를 제어합니다.

---

## 3. 주요 구성요소

### 3.1 UI 계층

`EquipmentSW`에서는 다이얼로그가 엔진 어댑터를 멤버로 소유합니다.

- 클래스: `CEquipmentSWDlg`
- 멤버: `DVH_VAT::CVatEngineUiAdapter m_engine`

현재 UI 코드에서 확인되는 핵심 사용 예는 다음과 같습니다.

- 초기화 시 소유자 등록
- 버튼 클릭 시 시퀀스 시작
- 종료 시 시퀀스 정지
- 비전 결과 메시지 수신

예시 흐름:

```cpp
// UI 코드 예시
void CEquipmentSWDlg::OnStartSequence()
{
    m_engine.StartSequence("Load1ZFocus");
}

void CEquipmentSWDlg::OnStopSequence()
{
    m_engine.StopSequence();
}

void CEquipmentSWDlg::OnVisionResultReceived(const VisionResult& result)
{
    // 비전 결과 처리
}
```

---

### 3.2 Strategy 계층

Strategy는 "어떤 시퀀스를 어떤 구성으로 실행할지"를 결정합니다.

예: `CLoad1ZFocusSequenceStrategy`

이 Strategy는 다음 역할을 수행합니다.

- 시퀀스 이름 제공
- Builder 생성
- 실행 파라미터 설정
- 상위 전략 기반 클래스에서 Repository / Vision Processor 생성

현재 확인된 구현 특징:

- `GetSequenceName() -> "Load1ZFocus"`
- `CreateBuilder() -> CLoad1ZFocusSequenceBuilder`
- `ConfigureParams()`에서 `CameraIndex`, `HandID`, `PkgID`, 비전 포인트 등을 설정

전략 코드 예시:

```cpp
// 전략 클래스 예시
class CLoad1ZFocusSequenceStrategy : public CBaseSequenceStrategy
{
public:
    virtual CString GetSequenceName() const override { return "Load1ZFocus"; }
    virtual CBaseSequenceBuilder* CreateBuilder() const override { return new CLoad1ZFocusSequenceBuilder(); }

protected:
    virtual void ConfigureParams()
    {
        // 카메라 인덱스, 핸드 ID, 패키지 ID 설정
        AddParameter("CameraIndex", 1);
        AddParameter("HandID", "LeftHand");
        AddParameter("PkgID", "PackageA");

        // 비전 포인트 추가
        AddVisionPoint("PickUpPoint", CVisionPoint(100, 200));
        AddVisionPoint("PlacePoint", CVisionPoint(300, 400));
    }
};
```

---

### 3.3 Builder 계층

Builder는 실제 시퀀스 객체를 만들고, 실행할 Task 목록을 조립합니다.

예: `CLoad1ZFocusSequenceBuilder`

현재 구현 예:

```cpp
// 빌더 클래스 예시
class CLoad1ZFocusSequenceBuilder : public CBaseSequenceBuilder
{
protected:
    virtual void BuildSequence() override
    {
        // 시퀀스 생성 및 태스크 추가
        AddTask(new CInitializeTask());
        AddTask(new CMoveToPickUpTask());
        AddTask(new CGrabPackageTask());
        AddTask(new CMoveToPlaceTask());
        AddTask(new CReleasePackageTask());
        AddTask(new CReturnToHomeTask());
    }
};
```

즉, 시퀀스 실행 순서는 Builder가 결정합니다.

---

### 3.4 Task 계층

Task는 실제 장비 동작 단위입니다.

예:

- `CLoad1MoveToStartPositionTask`
- `CLoad1PerformFocusScanningTask`
- `CLoad1FindAlignPosTask`

Task는 공통적으로 `DVH_VAT::NonBlockingStepBase`를 상속합니다.

`NonBlockingStepBase`가 제공하는 핵심 기능:

- 최초 1회 `OnInitialize()` 호출
- 반복 폴링 시 `OnPoll()` 호출
- stop 요청 체크
- 예외 처리
- 상태 전이 및 timeout 관리

파생 클래스 구현 포인트:

```cpp
// 파생 클래스 예시
class CLoad1MoveToStartPositionTask : public DVH_VAT::NonBlockingStepBase
{
protected:
    virtual void OnInitialize() override
    {
        // 초기화 작업
    }

    virtual void OnPoll() override
    {
        // 폴링 작업
        // 조건에 따라 상태 전이 또는 stop 요청
    }
};
```

---

### 3.5 Context 계층

`VAT_Context`는 시퀀스 실행 중 공유되는 런타임 컨텍스트입니다.

주요 용도:

- 실행 파라미터 접근
- stop/error 상태 관리
- Vision Processor 접근
- Repository 접근
- Task 간 데이터 공유

Task는 직접 모든 외부 객체를 소유하지 않고, `VAT_Context`를 통해 필요한 자원에 접근합니다.

---

### 3.6 Vision / Repository 계층

현재 메모리 기반 전략(`MemorySequenceStrategy`)에서 확인되는 기본 구성은 다음과 같습니다.

#### Repository
- 구현체: `DVH_VAT::SqliteDataRepository`
- 생성 예:
  - DB 경로: `Data\\VAT_DATABASE.db`
  - 이미지 경로: `Data\\Images`

#### Vision
- 구현체: `DVH_VAT::VisionMemoryProcessor`
- 초기화 예:
  - IP: `127.0.0.1`
  - Port: `8080`
  - Timeout: `3000`
- 수신 스레드 시작:
  - `InitializeRecvThread()`

---

## 4. `EquipmentSW`에서 VAT 시퀀스 시작 절차

## 4.1 헤더 포함

UI 또는 시작 지점에서 사용할 Strategy 헤더를 포함합니다.

```cpp
#include "Load1ZFocusSequenceStrategy.h"
```

## 4.2 엔진 어댑터 소유

다이얼로그 또는 컨트롤러 클래스에서 엔진 어댑터를 멤버로 소유합니다.

```cpp
// 엔진 어댑터 멤버 변수 예시
DVH_VAT::CVatEngineUiAdapter m_engine;

```
권장 이유:

- UI 생명주기와 엔진 생명주기를 맞추기 쉽습니다.
- 종료 시 stop 호출 위치가 명확합니다.

---

## 4.3 소유자 등록

초기화 시 엔진에 UI 소유자를 등록합니다.

m_engine.SetOwner(this);

이 단계는 UI 메시지 전달 및 콜백 연결에 필요합니다.

---



## 4.4 시퀀스 시작

시퀀스를 시작할 Strategy 클래스의 인스턴스를 생성하고, 엔진 어댑터를 통해 시퀀스를 시작합니다.


```cpp
// 시퀀스 시작
CLoad1ZFocusSequenceStrategy* pStrategy = new CLoad1ZFocusSequenceStrategy();
m_engine.StartSequence(pStrategy);
```

## 4.5 시퀀스 중지

시퀀스를 중지할 경우:

```cpp
m_engine.StopSequence();
```

---

권장 위치:

- `OnDestroy()`
- 작업 중단 버튼 핸들러
- 예외 복구 루틴

---

## 5. UI 메시지 처리

현재 UI는 등록 메시지 방식으로 비전 결과를 받습니다.

핵심 포인트:

- 메시지 ID는 `DVH_VAT::CVatEngineUiAdapter::GetVisionResultMsgId()`에서 획득
- `ON_REGISTERED_MESSAGE(...)`로 연결
- `WPARAM`에는 힙에 할당된 `boost::shared_ptr<DVH_VAT::VisionResultPayload>*`가 전달됨
- 수신 측에서 복사 후 즉시 delete 필요

현재 패턴:

typedef boostshared_ptr<DVH_VATVisionResultPayload>* HeapSpPtr;
HeapSpPtr pHeapSp = reinterpret_cast<HeapSpPtr>(wParam); if (!pHeapSp) return 0;
boostshared_ptr<DVH_VATVisionResultPayload> payload = *pHeapSp; delete pHeapSp;

### 주의
이 포인터는 수신 측이 해제해야 하므로 누락 시 메모리 누수가 발생합니다.

---

## 6. 새 Strategy 추가 방법

예: `Load1`의 다른 시퀀스를 추가하는 경우

### 6.1 클래스 생성

프로젝트 규칙에 맞춰 Strategy 클래스를 만듭니다.

- 네임스페이스: `VAT_LOAD1::Strategies`
- 클래스명: `CLoad1{SequenceName}SequenceStrategy`

예:
- `CLoad1HandPitchSequenceStrategy`

### 6.2 구현 항목

최소 구현 항목:

- `GetSequenceName()`
- `CreateBuilder()`
- `ConfigureParams()`

예시 패턴:
class CLoad1ExampleSequenceStrategy : public MemorySequenceStrategy { public: std::string GetSequenceName() const { return "Load1Example"; }
SequenceBuilderPtr CreateBuilder()
{
    return boost::make_shared<CLoad1ExampleSequenceBuilder>();
}

void ConfigureParams(VatContextPtr& ctx)
{
    VatRunParams params;
    SetParam(params, "CameraIndex", 6);
    ctx->SetVatRunParams(params);
}
};

---

## 7. 새 Builder 추가 방법

Builder는 Task 조합 책임만 가지는 것이 좋습니다.

### 7.1 클래스 생성 규칙

- 경로: `EquipmentSW/DVH_VAT/Load1/Sequences/`
- 클래스명: `CLoad1{SequenceName}SequenceBuilder`

### 7.2 구현 원칙

- `VatSequence` 생성
- Task 순서 정의
- Task별 책임은 Builder에 넣지 않음

예시:
class CLoad1ExampleSequenceBuilder : public DVH_VATSequenceBuilderBase { protected: DVH_VATIVatSequence* BuildSequence(stdstring sequenceName) { DVH_VATVatSequence* seq = new DVH_VATVatSequence(sequenceName); seq->AddTask(DVH_VATTaskStepPtr(new VAT_LOAD1TaskCLoad1MoveToStartPositionTask())); seq->AddTask(DVH_VATTaskStepPtr(new VAT_LOAD1Task::CLoad1FindAlignPosTask())); return seq; } };


---

## 8. 새 Task 추가 방법

## 8.1 클래스 생성 규칙

현재 프로젝트 규칙:

- 권장명: `C{Unit}{TaskName}Task`
- 예: `CLoad1FindAlignPosTask`

경로 권장:

- `EquipmentSW/DVH_VAT/Load1/Tasks/`

---

## 8.2 상속 구조

Task는 `DVH_VAT::NonBlockingStepBase`를 상속합니다.

class CLoad1FindAlignPosTask : public DVH_VAT::NonBlockingStepBase

---

## 8.3 최소 구현 항목

- 생성자 / 소멸자
- `GetName()`
- `OnInitialize()`
- `OnPoll()`
- 필요 시 상태별 핸들러 함수

예시 구조:

class CLoad1ExampleTask : public DVH_VAT::NonBlockingStepBase { public: CLoad1ExampleTask(); virtual ~CLoad1ExampleTask();
std::string GetName() const
{
    return "ExampleTask";
}
protected: virtual void OnInitialize(DVH_VATVAT_Context& ctx, DVH_VATIVatActuator* actuator); virtual DVH_VATStepResult OnPoll(DVH_VATVAT_Context& ctx, DVH_VAT::IVatActuator* actuator); };



---

## 8.4 상태 머신 방식 구현

`NonBlockingStepBase` 기반 Task는 내부 enum 상태를 두고 `OnPoll()`에서 분기하는 방식이 적합합니다.

예:

- `SUBSTEP_MOVE_SAFE_Z`
- `SUBSTEP_VISION_REQUEST`
- `SUBSTEP_VISION_WAIT`
- `SUBSTEP_DB_WRITING_RESULT`
- `SUBSTEP_DONE`
- `STATE_ERROR`

권장 패턴:

- `OnInitialize()`에서는 초기 상태 진입만 수행
- 실제 장비 진행과 완료 판정은 `OnPoll()`에서 처리
- 복잡한 로직은 `HandleXxx()` 함수로 분리

---

## 8.5 timeout / error 처리

`NonBlockingStepBase`가 제공하는 유틸을 적극 사용합니다.

- `EnterState(newState)`
- `EnterStateWithTimeout(newState, timeoutMs)`
- `IsDeadlineExpired()`
- `SetErrorAndReturn(ctx, msg, nextState)`

권장 원칙:

- 장비 이동 대기
- 비전 응답 대기
- DB 기록 대기

위 3종류는 timeout을 반드시 둡니다.

---

## 9. Vision 연동 원칙

Task는 Vision 구현체를 직접 의존하지 말고 `VAT_Context`를 통해 접근하는 구조를 유지합니다.

권장 흐름:

1. Task가 Vision 요청 ID를 생성/기록
2. Vision 요청 전송
3. `SUBSTEP_VISION_WAIT` 상태로 전환
4. 응답 도착 여부를 poll
5. 응답 결과를 Context 또는 payload에서 해석
6. 필요 시 DB 저장

### 주의
- 요청 ID(`m_pendingRequestId_`, `m_visionRequestId`)를 명확히 구분합니다.
- 복수 요청이 가능한 Task는 현재 요청 ID와 대기 중 요청 ID를 분리합니다.

---

## 10. DB 저장 원칙

Repository도 직접 구현체에 의존하지 말고 `VAT_Context`를 통해 사용합니다.

권장 사항:

- Task는 "무엇을 저장할지"만 결정
- 실제 저장 대상 경로/DB 파일 구성은 Strategy 또는 Repository 생성부가 책임짐
- JSON 요약 문자열(`m_resultSummaryJson_`) 등은 Task 내부에서 만들어도 됨

---

## 11. Actuator / 장비 제어 원칙

Task는 장비 제어 시 `IVatActuator*`만 사용합니다.

즉:

- Task는 축 제어 구체 클래스에 직접 의존하지 않음
- `Load1` 전용 구현은 Adapter 쪽으로 숨김
- Task는 "이동 명령", "완료 확인", "조명 제어" 수준의 의미만 사용

이 구조를 유지해야 UI/Sequence/Task 계층과 실제 하드웨어 계층이 분리됩니다.

---

## 12. 구현 시 주의사항

### 12.1 `GetName()`은 레거시 식별자와 분리해서 생각할 것
현재 Task 예제들은 레거시 호환을 위해 짧은 이름을 유지합니다.

예:
- `FindAlignPos`
- `PerformFocusScanning`

즉, 클래스명과 런타임 식별자는 반드시 같지 않을 수 있습니다.

---

### 12.2 `override` 제거 정책 확인
현재 일부 코드에는 `v100` 호환을 위해 `override`가 제거되어 있습니다.  
새 코드 작성 시에도 기존 컴파일러 제약을 먼저 확인해야 합니다.

---

### 12.3 공용 헤더에서 서드파티 노출 최소화
가능하면 공용 헤더에 Boost 타입을 과도하게 노출하지 않습니다.  
다만 현재 구조상 `boost::shared_ptr`를 이미 사용하는 구간은 기존 스타일을 따릅니다.

---

### 12.4 stop 요청은 모든 Task가 존중해야 함
`NonBlockingStepBase::Execute()`는 `ctx.GetStopRequested()`를 먼저 검사합니다.  
Task 내부에서도 장시간 대기 루프를 만들지 말고 poll 기반으로 잘게 나누는 것이 안전합니다.

---

## 13. 신규 시퀀스 추가 체크리스트

- [ ] `Strategies` 아래에 새 Strategy 클래스 추가
- [ ] `Sequences` 아래에 새 Builder 클래스 추가
- [ ] 필요한 Task 클래스 생성
- [ ] Builder에서 Task 순서 등록
- [ ] `ConfigureParams()`에 실행 파라미터 정의
- [ ] UI에서 `StartVatSequence<새Strategy>()` 호출
- [ ] 종료 시 `StopVatSequence()` 호출 확인
- [ ] Vision 결과 메시지 처리 누수 여부 확인
- [ ] timeout / error 경로
