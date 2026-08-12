# VisionMotionFramework 네이밍 규칙

## 1. 네이밍 컨벤션 개요

VisionMotionFramework 프로젝트의 모든 코드 요소는 일관된 네이밍 규칙을 따라야 합니다.

### 1.1 기본 원칙

- **C++14 표준 준수**: 소문자 헤더 사용, `std::` 네임스페이스 사용
- **Modern C++ 관례**: C 접두사/접미사 최소화
- **일관성**: 프로젝트 전체에서 동일한 패턴 적용

## 2. 네이밍 패턴

### 2.1 일반적인 네이밍 구조

```
[Unit][기능][행위/역할]
```

| 구성 요소 | 설명 | 예시 |
|----------|------|------|
| Unit | 기구 구성 Unit | SetPlate1, SetPlate2 |
| 기능 | 기능/기술 | PLVI, Vision, Motion |
| 행위/역할 | 역할/행위 | Strategy, Builder, Task |

### 2.2 클래스 네이밍 규칙

#### 2.2.1 전략 클래스 (Strategy)
```
[Unit][기능]Strategy
```
**예시**: `SetPlate1PLVIStrategy`

#### 2.2.2 빌더 클래스 (Builder)
```
[Unit][기능]SequenceBuilder
```
**예시**: `SetPlate1PLVISequenceBuilder`

#### 2.2.3 작업 클래스 (Task)
```
[Unit][기능][행위]
```
**예시**: 
- `SetPlate1PLVISetup`
- `SetPlate1PLVIExecuteScan`
- `SetPlate1PLVIFinish`

## 3. 네이밍 세부 규칙

### 3.1 클래스 네이밍

| 요소 | 규칙 | 적용 예시 |
|------|------|----------|
| 클래스명 | PascalCase | `SetPlate1PLVIStrategy` |
| 네임스페이스 | PascalCase | `VMF`, `VMF_PLUGIN` |
| 접두사 | Modern C++ 관례에 따라 최소화 | `C` 접두사 제거 |
| 접미사 | 컨텍스트에서 명시적인 경우 제거 | `Task` 접미사 제거 |

### 3.2 파일 네이밍

| 파일 유형 | 규칙 | 예시 |
|----------|------|------|
| 헤더 파일 | `[클래스명].h` | `SetPlate1PLVIStrategy.h` |
| 구현 파일 | `[클래스명].cpp` | `SetPlate1PLVIStrategy.cpp` |

### 3.3 함수/변수 네이밍

| 요소 | 규칙 | 예시 |
|------|------|------|
| 함수명 | camelCase | `SetParam`, `AddVisionPoint` |
| 변수명 | camelCase | `params`, `repo`, `ctx` |
| 상수 | UPPER_SNAKE_CASE | `MAX_RETRY_COUNT` |

### 3.4 매크로 네이밍

| 요소 | 규칙 | 예시 |
|------|------|------|
| 매크로 | UPPER_SNAKE_CASE | `#define VMF_API` |

## 4. SetPlate1PLVI 클래스 네이밍 표

### 4.1 현재 → 제안 네이밍

| 현재 이름 | 제안 이름 | 비고 |
|----------|----------|------|
| `SetPlate1PLVIStrategy` | `SetPlate1PLVIStrategy` | 유지 |
| `CSetPlate1PLVISequenceBuilder` | `SetPlate1PLVISequenceBuilder` | C 접두사 제거 |
| `CSetPlate1PLVISetupTask` | `SetPlate1PLVISetup` | C 접두사 제거, Task 제거 |
| `CSetPlate1PLVIExecuteScanTask` | `SetPlate1PLVIExecuteScan` | C 접두사 제거, Task 제거 |
| `CSetPlate1PLVIFinishTask` | `SetPlate1PLVIFinish` | C 접두사 제거, Task 제거 |

### 4.2 네이밍 구조 분석

| 클래스 | Unit | 기능 | 행위/역할 |
|--------|------|------|----------|
| `SetPlate1PLVIStrategy` | SetPlate1 | PLVI | Strategy |
| `SetPlate1PLVISequenceBuilder` | SetPlate1 | PLVI | SequenceBuilder |
| `SetPlate1PLVISetup` | SetPlate1 | PLVI | Setup |
| `SetPlate1PLVIExecuteScan` | SetPlate1 | PLVI | ExecuteScan |
| `SetPlate1PLVIFinish` | SetPlate1 | PLVI | Finish |

## 5. 적용 사례

### 5.1 SetPlate1PLVIStrategy.h
```cpp
#pragma once

#include "DefaultSetupStrategy.h"
#include "VMFEquipmentPluginExport.h"
#include "types.h"

namespace VMF_PLUGIN
{
    // 전방 선언만 필요한 클래스들
    class VisionPlviProcessor;
    class SetPlate1PLVISequenceBuilder;
    class SqliteDataRepository;

    class VMF_PLUGIN_API SetPlate1PLVIStrategy : public VMF::DefaultSetupStrategy
    {
    private:
        // Helper: VisionParams에 값 설정
        void SetParam(VMF::VisionParams& params, const std::string& key, const std::string& value);
        void SetParam(VMF::VisionParams& params, const std::string& key, int value);
        void SetParam(VMF::VisionParams& params, const std::string& key, double value);

        // Helper: VisionPosition 추가
        void AddVisionPoint(VMF::VisionParams& params, int locateId, int requestId,
            double x, double y, double z);

        // Helper: VisionParams를 Repository에 저장
        void SaveVisionParamsToRepo(VMF::VisionContextPtr ctx,
            const VMF::VisionParams& params);

    public:
        std::string GetSequenceName() const override;
        VMF::DataRepositoryPtr CreateRepository() override;
        VMF::VisionProcessorPtr CreateVisionProcessor() override;
        VMF::SequenceBuilderPtr CreateBuilder() override;
        void ConfigureParams(VMF::VisionContextPtr ctx) override;
        VMF::StringMap GetVisionParams(const std::string& presetName) const override;
    };
}
```

### 5.2 SetPlate1PLVIStrategy.cpp
```cpp
#include "pch.h"
#include "SetPlate1PLVIStrategy.h"
#include "SqliteDataRepository.h"
#include "..\\Protocol\\VisionPlviProcessor.h"
#include "..\\Strategies\\SetPlate1PLVISequenceBuilder.h"
#include <sstream>
#include <vector>

using namespace VMF;
using namespace VMF_PLUGIN;

// ... (기존 구현 내용 유지)

VMF::SequenceBuilderPtr SetPlate1PLVIStrategy::CreateBuilder()
{
    return std::make_shared<VMF_PLUGIN::SetPlate1PLVISequenceBuilder>();
}
```

## 6. 네이밍 변경 체크리스트

### 6.1 변경 전 확인 사항
- [ ] 파일명 변경 (헤더/구현)
- [ ] 클래스명 변경
- [ ] 전방 선언 변경
- [ ] 인클루드 경로 변경
- [ ] std::make_shared 호출 변경
- [ ] 문서/주석 업데이트
- [ ] 테스트 코드 업데이트

### 6.2 변경 후 검증 사항
- [ ] 컴파일 오류 없음
- [ ] 모든 테스트 통과
- [ ] 문서 일관성 유지
- [ ] Git 히스토리 추적 가능

## 7. 네이밍 예시 카테고리

### 7.1 인터페이스 클래스
| 패턴 | 예시 |
|------|------|
| `I[기능]` | `IActuator`, `IDataRepository` |
| `I[기능]Base` | `SequenceBuilderBase` |

### 7.2 구현 클래스
| 패턴 | 예시 |
|------|------|
| `[Unit][기능]Strategy` | `SetPlate1PLVIStrategy` |
| `[Unit][기능]SequenceBuilder` | `SetPlate1PLVISequenceBuilder` |
| `[Unit][기능][행위]` | `SetPlate1PLVISetup` |

### 7.3 헬퍼/유틸리티
| 패턴 | 예시 |
|------|------|
| `[기능][작업]Helper` | `VisionParamsHelper` |
| `[기능]Util` | `StringUtil` |

## 8. 부가 권장 사항

### 8.1 코드 스타일
- **들여쓰기**: 4 spaces (탭 금지)
- **줄 끝**: Windows CRLF (`\r\n`)
- **주석**: Doxygen 스타일 `/** */` 사용

### 8.2 네임스페이스
- **공개 API**: `VMF` 네임스페이스
- **플러그인**: `VMF_PLUGIN` 네임스페이스

### 8.3 예외 규칙
- 기존 코드와의 호환성을 위해 점진적 마이그레이션 진행
- 새로 추가하는 코드는 반드시 이 규칙 준수

---

**버전**: 1.0  
**작성일**: 2024  
**작성자**: VisionMotionFramework 팀
