#pragma once

#include "EC_API.h"

#include "ParamConverterFomatter.hpp"
#include <map>
#include <mutex>
#include <memory>
#include <atomic>
#include <string>
#include <vector>

namespace EC
{
    using StringMap = std::map<std::string, std::string>;
    using LockGuardType = std::lock_guard<std::mutex>;

    class AsyncExecutor;

    /// <summary>
    /// Context 실행 상태 (Thread-safe)
    /// </summary>
    enum class RunState : int
    {
        Pause   = 0,
        Resume  = 1,
        Stop    = 2
    };

    class EC_API Context
    {
    public:
        Context();
        ~Context();

        void SetParams(const StringMap& params);
        std::string GetParam(const std::string& key) const;

        void SetLastError(const std::string& error);
        std::string GetLastError() const;

        void SetResume();
        void SetPause();
        void SetStop();

        bool isResume() const;
        bool isPause() const;
        bool isStop() const;
        bool GetStopRequested() const;

        /// <summary>
        /// AsyncExecutor를 설정하여 Task가 결과를 전송할 수 있게 합니다.
        /// </summary>
        void SetExecutor(AsyncExecutor* executor);

        /// <summary>
        /// Task에서 결과를 전송합니다. (Context에 저장된 Executor로 전달)
        /// </summary>
        void SendResult(int requestId, const std::string& status);

        template <typename T>
        T GetParamAs(const std::string& key, const T& defaultValue) const
        {
            const std::string value = GetParam(key);
            if (value.empty())
                return defaultValue;

            T converted;
            if (!EC::ParamConverter<T>::Convert(value, converted))
                return defaultValue;

            return converted;
        }

        template <typename T>
        void SetParamAs(const std::string& key, const T& value)
        {
            LockGuardType guard(m_mutex);
            m_params[key] = EC::ParamFormatter<T>::Format(value);
        }

    private:
        mutable std::mutex      m_mutex;
        StringMap               m_params;
        std::string             m_lastError;
        std::atomic<RunState>   m_runState;
        AsyncExecutor*          m_executor = nullptr;
    };

    using ContextPtr = std::shared_ptr<Context>;

}

