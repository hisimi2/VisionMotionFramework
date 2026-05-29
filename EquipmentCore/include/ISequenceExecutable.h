#pragma once

#include <string>
#include <memory>

#include "EC_API.h"

namespace EC
{
    /// <summary>
    /// 시퀀스 실행을 위한 콜백 인터페이스
    /// 외부에서 실제 시퀀스 로직을 구현합니다.
    /// </summary>
    class EC_API ISequenceExecutable
    {
    public:
        virtual ~ISequenceExecutable() = default;

        /// <summary>
        /// 시퀀스 초기화 로직
        /// </summary>
        virtual void OnInitialize() = 0;

        /// <summary>
        /// 시퀀스 폴링 로직
        /// </summary>
        /// <returns>true면 계속 진행, false면 완료</returns>
        virtual bool OnPoll() = 0;

        /// <summary>
        /// 시퀀스 정리 로직
        /// </summary>
        virtual void OnCleanup() = 0;

        /// <summary>
        /// 에러 발생 시 호출
        /// </summary>
        /// <param name="errorMsg">에러 메시지</param>
        virtual void OnError(const std::string& errorMsg) = 0;
    };

    using SequenceExecutablePtr = std::shared_ptr<ISequenceExecutable>;
}
