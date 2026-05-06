# CONTRIBUTING.md

## 1. 목적

이 문서는 코드베이스의 일관성 유지, 가독성 향상, 그리고 안전한 점진적 리팩토링을 위한 팀 규칙과 절차를 안내합니다. 주요 목적은 다음과 같습니다:

- 코드 스타일과 네이밍을 표준화하여 리뷰·유지보수 비용을 낮춥니다.
- 런타임 식별자와 소스 구조(파일/디렉터리) 간의 명확한 매핑을 유지합니다.
- `GetName()` 등 런타임 식별자 변경 시 안전한 마이그레이션 절차를 제공합니다.
- 안정적인 점진적 리팩토링을 유도합니다.

---

## 2. 핵심 원칙(요약)

- Unit 단위 네임스페이스 유지: `VAT_LOAD1`, `VAT_LOAD2` 등.
- Unit 내부에 역할별 하위 네임스페이스 권장: `Sequence`, `Task`, `Strategy`.
- 모든 non-interface 클래스는 `C` 접두사 + PascalCase 사용.
- 인터페이스(추상 클래스)는 `I` 접두사 사용.
- 파일명은 클래스명과 1:1 대응(`ClassName.h` / `ClassName.cpp`).
- `GetName()` 문자열 표준: `"{Unit}.{Sequence}.{Task}"`.

---

## 3. 네임스페이스 규칙

- 최상위 네임스페이스는 Unit 이름(예: `VAT_LOAD1`)입니다.
- Unit 내부 역할별 네임스페이스 구성 권장:
  - `Sequence` — 시퀀스 정의/조합 관련 타입
  - `Task` — 개별 작업(스텝) 구현
  - `Strategy` — 전략/정책 구현
- 예시:

---

## 4. 클래스 네이밍 규칙

- non-interface 클래스: 접두사 `C` + PascalCase
  - 예: `CLoad1FindAlignPosTask`, `CLoad1ZFocusSequence`
- 인터페이스(추상 클래스): 접두사 `I` + PascalCase
  - 예: `IVisionProcessor`, `IDataRepository`
- 파일명은 클래스명과 동일:
  - `CLoad1FindAlignPosTask.h` / `CLoad1FindAlignPosTask.cpp`

---

## 5. Sequence 관련 규칙

- 빌더 클래스: `C{Unit}{SequenceName}SequenceBuilder`
  - 예: `CLoad1ZFocusSequenceBuilder`
- 시퀀스 클래스: `C{Unit}{SequenceName}Sequence`
  - 예: `CLoad1ZFocusSequence`

---

## 6. Task / Step 규칙

- 권장 패턴: `C{Unit}{SequenceName}{TaskName}Task` (Sequence 컨텍스트 포함)
  - 예: `CLoad1ZFocusMoveToStartPositionTask`
- 필요 시 Sequence 생략 가능: `C{Unit}{TaskName}Task`
  - 예: `CLoad1FindAlignPosTask`

---

## 7. Strategy / Policy 규칙

- 형태: `C{Unit}{Component}{Role}` 또는 `C{Component}{Role}`
- 유닛 특정: `CLoad1HandPitchSequenceStrategy`
- 범용(유닛 비특정): `CHandPitchSequenceStrategy`

---

## 8. 런타임 식별자(`GetName`) 규칙

- 권장 포맷: `"{Unit}.{Sequence}.{Task}"`
  - 예: `"Load1.ZFocus.MoveToStartPosition"`
- 런타임에 문자열을 직접 비교하거나 하드코딩하여 사용하는 코드는 변경 시 전체 동기화가 필요하므로 주의합니다.
- 권장: 문자열 상수화 또는 enum/ID 매핑을 사용하여 안전하게 변경할 수 있게 설계합니다.

---

## 9. 파일/디렉터리 레이아웃

- Unit 기반 폴더 구조 권장:
  - `EquipmentSW/FrameWork/Load1/Sequences/`
  - `EquipmentSW/FrameWork/Load1/Tasks/`
  - `EquipmentSW/FrameWork/Load1/Strategies/`
- 파일명은 클래스명과 일치:
  - 예: `EquipmentSW/FrameWork/Load1/Task/CLoad1FindAlignPosTask.h`

---

## 10. 인터페이스 표기(추가 규칙)

- 모든 인터페이스 타입(추상 클래스)은 `I` 접두사 사용:
  - 예: `IVisionProcessor`, `IDataRepository`, `IResultSink`
- 이유:
  - 구현체와 인터페이스 구분이 명확해지고 DI 및 테스트 더블 작성이 용이합니다.
- 예외:
  - 광범위하게 노출된 레거시 관례가 있는 경우 점진적 통일 계획을 수립한 뒤 리팩토링합니다.

---

## 11. 파일 인코딩 및 헤더 보호 규정

- 모든 소스(헤더/소스/스크립트)는 UTF-8(가능하면 BOM 없음)으로 저장합니다.
  - 이유: 한글 주석/문자열의 손실을 방지하고 CI 환경의 인코딩 불일치를 줄입니다.
  - 에디터 설정 예: Visual Studio에서 파일 → 고급 저장 옵션 → UTF-8
- 모든 헤더 파일은 `#pragma once` 사용 권장.
  - 표준 include-guard가 필요할 경우 팀 사전 합의 필요.
- 서드파티(예: Boost) 노출 제한:
  - 공용 헤더에서 직접 서드파티를 노출하지 않도록 하며, 가능하면 `.cpp`에 포함하거나 PIMPL 패턴을 사용합니다.
- 매크로 충돌 우려 시 포함 전후로 매크로 보호 사용 권장.

---

## 부록: GetName 변경 절차 (권장)

1. 영향 범위 분석
   - `GetName()` 호출과 하드코딩된 런타임 식별자 문자열을 검색합니다.
   - 의존 코드(문자열 비교, 메시지 키, DB 키, UI 매핑 등)를 분류합니다.
2. 대체 전략
   - 권장: 문자열 상수로 추출하거나 enum/ID 기반 매핑 사용.
   - 빠른 방법: 식별자 상수를 중앙 파일(예: `Names.h`)에 모읍니다.
   - 타입 안전 방법: enum/ID + 레지스트리 매핑을 도입합니다.
3. 단계적 마이그레이션
   - 호환 레이어를 두어 기존 소비자를 점진적으로 전환합니다.
4. 검증
   - 유닛/통합 테스트와 런타임 검증을 통해 변경 영향을 확인합니다.

---

끝.
