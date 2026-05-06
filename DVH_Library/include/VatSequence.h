#pragma once
#include "DVH_VAT_API.h"
#include "IVatSequence.h" 
#include "ITask.h"
#include "CompatUtils.h" // MutexType, ConditionVariableType 등


namespace DVH_VAT 
{
    class IVatActuator;
    class IDataRepository;

    // VS2010(v100) 호환: std::atomic 미지원 -> volatile 플래그 사용
    class DVH_VAT_API VatSequence : public IVatSequence
    {
    public:
        explicit VatSequence(const std::string& name = "");
        virtual ~VatSequence();

        virtual bool Execute(VAT_Context& context, IVatActuator* actuator);
        virtual void Abort();
        virtual std::string GetSequenceName() const;
        virtual std::string GetTaskName() const;

        void AddTask(TaskStepPtr step);

        // 폴링 간격(밀리초). 필요하면 런타임에 조정 가능.
        void SetPollIntervalMs(int ms) { m_pollIntervalMs = ms; }

    private:
        // 복사/대입 금지 (C++11 = delete 대신 private 선언)
        VatSequence(const VatSequence&) {};
        VatSequence& operator=(const VatSequence&) { return *this; }

    private:
        std::vector<TaskStepPtr> m_tasks;
        
        // 동기화 객체
        boost::mutex m_mutex;
        ConditionVariableType m_cv;

        // [v100] atomic<bool> 대신 volatile bool + mutex 보호
        volatile bool m_abortRequested; 
        
        int m_pollIntervalMs;
        std::string m_SequenceName;
        std::string m_TaskName;

        // 리소스 홀더 (실행 중 생명주기 관리용)
        VatActuatorPtr      m_actuator;
        DataRepositoryPtr   m_repo;
    };

} // namespace DVH_VAT
