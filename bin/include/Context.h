#pragma once
#include "VMF_API.h"
#include "Types.h"

#include "IVisionClient.h"
#include "IDataRepository.h"

#include <cstdlib>
#include <sstream>
#include <memory> // std::enable_shared_from_this 사용 목적
#include <mutex>  // std::mutex 사용 목적

namespace VMF 
{

    namespace detail
    {
        template <typename T>
        struct ParamConverter
        {
            static bool Convert(const std::string& text, T& value)
            {
                std::istringstream iss(text);
                T temp;
                iss >> temp;

                if (iss.fail())
                    return false;

                value = temp;
                return true;
            }
        };

        template <>
        struct ParamConverter<std::string>
        {
            static bool Convert(const std::string& text, std::string& value)
            {
                value = text;
                return true;
            }
        };

        template <>
        struct ParamConverter<int>
        {
            static bool Convert(const std::string& text, int& value)
            {
                if (text.empty())
                    return false;

                char* endPtr = nullptr;
                const long parsed = std::strtol(text.c_str(), &endPtr, 10);

                if (endPtr == text.c_str() || *endPtr != '\0')
                    return false;

                value = static_cast<int>(parsed);
                return true;
            }
        };

        template <>
        struct ParamConverter<double>
        {
            static bool Convert(const std::string& text, double& value)
            {
                if (text.empty())
                    return false;

                char* endPtr = nullptr;
                const double parsed = std::strtod(text.c_str(), &endPtr);

                if (endPtr == text.c_str() || *endPtr != '\0')
                    return false;

                value = parsed;
                return true;
            }
        };

        template <>
        struct ParamConverter<bool>
        {
            static bool Convert(const std::string& text, bool& value)
            {
                if (text == "1" || text == "true" || text == "TRUE" || text == "True")
                {
                    value = true;
                    return true;
                }

                if (text == "0" || text == "false" || text == "FALSE" || text == "False")
                {
                    value = false;
                    return true;
                }

                return false;
            }
        };

    } // namespace detail

    class VMF_API Context : public std::enable_shared_from_this<Context>
    {
    public:
        /// <summary>
        /// VAT 실행 컨텍스트를 초기화합니다.
        /// </summary>
        Context();

        /// <summary>
        /// VAT 실행 컨텍스트를 정리합니다.
        /// </summary>
        ~Context();

        /// <summary>
        /// 비전 명령 실행에 사용할 비전 프로세서 인터페이스를 설정합니다.
        /// </summary>
        void SetVisionProcessor(VisionProcessorPtr vp);

        /// <summary>
        /// 현재 등록된 비전 프로세서 인터페이스를 반환합니다.
        /// </summary>
        VisionProcessorPtr GetVisionProcessorInterface() const;

        /// <summary>
        /// 데이터 조회 및 저장에 사용할 저장소 인터페이스를 설정합니다.
        /// </summary>
        void SetDataRepository(DataRepositoryPtr repo);

/// <summary>
        /// 현재 등록된 데이터 저장소 인터페이스를 반환합니다.
        /// </summary>
        DataRepositoryPtr GetRepository() const;

        /// <summary>
        /// 마지막 오류 메시지를 저장합니다.
        /// </summary>
        void SetLastError(const std::string& error);

/// <summary>
        /// 마지막으로 저장된 오류 메시지를 반환합니다.
        /// </summary>
        std::string GetLastError() const;

        /// <summary>
        /// 작업 중지 요청 상태를 설정합니다.
        /// </summary>
        void SetStopRequested(bool stop);

        /// <summary>
        /// 현재 작업 중지 요청 상태를 반환합니다.
        /// </summary>
        bool GetStopRequested() const;

/// <summary>
        /// 현재 저장된 비전 파라미터를 사용하여 지정한 비전 명령을 실행합니다.
        /// </summary>
        bool ExecuteVisionCommand(VisionCommand cmd);

private:
        VisionProcessorPtr      m_processor;
        DataRepositoryPtr       m_repo;
        
        mutable std::mutex      m_mutex;
        std::string             m_lastError;
        bool                    m_isStopRequested;
    };
} // namespace VMF
