# CONTRIBUTING.md

## 목적
이 문서는 코드베이스의 일관성 유지, 가독성 향상, 그리고 안전한 점진적 리팩토링을 위한 팀 규칙과 절차를 안내합니다. 주요 목표는 다음과 같습니다:

- 코드 스타일과 네이밍을 표준화하여 리뷰·유지보수 비용을 낮춥니다.
- 런타임 식별자와 소스 구조(파일/디렉터리) 간의 명확한 매핑을 유지합니다.
- GetName 등 런타임 식별자 변경 시 안전한 마이그레이션 절차를 제공합니다.

## 핵심 원칙(요약)
- Unit 단위 네임스페이스 유지: `VAT_LOAD1`, `VAT_LOAD2` 등
- Unit 내부에 역할별 하위 네임스페이스 권장: `Sequence`, `Task`, `Strategies`
- 모든 non-interface 클래스는 `C` 접두사 + PascalCase 사용
- 파일명은 클래스명과 1:1 대응(`ClassName.h` / `ClassName.cpp`)
- `GetName()` 문자열 표준: `"{Unit}.{Sequence}.{Task}"`

---

## 인터페이스 표기 규칙
- 규칙: 모든 인터페이스 타입(추상 클래스)은 `I` 접두사를 사용하여 표기합니다.
  - 예: `IVisionProcessor`, `IDataRepository`, `IResultSink`
- 이유:
  - 구현체(구체 클래스)와 인터페이스를 빠르게 구분할 수 있음
  - DI(Dependency Injection)과 테스트 더블 작성 시 명확성 제공
- 예외:
  - 기존 레거시 코드 중 광범위하게 노출된 관례가 있다면, 리팩토링 플랜을 수립한 뒤 점진적으로 `I` 접두 규칙으로 통일합니다.

---

## 파일 인코딩 및 헤더 보호 규정
- 모든 소스(헤더/소스/스크립트)는 UTF-8 (BOM 없음 권장)으로 저장합니다.
  - 이유: 한글 주석/문자열의 손실을 방지하고 CI 환경의 인코딩 불일치를 줄입니다.
  - 에디터 설정 예: Visual Studio에서 파일 → 고급 저장 옵션 → UTF-8
- 모든 헤더 파일은 `#pragma once`를 사용하여 중복 포함을 방지합니다.
  - 이유: 간결하고 플랫폼에서 널리 지원됩니다.
  - 예외적으로 표준 include-guard를 사용해야 하는 특별한 이유가 있는 경우(크로스 컴파일러 호환 등)에는 팀에 사전 고지 후 사용합니다.
- Boost 등 서드파티 헤더로 인한 매크로 충돌(예: `new`/`DEBUG_NEW`)을 방지하기 위해 공용 헤더에서 직접 서드파티가 노출되지 않도록 합니다.
  - 권장: 서드파티 의존은 `.cpp`에 국한하거나 PIMPL 패턴으로 숨기십시오.
  - 필요 시, Boost 포함 전후로 매크로 보호(`#pragma push_macro("new")` / `#undef new` / `#pragma pop_macro("new")`) 사용을 권장합니다.

---

## NuGet 패키지 배포 전략

### 배포 모델 및 Toolset 지원

VisionMotionFramework는 **2개의 독립적인 NuGet 패키지**로 배포됩니다:

#### 패키지 1: VisionMotionFramework.v100 (레거시 지원)
- **대상**: Visual Studio 2010 (v100 Toolset)
- **CRT**: msvcr100.dll
- **소스**: VisionMotionFramework/src-v100/ 에서 v100 호환 코드
- **구성**: 
  - lib/net45/v100/ → VisionMotionFramework.dll (v100)
  - include/ → 공개 헤더

#### 패키지 2: VisionMotionFramework (최신 지원)
- **대상**: Visual Studio 2015 이상 (v140 Toolset)
  - v140 (VS2015)
  - v141 (VS2017)
  - v142 (VS2019)
  - v143 (VS2022)
  - 모두 동일 CRT (msvcr140.dll) 사용
- **CRT**: msvcr140.dll (v140, v141, v142, v143 공통)
- **소스**: VisionMotionFramework/src-v140/ 에서 v140+ 코드
- **구성**:
  - lib/net45/v140/ → VisionMotionFramework.dll (v140+)
  - include/ → 공개 헤더

### Toolset 호환성 정책
- **v140, v141, v142, v143은 동일 CRT 사용**: 1개 바이너리로 모두 지원
- **Props 파일에서 자동 매핑**:
  ```xml
  v140 → v140
  v141 → v140
  v142 → v140
  v143 → v140
  ```
- **결과**: 배포 패키지 단순화, 빌드 횟수 최소화

### 개발자 워크플로우

#### v100 환경 (VS2010)
```
1. NuGet 설치: Install-Package VisionMotionFramework.v100
2. 자동으로 v100용 DLL + 헤더 다운로드
3. 프로젝트의 Toolset이 v100이면 자동 인식
4. Equipment App (v100) ↔ Plugin (v100) 일치
```

#### v140+ 환경 (VS2015 이상)
```
1. NuGet 설치: Install-Package VisionMotionFramework
2. 자동으로 v140용 DLL + 헤더 다운로드
3. Props에서 현재 Toolset 감지
   - v140: 그대로 사용
   - v141/v142/v143: v140으로 매핑
4. 모든 VS 버전에서 동일 DLL 사용
```

### 패키지 구조

```
VisionMotionFramework.v100.1.0.0.nupkg
├── lib/net45/v100/
│   ├── VisionMotionFramework.dll (v100, msvcr100.dll 링크)
│   ├── VisionMotionFramework.lib
│   └── VisionMotionFramework.pdb
├── include/ (공개 헤더)
└── native/ (v100용 Props)

VisionMotionFramework.1.0.0.nupkg
├── lib/net45/v140/
│   ├── VisionMotionFramework.dll (v140+, msvcr140.dll 링크)
│   ├── VisionMotionFramework.lib
│   └── VisionMotionFramework.pdb
├── include/ (공개 헤더)
└── native/ (v140+ Props 자동 매핑)
```

---

## GetName 변경 절차 (검색-치환 및 검증)
GetName 또는 런타임 식별자 문자열(예: `"Load1.ZFocus.MoveToStartPosition"`)을 변경해야 하는 경우 아래 절차를 준수하십시오.

1) 영향 범위 분석
   - 스크립트(`tools/find_getname_usages.py`)로 프로젝트 내 `GetName()` 호출과 하드코딩된 런타임 식별자 문자열을 모두 식별합니다.
   - 결과 리포트를 검토하여 런타임 의존 코드(문자열 비교, 메시지 키, DB 키, UI 매핑 등)를 분류합니다.

2) 대체 전략 결정
   - 권장: 문자열 리터럴을 상수로 추출하거나 enum/ID 기반 매핑으로 대체합니다.
   - 옵션 A (빠름): 상수로 추출
     - `DVH_VAT/Names.h` 같은 파일에 `constexpr const char*` 또는 `static const std::string` 상수로 선언합니다.
   - 옵션 B (타입 안전): enum/ID + 레지스트리
     - 런타임 비교를 정수 기반 enum으로 변경하고, 문자열 ↔ enum 매핑을 중앙 레지스트리에 둡니다.

3) 자동 치환 스크립트(샘플)
   - 간단한 검색-치환 스크립트를 사용하여 하드코딩된 리터럴을 상수로 대체합니다. 아래는 예시 스크립트 템플릿입니다.

```python tools/replace_getname_literal.py
#!/usr/bin/env python3
# 간단한 GetName 리터럴 치환 스크립트 (백업 생성)
import os, re, sys, shutil

ROOT = sys.argv[1] if len(sys.argv) > 1 else '.'
PAT = re.compile(r'"([A-Za-z0-9_]+)\.([A-Za-z0-9_]+)\.([A-Za-z0-9_]+)"')
REPLACEMENTS = {
    # 예: 'Load1.ZFocus.MoveToStartPosition': 'DVH_VAT::Names::Load1_ZFocus_Move'
}

for dirpath, dirs, files in os.walk(ROOT):
    if '/.git/' in dirpath or '/thirdparty/' in dirpath:
        continue
    for f in files:
        if not f.endswith(('.cpp', '.h', '.hpp', '.cxx', '.cc')):
            continue
        path = os.path.join(dirpath, f)
        with open(path, 'r', encoding='utf-8', errors='replace') as fh:
            s = fh.read()
        orig = s
        def repl(m):
            key = m.group(0).strip('"')
            return REPLACEMENTS.get(key, m.group(0))
        s = PAT.sub(repl, s)
        if s != orig:
            bak = path + '.bak'
            shutil.copy2(path, bak)     
            with open(path, 'w', encoding='utf-8') as fh:
                fh.write(s)
            print('Patched', path)
```

   - 주의: 자동 교체 전 반드시 백업을 생성하고, 변경 목록을 코드 리뷰에 제출하세요.

4) 단위 테스트 및 검증
   - 변경 전/후 다음 항목을 실행합니다:
     - 정적 분석 및 빌드(전체 솔루션 Clean + Rebuild)
     - 단위 테스트 및 통합 테스트(가능한 경우 시뮬레이터/모의환경 사용)
     - 런타임 시나리오 검증(특히 문자열 기반 라우팅/매핑 코드)
   - 추가로 `getname_usage_report.json` 생성 스크립트를 사용해 잔여 하드코딩 항목이 없는지 확인합니다.

5) 배포 및 모니터링
   - 변경을 배포한 후(특히 장비/현장 환경) 로그를 모니터링하여 식별자 불일치로 인한 오류가 없는지 점검합니다.

---

## 파일 인코딩 변경 체크리스트 (권장)
- 모든 변경 파일은 UTF-8로 저장합니다.
- Git에서 인코딩 문제가 있는 파일은 `git status` 이후 `iconv` 또는 편집기로 변환합니다.
- CI 빌드가 통과하지 않으면 우선 인코딩/문자열 문제를 확인하십시오.

---

## End of CONTRIBUTING.md
