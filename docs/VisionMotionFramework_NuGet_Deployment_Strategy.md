# VisionMotionFramework NuGet 배포 전략 문서

## Version History

| Version | Date       | Author | Description |
|---------|------------|--------|-------------|
| 1.0     | 2025-01-24 | System | 초안 작성   |

---

## 1. 개요

VisionMotionFramework는 ISO C++14 표준(`/std:c++14`)으로 작성된 비전-모션 통합 프레임워크입니다.  
본 문서는 이 프레임워크를 **NuGet 패키지**와 **Visual Studio 프로젝트 템플릿**으로 배포하는 전략을 정의합니다.

### 1.1 대상 Visual Studio 버전 및 바이너리 호환성

| MSVC Toolset | _MSC_VER | 대상 VS 버전      | 런타임 호환성 | 패키지 그룹 |
|--------------|----------|-------------------|---------------|-------------|
| v100         | 1600     | VS2010            | 독립          | v100        |
| v140         | 1900     | VS2015            | v140 = v141   | v140        |
| v141         | 1910~16  | VS2017            | v140 = v141   | v140        |
| v142         | 1920~29  | VS2019            | v142 = v143   | v142        |
| v143         | 1930~39  | VS2022            | v142 = v143   | v142        |

**핵심 원칙:** `_MSC_VER` 메이저 버전이 같으면 ABI 호환  
- v140(v140) ↔ v141(VS2017): 동일 런타임(vcruntime140.dll) → **호환**
- v142(VS2019) ↔ v143(VS2022): 동일 런타임(vcruntime140_1.dll) → **호환**
- v100(VS2010): 완전 독립

### 1.2 배포 패키지 구성 (총 3개)

| NuGet 패키지 ID                    | 대상 플랫폼        | Toolsets    | CRT 의존성                       |
|------------------------------------|--------------------|-------------|----------------------------------|
| `VisionMotionFramework.Core.v100`  | VS2010             | v100        | `Microsoft.VC100.CRT` [10.0,11)  |
| `VisionMotionFramework.Core.v140`  | VS2015, VS2017     | v140, v141  | `Microsoft.VC140.CRT` [14.0,15)  |
| `VisionMotionFramework.Core.v142`  | VS2019, VS2022, VS2026 | v142, v143  | `Microsoft.VC142.CRT` [14.20,15) |

---

## 2. 전체 아키텍처 개요

### 2.1 레이어 구조

```
┌─────────────────────────────────────────────────────────┐
│                  Equipment Application                    │
│  (고객사 Main EXE - LoadLibrary로 Plugin DLL 로드)       │
└──────────────────────────┬──────────────────────────────┘
                           │ LoadLibrary / GetProcAddress
┌──────────────────────────▼──────────────────────────────┐
│         Equipment Plugin DLL (VS Project Template)       │
│  Tasks/  Sequences/  Strategies/  PluginFactory.cpp     │
│  (고객이 직접 작성/수정 - 자주 변경됨)                   │
└──────────────────────────┬──────────────────────────────┘
                           │ 정적 링크
┌──────────────────────────▼──────────────────────────────┐
│           VisionMotionFramework.Core (NuGet)             │
│  엔진 바이너리(.lib) + 헤더 + Mock 라이브러리           │
│  (거의 변경되지 않음)                                    │
└──────────────────────────┬──────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────┐
│    EquipmentCore (NuGet 서브 패키지)                     │
│    Activity, ITask, AsyncExecutor, Context               │
└──────────────────────────┬──────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────┐
│      VisionComm (NuGet 서브 패키지)                     │
│      Controller, TCPClient, Protocol                    │
└─────────────────────────────────────────────────────────┘
```

### 2.2 변경 빈도별 분류

| 변경 빈도 | 구성 요소 | 배포 방식 |
|-----------|----------|-----------|
| **거의 없음** | `NonBlockingTaskBase`, `ComponentSetupBase`, `SequenceBuilderBase`, `IDataRepository`, `IVisionProcessor`, `Types`, `ITask`, `TaskBase`, `Controller`, `TCPClient` | NuGet (.lib + 헤더) |
| **드묾** | `DefineVAT.h` (공용 상수), `CMockDataRepository`, `CMockVisionEventHandler` | NuGet (헤더 + .lib/.cpp) |
| **자주 변경** | Task 구현체 (`CLoad1MoveToStartPositionTask` 등), SequenceBuilder (`CLoad1ZFocusSequenceBuilder`), Strategy (`CLoad1LeftPlateJIGFocusCheckSequenceStrategy`), `DefaultSetupStrategy` | VS Project Template (소스 코드) |

---

## 3. NuGet 패키지 상세 설계

### 3.1 패키지별 포함 파일

#### 3.1.1 `VisionMotionFramework.Core.v142` (대표)

```
📦 VisionMotionFramework.Core.v142.1.0.0.nupkg
├── 📁 lib\native\v142\                   # v142 용 바이너리
│   ├── x86\Release\VMF.Core.lib
│   ├── x64\Release\VMF.Core.lib
│   ├── x86\Debug\VMF.Core.lib
│   └── x64\Debug\VMF.Core.lib
├── 📁 lib\native\v143\                   # v143 용 바이너리 (동일 소스 재빌드)
│   ├── x86\Release\VMF.Core.lib
│   ├── x64\Release\VMF.Core.lib
│   ├── x86\Debug\VMF.Core.lib
│   └── x64\Debug\VMF.Core.lib
├── 📁 include\VisionMotionFramework\     # 헤더 파일
│   ├── NonBlockingTaskBase.h
│   ├── ComponentSetupBase.h
│   ├── SequenceBuilderBase.h
│   ├── IDataRepository.h
│   ├── IVisionProcessor.h
│   ├── ISequence.h
│   ├── ISequenceSetup.h
│   ├── IComponentSetup.h
│   ├── IActuator.h
│   ├── ITask.h
│   ├── Context.h
│   ├── Types.h
│   ├── VMF_API.h
│   ├── RunController.h
│   ├── Orchestrator.h
│   ├── AsyncExecutor.h
│   ├── SaveUtils.h
│   ├── FileUtils.h
│   ├── CompatUtils.h
│   ├── VisionProcessorBase.h
│   ├── Sequence.h
│   ├── ConnectionManager.h
│   ├── RepositoryFactory.h
│   ├── FileDataRepository.h
│   ├── SqliteDataRepository.h
│   └── ResultSink.h
├── 📁 include\VisionMotionFramework\Mock\    # Mock 헤더
│   ├── CMockDataRepository.h
│   └── CMockVisionEventHandler.h
├── 📁 contentFiles\any\any\VisionMotionFramework\Mock\  # Mock 소스 (컴파일 필요)
│   ├── CMockDataRepository.cpp
│   └── CMockVisionEventHandler.cpp
├── 📁 build\native\                        # MSBuild targets/props
│   └── VisionMotionFramework.Core.v142.targets
└── 📁 lib\native\v142\x86\Debug\           # PDB 파일 (선택)
    └── VMF.Core.pdb
```

#### 3.1.2 `VisionMotionFramework.Core.v140` 패키지

동일한 구조. `v142` → `v140` / `v141` 폴더명 변경.

#### 3.1.3 `VisionMotionFramework.Core.v100` 패키지

동일한 구조. `v142` → `v100` 폴더명 변경.  
(참고: VS2010은 C++14 표준 미지원이므로, `/std:c++14` 대신 별도 조건부 컴파일 전략 필요)

### 3.2 `.nuspec` 파일 예제

#### `VisionMotionFramework.Core.v142.nuspec`

```xml
<?xml version="1.0"?>
<package xmlns="http://schemas.microsoft.com/packaging/2013/05/nuspec.xsd">
  <metadata>
    <id>VisionMotionFramework.Core.v142</id>
    <version>1.0.0</version>
    <title>VisionMotionFramework Core Engine (VS2019/2022)</title>
    <authors>VMF Team</authors>
    <owners>VMF Team</owners>
    <requireLicenseAcceptance>false</requireLicenseAcceptance>
    <description>
      VisionMotionFramework Core Engine Library.
      Supports Visual Studio 2019 (v142) and Visual Studio 2022 (v143).
      Includes NonBlockingTaskBase, ComponentSetupBase, SequenceBuilderBase,
      IDataRepository, IVisionProcessor, and Mock implementations.
      ISO C++14 (std:c++14), Dynamic CRT (/MD).
    </description>
    <tags>Vision Motion Framework VMF Core</tags>
    <projectUrl>https://github.com/your-org/VisionMotionFramework</projectUrl>
    <dependencies>
      <group targetFramework="native">
        <dependency id="Microsoft.VC142.CRT" version="[14.20,15.0)" />
      </group>
    </dependencies>
  </metadata>
  <files>
    <!-- v142 (VS2019) -->
    <file src="lib\v142\Win32\Release\VMF.Core.lib"
          target="lib\native\v142\x86\Release\VMF.Core.lib" />
    <file src="lib\v142\x64\Release\VMF.Core.lib"
          target="lib\native\v142\x64\Release\VMF.Core.lib" />
    <file src="lib\v142\Win32\Debug\VMF.Core.lib"
          target="lib\native\v142\x86\Debug\VMF.Core.lib" />
    <file src="lib\v142\x64\Debug\VMF.Core.lib"
          target="lib\native\v142\x64\Debug\VMF.Core.lib" />

    <!-- v143 (VS2022) -->
    <file src="lib\v143\Win32\Release\VMF.Core.lib"
          target="lib\native\v143\x86\Release\VMF.Core.lib" />
    <file src="lib\v143\x64\Release\VMF.Core.lib"
          target="lib\native\v143\x64\Release\VMF.Core.lib" />
    <file src="lib\v143\Win32\Debug\VMF.Core.lib"
          target="lib\native\v143\x86\Debug\VMF.Core.lib" />
    <file src="lib\v143\x64\Debug\VMF.Core.lib"
          target="lib\native\v143\x64\Debug\VMF.Core.lib" />

    <!-- Core Headers -->
    <file src="..\VisionMotionFramework\include\*.h"
          target="include\VisionMotionFramework\" />

    <!-- Mock Headers -->
    <file src="..\Equipment2015\VMFComposition\Mock\*.h"
          target="include\VisionMotionFramework\Mock\" />

    <!-- Mock Sources (compile in consumer project) -->
    <file src="..\Equipment2015\VMFComposition\Mock\*.cpp"
          target="contentFiles\any\any\VisionMotionFramework\Mock\" />

    <!-- MSBuild targets (자동 include 경로 추가) -->
    <file src="build\VisionMotionFramework.Core.v142.targets"
          target="build\native\VisionMotionFramework.Core.v142.targets" />
  </files>
</package>
```

### 3.3 MSBuild targets 파일

#### `build\VisionMotionFramework.Core.v142.targets`

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>
        $(MSBuildThisFileDirectory)..\..\include\;%(AdditionalIncludeDirectories)
      </AdditionalIncludeDirectories>
      <PreprocessorDefinitions>VMF_CORE_STATIC;%(PreprocessorDefinitions)</PreprocessorDefinitions>
    </ClCompile>
    <Link>
      <AdditionalDependencies>
        VMF.Core.lib;%(AdditionalDependencies)
      </AdditionalDependencies>
      <AdditionalLibraryDirectories>
        $(MSBuildThisFileDirectory)..\..\lib\$(Platform)\$(Configuration)\;%(AdditionalLibraryDirectories)
      </AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
</Project>
```

### 3.4 패키지 빌드 스크립트

#### `Build-VMF-NuGet.ps1`

```powershell
param(
    [ValidateSet("v100","v140","v142","all")]
    [string]$Toolset = "all",
    [ValidateSet("Debug","Release")]
    [string]$Configuration = "Release"
)

$PackageVersion = "1.0.0"
$SolutionPath = "D:\project\VisionMotionFramework\VisionMotionFramework.sln"
$NuGetExe = "D:\tools\nuget.exe"

function Build-Toolset {
    param($ToolsetName, $PlatformToolset)

    Write-Host "=== Building for $ToolsetName (PlatformToolset=$PlatformToolset) ==="

    # x86
    msbuild $SolutionPath /t:VMF_Core /p:Configuration=$Configuration `
        /p:Platform=Win32 /p:PlatformToolset=$PlatformToolset `
        /p:OutDir="..\..\lib\$ToolsetName\Win32\$Configuration\"

    # x64
    msbuild $SolutionPath /t:VMF_Core /p:Configuration=$Configuration `
        /p:Platform=x64 /p:PlatformToolset=$PlatformToolset `
        /p:OutDir="..\..\lib\$ToolsetName\x64\$Configuration\"
}

function Create-Package {
    param($NuspecFile)

    Write-Host "=== Creating package from $NuspecFile ==="
    & $NuGetExe pack $NuspecFile -Version $PackageVersion
}

# Build
switch ($Toolset) {
    "v100" { Build-Toolset "v100" "v100"; Create-Package "VisionMotionFramework.Core.v100.nuspec" }
    "v140" { Build-Toolset "v140" "v140"; Create-Package "VisionMotionFramework.Core.v140.nuspec" }
    "v142" {
        Build-Toolset "v142" "v142"   # VS2019
        Build-Toolset "v143" "v143"   # VS2022
        Create-Package "VisionMotionFramework.Core.v142.nuspec"
    }
    "all" {
        Build-Toolset "v100" "v100"
        Build-Toolset "v140" "v140"
        Build-Toolset "v142" "v142"
        Build-Toolset "v143" "v143"
        Create-Package "VisionMotionFramework.Core.v100.nuspec"
        Create-Package "VisionMotionFramework.Core.v140.nuspec"
        Create-Package "VisionMotionFramework.Core.v142.nuspec"
    }
}

Write-Host "=== Done ==="
```

---

## 4. VS Project Template 상세 설계

### 4.1 템플릿 개요

템플릿은 Equipment별로 **자주 변경되는 코드**만 포함하는 동적 라이브러리(DLL) 프로젝트입니다.  
고객은 이 템플릿으로 새 프로젝트를 생성한 후, Task/Sequence/Strategy를 자유롭게 수정하고 빌드하여 DLL을 생성합니다.

### 4.2 템플릿 폴더 구조

```
📦 VisionMotionFramework.EquipDLL.zip
└── 📁 EquipmentPlugin\
    ├── 📁 Tasks\
    │   ├── SampleMoveToStartPositionTask.h
    │   ├── SampleMoveToStartPositionTask.cpp
    │   ├── SamplePerformFocusScanningTask.h
    │   └── SamplePerformFocusScanningTask.cpp
    ├── 📁 Sequences\
    │   └── SampleZFocusSequenceBuilder.h
    ├── 📁 Strategies\
    │   └── SampleStrategy.h
    ├── DllMain.cpp
    ├── PluginFactory.h
    ├── EquipmentPlugin.vcxproj
    ├── EquipmentPlugin.vcxproj.filters
    ├── stdafx.h
    ├── targetver.h
    └── ReadMe.txt
```

### 4.3 `EquipmentPlugin.vcxproj` (핵심 프로젝트 파일)

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" ToolsVersion="14.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>

  <PropertyGroup Label="Globals">
    <ProjectGuid>{$guid1$}</ProjectGuid>
    <RootNamespace>$safeprojectname$</RootNamespace>
    <Keyword>Win32Proj</Keyword>
    <WindowsTargetPlatformVersion>$targetplatformversion$</WindowsTargetPlatformVersion>
  </PropertyGroup>

  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />

  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>DynamicLibrary</ConfigurationType>
    <PlatformToolset>$currentplatformtoolset$</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
    <WholeProgramOptimization>true</WholeProgramOptimization>
  </PropertyGroup>

  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>DynamicLibrary</ConfigurationType>
    <PlatformToolset>$currentplatformtoolset$</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
    <WholeProgramOptimization>true</WholeProgramOptimization>
  </PropertyGroup>

  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />

  <!-- NuGet 패키지 참조 -->
  <ItemGroup>
    <PackageReference Include="VisionMotionFramework.Core.$(PlatformToolset)">
      <Version>$vmfversion$</Version>
    </PackageReference>
  </ItemGroup>

  <PropertyGroup>
    <!-- 출력 DLL 명명 규칙: ProjectName_Platform_Configuration.dll -->
    <TargetName>$(ProjectName)_$(Platform)_$(Configuration)</TargetName>
    <OutDir>$(SolutionDir)bin\$(Platform)\$(Configuration)\</OutDir>
  </PropertyGroup>

  <ItemDefinitionGroup Condition="'$(Configuration)'=='Release'">
    <ClCompile>
      <Optimization>MaxSpeed</Optimization>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>  <!-- /MD -->
      <LanguageStandard>stdcpp14</LanguageStandard>       <!-- /std:c++14 -->
      <AdditionalIncludeDirectories>
        $(ProjectDir);%(AdditionalIncludeDirectories)
      </AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
      <OptimizeReferences>true</OptimizeReferences>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>

  <!-- 소스 파일 목록 -->
  <ItemGroup>
    <ClInclude Include="PluginFactory.h" />
    <ClInclude Include="Tasks\SampleMoveToStartPositionTask.h" />
    <ClInclude Include="Tasks\SamplePerformFocusScanningTask.h" />
    <ClInclude Include="Sequences\SampleZFocusSequenceBuilder.h" />
    <ClInclude Include="Strategies\SampleStrategy.h" />
    <ClInclude Include="stdafx.h" />
    <ClInclude Include="targetver.h" />
  </ItemGroup>

  <ItemGroup>
    <ClCompile Include="DllMain.cpp" />
    <ClCompile Include="Tasks\SampleMoveToStartPositionTask.cpp" />
    <ClCompile Include="Tasks\SamplePerformFocusScanningTask.cpp" />
    <ClCompile Include="stdafx.cpp" />
  </ItemGroup>

  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
```

### 4.4 `PluginFactory.h` (DLL 진입점 인터페이스)

```cpp
#pragma once

#include "ComponentSetupBase.h"

#ifdef PLUGIN_EXPORTS
#define PLUGIN_API __declspec(dllexport)
#else
#define PLUGIN_API __declspec(dllimport)
#endif

extern "C"
{
    /// <summary>
    /// Strategy 인스턴스를 생성합니다.
    /// 메인 애플리케이션은 LoadLibrary + GetProcAddress로 이 함수를 호출합니다.
    /// </summary>
    /// <returns>VMF::ComponentSetupBase* (VMF::ComponentSetupBase 상속 객체)</returns>
    PLUGIN_API VMF::ComponentSetupBase* CreateSetupStrategy();

    /// <summary>
    /// CreateSetupStrategy로 생성된 인스턴스를 소멸합니다.
    /// </summary>
    PLUGIN_API void DestroySetupStrategy(VMF::ComponentSetupBase* pStrategy);
}
```

### 4.5 `DllMain.cpp` (샘플 구현)

```cpp
// DllMain.cpp
// 주의: 이 파일은 샘플입니다. 실제 장비에 맞게 Strategy 클래스를 교체하세요.

#include "stdafx.h"
#include "PluginFactory.h"
#include "Strategies/SampleStrategy.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

PLUGIN_API VMF::ComponentSetupBase* CreateSetupStrategy()
{
    // ★ 여기서 원하는 Strategy 클래스로 교체하세요 ★
    // 예: return new VMF_Load1::CLoad1LeftPlateJIGFocusCheckSequenceStrategy();
    return new SampleStrategy();
}

PLUGIN_API void DestroySetupStrategy(VMF::ComponentSetupBase* pStrategy)
{
    delete pStrategy;
}
```

### 4.6 샘플 Task 코드 (`SampleMoveToStartPositionTask.h`)

```cpp
#pragma once

#include "NonBlockingTaskBase.h"

/// <summary>
/// [샘플] 시작 위치로 이동하는 Task입니다.
/// 복사하여 장비에 맞게 수정하세요.
/// </summary>
class SampleMoveToStartPositionTask : public VMF::NonBlockingTaskBase
{
    enum Substep
    {
        MoveSafeZ = 0,
        MoveOrigin,
        MoveTargetPositionXY,
        MoveTargetPositionZ,
        CompleteMove,
    };

    VMF::TaskResult HandleMoveSafeZ(VMF::Context& ctx, VMF::IActuator* actuator);
    VMF::TaskResult HandleMoveOrigin(VMF::Context& ctx, VMF::IActuator* actuator);
    VMF::TaskResult HandleMoveTargetPositionXY(VMF::Context& ctx, VMF::IActuator* actuator);
    VMF::TaskResult HandleMoveTargetPositionZ(VMF::Context& ctx, VMF::IActuator* actuator);
    VMF::TaskResult HandleCompleteMove(VMF::Context& ctx, VMF::IActuator* actuator);

public:
    SampleMoveToStartPositionTask();
    virtual ~SampleMoveToStartPositionTask();

    std::string GetName() const override
    {
        return "Task_MoveToStartPosition";
    }

protected:
    void OnInitialize(VMF::Context& ctx) override;
    VMF::TaskResult OnPoll(VMF::Context& ctx, VMF::IActuator* actuator) override;

    std::vector<double> m_targetPosition;
    long m_moveTimeoutMs;
};
```

### 4.7 샘플 SequenceBuilder (`SampleZFocusSequenceBuilder.h`)

```cpp
#pragma once

#include "SequenceBuilderBase.h"
#include "Tasks/SampleMoveToStartPositionTask.h"
#include "Tasks/SamplePerformFocusScanningTask.h"

/// <summary>
/// [샘플] Z-Focus 시퀀스를 구축합니다.
/// 복사하여 장비에 맞게 Task 순서/종류를 수정하세요.
/// </summary>
class SampleZFocusSequenceBuilder : public VMF::SequenceBuilderBase
{
protected:
    VMF::SequencePtr BuildSequence(const std::string& sequenceName) override
    {
        VMF::SequencePtr seq(new VMF::Sequence(sequenceName));
        seq->AddTask(VMF::TaskPtr(new SampleMoveToStartPositionTask()));
        seq->AddTask(VMF::TaskPtr(new SamplePerformFocusScanningTask()));
        return seq;
    }
};
```

### 4.8 샘플 Strategy (`SampleStrategy.h`)

```cpp
#pragma once

#include "MemorySequenceStrategy.h"
#include "SampleZFocusSequenceBuilder.h"

/// <summary>
/// [샘플] Strategy 구현 예제입니다.
/// ConfigureParams에서 장비별 파라미터를 설정합니다.
/// 복사하여 실제 장비(Load1, Load2, Picker 등)에 맞게 수정하세요.
/// </summary>
class SampleStrategy : public VMF::MemorySequenceStrategy
{
public:
    std::string GetSequenceName() const override
    {
        return "SampleZFocus";
    }

    VMF::SequenceBuilderPtr CreateBuilder() override
    {
        return std::make_shared<SampleZFocusSequenceBuilder>();
    }

    void ConfigureParams(VMF::VisionContextPtr ctx) override
    {
        VMF::VisionParams params;

        // ★ 장비별 파라미터를 여기에 설정하세요 ★
        SetParam(params, "CameraIndex", 1);
        SetParam(params, "HandID", 1);
        SetParam(params, "PkgID", 1);
        SetParam(params, "InspectionType", 1);
        SetParam(params, "nMovePart", 0);

        ctx->SetVisionParams(params);
    }
};
```

### 4.9 템플릿 메타데이터 (`MyTemplate.vstemplate`)

```xml
<VSTemplate Version="3.0.0" Type="Project"
    xmlns="http://schemas.microsoft.com/developer/vstemplate/2005">
  <TemplateData>
    <Name>VisionMotionFramework Equipment Plugin</Name>
    <Description>
      VisionMotionFramework 장비별 DLL 프로젝트 템플릿입니다.
      Task/Sequence/Strategy를 구현하여 장비 DLL을 생성합니다.
    </Description>
    <ProjectType>VC</ProjectType>
    <ProjectSubType>
    </ProjectSubType>
    <SortOrder>1000</SortOrder>
    <CreateNewFolder>true</CreateNewFolder>
    <DefaultName>EquipmentPlugin</DefaultName>
    <ProvideDefaultName>true</ProvideDefaultName>
    <LocationField>Enabled</LocationField>
    <EnableLocationBrowseButton>true</EnableLocationBrowseButton>
    <Icon>__TemplateIcon.ico</Icon>
    <PreviewImage>__PreviewImage.png</PreviewImage>
  </TemplateData>
  <TemplateContent>
    <Project TargetFileName="$safeprojectname$.vcxproj"
             File="EquipmentPlugin.vcxproj"
             ReplaceParameters="true">
      <Folder Name="Tasks">
        <ProjectItem ReplaceParameters="true"
                     TargetFileName="Tasks\SampleMoveToStartPositionTask.h">
          Tasks\SampleMoveToStartPositionTask.h
        </ProjectItem>
        <ProjectItem ReplaceParameters="true"
                     TargetFileName="Tasks\SampleMoveToStartPositionTask.cpp">
          Tasks\SampleMoveToStartPositionTask.cpp
        </ProjectItem>
        <ProjectItem ReplaceParameters="true"
                     TargetFileName="Tasks\SamplePerformFocusScanningTask.h">
          Tasks\SamplePerformFocusScanningTask.h
        </ProjectItem>
        <ProjectItem ReplaceParameters="true"
                     TargetFileName="Tasks\SamplePerformFocusScanningTask.cpp">
          Tasks\SamplePerformFocusScanningTask.cpp
        </ProjectItem>
      </Folder>
      <Folder Name="Sequences">
        <ProjectItem ReplaceParameters="true"
                     TargetFileName="Sequences\SampleZFocusSequenceBuilder.h">
          Sequences\SampleZFocusSequenceBuilder.h
        </ProjectItem>
      </Folder>
      <Folder Name="Strategies">
        <ProjectItem ReplaceParameters="true"
                     TargetFileName="Strategies\SampleStrategy.h">
          Strategies\SampleStrategy.h
        </ProjectItem>
      </Folder>
      <ProjectItem ReplaceParameters="true" TargetFileName="DllMain.cpp">
        DllMain.cpp
      </ProjectItem>
      <ProjectItem ReplaceParameters="true" TargetFileName="PluginFactory.h">
        PluginFactory.h
      </ProjectItem>
      <ProjectItem ReplaceParameters="true" TargetFileName="stdafx.h">
        stdafx.h
      </ProjectItem>
      <ProjectItem ReplaceParameters="true" TargetFileName="targetver.h">
        targetver.h
      </ProjectItem>
      <ProjectItem ReplaceParameters="true" TargetFileName="ReadMe.txt">
        ReadMe.txt
      </ProjectItem>
    </Project>
  </TemplateContent>
  <WizardExtension>
    <Assembly>NuGet.VisualStudio.Interop, Version=1.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a</Assembly>
    <FullClassName>NuGet.VisualStudio.TemplateWizard</FullClassName>
  </WizardExtension>
</VSTemplate>
```

### 4.10 `ReadMe.txt` (템플릿 사용법)

```
========================================================================
 VisionMotionFramework Equipment Plugin - 프로젝트 생성 완료!
========================================================================

축하합니다! VisionMotionFramework Equipment Plugin 프로젝트가 생성되었습니다.

[프로젝트 구조]
  Tasks/         - 장비별 Task 구현 (NonBlockingTaskBase 상속)
  Sequences/     - 시퀀스 빌더 (SequenceBuilderBase 상속)
  Strategies/    - 전략 클래스 (MemorySequenceStrategy 상속)
  PluginFactory.h - DLL 인터페이스 (CreateSetupStrategy)
  DllMain.cpp    - DLL 진입점 + 팩토리 함수 구현

[시작 가이드]
1. NuGet 패키지 복원
   - 솔루션 탐색기에서 우클릭 → "NuGet 패키지 복원"
   - 자동으로 VisionMotionFramework.Core가 참조됩니다.

2. Strategy 작성
   - Strategies/SampleStrategy.h를 복사하여 장비명으로 변경
   - GetSequenceName(), CreateBuilder(), ConfigureParams() 구현

3. Task 작성
   - Tasks/ 폴더에 실제 장비 동작에 맞는 Task 클래스 구현
   - NonBlockingTaskBase 상속 → OnInitialize() + OnPoll() 구현

4. SequenceBuilder 작성
   - Sequences/ 폴더에 Task 조립 로직 구현

5. PluginFactory 연결
   - DllMain.cpp의 CreateSetupStrategy()에서 자신의 Strategy 반환

6. 빌드
   - 빌드하면 bin/x64/Release/EquipmentPlugin_x64_Release.dll 생성

7. 메인 APP에서 로드
   - LoadLibrary("EquipmentPlugin_x64_Release.dll")
   - GetProcAddress("CreateSetupStrategy")
   - 반환된 ComponentSetupBase*로 시퀀스 실행

[참고]
- 모든 코드는 ISO C++14 표준을 따릅니다.
- CRT는 동적(/MD)으로 링크됩니다.
- Debug/Release 모두 빌드하여 배포하세요.
```

---

## 5. Mock 라이브러리 배포 전략

### 5.1 Mock의 위치

**결론: Mock은 NuGet 패키지에 포함** (VS 템플릿에 포함하지 않음)

### 5.2 Mock 포함 항목

| 파일 | 설명 |
|------|------|
| `CMockDataRepository.h` | `IDataRepository`의 Mock 구현 (메모리 내 저장) |
| `CMockDataRepository.cpp` | 구현 소스 |
| `CMockVisionEventHandler.h` | `IVisionProcessor`의 Mock 구현 (가상 결과 생성) |
| `CMockVisionEventHandler.cpp` | 구현 소스 |

### 5.3 Mock이 NuGet에 포함되어야 하는 이유

1. **공용 의존성**: Mock은 `IDataRepository`와 `IVisionProcessor` 인터페이스를 구현한 공용 테스트 더블로, 모든 Equipment에서 동일하게 사용됨
2. **변경 빈도 낮음**: 인터페이스가 바뀌지 않는 한 Mock 코드는 거의 수정되지 않음
3. **템플릿 경량화**: 사용자에게 보여줄 필요 없는 코드를 템플릿에서 제외
4. **재사용성**: 단위 테스트, 시뮬레이션 등 다양한 용도로 재사용 가능

### 5.4 Mock 사용 예제 (`DefaultSetupStrategy.h`)

NuGet의 Mock을 사용하는 방법은 이미 `DefaultSetupStrategy.h`에 구현되어 있습니다:

```cpp
#include "VisionMotionFramework/Mock/CMockDataRepository.h"
#include "VisionMotionFramework/Mock/CMockVisionEventHandler.h"

VMF::DataRepositoryPtr CreateRepository() override
{
    // 생산 모드: 실제 DB 사용
    // auto repo = std::make_shared<VMF::SqliteDataRepository>("Data\\VAT_DATABASE.db", "Data\\Images");

    // 테스트/시뮬레이션 모드: Mock 사용
    auto repo = std::make_shared<VMF::CMockDataRepository>();
    repo->Initialize();
    return repo;
}
```

### 5.5 Mock 배포 방식

NuGet 패키지에서 Mock 소스(`.cpp`)는 `contentFiles`로 포함합니다:

```xml
<!-- .nuspec -->
<file src="..\Equipment2015\VMFComposition\Mock\*.h"
      target="include\VisionMotionFramework\Mock\" />
<file src="..\Equipment2015\VMFComposition\Mock\*.cpp"
      target="contentFiles\any\any\VisionMotionFramework\Mock\" />
```

`contentFiles`로 포함된 `.cpp`는 소비자 프로젝트에서 자동으로 컴파일됩니다.  
(또는 미리 빌드된 `VMF.Mock.lib`를 별도로 제공할 수 있습니다.)

---

## 6. 메인 애플리케이션 연동 방식

### 6.1 DLL 로딩 및 실행 흐름

```cpp
// 메인 애플리케이션 (EXE)
#include "ComponentSetupBase.h"
#include "IDataRepository.h"
#include "IVisionProcessor.h"
#include "RunController.h"

void RunEquipmentSequence()
{
    // 1. 장비 DLL 로드 (Equipment Plugin)
    HMODULE hDll = LoadLibrary(L"Equipment2015_x64_Release.dll");
    if (!hDll) throw std::runtime_error("DLL 로드 실패");

    // 2. 팩토리 함수 획득
    auto fnCreate = (VMF::ComponentSetupBase* (*)())
        GetProcAddress(hDll, "CreateSetupStrategy");
    auto fnDestroy = (void (*)(VMF::ComponentSetupBase*))
        GetProcAddress(hDll, "DestroySetupStrategy");

    // 3. Strategy 생성
    VMF::ComponentSetupBase* strategy = fnCreate();

    // 4. Repository, VisionProcessor 생성
    auto repo = strategy->CreateRepository();       // NuGet Core
    auto vision = strategy->CreateVisionProcessor(); // NuGet Core

    // 5. Context 구성
    auto ctx = std::make_shared<VMF::VisionContext>(repo, vision, /*...*/);

    // 6. Strategy 파라미터 설정
    strategy->ConfigureParams(ctx);

    // 7. Sequence Builder 생성
    auto builder = strategy->CreateBuilder();

    // 8. Sequence 실행
    VMF::RunController controller;
    controller.SetSequenceBuilder(builder);
    controller.SetContext(ctx);
    VMF::TaskResult result = controller.Run();

    // 9. 정리
    fnDestroy(strategy);
    FreeLibrary(hDll);
}
```

### 6.2 의존성 그래프

```
EquipmentPlugin.dll
    ├── VMF.Core.lib (NuGet 정적 링크)
    │   ├── NonBlockingTaskBase
    │   ├── ComponentSetupBase
    │   ├── SequenceBuilderBase
    │   └── ...
    ├── EquipmentCore.lib (NuGet 정적 링크)
    │   ├── ITask, IActivity
    │   ├── AsyncExecutor
    │   └── ...
    └── VisionComm.lib (NuGet 정적 링크)
        ├── Controller
        ├── TCPClient
        └── ...
```

---

## 7. CI/CD 자동화

### 7.1 GitHub Actions Workflow (`.github/workflows/build-deploy.yml`)

```yaml
name: Build and Deploy VMF NuGet Packages

on:
  push:
    tags:
      - 'v*'          # v1.0.0, v1.1.0 등
  workflow_dispatch:   # 수동 실행

jobs:
  build-v100:
    runs-on: windows-2010  # VS2010 전용 에이전트
    steps:
      - uses: actions/checkout@v3
      - name: Build v100
        run: |
          msbuild VisionMotionFramework.sln /t:VMF_Core /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v100
          msbuild VisionMotionFramework.sln /t:VMF_Core /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v100
      - name: Pack v100
        run: nuget pack VisionMotionFramework.Core.v100.nuspec -Version ${{ github.ref_name }}
      - name: Push v100
        run: nuget push *.nupkg -Source https://api.nuget.org/v3/index.json -ApiKey ${{ secrets.NUGET_API_KEY }}

  build-v140:
    runs-on: windows-2016  # VS2017 에이전트
    steps:
      - uses: actions/checkout@v3
      - name: Build v140
        run: |
          msbuild VisionMotionFramework.sln /t:VMF_Core /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v140
          msbuild VisionMotionFramework.sln /t:VMF_Core /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v140
      - name: Pack v140
        run: nuget pack VisionMotionFramework.Core.v140.nuspec -Version ${{ github.ref_name }}
      - name: Push v140
        run: nuget push *.nupkg -Source https://api.nuget.org/v3/index.json -ApiKey ${{ secrets.NUGET_API_KEY }}

  build-v142:
    runs-on: windows-2022  # VS2022 에이전트
    steps:
      - uses: actions/checkout@v3
      - name: Build v142 & v143
        run: |
          msbuild VisionMotionFramework.sln /t:VMF_Core /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v142
          msbuild VisionMotionFramework.sln /t:VMF_Core /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v142
          msbuild VisionMotionFramework.sln /t:VMF_Core /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v143
          msbuild VisionMotionFramework.sln /t:VMF_Core /p:Configuration=Release /p:Platform=x64 /p:PlatformToolset=v143
      - name: Pack v142
        run: nuget pack VisionMotionFramework.Core.v142.nuspec -Version ${{ github.ref_name }}
      - name: Push v142
        run: nuget push *.nupkg -Source https://api.nuget.org/v3/index.json -ApiKey ${{ secrets.NUGET_API_KEY }}
```

### 7.2 NuGet 패키지 업로드 명령어

```bash
# 개발/테스트 피드
nuget push VisionMotionFramework.Core.v142.1.0.0.nupkg -Source http://localhost:5555/v3/index.json -ApiKey dev-key

# 프로덕션 NuGet.org
nuget push VisionMotionFramework.Core.v142.1.0.0.nupkg -Source https://api.nuget.org/v3/index.json -ApiKey <NUGET_API_KEY>
```

---

## 8. 추가 고려사항

### 8.1 C++14 표준 준수

모든 코드는 ISO C++14로 작성:
- `<memory>`, `<thread>`, `<mutex>`, `<chrono>` 등 C++11/14 기능 사용
- `<filesystem>` (C++17) 사용 금지 → 대신 C++14 호환 경로 처리 사용
- `make_unique` (C++14) → 자체 구현 또는 `std::unique_ptr(new T)` 사용
- `string_view` (C++17) → `const std::string&` 또는 `const char*` 사용

### 8.2 동적 CRT (/MD) 사용 이유

- 다중 DLL 간 CRT 상태 공유
- 메모리 할당/해제 일관성 유지
- Microsoft VC++ Redistributable을 통한 런타임 배포

### 8.3 VS2010 (v100) 특별 고려사항

- C++14 표준 미지원 → 조건부 컴파일 필요
  ```cpp
  #if _MSC_VER >= 1900  // VS2015+
      // C++14 코드
  #else
      // VS2010 호환 코드 (C++03 수준)
  #endif
  ```
- `std::shared_ptr`, `std::mutex` 등은 VS2010 SP1에서 제공
- `std::chrono`는 VS2010에서 제한적 → 자체 타이머 클래스 필요 가능

### 8.4 버전 관리 정책

- **Semantic Versioning (SemVer)** 준수
- 메이저 버전: 인터페이스 변경 (ABI 브레이킹)
- 마이너 버전: 기능 추가 (ABI 호환)
- 패치 버전: 버그 수정 (ABI 호환)
- 모든 패키지(v100/v140/v142)는 동일한 버전 번호 사용

### 8.5 심볼 패키지 (선택 사항)

```bash
nuget pack VisionMotionFramework.Core.v142.nuspec -Symbols -SymbolPackageFormat snupkg
nuget push VisionMotionFramework.Core.v142.1.0.0.snupkg -Source https://api.nuget.org/v3/index.json
```

---

## 9. 배포 체크리스트

### 9.1 최초 배포 시 확인 항목

- [ ] 각 Toolset별 빌드 성공 확인 (v100, v140, v142, v143)
- [ ] 3개 `.nuspec` 파일 준비 완료
- [ ] MSBuild targets 파일 작성 완료
- [ ] Mock 라이브러리(contentFiles) 포함 확인
- [ ] CRT 의존성 올바르게 설정
- [ ] VS Project Template ZIP 생성 완료
- [ ] 템플릿 WizardExtension (NuGet 자동 참조) 설정 완료
- [ ] 템플릿에서 새 프로젝트 생성 → 빌드 → DLL 생성 테스트 완료
- [ ] 메인 APP에서 DLL 로드 → 시퀀스 실행 테스트 완료

### 9.2 버전 업데이트 시 확인 항목

- [ ] 변경 사항이 NuGet에 속하는지, 템플릿에 속하는지 결정
- [ ] 인터페이스 변경 시 모든 패키지 동시 업데이트
- [ ] 릴리스 노트 작성
- [ ] 모든 패키지 동일 버전으로 푸시

---

## 10. 결론

### 10.1 권장 배포 전략 요약

| 구성 요소 | 배포 방식 | 변경 주기 |
|-----------|-----------|-----------|
| Core Engine (NonBlockingTaskBase, ComponentSetupBase, SequenceBuilderBase 등) | **NuGet 패키지** (3개: v100/v140/v142) | 거의 없음 |
| 인터페이스 (IDataRepository, IVisionProcessor, IActuator 등) | **NuGet 패키지** (헤더) | 드묾 |
| Mock (CMockDataRepository, CMockVisionEventHandler) | **NuGet 패키지** (contentFiles) | 거의 없음 |
| 장비별 Task (CLoad1MoveToStartPositionTask 등) | **VS Project Template** (소스 코드) | 자주 변경 |
| 장비별 SequenceBuilder (CLoad1ZFocusSequenceBuilder 등) | **VS Project Template** (소스 코드) | 자주 변경 |
| 장비별 Strategy (CLoad1LeftPlateJIGFocusCheckSequenceStrategy 등) | **VS Project Template** (소스 코드) | 자주 변경 |
| 장비별 Param 상수 (DefineVAT.h 등) | **NuGet 패키지** 또는 **Template** | 경우에 따라 다름 |

### 10.2 핵심 장점

1. **분리된 변경 주기**: 엔진(NuGet)은 안정적, 장비 코드(Template)는 유연하게 변경
2. **플랫폼 호환성**: 3개 패키지로 VS2010~VS2026 전 범위 커버
3. **재사용성**: Mock이 NuGet에 있어 모든 장비에서 테스트/시뮬레이션 가능
4. **확장성**: 새 장비 추가 시 템플릿으로 프로젝트 생성 → Task/Sequence/Strategy만 작성
5. **유지보수성**: 엔진 버그 수정 시 NuGet만 업데이트, 각 장비 DLL 재빌드 필요 없음

---

> **문서 버전**: 1.0  
> **마지막 수정일**: 2025-01-24  
> **작성자**: VisionMotionFramework Team
