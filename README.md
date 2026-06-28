# VisionMotionFramework 배포 가이드

## 개요

VisionMotionFramework는 Vision 검사 시퀀스를 위한 C++ 프레임워크입니다.
프레임워크는 **NuGet 패키지**로, 장비별 커스터마이징 코드는 **VS Project Template**으로 배포됩니다.

---

## 배포 구성

| 항목 | 배포 방식 | 대상 |
|------|----------|------|
| Core 엔진 (.lib + 헤더) | NuGet 패키지 | 모든 Equipment 프로젝트 |
| Mock 객체 | NuGet 패키지 (contentFiles) | 테스트 환경 |
| 장비별 샘플 코드 | VS Project Template (.zip) | 신규 Equipment 개발자 |

---

## 1. NuGet 패키지 설치

### 지원 VS 버전

| 패키지 ID | 대상 VS 버전 | Toolset |
|-----------|-------------|---------|
| `VisionMotionFramework.Core.v100` | VS2010 | v100 |
| `VisionMotionFramework.Core.v140` | VS2015, VS2017 | v140, v141 |
| `VisionMotionFramework.Core.v142` | VS2019, VS2022, VS2026 | v142, v143 |

### 설치 방법

**Package Manager Console:**
```powershell
Install-Package VisionMotionFramework.Core.v142
```

**또는 .vcxproj에 직접 추가:**
```xml
<ItemGroup>
  <PackageReference Include="VisionMotionFramework.Core.v142" Version="1.*" />
</ItemGroup>
```

### 포함 항목

- **라이브러리:** `VMF.Core.lib` (정적 라이브러리, `/MD` 빌드)
- **헤더:**
  - `VMF/Core/NonBlockingTaskBase.h` — 비동기 Task 기초 클래스
  - `VMF/Core/SequenceBuilderBase.h` — 시퀀스 조립 팩토리
  - `VMF/Core/ComponentSetupBase.h` — 컴포넌트 설정 기본 클래스
  - `VMF/Core/DefaultSetupStrategy.h` — 기본 Setup 전략
  - `VMF/Core/IDataRepository.h` — 데이터 저장소 인터페이스
  - `VMF/Core/IVisionProcessor.h` — Vision 처리 인터페이스
  - `VMF/Core/IActuator.h` — 구동기 인터페이스
  - `VMF/Core/Context.h` — 컨텍스트
  - `VMF/Core/Sequence.h` — 시퀀스
  - `VMF/Core/Types.h` — 공통 타입
  - `VMF/Mock/CMockDataRepository.h` — Mock 데이터 저장소
  - `VMF/Mock/CMockVisionEventHandler.h` — Mock Vision 이벤트 핸들러

---

## 2. VS Project Template 설치

### 템플릿 파일
- `VMFEquipmentPlugin.zip`

### 설치 방법
1. `VMFEquipmentPlugin.zip`을 다음 폴더에 복사:
   - **VS2019:** `%USERPROFILE%\Documents\Visual Studio 2019\Templates\ProjectTemplates\`
   - **VS2022:** `%USERPROFILE%\Documents\Visual Studio 2022\Templates\ProjectTemplates\`
2. Visual Studio 실행
3. **새 프로젝트 만들기** → "VMF Equipment Plugin (v142)" 검색

### 템플릿 프로젝트 구조
```
VMFEquipmentPlugin/
├── Tasks/
│   ├── SampleMoveToStartPositionTask.h      ← 수정 대상
│   ├── SampleMoveToStartPositionTask.cpp    ← 수정 대상
│   ├── SamplePerformFocusScanningTask.h     ← 수정 대상
│   └── SamplePerformFocusScanningTask.cpp   ← 수정 대상
├── Sequences/
│   └── SampleZFocusSequenceBuilder.h        ← 수정 대상
├── Strategies/
│   └── SampleSequenceStrategy.h             ← 수정 대상
├── Actuator/
│   ├── SampleActuatorAdapter.h              ← 수정 대상
│   └── SampleActuatorAdapter.cpp            ← 수정 대상
├── DefineVAT.h                              ← 수정 대상
├── PluginFactory.h                          ← DLL 인터페이스 (고정)
├── dllmain.cpp                              ← DLL 진입점 (고정)
├── pch.h / pch.cpp                          ← 미리 컴파일된 헤더
├── framework.h                              ← 프레임워크 헤더
└── VMFEquipmentPlugin.vcxproj              ← 프로젝트 설정
```

### DLL 빌드 출력
```
$(SolutionDir)$(Platform)\$(Configuration)\VMFEquipmentPlugin.dll
예) x64\Release\VMFEquipmentPlugin.dll
```

---

## 3. 장비별 커스터마이징 방법

### 기본 워크플로우
1. 템플릿으로 새 프로젝트 생성
2. NuGet 패키지 자동 복원
3. 샘플 파일 복사/리네이밍
4. 장비에 맞게 코드 수정
5. DLL 빌드
6. 메인 APP에서 DLL 로드

### 수정 가이드

#### Task 수정
```cpp
// 1. 샘플 복사
class MyMoveToStartPositionTask : public VMF::NonBlockingTaskBase
{
    // 2. 축 이름, 좌표, 안전 조건을 장비에 맞게 수정
    VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* act) override
    {
        // 예: act->MoveZ(SAFE_Z); → 장비의 SafeZ 값으로 변경
        return VMF::TaskResult::Running;
    }
    // ...
};
```

#### Strategy 수정
```cpp
// 1. DefaultSetupStrategy 상속
class MySequenceStrategy : public VMF::DefaultSetupStrategy
{
    void ConfigureParams(VMF::Context& ctx, VMF::IDataRepository* repo) override
    {
        // 2. CameraIndex, Picker 설정, VisionPoint 좌표를 장비에 맞게 수정
        ctx.SetParam(VAT_SEQ_PARAM_CAMERA_INDEX,  6);   // 장비별 Camera ID
        ctx.SetParam(VAT_SEQ_PARAM_MAX_INSP_COUNT, 36);  // 검사 포인트 수
        // ...
    }
};
```

#### Actuator Adapter 수정
```cpp
// 1. IActuator 구현
class MyActuatorAdapter : public VMF::IActuator
{
    // 2. 축 매핑, Move 로직을 장비 HW에 맞게 구현
    VMF::ActError MoveZ(double targetZ) override
    {
        // 장비별 Z축 이동 명령
        return VMF::ActOk;
    }
    // ...
};
```

#### PluginFactory 연결
```cpp
// dllmain.cpp — Strategy 클래스만 교체
#include "Strategies/MySequenceStrategy.h"

extern "C" __declspec(dllexport)
VMF::ComponentSetupBase* CreateSetupStrategy()
{
    return new MySequenceStrategy();  // ← 여기만 교체
}
```

---

## 4. 메인 APP에서 DLL 로드

```cpp
// 1. DLL 로드
HMODULE hDll = LoadLibrary(L"EquipmentPlugin_x64_Release.dll");

// 2. 팩토리 함수 획득
using CreateFn = VMF::ComponentSetupBase*(*)();
CreateFn createFn = (CreateFn)GetProcAddress(hDll, "CreateSetupStrategy");

// 3. Strategy 생성
VMF::ComponentSetupBase* strategy = createFn();

// 4. Vision Context 설정
VMF::Context ctx;
strategy->ConfigureParams(ctx, dataRepo);
strategy->SetConnectionConfig(connConfig);

// 5. Sequence 실행
auto builder = strategy->CreateBuilder();
auto sequence = builder->BuildSequence(ctx, actuator);
while (sequence->Poll(ctx, actuator) == VMF::TaskResult::Running);

// 6. 정리
strategy->DestroySetupStrategy();
FreeLibrary(hDll);
```

---

## 5. 변경 주기별 관리

| 변경 내용 | 작업 | 영향 범위 |
|-----------|------|----------|
| Core 엔진 버그 수정 | NuGet 버전 업데이트 | 모든 장비 (DLL 재빌드 불필요) |
| 장비별 Task/Sequence 로직 변경 | DLL 소스 수정 후 재빌드 | 해당 장비만 영향 |
| 신규 장비 추가 | 템플릿으로 프로젝트 생성 | 신규 장비만 영향 |
| 인터페이스 변경 | NuGet + 모든 DLL 재빌드 | 전면 영향 (드물게 발생) |

---

## 6. 빌드 요구사항

- **C++ 표준:** ISO C++14 (`/std:c++14`)
- **Runtime:** 동적 CRT (`/MD`)
- **Platform:** Win32 (x86) / x64

---

## 7. NuGet 패키지 로컬 빌드 (개발자용)

```powershell
# 1. .nuspec 파일 위치로 이동
cd NuGetDeploy

# 2. 패키지 빌드
nuget pack VisionMotionFramework.Core.v142.nuspec

# 3. 로컬 NuGet 저장소에 추가
nuget add VisionMotionFramework.Core.v142.1.0.0.nupkg -Source C:\LocalNuGetRepo

# 4. VS에서 패키지 소스에 C:\LocalNuGetRepo 추가 후 테스트
```

---

## 라이선스

Copyright (c) 2026 VisionMotionFramework Team. All rights reserved.
