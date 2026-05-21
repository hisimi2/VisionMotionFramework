# VMF(Vision Motion Framework) 개발 계획서

작성일: 2026-05-14  
대상: `VisionMotionFramework` (C++14), Equipment SW 연동 프레임워크

---

## 1. 목적 및 범위

### 1.1 목적
- VMF는 Equipment SW에서 공통으로 사용하는 **Vision 연동 시퀀스 실행 프레임워크**를 제공한다.
- VMF는 **렌더링(UI 표시/그리기/다이얼로그 표시)을 수행하지 않고**, 결과/상태/에러 데이터를 UI 프레임워크 비종속 방식(Observer/Callback)으로 제공한다.
- 현재 사내 장비에서 사용되는 Vision 연동 기능을 지원하고, 향후 신규 장비/기능 확장이 용이한 구조를 제공한다.

### 1.2 범위 (In-Scope)
- 시퀀스 실행 엔진(비동기 실행, 중복 실행/재진입 정책)
- Strategy/Builder/Task 기반 시퀀스 구성
- `VAT_Context` 기반 실행 컨텍스트(파라미터/상태/오류/공유 데이터)
- Vision 통신 추상화(TCP/Mock 교체 가능)
- 결과/상태/에러 전달(Observer 기반) 및 이벤트 모델
- 복수 카메라 시퀀스 지원(파라미터 표준화, 요청 분리, 결과 구분/매핑)
- Repository 추상화 및 데이터 표준화 정책
- Mock/Simulation 기반 테스트 및 샘플
- SDK 배포 구조(`bin/lib/include/docs`) 및 통합 가이드(.props 또는 문서)

### 1.3 범위 (Out-of-Scope)
- Equipment 운전 로직(OP Switch Start/Stop 상태에 따른 작동/정지 판정 및 제어)
- UI 스레드 마샬링(필요 시 호스트가 구현)

---

## 2. 요구사항 기준(Traceability)

- 기능 요구사항: `SFR-001 ~ SFR-027`
- 비기능 요구사항: `PER-001 ~ PER-006`, `ECR-*`, `DAR-001`, `INR-*`, `QUA-*`, `TER-001`, `COR-*`, `PSR-*`

요구사항-구현-테스트 매핑은 별도 추적표(Trace Matrix)로 관리한다.

---

## 3. 아키텍처 요약(구현 지향)

### 3.1 코어 실행
- `VatCorrectionEngine`: 시퀀스 실행/중지 API 제공
- `AsyncSequenceRunner`: 내부 비동기 스레드에서 `IVatSequence` 실행

### 3.2 조립 및 호스트 연동(Facade)
- `CVatEngineObserverAdapter`: Strategy 기반 조립(Builder/Repo/Vision/Context/Runner) + 결과 Observer 통지

### 3.3 확장 포인트
- `IVatSequenceStrategy`: 시퀀스 구성을 결정(Builder/Repo/Vision 생성, 파라미터 Configure, Actuator 주입)
- `SequenceBuilderBase`: Task 조립
- `NonBlockingTaskBase`: `OnInitialize` + `OnPoll` 기반 상태머신 실행
- `VAT_Context`: 파라미터/상태/오류, stop 요청, Vision/Repo 참조 제공

### 3.4 결과 통지(Observer) 스레드 정책
- Observer/Callback은 **VMF 비동기 실행 스레드에서 호출될 수 있으며 UI 스레드를 보장하지 않는다.**
- UI 컨트롤 접근이 필요하면 호스트가 UI 스레드로 마샬링한다.

---

## 4. 개발 일정(안) — 8주 압축(MVP + 장비 검증 포함)

> 전제: MVP 중심(필수 기능 우선), Mock 우선 개발 후 장비 검증을 병행한다.

### Week 1 — 착수/설계 고정 + 프로젝트 골격
- 요구사항 컷라인(MVP) 확정
- 솔루션/프로젝트 구조 구성(C++14, `include/src`, DLL/Lib)
- 핵심 인터페이스/타입 확정(`IVatActuator`, `IResultSink`, `IDataRepository` 최소, Vision Processor 최소)
- 문서 뼈대 생성(아키텍처/스레드 모델/통합 가이드)

**산출물**
- 빌드 가능한 skeleton
- 요구사항/아키텍처 v0.1

### Week 2 — Engine/Runner 구현(비동기 실행) + Stop 정책 확정
- `VatCorrectionEngine`, `AsyncSequenceRunner` 구현
- Start/Stop 안정화(StopRequested 반영 및 종료 정책 정의)
- 중복 실행/재진입 정책 1차 적용
- 최소 로깅 연동

**검증**
- PC Start/Stop 반복 스트레스(누수/스레드 종료 확인)

**산출물**
- 엔진 API 초안 + 스레드/Stop 정책 문서

### Week 3 — Context/Task/Builder 모델 구현 + E2E(하드웨어 없이)
- `VAT_Context` 구현(파라미터/상태/stop/error)
- `NonBlockingTaskBase` 구현(OnInitialize/OnPoll + timeout 공통)
- `SequenceBuilderBase` + 최소 `IVatSequence` 실행 모델 구성
- 데모 시퀀스 1개 구현(모션/비전 없이도 동작)

**검증**
- 단위 테스트(파라미터, timeout)
- 통합 테스트(Builder → Runner → 완료)

### Week 4 — Vision Mock 우선 구현 + 결과 전달(Observer) 고정
- Mock Vision Processor 구현(응답/지연/에러 주입)
- `CVatEngineObserverAdapter` 기반 조립/결과 전달(Observer) 사용 패턴 확정
- 콜백 호출 스레드 정책 문서화

**검증**
- Mock 기반 E2E: 시퀀스 실행 → Vision 응답 → 결과 수신

### Week 5 — 장비 통합 준비 + Offline 장비 검증 시작(병행)
- Equipment SW 연동 포인트 확정(Actuator Adapter, Vision 설정, 로그 수집)
- Offline 검증(비가동/Dry-run)
  - Engine Start/Stop
  - timeout/stop 기본 동작
  - 결과/에러 전달 확인

**산출물**
- 장비 통합 체크리스트
- Offline 시험 결과(이슈 리스트)

### Week 6 — Vision 실연동 최소 구현 + 복수 카메라(MVP 수준)
- Vision 실연동 구현(가능 최소 범위: 연결/요청/응답/타임아웃)
- 복수 카메라 MVP
  - 파라미터 키 표준화 문서(`CameraIndex` 중심)
  - 카메라별 요청 분리/결과 구분은 **순차 처리** 우선

**검증**
- 장비 Offline/부분 Online에서 비전 요청/응답 확인

### Week 7 — Online 장비 검증(실가동) + 안정화/회귀
- Online 검증
  - 정상 시퀀스 1~2개 end-to-end
  - Stop/타임아웃/통신 단절 시나리오
  - 호스트 운전 스레드가 대기/재개되는 동안 VMF 비동기 스레드 정상 동작 확인(`SFR-003`, `PER-003`)
- 결함 수정 및 회귀 테스트 반복

**산출물**
- 장비 검증 리포트(결함/재현/해결)

### Week 8 — 패키징/문서/샘플 정리 + 릴리즈
- SDK 패키징: `bin/lib/include/docs`
- 통합 가이드/Mock 가이드/샘플 시퀀스/릴리즈 노트 정리
- 최종 회귀(PC + 장비 최소 시나리오)

**산출물**
- SDK v1
- docs 세트(요구사항/아키텍처/통합/Mock/릴리즈노트)
- 샘플 솔루션 1개

---

## 5. 장비 검증/테스트 범위(필수 시나리오)

### 5.1 Offline(비가동) 시나리오
- Start/Stop 반복(예: 100회)에서 hang/누수/스레드 잔존 없음
- Vision 요청 타임아웃 정상 처리 및 오류 전달
- Stop 요청 후 안전 정지(모션/조명 OFF 포함) 정책 확인

### 5.2 Online(실가동) 시나리오
- 단일 카메라 검사 시퀀스 end-to-end 완료
- 복수 카메라(최소 2대) 순차 시나리오 end-to-end 완료
- 카메라 1대 타임아웃/단절 시 장애 격리/오류 통지
- 호스트 운전 스레드 대기/재개 중에도 VMF 비동기 실행 및 결과 전달 안정(`PER-003`)

---

## 6. 리스크 및 대응

| 리스크 | 내용 | 대응 |
|---|---|---|
| UI 스레드 문제 | Observer 콜백이 작업 스레드에서 호출됨 | 스레드 정책을 명시하고 호스트 마샬링 패턴 샘플 제공 |
| Stop 처리 종료 시점 불명확 | Stop 요청 후 실제 종료 완료 시점 모호 | Stop 정책(완료/대기) 문서화, 필요 시 Wait API 추가 검토 |
| 파라미터 키 난립 | `CameraIndex/CamIndex/CameraID` 혼재 | 표준 키 정의 + 별칭/마이그레이션 가이드 |
| 장비별 변형 누적 | 장비별 요구가 코어를 오염시킴 | Strategy/Task 확장 원칙, 코어 변경 최소화 리뷰 |

---

## 7. 문서 산출물(권장)

- `docs/VMF_DevelopmentPlan.md` : 개발 계획서(본 문서)
- `docs/VMF_Requirements.md` : 요구사항 명세서(SFR/PER/…)
- `docs/VMF_Architecture.md` : 구조/스레드 모델/확장 포인트
- `docs/VMF_IntegrationGuide.md` : 호스트 통합 가이드(Observer, UI 마샬링 포함)
- `docs/VMF_MockSimulation.md` : Mock 시나리오 및 테스트 방법
- `docs/VMF_SDK_Packaging.md` : 배포/통합 설정 가이드(.props 등)
