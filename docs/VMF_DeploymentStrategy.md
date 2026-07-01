# VisionMotionFramework 배포 가이드

## 개요

VisionMotionFramework는 Vision 검사 시퀀스를 위한 C++ 프레임워크입니다.
프레임워크는 **NuGet 패키지**로, 장비별 커스터마이징 코드는 **VS Project Template**으로 배포됩니다.

**배포 구조:**

```
VisionMotionFramework.dll + VisionComm.dll + 헤더
        │
        ├──▶ Equipment App (직접 프로젝트 참조)
        │        솔루션 내 VisionMotionFramework, VisionComm 참조
        │        LoadLibrary("VMFEquipmentPlugin.dll")
        │
        └──▶ NuGet 패키지: VisionMotionFramework.Core.v140
                 └──▶ VMFEquipmentPlugin (VS Template)
                          PackageReference 자동 복원 → DLL 빌드
```

> **Equipment App (Equipment2015):** 직접 프로젝트 참조 방식 사용 (NuGet 대상 아님)
> **VMFEquipmentPlugin:** NuGet PackageReference (`VisionMotionFramework.Core.v140`)로 Core 참조

---

## 배포 구성

| 항목 | 배포 방식 | 대상 |
|------|----------|------|
| Core 엔진 (DLL + import lib + 헤더) | NuGet 패키지 | **VMFEquipmentPlugin** (VS Template) |
| Mock 객체 | NuGet 패키지 (contentFiles) | **VMFEquipmentPlugin** (테스트 환경, 단위 테스트) |
| 장비별 샘플 코드 | VS Project Template (.zip) | 신규 Equipment 개발자 |
| Equipment App | 직접 프로젝트 참조 | **Equipment2015** (솔루션 내 프로젝트 참조) |

---

## 1. NuGet 패키지 설치

> **대상:** 이 섹션은 **VMFEquipmentPlugin** 프로젝트 (VS Template) 기준입니다.
> **Equipment2015 (Equipment App):** 솔루션 내 직접 프로젝트 참조 방식이므로 NuGet 패키지 설치 불필요.

### 지원 VS 버전

| 패키지 ID | 대상 VS 버전 | Toolset | CRT | 사용 권장 |
|-----------|-------------|---------|-----|----------|
| `VisionMotionFramework.Core.v140` | **VS2015 / VS2019 / VS2026** | v140, v142 | vcruntime140.dll | **✅ 권장** |

**주요 특징:**
- `VisionMotionFramework.Core.v140`는 **VS2015(v140), VS2019(v142), VS2026** 모두에서 **동일한 DLL 사용**
- 이유: v140 이후 MSVC 컴파일러가 동일한 CRT ABI(`/MD`) 유지
- NuGet `.props` 파일이 include 경로와 라이브러리 링크를 자동 설정
- NuGet `.targets` 파일이 빌드 후 DLL을 출력 디렉터리로 자동 복사

### 설치 방법

**Package Manager Console:**
```
Install-Package VisionMotionFramework.Core.v140
```

**또는 .vcxproj에 직접 추가:**
```xml
<ItemGroup>
  <PackageReference Include="VisionMotionFramework.Core.v140" Version="1.*" />
</ItemGroup>
```

### CRT 호환성

**VisionMotionFramework.Core.v140의 특별한 점:**

| 항목 | 상세 |
|------|------|
| **빌드 설정** | `/MD` (동적 MSVCRT 링크) |
| **CRT 버전** | vcruntime140.dll (VS2015 이상) |
| **호환 범위** | VS2015 / VS2019 / VS2026 모두 동일 DLL 사용 |
| **자동 설정** | NuGet `.props` 파일이 include 경로와 라이브러리 링크 자동 설정 |
| **DLL 복사** | NuGet `.targets` 파일이 빌드 후 대상 디렉터리로 DLL 자동 복사 |

**개발자가 할 일:**
1. `Install-Package VisionMotionFramework.Core.v140` 실행
2. 이후 include 경로, 라이브러리 링크, DLL 복사는 **NuGet에서 자동으로 처리됨** (수동 설정 불필요)

### 포함 항목

**라이브러리 (Win32, Debug / Release 각각 포함):**
- `VisionMotionFramework.dll` — Core 엔진 DLL
- `VisionMotionFramework.lib` — Core 엔진 import library
- `VisionComm.dll` — 통신 DLL
- `VisionComm.lib` — 통신 DLL import library

> **참고:** Core 엔진은 **DLL**로 제공됩니다. 버그 수정 시 Core DLL만 교체하면 모든 Plugin에서 즉시 적용되며, Plugin DLL 재빌드는 불필요합니다.

**핵심 헤더:**
- `NonBlockingTaskBase.h` — 비동기 Task 기초 클래스
- `SequenceBuilderBase.h` — 시퀀스 조립 팩토리
- `ComponentSetupBase.h` — 컴포넌트 설정 기본 클래스
- `DefaultSetupStrategy.h` — 기본 Setup 전략
- `IDataRepository.h` — 데이터 저장소 인터페이스
- `IVisionProcessor.h` — Vision 처리 인터페이스
- `IActuator.h` — 구동기 인터페이스
- `Context.h` — 컨텍스트
- `Sequence.h` — 시퀀스
- `Types.h` — 공통 타입

**Mock 객체 (테스트 환경용):**
- `Mock/CMockDataRepository.h` — Mock 데이터 저장소 (DB 없이 테스트)
- `Mock/CMockVisionEventHandler.h` — Mock Vision 이벤트 (카메라 없이 테스트)

#### Mock 객체 사용 예

```cpp
// 1. Mock 객체 포함
#include "Mock/CMockDataRepository.h"

// 2. 테스트 코드에서 사용
auto mockRepo = std::make_unique<VMF::CMockDataRepository>();
strategy->ConfigureParams(ctx, mockRepo.get());  // 실제 DB 대신 Mock 전달

// 3. 테스트 케이스 실행
// 카메라나 DB 없이도 시퀀스 로직 검증 가능
while (sequence->Poll(ctx, actuator) == VMF::TaskResult::Running);
```

### 의존성

NuGet 패키지 설치 시 **다음 런타임 DLL이 필요합니다:**
- `vcruntime140.dll` (VC++ 2015~2022 재배포 가능 패키지)
- `mfc140u.dll` (MFC DLL — Core/Comm DLL이 MFC Dynamic 링크로 빌드됨)
- `msvcp140.dll` (C++ 표준 라이브러리)

---

## 2. VS Project Template 설치

### 템플릿 파일 다운로드

**GitHub Release에서 다운로드:**
- https://github.com/hisimi2/VisionMotionFramework/releases
- 파일: `VMFEquipmentPlugin-v140.zip` (또는 최신 버전)

### 설치 방법

1. `VMFEquipmentPlugin-v140.zip`을 다음 폴더에 복사:
   - **VS2015:** `%USERPROFILE%\Documents\Visual Studio 2015\Templates\ProjectTemplates\`
   - **VS2019:** `%USERPROFILE%\Documents\Visual Studio 2019\Templates\ProjectTemplates\`
   - **VS2026:** `%USERPROFILE%\Documents\Visual Studio 2026\Templates\ProjectTemplates\`

2. Visual Studio 재시작

3. **파일 > 새로 만들기 > 프로젝트** → "VMF Equipment Plugin" 검색 및 선택

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
├── DefineVAT.h                              ← 수정 대상 (장비 파라미터)
├── PluginFactory.h                          ← DLL 인터페이스 (고정, 수정 금지)
├── dllmain.cpp                              ← DLL 진입점 (고정, 수정 금지)
├── pch.h / pch.cpp                          ← 미리 컴파일된 헤더
├── framework.h                              ← 프레임워크 헤더
└── VMFEquipmentPlugin.vcxproj              ← 프로젝트 설정
```

### DLL 빌드 출력

```
$(SolutionDir)bin\$(Platform)\$(Configuration)\VMFEquipmentPlugin.dll
예) bin\Win32\Release\VMFEquipmentPlugin.dll
```

---

## 3. 장비별 커스터마이징 방법

### 기본 워크플로우

1. 템플릿으로 새 프로젝트 생성
2. NuGet 패키지 자동 복원 (PackageReference에 의해 자동 처리)
3. 샘플 파일 복사/리네이밍
4. 장비에 맞게 코드 수정 (아래 가이드 참고)
5. DLL 빌드 (`Release` 권장)
6. 메인 APP에서 DLL 로드
    
### 수정 가이드

#### Task 수정 예시

```cpp
// 1. SampleMoveToStartPositionTask.h 복사 후 MyMoveToStartPositionTask.h로 리네이밍
class MyMoveToStartPositionTask : public VMF::NonBlockingTaskBase
{
public:
    MyMoveToStartPositionTask() : m_step(0) {}
    
    VMF::TaskResult Execute(VMF::Context& ctx, VMF::IActuator* act) override
    {
        // 2. 축 이름, 좌표, 안전 조건을 장비에 맞게 수정
        switch (m_step)
        {
        case 0:
            // 예: 기존 SAFE_Z → 실제 장비의 SafeZ 값으로 변경
            return act->MoveZ(150.0);  // 장비별 안전 높이
        case 1:
            return act->MoveX(0.0);
        case 2:
            return act->MoveY(0.0);
        }
        return VMF::TaskResult::Success;
    }
    
private:
    int m_step;
};
```

#### Strategy 수정 예시

```cpp
// 1. SampleSequenceStrategy.h 복사 후 MySequenceStrategy.h로 리네이밍
class MySequenceStrategy : public VMF::DefaultSetupStrategy
{
public:
    void ConfigureParams(VMF::Context& ctx, VMF::IDataRepository* repo) override
    {
        // 2. 장비에 맞게 파라미터 설정
        ctx.SetParam(VAT_SEQ_PARAM_CAMERA_INDEX,      6);      // 장비별 카메라 ID
        ctx.SetParam(VAT_SEQ_PARAM_MAX_INSP_COUNT,    36);     // 검사 포인트 수
        ctx.SetParam(VAT_SEQ_PARAM_MOTION_TIMEOUT_MS, 5000);   // 5초
        // ... 기타 파라미터
    }
    
    VMF::SequenceBuilderBase* CreateBuilder() override
    {
        return new MyZFocusSequenceBuilder();  // 커스텀 Sequence Builder
    }
};
```

#### Actuator Adapter 수정 예시

```cpp
// 1. SampleActuatorAdapter.h 복사 후 MyActuatorAdapter.h로 리네이밍
class MyActuatorAdapter : public VMF::IActuator
{
public:
    // 2. 축 매핑과 Move 로직을 장비 HW 인터페이스에 맞게 구현
    VMF::ActError MoveZ(double targetZ) override
    {
        // 예: 장비의 Z축 이동 명령 (예: Modbus, EtherCAT 등)
        if (m_hardware->WriteRegister(REG_Z_TARGET, (int)targetZ))
            return VMF::ActOk;
        return VMF::ActError;
    }
    
    VMF::ActError MoveX(double targetX) override { /* ... */ }
    VMF::ActError MoveY(double targetY) override { /* ... */ }
    
private:
    IHardwareInterface* m_hardware;  // 장비 제어 인터페이스
};
```

#### PluginFactory 연결 (중요!)

```cpp
// dllmain.cpp — Strategy 클래스만 교체
#include "Strategies/MySequenceStrategy.h"

extern "C" __declspec(dllexport)
VMF::ComponentSetupBase* CreateSetupStrategy()
{
    return new MySequenceStrategy();  // ← 커스텀 Strategy 반환
}

// PluginFactory.h 는 절대 수정하지 마세요!
```

---

## 4. 메인 APP에서 DLL 로드

```cpp
#include <windows.h>
#include "ComponentSetupBase.h"

// 1. DLL 로드
HMODULE hDll = LoadLibrary(L"bin/Win32/Release/VMFEquipmentPlugin.dll");
if (!hDll) {
    printf("DLL 로드 실패\n");
    return;
}

// 2. 팩토리 함수 획득
typedef VMF::ComponentSetupBase* (*CreateFn)();
CreateFn createFn = (CreateFn)GetProcAddress(hDll, "CreateSetupStrategy");
if (!createFn) {
    printf("팩토리 함수 획득 실패\n");
    FreeLibrary(hDll);
    return;
}

// 3. Strategy 생성
VMF::ComponentSetupBase* strategy = createFn();
if (!strategy) {
    printf("Strategy 생성 실패\n");
    FreeLibrary(hDll);
    return;
}

// 4. Vision Context 및 Data Repository 설정
VMF::Context ctx;
auto dataRepo = std::make_unique<MyDataRepository>();  // 또는 CMockDataRepository
strategy->ConfigureParams(ctx, dataRepo.get());
strategy->SetConnectionConfig(connConfig);

// 5. Sequence 빌드 및 실행
auto builder = strategy->CreateBuilder();
auto sequence = builder->BuildSequence(ctx, actuator.get());

while (sequence->Poll(ctx, actuator.get()) == VMF::TaskResult::Running)
{
    // 비동기 Task 진행 중...
    Sleep(10);  // 또는 적절한 대기 시간
}

// 6. 정리
strategy->DestroySetupStrategy();
FreeLibrary(hDll);
```

---

## 5. 변경 주기별 관리

| 변경 내용 | 작업 | 영향 범위 | 버전 관리 |
|-----------|------|----------|----------|
| Core 엔진 버그 수정 | NuGet 버전 업데이트 (패치) | 모든 VMFEquipmentPlugin (**DLL 재빌드 불필요** — Core DLL만 교체) | v1.0.**1** |
| 새로운 Interface 추가 | NuGet 버전 업데이트 (마이너) | 모든 VMFEquipmentPlugin (선택적 적용, Plugin 재빌드 필요) | v1.**1**.0 |
| 인터페이스 시그니처 변경 | NuGet + 모든 Plugin DLL 재빌드 (메이저) | 전면 영향 (드물게 발생) | v**2**.0.0 |
| 장비별 Task/Sequence 로직 변경 | Plugin DLL 소스 수정 후 재빌드 | 해당 장비만 | - |
| 신규 장비 추가 | 템플릿으로 Plugin 프로젝트 생성 | 신규 장비만 | - |

> **Core가 DLL이므로** Core DLL만 교체하면 모든 Equipment App에서 즉시 적용됩니다. Plugin DLL 재빌드는 불필요합니다.

---

## 6. 빌드 요구사항

- **C++ 표준:** ISO C++14 (`/std:c++14`)
- **Runtime:** 동적 CRT (`/MD`)
- **MFC:** 동적 라이브러리 (`/MD`) — Core/Comm DLL이 MFC Dynamic 링크로 빌드됨
- **Platform:** Win32 (x86)
- **권장 빌드:** Release 구성
        
---

## 7. NuGet 패키지 로컬 빌드 (개발자용)

### 패키지 소스 구조

```
NuGetDeploy/
├── VisionMotionFramework.Core.v140.nuspec
└── build/
    └── native/
        ├── include/         (헤더 파일들)
        ├── VisionMotionFramework.Core.v140.props    (include/lib 경로 자동 설정)
        ├── VisionMotionFramework.Core.v140.targets  (빌드 후 DLL 자동 복사)
        └── bin/
            └── Win32/
                ├── Debug/
                │   ├── VisionMotionFramework.dll
                │   ├── VisionMotionFramework.lib
                │   ├── VisionComm.dll
                │   └── VisionComm.lib
                └── Release/
                    ├── VisionMotionFramework.dll
                    ├── VisionMotionFramework.lib
                    ├── VisionComm.dll
                    └── VisionComm.lib
```

### 로컬 빌드 및 테스트

```powershell
# 1. NuGetDeploy 디렉터리로 이동
cd NuGetDeploy

# 2. 패키지 빌드 (v140)
nuget pack VisionMotionFramework.Core.v140.nuspec -OutputDirectory .\bin

# 3. 로컬 NuGet 저장소에 추가
nuget add .\bin\VisionMotionFramework.Core.v140.1.0.0.nupkg -Source C:\LocalNuGetRepo

# 4. Visual Studio에서 패키지 소스 설정
#    도구 > 옵션 > NuGet 패키지 관리자 > 패키지 소스
#    → "Local Repo" = C:\LocalNuGetRepo 추가

# 5. 템플릿 프로젝트에서 테스트
Install-Package VisionMotionFramework.Core.v140 -Source C:\LocalNuGetRepo
```

### .props 파일의 설정 내용

```xml
<!-- VisionMotionFramework.Core.v140.props -->
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">

  <PropertyGroup>
    <!-- 패키지 루트: build\native\ -->
    <VMF_Root>$(MSBuildThisFileDirectory)</VMF_Root>
    <VMF_Include>$(VMF_Root)include\</VMF_Include>
    <VMF_LibDir>$(VMF_Root)bin\$(Platform)\$(Configuration)\</VMF_LibDir>
  </PropertyGroup>

  <!-- Include 경로 + 라이브러리 링크 자동 설정 -->
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>
        $(VMF_Include);
        %(AdditionalIncludeDirectories)
      </AdditionalIncludeDirectories>
      <!-- DLL이므로 VMF_STATIC_LIB 전처리기 정의 불필요 -->
    </ClCompile>
  </ItemDefinitionGroup>

  <ItemDefinitionGroup>
    <Link>
      <AdditionalLibraryDirectories>$(VMF_LibDir);%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <AdditionalDependencies>VisionMotionFramework.lib;VisionComm.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>

</Project>
```

### .targets 파일의 설정 내용 (DLL 자동 복사)

```xml
<!-- VisionMotionFramework.Core.v140.targets -->
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">

  <Target Name="CopyVMFDll" AfterTargets="Build">
    <Message Text="=== VMF DLL Copy ===" Importance="high" />

    <!-- 빌드 후 출력 디렉터리로 Core DLL + Comm DLL 자동 복사 -->
    <Exec IgnoreExitCode="true"
          Command="if exist &quot;$(MSBuildThisFileDirectory)bin\$(Platform)\$(Configuration)\*.dll&quot;
                   xcopy /Y /D /Q &quot;$(MSBuildThisFileDirectory)bin\$(Platform)\$(Configuration)\*.dll&quot; &quot;$(TargetDir)&quot;" />
  </Target>

</Project>
```

---

## 8. CI/CD 자동 배포 (선택사항)

### GitHub Actions 예시

```yaml
name: NuGet Deploy

on:
  push:
    tags:
      - 'v[0-9]+.[0-9]+.[0-9]+'  # v1.0.0 형식 태그만 트리거

jobs:
  build-and-publish:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v3
      
      - name: Setup .NET
        uses: actions/setup-dotnet@v3
        with:
          dotnet-version: '6.0.x'
      
      - name: Build NuGet package (v140)
        run: |
          cd NuGetDeploy
          nuget pack VisionMotionFramework.Core.v140.nuspec
      
      - name: Publish to NuGet
        run: |
          nuget push VisionMotionFramework.Core.v140.*.nupkg `
            -Source https://api.nuget.org/v3/index.json `
            -ApiKey ${{ secrets.NUGET_API_KEY }} `
            -SkipDuplicate
      
      - name: Create Release
        uses: softprops/action-gh-release@v1
        with:
          files: NuGetDeploy/VisionMotionFramework.Core.v140.*.nupkg
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

### 로컬 배포 체크리스트

- [ ] 모든 테스트 통과
- [ ] 버전 번호 업데이트 (`.nuspec`, `Constants.h`)
- [ ] CHANGELOG.md 작성
- [ ] Git 태그 생성: `git tag v1.0.0`
- [ ] `nuget pack` 실행
- [ ] 로컬 테스트: 새 프로젝트에서 NuGet 설치 테스트
- [ ] `nuget push` 또는 GitHub 수동 Release 생성

---

## 9. 문제 해결 (Troubleshooting)

### NuGet 패키지 설치 실패

```
오류: Could not find a package with id 'VisionMotionFramework.Core.v140'
```

**해결:**
1. 패키지 소스 확인: **도구 > 옵션 > NuGet 패키지 관리자 > 패키지 소스**
2. `nuget.org` 또는 로컬 저장소 경로 확인
3. 패키지 이름과 버전 정확히 입력

### 라이브러리 링크 오류

```
오류: LNK1104 파일을 열 수 없음: 'VisionMotionFramework.lib'
```

**해결:**
1. 프로젝트 Toolset 확인: **프로젝트 속성 > 일반 > 플랫폼 도구 집합**
2. v140 / v142 인지 확인 (v140 패키지는 v140 및 v142 툴셋 모두 호환)
3. NuGet 패키지 재설치: `Install-Package VisionMotionFramework.Core.v140 -Force`
4. `.props` 파일의 플랫폼 경로 확인: `$(Platform)`이 `Win32`로 설정되어 있는지 확인

### Mock 객체 헤더를 찾을 수 없음

```
오류: 'Mock/CMockDataRepository.h': No such file or directory
```

**해결:**
1. NuGet 패키지 버전 확인: `Get-Package -Name VisionMotionFramework.Core.v140`
2. contentFiles 포함 여부 확인 (`.nuspec` 파일에 `<contentFiles>` 섹션 있는지)
3. Visual Studio 재시작 후 IntelliSense 갱신

### DLL 로드 실패 (LoadLibrary)

```
오류: LoadLibrary failed. Error code: 126 (모듈을 찾을 수 없습니다)
```

**해결:**
1. `vcruntime140.dll`, `mfc140u.dll`, `msvcp140.dll`이 시스템에 설치되어 있는지 확인
2. Visual C++ 재배포 가능 패키지 설치: https://aka.ms/vs/17/release/vc_redist.x86.exe
3. Core DLL(`VisionMotionFramework.dll`, `VisionComm.dll`)이 Plugin DLL과 같은 디렉터리에 있는지 확인

---
## 라이선스
Copyright (c) 2026 VisionMotionFramework Team. All rights reserved.
