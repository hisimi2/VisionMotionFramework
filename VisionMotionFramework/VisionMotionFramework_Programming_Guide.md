# VisionMotionFramework (VMF) 프로그래밍 가이드

## 목차
1. [소개](#1-소개)
2. [프로젝트 아키텍처 개요](#2-프로젝트-아키텍처-개요)
3. [핵심 인터페이스 이해하기](#3-핵심-인터페이스-이해하기)
4. [시퀀스(Sequence) 시스템](#4-시퀀스sequence-시스템)
5. [태스크(Task) 작성 가이드](#5-태스크task-작성-가이드)
6. [데이터 저장소(Repository)](#6-데이터-저장소repository)
7. [비전 프로세서(Vision Processor)](#7-비전-프로세서vision-processor)
8. [Orchestrator와 Strategy 패턴](#8-orchestrator와-strategy-패턴)
9. [비동기 실행(AsyncExecutor)](#9-비동기-실행asyncexecutor)
10. [로깅(Logging) 시스템](#10-로깅logging-시스템)
11. [코딩 규칙 및 Best Practice](#11-코딩-규칙-및-best-practice)
12. [새로운 장비 시퀀스 개발하기 (예제)](#12-새로운-장비-시퀀스-개발하기-예제)

---

## 1. 소개

**VisionMotionFramework (VMF)**는 비전 검사 장비의 시퀀스 제어를 위한 C++ 프레임워크입니다.  
VMF는 **MFC Extension DLL**로 제공되며, 다음과 같은 핵심 기능을 제공합니다:

- **시퀀스 기반 작업 실행**: 장비의 동작을 단계(Task) 단위로 나누어 순차 실행
- **비동기 실행**: `AsyncExecutor`를 통한 비차단 시퀀스 실행
- **데이터 저장소 추상화**: File 기반 / SQLite 기반 저장소를 인터페이스로 추상화
- **비전 프로세서 통합**: 비전 시스템(SECS/GEM)과의 통신 처리
- **Strategy 패턴**: 장비별 동작을 전략(Strategy) 객체로 캡슐화

### 기술 스택 요약

| 항목 | 내용 |
|------|------|
| 언어 | C++11/14 (std::thread, std::mutex, std::unique_ptr 등 표준 라이브러리 사용) |
| 프레임워크 | MFC Extension DLL |
| 저장소 | SQLite3 (선택), File System |
| 통신 | SECS/GEM (VisionComm 라이브러리) |
| 빌드 | Visual Studio (Visual C++) |

---

## 2. 프로젝트 아키텍처 개요

### 2.1 주요 디렉터리 구조

```
VisionMotionFramework/
├── include/            # 공개 헤더 파일
│   ├── VMF_API.h       # DLL Export/Import 매크로
│   ├── Types.h         # 공통 타입 및 using 선언
│   ├── CompatUtils.h   # 호환성 유틸리티 (디버그 출력, 로그 헬퍼)
│   ├── IDataRepository.h      # 데이터 저장소 인터페이스
│   ├── ISequence.h            # 시퀀스 인터페이스
│   ├── ITask.h                # 태스크 인터페이스
│   ├── IActuator.h            # 액추에이터(하드웨어) 인터페이스
│   ├── IVisionProcessor.h     # 비전 프로세서 인터페이스
│   ├── ISequenceStrategy.h    # 전략 인터페이스
│   ├── IResultSink.h          # 결과 수신 인터페이스
│   ├── IObserver.h            # 옵저버 인터페이스
│   ├── Sequence.h             # 시퀀스 구현 클래스
│   ├── SequenceBuilderBase.h  # 시퀀스 빌더 기본 클래스
│   ├── SequenceStrategyBase.h # 전략 기본 클래스
│   ├── NonBlockingTaskBase.h  # 비차단 태스크 기본 클래스
│   ├── Context.h              # 실행 컨텍스트
│   ├── RunController.h        # 실행 제어기
│   ├── AsyncExecutor.h        # 비동기 실행기
│   ├── Orchestrator.h         # 오케스트레이터
│   ├── FileDataRepository.h   # 파일 기반 저장소
│   ├── SqliteDataRepository.h # SQLite 기반 저장소
│   ├── RepositoryFactory.h    # 저장소 팩토리
│   ├── VisionProcessorBase.h  # 비전 프로세서 기본 클래스
│   ├── FileUtils.h            # 파일 유틸리티
│   ├── SaveUtils.h            # 저장 재시도 유틸리티
│   └── Logger.h               # 로거
├── src/                # 구현 소스 파일
│   ├── Sequence.cpp
│   ├── AsyncExecutor.cpp
│   ├── AsyncDataRepository.cpp
│   ├── Context.cpp
│   ├── RunController.cpp
│   ├── Orchestrator.cpp
│   ├── SqliteDataRepository.cpp
│   ├── FileDataRepository.cpp
│   ├── RepositoryFactory.cpp
│   ├── SequenceBuilderBase.cpp
│   ├── SequenceStrategyBase.cpp
│   ├── VisionProcessorBase.cpp
│   ├── Logger.cpp
│   └── SaveUtils.cpp
├── res/                # 리소스 파일
├── stdafx.h            # 미리 컴파일된 헤더
├── targetver.h         # Windows 버전 타겟
├── dllmain.cpp         # DLL 진입점
└── VisionMotionFramework.rc # 리소스 스크립트
```

### 2.2 아키텍처 다이어그램

```
┌─────────────────────────────────────────────────────────┐
│                    Orchestrator                          │
│  (시퀀스 생명주기 관리, Observer 통지)                    │
└──────────┬────────────────────────────────────┬──────────┘
           │                                    │
           ▼                                    ▼
┌──────────────────────┐          ┌──────────────────────┐
│   Strategy (전략)     │          │    IResultSink        │
│  - GetSequenceName()  │          │  (Observer에게 전달)   │
│  - CreateBuilder()    │          └──────────────────────┘
│  - CreateRepository() │
│  - CreateVisionProc() │
│  - ConfigureParams()  │
│  - Get/SetActuator()  │
└──────────┬────────────┘
           │
           ▼
┌─────────────────────────────────────────────────────────┐
│                   RunController                          │
│  (Builder → Sequence → AsyncExecutor.Start)              │
└──────────┬────────────────────────────────────┬──────────┘
           │                                    │
           ▼                                    ▼
┌──────────────────────┐          ┌──────────────────────┐
│   SequenceBuilder     │          │   AsyncExecutor       │
│  (Factory Method)     │          │  - 별도 스레드 실행   │
│   BuildSequence()     │          │  - Start/Stop/Abort   │
└──────────────────────┘          └──────────────────────┘
                                           │
                                           ▼
┌─────────────────────────────────────────────────────────┐
│                     Sequence                             │
│  Task[] → Task[].Execute(ctx, actuator)                  │
│  TR_KEEP / TR_NEXT / TR_PREV / TR_DONE / TR_ERROR       │
└─────────────────────────────────────────────────────────┘
```

---

## 3. 핵심 인터페이스 이해하기

### 3.1 ITask (작업 단위)

```cpp
// include/ITask.h
enum TaskResult {
    TR_KEEP,    // 현재 태스크 유지 (다음 폴링 주기에 재실행)
    TR_NEXT,    // 다음 태스크로 이동
    TR_PREV,    // 이전 태스크로 이동
    TR_DONE,    // 시퀀스 완료
    TR_ERROR    // 오류 발생 → 시퀀스 중단
};

class ITask {
public:
    virtual ~ITask() = default;
    virtual TaskResult Execute(Context& ctx, IActuator* actuator) = 0;
    virtual TaskResult SetErrorAndReturn(Context& ctx, const std::string& msg) = 0;
    virtual void Abort() = 0;
    virtual void EnterState(int newState) = 0;
    virtual std::string GetName() const = 0;
};
```

**TaskResult 반환 규칙:**
| 반환값 | 의미 | 동작 |
|--------|------|------|
| `TR_KEEP` | 작업 진행 중 | 설정된 폴링 간격 후 다시 Execute() 호출 |
| `TR_NEXT` | 작업 완료 | 다음 Task로 인덱스 증가 |
| `TR_PREV` | 이전 단계로 | 이전 Task로 인덱스 감소 |
| `TR_DONE` | 시퀀스 완료 | Execute()가 true 반환하며 종료 |
| `TR_ERROR` | 오류 발생 | Execute()가 false 반환하며 종료 |

### 3.2 ISequence (시퀀스)

```cpp
// include/ISequence.h
class ISequence {
public:
    virtual ~ISequence() = default;
    virtual bool Execute(Context& context, IActuator* actuator) = 0;
    virtual void Abort() = 0;
    virtual std::string GetSequenceName() const = 0;
    virtual std::string GetTaskName() const = 0;
    virtual void AddTask(TaskPtr step) = 0;
};
```

### 3.3 IActuator (하드웨어 제어)

```cpp
// include/IActuator.h
enum ActError {
    ActOk = 0,      // 정상
    ActWait,        // 동작 진행 중
    ActFail,        // 일반 실패
    ActInvalidParam, ActOutOfRange, ActSpeedLimit,
    ActNotReady,    ActBusy, ActTimeout,
    ActUnsafe,      ActZNotSafe, ActInterlock, ActCollisionRisk,
    ActNoParts,     ActHwError, ActCommError,
    ActMoveFail,    ActNotInPosition,
    ActStateMismatch
};

class IActuator {
public:
    virtual PitchType GetPitchType() = 0;
    virtual ActError IsReadyToMove() = 0;
    virtual ActError MoveZ(double targetZ) = 0;
    virtual ActError Move(std::vector<double> pos, PitchStatus action) = 0;
    virtual ActError isMoveZ(double targetZ) = 0;
    virtual ActError isMove(std::vector<double> pos, PitchStatus action) = 0;
    virtual std::vector<double> getPosition() = 0;
    virtual std::vector<double> getPulse() = 0;
    virtual int SetLightState(int camIndex, bool on) = 0;
    virtual int GetLightState(int camIndex, bool& outOn) = 0;
};
```

### 3.4 IDataRepository (데이터 저장소)

```cpp
// include/IDataRepository.h
enum StorageError {
    StorageSuccess = 0,
    StorageErrorWriteFailed,
    StorageErrorNotFound,
    StorageWriteFailed,
    StorageFileNotFound,
    StorageNotFound,
    StorageGeneral
};

class IDataRepository {
public:
    virtual ~IDataRepository() = default;

    // 기본 키-값 저장/로드
    virtual StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) = 0;
    virtual StorageError LoadParam(const std::string& recipe, const std::string& name, std::string& outValue) = 0;

    // 이미지 저장
    virtual StorageError SaveImage(const std::string& contextTag, const std::vector<uint8_t>& imageData, std::string& outPath) = 0;

    // 시퀀스 실행 요약 저장
    virtual StorageError SaveSequenceRun(const std::string& sequenceName, const std::string& summary) = 0;

    // 초기화/종료
    virtual StorageError Initialize() = 0;
    virtual StorageError Shutdown() = 0;

    // 확장 API (검사 결과 저장/로드)
    virtual StorageError CreateSequenceRun(...) = 0;
    virtual StorageError SaveZFocusPoint(...) = 0;
    virtual StorageError SaveZFocusResult(...) = 0;
    virtual StorageError SavePickerCamDistanceResult(...) = 0;
    virtual StorageError SaveCalibrationPosResult(...) = 0;
    virtual StorageError SaveHandPitchResult(...) = 0;
    virtual StorageError SaveTeachingResult(...) = 0;
    virtual StorageError UpdateSequenceRunStatus(...) = 0;
    virtual StorageError LoadInspInitPos(...) = 0;
    virtual StorageError LoadPickerCamDistance(...) = 0;
    virtual StorageError LoadHandPitch(...) = 0;
    virtual StorageError LoadTeachingResult(...) = 0;
    virtual StorageError LoadHandCamGroup(...) = 0;
    virtual StorageError LoadCamLocationGroup(...) = 0;
    virtual StorageError LoadLocationIdByName(...) = 0;
};
```

### 3.5 ISequenceStrategy (시퀀스 전략)

```cpp
// include/ISequenceStrategy.h
class ISequenceStrategy {
public:
    virtual ~ISequenceStrategy() = default;
    virtual std::string GetSequenceName() const = 0;
    virtual SequenceBuilderPtr CreateBuilder() = 0;
    virtual void ConfigureParams(VatContextPtr context) = 0;
    virtual DataRepositoryPtr CreateRepository() = 0;
    virtual VisionEventHandlerPtr CreateVisionProcessor() = 0;
    virtual void SetActuator(IActuator* adapter) = 0;
    virtual IActuator* GetActuator() = 0;
};
```

### 3.6 Context (실행 컨텍스트)

```cpp
// include/Context.h
class Context : public std::enable_shared_from_this<Context> {
public:
    Context();
    ~Context();

    // 파라미터 관리
    void SetVatParams(const VatParams& params);
    void SetSeqParam(const std::string& key, int value);
    template<typename T> void SetSeqParamAs(const std::string& key, const T& value);
    template<typename T> void SetVisionParamAs(const std::string& key, const T& value);
    std::string GetSeqParam(const std::string& key) const;
    std::string GetVisionParam(const std::string& key) const;
    template<typename T> T GetSeqParamAs(const std::string& key, const T& defaultValue) const;
    template<typename T> T GetVisionParamAs(const std::string& key, const T& defaultValue) const;

    // 비전 프로세서
    void SetVisionProcessor(VisionEventHandlerPtr vp);
    VisionEventHandlerPtr GetVisionProcessorInterface() const;

    // 데이터 저장소
    void SetDataRepository(DataRepositoryPtr repo);
    DataRepositoryPtr getRepository() const;

    // 상태 제어
    void SetStopRequested(bool stop);
    bool GetStopRequested() const;
    void SetLastError(const std::string& error);
    const std::string& GetLastError() const;

    // 비전 명령 실행
    bool ExecuteVisionCommand(VatCommand cmd);

    // 비전 위치 관리
    std::vector<VisionPosition> GetVisionPositions() const;
    bool PopVisionPosition(VisionPosition& outPos);
    bool PeekVisionPosition(VisionPosition& outPos);
    void AddVisionPosition(const VisionPosition& pos);
    bool IsVisionPositionEmpty() const;
};
```

---

## 4. 시퀀스(Sequence) 시스템

### 4.1 Sequence 클래스

`Sequence`는 `ISequence`의 기본 구현체로, 여러 개의 `ITask`를 순차적으로 실행합니다.

```cpp
// include/Sequence.h
class Sequence : public ISequence {
public:
    explicit Sequence(const std::string& name = "");
    ~Sequence() override;

    bool Execute(Context& context, IActuator* actuator) override;
    void Abort() override;
    std::string GetSequenceName() const override;
    std::string GetTaskName() const override;
    void AddTask(TaskPtr step) override;
    void SetPollIntervalMs(int ms);  // TR_KEEP 시 폴링 간격 (기본: 10ms)
};
```

### 4.2 실행 흐름

1. `Execute()`가 호출되면 Task 목록을 순회합니다.
2. 각 Task의 `Execute()`를 호출하고 반환값에 따라 동작합니다.
3. `TR_KEEP` 반환 시 설정된 `m_pollIntervalMs`(기본 10ms) 동안 대기 후 재실행.
4. 중단 요청(`Abort()` 또는 `Context::SetStopRequested(true)`)이 있으면 즉시 중단.
5. 모든 Task 완료 또는 `TR_DONE`/`TR_ERROR` 반환 시 종료.

### 4.3 SequenceBuilderBase

시퀀스 빌더는 Task를 조립하는 팩토리 역할을 합니다.

```cpp
// include/SequenceBuilderBase.h
class SequenceBuilderBase {
public:
    SequenceBuilderBase();
    virtual ~SequenceBuilderBase();

    SequencePtr CreateSequence(std::string sequenceName);

protected:
    virtual SequencePtr BuildSequence(std::string sequenceName) = 0; // 상속 구현
};
```

---

## 5. 태스크(Task) 작성 가이드

### 5.1 NonBlockingTaskBase 사용 (권장)

대부분의 Task는 `NonBlockingTaskBase`를 상속받아 구현합니다.  
이 클래스는 **초기화(OnInitialize)** → **폴링(OnPoll)** 패턴으로 동작합니다.

```cpp
// include/NonBlockingTaskBase.h
class NonBlockingTaskBase : public ITask {
public:
    static const int CS_INITIALIZING = -1;
    static const int CS_ERROR        = -2;
    static const int CS_IDLE         = -3;

    NonBlockingTaskBase();
    ~NonBlockingTaskBase() override;

    // ITask 구현 (final)
    TaskResult Execute(Context& ctx, IActuator* actuator) override;
    void Abort() override;
    void EnterState(int newState) override;
    TaskResult SetErrorAndReturn(Context& ctx, const std::string& msg) override;

protected:
    // 상속 구현 필요
    virtual void OnInitialize(Context& ctx) = 0;
    virtual TaskResult OnPoll(Context& ctx, IActuator* actuator) = 0;

    // 상태 관리
    int GetState() const;
    void EnterCommonState(int state);
    void EnterStateWithTimeout(int newState, long timeoutMs);
    bool IsDeadlineExpired() const;

    // 동기화
    std::mutex& GetMutex();
};
```

### 5.2 Task 작성 예제

```cpp
// 예제: Z축 이동 Task
class MoveZTask : public NonBlockingTaskBase {
public:
    MoveZTask(double targetZ) : m_targetZ(targetZ) {}

    std::string GetName() const override { return "MoveZTask"; }

protected:
    void OnInitialize(Context& ctx) override {
        // 초기화: 목표 위치 설정
        LogTask("MoveZTask: Initialized. Target Z=" + std::to_string(m_targetZ));
        EnterState(1);  // 상태 1: 이동 시작
    }

    TaskResult OnPoll(Context& ctx, IActuator* actuator) override {
        if (!actuator) {
            return SetErrorAndReturn(ctx, "Actuator is null");
        }

        switch (GetState()) {
        case 1: {
            // 이동 명령
            ActError err = actuator->MoveZ(m_targetZ);
            if (err == ActOk) {
                EnterState(2);  // 이동 완료, 확인 상태로 전이
                return TR_KEEP;
            } else if (err == ActWait) {
                return TR_KEEP;  // 아직 이동 중, 계속 폴링
            } else {
                return SetErrorAndReturn(ctx, "MoveZ failed: " + std::to_string(err));
            }
        }
        case 2: {
            // 이동 완료 확인
            ActError err = actuator->isMoveZ(m_targetZ);
            if (err == ActOk) {
                LogTask("MoveZTask: Completed to Z=" + std::to_string(m_targetZ));
                return TR_NEXT;  // 다음 Task로
            } else if (err == ActWait) {
                if (IsDeadlineExpired()) {
                    return SetErrorAndReturn(ctx, "MoveZ timeout");
                }
                return TR_KEEP;
            } else {
                return SetErrorAndReturn(ctx, "MoveZ check failed");
            }
        }
        default:
            return SetErrorAndReturn(ctx, "Unknown state");
        }
    }

private:
    double m_targetZ;
};
```

### 5.3 Task 작성 가이드라인

1. **블로킹 금지**: `Execute()` 내에서는 절대 blocking되지 않도록 합니다. 대신 상태 기계로 분할하고 `TR_KEEP`을 반환합니다.
2. **짧은 실행 시간**: 각 Execute() 호출은 수 밀리초 이내에 완료되어야 합니다.
3. **상태 기계 패턴**: `EnterState()`/`GetState()`를 사용하여 상태 전이를 명확히 구현합니다.
4. **타임아웃 처리**: `EnterStateWithTimeout()`과 `IsDeadlineExpired()`를 활용합니다.
5. **오류 처리**: `SetErrorAndReturn()`을 사용하여 일관된 오류 처리를 합니다.
6. **로깅**: `LogTask()`를 사용하여 주요 이벤트를 기록합니다.

```cpp
// 상태 전이 템플릿
TaskResult OnPoll(Context& ctx, IActuator* actuator) override {
    switch (GetState()) {
    case 1:  // 상태 1
        // 작업 수행
        if (/* 조건 충족 */) {
            EnterStateWithTimeout(2, 5000);  // 5초 타임아웃 설정
            return TR_KEEP;
        }
        return TR_KEEP;

    case 2:  // 상태 2 (타임아웃 있음)
        // 완료 확인
        if (IsDeadlineExpired()) {
            return SetErrorAndReturn(ctx, "Timeout in state 2");
        }
        if (/* 완료 */) {
            return TR_NEXT;
        }
        return TR_KEEP;

    default:
        return SetErrorAndReturn(ctx, "Unknown state");
    }
}
```

---

## 6. 데이터 저장소(Repository)

### 6.1 구현체 선택

`RepositoryFactory`를 사용하여 구현체를 생성합니다.

```cpp
// SQLite 기반 (권장)
auto repo = RepositoryFactory::CreateRepository("sqlite", "C:\\data\\vision.db;C:\\data\\images");

// File 기반 (간단한 텍스트 파일 로깅)
auto repo = RepositoryFactory::CreateRepository("file", "C:\\data\\");
```

### 6.2 SqliteDataRepository

SQLite를 사용한 완전한 저장소 구현입니다.  
초기화 시 `migration_v1.sql` 파일을 자동으로 로드하여 테이블을 생성합니다.

**주요 특징:**
- 트랜잭션 기반 저장 (BEGIN/COMMIT)
- WAL 모드 사용 (동시성 향상)
- upsert (ON CONFLICT ... DO UPDATE) 사용
- 이미지 저장 시 temp 파일 → rename 방식 (안전성)

### 6.3 FileDataRepository

간단한 파일 기반 저장소입니다. 주로 로그 형태로 데이터를 기록합니다.

**저장 형식:**
- 파라미터: `{recipe}.params` 파일에 `name=value` 형식
- 이미지: `{contextTag}_{timestamp}.raw` 바이너리 파일
- 시퀀스 로그: `sequence_runs.log` 파일에 타임스탬프와 함께 기록
- Z-Focus 데이터: `zfocus_points.log`, `zfocus_results.log` 등

### 6.4 AsyncDataRepository (비동기 래퍼)

`IDataRepository`를 비동기 큐로 래핑합니다.  
`SaveZFocusPoint`, `SaveZFocusResult`, `UpdateSequenceRunStatus`는 내부 큐에 추가되고,  
별도 워커 스레드가 순차적으로 처리합니다 (**최대 3회 재시도**).

```cpp
// AsyncDataRepository 사용 예
auto innerRepo = RepositoryFactory::CreateRepository("sqlite", "path.db;imgpath");
auto asyncRepo = std::make_shared<AsyncDataRepository>(innerRepo.release(), true);
// ownInner=true → AsyncDataRepository가 소멸자에서 inner를 delete
```

### 6.5 SaveUtils (재시도 로직)

```cpp
// include/SaveUtils.h
template <typename SaveFunc>
StorageError SaveWithRetries(SaveFunc saveFunc, int maxRetries, int retryDelayMs = 100);
```

---

## 7. 비전 프로세서(Vision Processor)

### 7.1 VisionProcessorBase

비전 시스템과의 통신을 처리하는 기본 클래스입니다.

```cpp
// include/VisionProcessorBase.h
class VisionProcessorBase : public IVisionEventHandler {
public:
    VisionProcessorBase();
    ~VisionProcessorBase() override;

    void Start();
    void Stop();
    void InitializeRecvThread() override;
    void RunLoop();
    virtual void Process();

    // IVisionEventHandler 구현
    VC::Status Initialize(const VisionConnectionConfig& config) override;
    void Disconnect() override;
    bool IsConnected() const override;
    DataMap GetLatestData(VatCommand type) const override;
    void SetLatestData(VatCommand type, const DataMap& data);
    void ClearLatestData(VatCommand type) override;
    bool IsValid(VatCommand type) const override;
    bool HasReceived(VatCommand type) const override;
};
```

### 7.2 VisionConnectionConfig

```cpp
struct VisionConnectionConfig {
    std::string type;      // "tcp" 등
    std::string address;   // "127.0.0.1"
    int port;              // 5000 등
    int timeoutMs;         // 타임아웃 (ms)
};
```

### 7.3 비전 명령어

```cpp
enum VatCommand : int {
    SetCok      = 0,  // COK 설정
    InspReady   = 1,  // 검사 준비
    Measure     = 2,  // 측정 실행
    DeviceCheck = 3,  // 장비 체크
    Light       = 4,  // 조명 제어
    Unknown     = 200
};
```

---

## 8. Orchestrator와 Strategy 패턴

### 8.1 동작 흐름

```
Orchestrator::StartSequence<MyStrategy>(actuator)
  │
  ├─ 1. Strategy::CreateBuilder()    → SequenceBuilder
  ├─ 2. Strategy::CreateRepository() → IDataRepository
  ├─ 3. Strategy::CreateVisionProcessor() → IVisionEventHandler
  ├─ 4. Orchestrator::CreateContext(vm, repo) → Context
  ├─ 5. Strategy::ConfigureParams(ctx)
  ├─ 6. RunController 생성 (builder, ctx, actuator)
  ├─ 7. AsyncExecutor 생성 및 SetResultSink(orchestrator)
  ├─ 8. RunController::RunSequence(sequenceName)
  │      └─ builder->CreateSequence(seqName)
  │      └─ executor->Start(sequence, ctx, actuator)
  └─ 9. 결과 → IResultSink → Observer 통지
```

### 8.2 Strategy 구현 예제

```cpp
// equipment2015/vmfcomposition/strategies/MySequenceStrategy.h
class CMySequenceStrategy : public SequenceStrategyBase {
public:
    std::string GetSequenceName() const override {
        return "MyFocusCheckSequence";
    }

    SequenceBuilderPtr CreateBuilder() override {
        return std::make_shared<CMySequenceBuilder>();
    }

    void ConfigureParams(VatContextPtr context) override {
        // Context에 파라미터 설정
        context->SetSeqParam("target_z", "15.0");
        context->SetSeqParam("scan_range", "2.0");
        context->SetVisionParam("measure_type", "focus");
    }

    DataRepositoryPtr CreateRepository() override {
        auto repo = RepositoryFactory::CreateRepository(
                        "sqlite", "C:\\data\\vision.db;C:\\data\\images");
        if (repo) repo->Initialize();
        return repo;
    }

    VisionEventHandlerPtr CreateVisionProcessor() override {
        auto proc = std::make_shared<CMockVisionEventHandler>();
        // 프로세서 초기화가 필요하면 여기서 수행
        return proc;
    }
};
```

### 8.3 Observer 패턴

```cpp
// 옵저버 등록
ObserverId id = orchestrator->AddObserver(
    [](const VisionResultPayload& payload) {
        // payload.requestId
        // payload.results (vector<string>)
        // results[0] = "Sequence: ..."
        // results[1] = "Task: ..."
        // results[2] = "Status: ..."
    }
);

// 옵저버 해제
orchestrator->RemoveObserver(id);
```

---

## 9. 비동기 실행(AsyncExecutor)

### 9.1 AsyncExecutor

별도의 스레드에서 시퀀스를 실행합니다.

```cpp
auto executor = std::make_shared<AsyncExecutor>();
executor->SetResultSink(this);  // IResultSink 인터페이스

// 실행
bool ok = executor->Start(std::move(seq), ctx, actuator);

// 대기 (타임아웃 지정 가능)
bool completed = executor->WaitForCompletion(5000);  // 5초 대기

// 중단
executor->Stop();   // Context::SetStopRequested(true)
executor->Abort();  // Stop + WaitForCompletion(5000)

// 상태 확인
bool running = executor->IsRunning();
```

### 9.2 SequenceThreadFunc

`AsyncExecutor` 내부에서 실제 스레드가 실행하는 함수 객체입니다.
시퀀스 실행 결과를 `IResultSink::NotifyVisionResult()`로 통지합니다.

---

## 10. 로깅(Logging) 시스템

### 10.1 Logger (싱글톤)

```cpp
// include/Logger.h
enum LogLevel { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR };

Logger& logger = Logger::Instance();

// 설정
logger.SetLogFile("C:\\logs\\vision.log");
logger.SetConsoleOutput(true);  // 콘솔 출력 (기본: true)

// 로그 기록
logger.Log(LOG_INFO, "Sequence started");
logger.LogF(LOG_ERROR, "Error at step %d: %s", stepId, msg.c_str());
```

**로그 포맷:** `[2024-01-15 10:30:45] [INFO] 메시지 내용`

### 10.2 Compatibility 헬퍼

```cpp
// CompatUtils.h
LogTask("메시지");           // INFO 레벨 로그
LogError("에러 메시지");     // ERROR 레벨 로그
makeLogPrefix("SeqName");    // "[Sequence:SeqName] " 접두사 생성
DBG("함수명");               // 디버그 출력 (OutputDebugString)
DBG_FMT("함수명", "메시지"); // 디버그 출력 with 메시지
ToString(value);             // 값을 문자열로 변환
```

---

## 11. 코딩 규칙 및 Best Practice

### 11.1 C++ 표준 라이브러리 사용

VMF는 Boost 대신 **C++11/14 표준 라이브러리**를 사용합니다.

| Boost (사용 금지) | C++ 표준 (사용) |
|-------------------|-----------------|
| `boost::thread` | `std::thread` |
| `boost::mutex` | `std::mutex` |
| `boost::lock_guard` | `std::lock_guard` |
| `boost::unique_lock` | `std::unique_lock` |
| `boost::condition_variable` | `std::condition_variable` |
| `boost::shared_ptr` | `std::shared_ptr` |
| `boost::make_shared` | `std::make_shared` |
| `boost::unique_ptr` / `mov` | `std::unique_ptr` / `std::move` |
| `boost::bind` | Lambda 또는 `std::bind` |
| `boost::chrono` | `std::chrono` |
| `boost::this_thread::sleep` | `std::this_thread::sleep_for` |
| `boost::atomic` | `std::atomic` |
| `boost::function` | `std::function` |

### 11.2 동기화 규칙

```cpp
// 기본 LockGuard (예외 안전)
std::lock_guard<std::mutex> lock(m_mutex);

// Condition Variable 사용 시
std::unique_lock<std::mutex> lock(m_mutex);
m_cv.wait(lock, [this]() { return condition; });
m_cv.wait_for(lock, std::chrono::milliseconds(100));

// 알림
m_cv.notify_one();
m_cv.notify_all();
```

### 11.3 메모리 관리

```cpp
// unique_ptr: 단독 소유권 (이동만 가능)
std::unique_ptr<ISequence> seq = std::make_unique<MySequence>();
auto executor = std::make_unique<AsyncExecutor>();

// shared_ptr: 공유 소유권
auto ctx = std::make_shared<Context>();
DataRepositoryPtr repo = std::make_shared<SqliteDataRepository>(path, imgPath);
```

### 11.4 명명 규칙

| 항목 | 규칙 | 예시 |
|------|------|------|
| 네임스페이스 | PascalCase | `VMF` |
| 클래스/인터페이스 | PascalCase (I 접두사) | `ITask`, `SequenceBuilderBase` |
| 메서드 | PascalCase | `Execute()`, `SaveParam()` |
| 멤버 변수 | m_ 접두사 + PascalCase | `m_state_`, `m_SequenceName` |
| 매개변수 | camelCase | `runId`, `outValue` |
| 열거형 | PascalCase | `TaskResult`, `StorageError` |
| 열거형 값 | UPPER_CASE 또는 PascalCase | `TR_NEXT`, `StorageSuccess` |
| using 별칭 | PascalCase + Ptr 접미사 | `TaskPtr`, `DataRepositoryPtr` |

### 11.5 DLL 익스포트

```cpp
// VMF_API 매크로 사용
class VMF_API MyClass { ... };   // 클래스 전체 익스포트
void VMF_API MyFunction();       // 개별 함수 익스포트
```

### 11.6 오류 처리

```cpp
// Task 오류 처리
return SetErrorAndReturn(ctx, "오류 메시지");

// Repository 오류 처리
StorageError err = repo->SaveParam(recipe, name, value);
if (err != StorageSuccess) {
    LogError("SaveParam failed: " + std::to_string(err));
    return err;
}

// Context 오류 저장
ctx.SetLastError("오류 설명");
const std::string& lastError = ctx.GetLastError();

// StopRequested 확인 (Task 중단 조건)
if (ctx.GetStopRequested()) {
    return TR_ERROR;
}
```

---

## 12. 새로운 장비 시퀀스 개발하기 (예제)

이 섹션에서는 새로운 장비(예: Load1)의 Z-Focus 검사 시퀀스를 처음부터 끝까지 개발하는 과정을 단계별로 설명합니다.

### 12.1 개발 단계 개요

```
1. Task 구현      → Z축 이동, 포커스 스캐닝 등의 단위 작업
2. Builder 구현   → Task를 조립하여 시퀀스 생성
3. Strategy 구현  → Builder/Repository/Processor 조립 및 파라미터 설정
4. Orchestrator 연결 → UI 또는 메인 코드에서 실행
```

### 12.2 Step 1: Task 구현

**Z축 이동 Task** (예제: `CLoad1MoveToStartPositionTask`)

```cpp
// equipment2015/vmfcomposition/load1/tasks/CLoad1MoveToStartPositionTask.h
#pragma once
#include "NonBlockingTaskBase.h"

class CLoad1MoveToStartPositionTask : public NonBlockingTaskBase {
public:
    CLoad1MoveToStartPositionTask() {}

    std::string GetName() const override {
        return "Load1MoveToStartPosition";
    }

protected:
    void OnInitialize(Context& ctx) override {
        // Context로부터 파라미터 읽기
        m_targetZ = ctx.GetSeqParamAs("target_z", 15.0);
        LogTask("Load1MoveToStartPosition: Target Z=" + std::to_string(m_targetZ));
        EnterState(1);
    }

    TaskResult OnPoll(Context& ctx, IActuator* actuator) override {
        if (!actuator) return SetErrorAndReturn(ctx, "Actuator is null");

        switch (GetState()) {
        case 1: {
            ActError err = actuator->MoveZ(m_targetZ);
            if (err == ActOk) {
                EnterStateWithTimeout(2, 10000); // 10초 타임아웃
            } else if (err == ActWait) {
                return TR_KEEP;
            } else {
                return SetErrorAndReturn(ctx, "MoveZ failed");
            }
            return TR_KEEP;
        }
        case 2: {
            if (IsDeadlineExpired()) {
                return SetErrorAndReturn(ctx, "MoveZ timeout");
            }
            ActError err = actuator->isMoveZ(m_targetZ);
            if (err == ActOk) {
                LogTask("Load1MoveToStartPosition: Completed");
                return TR_NEXT;
            } else if (err == ActWait) {
                return TR_KEEP;
            } else {
                return SetErrorAndReturn(ctx, "isMoveZ check failed");
            }
        }
        default:
            return SetErrorAndReturn(ctx, "Unknown state");
        }
    }

private:
    double m_targetZ = 0.0;
};
```

**포커스 스캐닝 Task** (예제: `CLoad1PerformFocusScanningTask`)

```cpp
// equipment2015/vmfcomposition/load1/tasks/CLoad1PerformFocusScanningTask.h
#pragma once
#include "NonBlockingTaskBase.h"

class CLoad1PerformFocusScanningTask : public NonBlockingTaskBase {
public:
    CLoad1PerformFocusScanningTask() {}

    std::string GetName() const override {
        return "Load1PerformFocusScanning";
    }

protected:
    void OnInitialize(Context& ctx) override {
        m_runId = ctx.GetSeqParamAs("run_id", 0);
        m_scanStart = ctx.GetSeqParamAs("scan_start", 10.0);
        m_scanEnd = ctx.GetSeqParamAs("scan_end", 20.0);
        m_scanStep = ctx.GetSeqParamAs("scan_step", 0.5);
        m_currentZ = m_scanStart;
        m_scanResults.clear();
        LogTask("Load1PerformFocusScanning: Start=" + std::to_string(m_scanStart)
                + " End=" + std::to_string(m_scanEnd)
                + " Step=" + std::to_string(m_scanStep));
        EnterState(1); // 이동 상태 시작
    }

    TaskResult OnPoll(Context& ctx, IActuator* actuator) override {
        switch (GetState()) {
        case 1: { // Z축 이동
            if (!actuator) return SetErrorAndReturn(ctx, "Actuator null");
            ActError err = actuator->MoveZ(m_currentZ);
            if (err == ActOk) {
                EnterStateWithTimeout(2, 5000);
            } else if (err == ActWait) {
                return TR_KEEP;
            } else {
                return SetErrorAndReturn(ctx, "MoveZ failed at Z=" + std::to_string(m_currentZ));
            }
            return TR_KEEP;
        }
        case 2: { // 위치 도달 확인
            if (IsDeadlineExpired()) {
                return SetErrorAndReturn(ctx, "MoveZ timeout at Z=" + std::to_string(m_currentZ));
            }
            if (!actuator) return SetErrorAndReturn(ctx, "Actuator null");
            ActError err = actuator->isMoveZ(m_currentZ);
            if (err == ActOk) {
                EnterState(3); // 측정 상태로 전이
            } else if (err == ActWait) {
                return TR_KEEP;
            } else {
                return SetErrorAndReturn(ctx, "isMoveZ failed");
            }
            return TR_KEEP;
        }
        case 3: { // 비전 측정 요청
            bool ok = ctx.ExecuteVisionCommand(VatCommand::Measure);
            if (!ok) {
                return SetErrorAndReturn(ctx, "Failed to request Measure");
            }
            EnterStateWithTimeout(4, 3000);
            return TR_KEEP;
        }
        case 4: { // 측정 결과 대기
            if (IsDeadlineExpired()) {
                return SetErrorAndReturn(ctx, "Measure timeout at Z=" + std::to_string(m_currentZ));
            }
            if (ctx.IsVisionPositionEmpty()) {
                return TR_KEEP; // 아직 결과 없음
            }
            // 결과 처리
            VisionPosition pos;
            ctx.PopVisionPosition(pos);
            double score = pos.pos.size() > 0 ? pos.pos[0] : 0.0;
            m_scanResults.push_back({m_currentZ, score});

            // 저장 (비동기 큐 사용 권장)
            DataRepositoryPtr repo = ctx.getRepository();
            if (repo) {
                repo->SaveZFocusPoint(m_runId, m_currentZ, score, 1, "{}");
            }

            // 다음 Z 위치로 이동
            m_currentZ += m_scanStep;
            if (m_currentZ > m_scanEnd) {
                // 스캔 완료 → 최적 Z 계산 후 저장
                double bestZ = FindBestFocusZ();
                LogTask("Focus scanning complete. Best Z=" + std::to_string(bestZ));
                ctx.SetSeqParam("best_focus_z", std::to_string(bestZ));

                DataRepositoryPtr repo2 = ctx.getRepository();
                if (repo2) {
                    repo2->SaveZFocusResult(0, 0, 0, bestZ);
                }
                return TR_NEXT;
            }
            EnterState(1); // 다음 Z로 이동
            return TR_KEEP;
        }
        default:
            return SetErrorAndReturn(ctx, "Unknown state");
        }
    }

private:
    struct ScanPoint {
        double z;
        double score;
    };
    double FindBestFocusZ() const {
        if (m_scanResults.empty()) return 0.0;
        auto best = std::max_element(m_scanResults.begin(), m_scanResults.end(),
            [](const ScanPoint& a, const ScanPoint& b) { return a.score < b.score; });
        return best->z;
    }

    int m_runId = 0;
    double m_scanStart = 0.0, m_scanEnd = 0.0, m_scanStep = 0.0;
    double m_currentZ = 0.0;
    std::vector<ScanPoint> m_scanResults;
};
```

### 12.3 Step 2: Builder 구현

Builder는 Task를 조립하여 시퀀스 객체를 생성합니다.

```cpp
// equipment2015/vmfcomposition/load1/sequences/CLoad1ZFocusSequenceBuilder.h
#pragma once
#include "SequenceBuilderBase.h"
#include "CLoad1MoveToStartPositionTask.h"
#include "CLoad1PerformFocusScanningTask.h"

class CLoad1ZFocusSequenceBuilder : public SequenceBuilderBase {
public:
    CLoad1ZFocusSequenceBuilder() {}

protected:
    SequencePtr BuildSequence(std::string sequenceName) override {
        // 시퀀스 이름으로 분기하여 원하는 시퀀스 생성 가능
        if (sequenceName == "ZFocusSequence") {
            auto seq = std::make_unique<Sequence>("ZFocusSequence");

            // Task를 순서대로 추가
            seq->AddTask(std::make_shared<CLoad1MoveToStartPositionTask>());
            seq->AddTask(std::make_shared<CLoad1PerformFocusScanningTask>());

            return seq;
        }
        // 다른 시퀀스 이름 처리...
        return nullptr;
    }
};
```

### 12.4 Step 3: Strategy 구현

Strategy는 Builder, Repository, VisionProcessor, Actuator, 파라미터를 모두 조립합니다.

```cpp
// equipment2015/vmfcomposition/load1/strategies/CLoad1ZFocusSequenceStrategy.h
#pragma once
#include "SequenceStrategyBase.h"
#include "CLoad1ZFocusSequenceBuilder.h"

class CLoad1ZFocusSequenceStrategy : public SequenceStrategyBase {
public:
    std::string GetSequenceName() const override {
        return "ZFocusSequence";
    }

    SequenceBuilderPtr CreateBuilder() override {
        return std::make_shared<CLoad1ZFocusSequenceBuilder>();
    }

    void ConfigureParams(VatContextPtr context) override {
        // 시퀀스 파라미터 설정
        context->SetSeqParam("target_z", "15.0");
        context->SetSeqParam("scan_start", "10.0");
        context->SetSeqParam("scan_end", "20.0");
        context->SetSeqParam("scan_step", "0.5");

        // 비전 파라미터 설정
        context->SetVisionParam("exposure", "30");
        context->SetVisionParam("gain", "1.0");
        context->SetVisionParam("light", "on");
    }

    DataRepositoryPtr CreateRepository() override {
        // SQLite 저장소 생성 (비동기 래퍼 적용)
        auto innerRepo = RepositoryFactory::CreateRepository(
            "sqlite", "C:\\VisionData\\vision.db;C:\\VisionData\\images");
        if (!innerRepo) return nullptr;

        innerRepo->Initialize();

        // AsyncDataRepository로 래핑하여 반환
        return std::make_shared<AsyncDataRepository>(innerRepo.release(), true);
    }

    VisionEventHandlerPtr CreateVisionProcessor() override {
        // Mock 프로세서 (실제 연동 시 VisionProcessorBase 상속 클래스 사용)
        auto proc = std::make_shared<CMockVisionEventHandler>();
        return proc;
    }
};
```

### 12.5 Step 4: Orchestrator를 통한 실행

```cpp
// equipment2015 메인 코드 (예: Equipment2015Dlg.cpp)
#include "Orchestrator.h"
#include "CLoad1ZFocusSequenceStrategy.h"

// Orchestrator 생성
auto orchestrator = std::make_shared<VMF::Orchestrator>();

// 옵저버 등록 (결과 통지 수신)
orchestrator->AddObserver([this](const VisionResultPayload& payload) {
    // UI 업데이트 등
    m_resultList.AddString(("RequestID: " + std::to_string(payload.requestId)).c_str());
    for (const auto& msg : payload.results) {
        m_resultList.AddString(msg.c_str());
    }
});

// Actuator 생성 및 설정
IActuator* actuator = new CVatAdapterLoad1();  // 실제 하드웨어 어댑터

// 시퀀스 실행 (템플릿 인자로 Strategy 타입 전달)
bool started = orchestrator->StartSequence<CLoad1ZFocusSequenceStrategy>(actuator);
if (started) {
    LogTask("ZFocusSequence started successfully");
} else {
    LogError("Failed to start ZFocusSequence");
}

// 중단
// orchestrator->StopSequence();

// 저장소 접근
// DataRepositoryPtr repo = orchestrator->getDataRepository();
```

### 12.6 Mock 객체를 사용한 개발 및 테스트

실제 하드웨어 없이 개발/테스트를 위해 Mock 객체를 사용할 수 있습니다.

```cpp
// equipment2015/vmfcomposition/mock/CMockDataRepository.h
class CMockDataRepository : public IDataRepository {
    // 모든 메서드를 StorageSuccess 반환하며 stub으로 구현
    // 필요시 내부 map에 저장하여 검증 가능
public:
    StorageError SaveParam(const std::string& recipe, const std::string& name, const std::string& value) override {
        m_params[recipe][name] = value;
        LogTask("MockDataRepository::SaveParam: " + recipe + "/" + name + "=" + value);
        return StorageSuccess;
    }
    // ... 나머지 메서드도 동일 패턴으로 구현
private:
    std::map<std::string, std::map<std::string, std::string>> m_params;
};

// equipment2015/vmfcomposition/mock/CMockVisionEventHandler.h
class CMockVisionEventHandler : public IVisionEventHandler {
public:
    // 요청 즉시 성공 반환
    bool RequestMeasureAsync(const StringMap& params) override {
        LogTask("MockVisionEventHandler::RequestMeasureAsync");
        // 필요시 지연 후 결과 설정 가능
        return true;
    }
    // ... 나머지 메서드 구현
};
```

---

## 부록 A: Types.h 주요 타입 정의

```cpp
// include/Types.h
namespace VMF {
    using VatEnginePtr          = std::shared_ptr<RunController>;
    using DataRepositoryPtr     = std::shared_ptr<IDataRepository>;
    using VisionEventHandlerPtr = std::shared_ptr<IVisionEventHandler>;
    using SequenceBuilderPtr    = std::shared_ptr<SequenceBuilderBase>;
    using VatContextPtr         = std::shared_ptr<Context>;
    using AsyncExecutorPtr      = std::shared_ptr<AsyncExecutor>;
    using SequencePtr           = std::unique_ptr<ISequence>;
    using StringMap             = std::map<std::string, std::string>;
    using LockGuardType         = std::lock_guard<std::mutex>;
    using UniqueLockType        = std::unique_lock<std::mutex>;
    using ConditionVariableType = std::condition_variable;
    using VatActuatorPtr        = IActuator*;
}
```

## 부록 B: VMF_API 익스포트 매크로

```cpp
// include/VMF_API.h
#ifdef VMF_EXPORTS
#define VMF_API __declspec(dllexport)
#else
#define VMF_API __declspec(dllimport)
#endif
```

## 부록 C: SQLite 마이그레이션 SQL 예제 (migration_v1.sql)

```sql
-- migration_v1.sql
-- VMF SQLite Database Schema

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
```

---

> **문서 버전**: 1.0  
> **최종 업데이트**: 2024년  
> **프로젝트**: VisionMotionFramework (VMF)