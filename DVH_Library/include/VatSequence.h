#pragma once
#include "DVH_VAT_API.h"
#include "IVatSequence.h" 
#include "ITask.h"
#include "CompatUtils.h" 

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace DVH_VAT 
{
    class IVatActuator;
    class IDataRepository;

    // C++14 호환 모던 C++ 클래스
    class DVH_VAT_API VatSequence : public IVatSequence
    {
    public:
        explicit VatSequence(const std::string& name = "");
        
        // C++11/14: 다형성 클래스 소멸자는 명시적으로 override 권장
        ~VatSequence() override;

        // C++11/14: 부모 인터페이스(IVatSequence)에서 상속받은 메서드들에 override 지정
        bool Execute(VAT_Context& context, IVatActuator* actuator) override;
        void Abort() override;
        std::string GetSequenceName() const override;
        std::string GetTaskName() const override;

        void AddTask(TaskStepPtr step) override;

        // 폴링 간격(밀리초). 필요하면 런타임에 조정 가능.
        void SetPollIntervalMs(int ms) { m_pollIntervalMs = ms; }

        // C++11: 복사 생성자 및 대입 연산자 호출 방지를 명시적 = delete 처리하여 public 선언 권장
        VatSequence(const VatSequence&) = delete;
        VatSequence& operator=(const VatSequence&) = delete;

    private:
        std::vector<TaskStepPtr> m_tasks;
        
        // C++11: 표준 동기화 객체 적용
        std::mutex m_mutex;
        std::condition_variable m_cv;

        // C++11: volatile 대신 안전한 원자적 변수 사용
        std::atomic<bool> m_abortRequested; 
        
        int m_pollIntervalMs;
        std::string m_SequenceName;
        std::string m_TaskName;

        // 리소스 홀더 (실행 중 생명주기 관리용)
        VatActuatorPtr      m_actuator;
        DataRepositoryPtr   m_repo;
    };

} // namespace DVH_VAT
