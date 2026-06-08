#pragma once
#include "VMF_API.h"
#include "Types.h"

#include "IVisionProcessor.h"
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

        template <typename T>
        struct ParamFormatter
        {
            static std::string Format(const T& value)
            {
                std::ostringstream oss;
                oss << value;
                return oss.str();
            }
        };

        template <>
        struct ParamFormatter<std::string>
        {
            static std::string Format(const std::string& value)
            {
                return value;
            }
        };

        template <>
        struct ParamFormatter<const char*>
        {
            static std::string Format(const char* value)
            {
                return value != nullptr ? std::string(value) : std::string();
            }
        };

        template <>
        struct ParamFormatter<bool>
        {
            static std::string Format(const bool& value)
            {
                return value ? "true" : "false";
            }
        };
    }

    struct VisionPosition
    {
        std::vector<double> pos;
        int locateId;
        int visionRequestId;

        VisionPosition() : locateId(0), visionRequestId(0) {}
        
        // std::move 를 이용하여 파라미터 복사 오버헤드 축소
        VisionPosition(std::vector<double> _pos, int _locateId, int _visionRequestId)
            : pos(std::move(_pos)), locateId(_locateId), visionRequestId(_visionRequestId) {}
    };

    struct VatParams
    {
        StringMap                       seqParams;
        StringMap                       visionParams;
        std::vector<VisionPosition>     visionPositions;
    };

    class VMF_API Context : public std::enable_shared_from_this<Context>
    {
    public:
        /// <summary>
        /// VAT 실행에 필요한 전체 파라미터 집합을 설정합니다.
        /// </summary>
        void SetVatParams(const VatParams& params);

        /// <summary>
        /// 시퀀스 파라미터에 정수 값을 문자열 형태로 저장합니다.
        /// </summary>
        void SetSeqParam(const std::string& key, int value);

        /// <summary>
        /// 지정한 키의 시퀀스 파라미터 문자열 값을 반환합니다.
        /// </summary>
        std::string GetSeqParam(const std::string& key) const;

        /// <summary>
        /// 지정한 키의 비전 파라미터 문자열 값을 반환합니다.
        /// </summary>
        std::string GetVisionParam(const std::string& key) const;

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
        void SetVisionProcessor(VisionEventHandlerPtr vp);

        /// <summary>
        /// 현재 등록된 비전 프로세서 인터페이스를 반환합니다.
        /// </summary>
        VisionEventHandlerPtr GetVisionProcessorInterface() const;

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
        bool ExecuteVisionCommand(VatCommand cmd);

        /// <summary>
        /// 지정한 시퀀스 파라미터를 원하는 타입으로 변환하여 반환합니다.
        /// </summary>
        template <typename T>
        T GetSeqParamAs(const std::string& key, const T& defaultValue) const
        {
            const std::string value = GetSeqParam(key);
            if (value.empty())
                return defaultValue;

            T converted;
            if (!detail::ParamConverter<T>::Convert(value, converted))
                return defaultValue;

            return converted;
        }

        /// <summary>
        /// 지정한 비전 파라미터를 원하는 타입으로 변환하여 반환합니다.
        /// </summary>
        template <typename T>
        T GetVisionParamAs(const std::string& key, const T& defaultValue) const
        {
            const std::string value = GetVisionParam(key);
            if (value.empty())
                return defaultValue;

            T converted;
            if (!detail::ParamConverter<T>::Convert(value, converted))
                return defaultValue;

            return converted;
        }

        /// <summary>
        /// 시퀀스 파라미터에 지정한 타입의 값을 문자열로 변환하여 저장합니다.
        /// </summary>
        template <typename T>
        void SetSeqParamAs(const std::string& key, const T& value)
        {
            // Types.h에서 수정된 LockGuardType (std::lock_guard) 매칭 사용
            LockGuardType guard(m_mutex);
            m_params.seqParams[key] = detail::ParamFormatter<T>::Format(value);
        }

        /// <summary>
        /// 비전 파라미터에 지정한 타입의 값을 문자열로 변환하여 저장합니다.
        /// </summary>
        template <typename T>
        void SetVisionParamAs(const std::string& key, const T& value)
        {
            LockGuardType guard(m_mutex);
            m_params.visionParams[key] = detail::ParamFormatter<T>::Format(value);
        }

        /// <summary>
        /// 현재 저장된 비전 위치 목록 전체를 복사하여 반환합니다.
        /// </summary>
        std::vector<VisionPosition> GetVisionPositions() const;

        /// <summary>
        /// 비전 위치 목록의 첫 번째 항목을 꺼내어 반환하고 목록에서 제거합니다.
        /// </summary>
        bool PopVisionPosition(VisionPosition& outPos);

        /// <summary>
        /// 비전 위치 목록의 마지막 항목을 제거하지 않고 조회합니다.
        /// </summary>
        bool PeekVisionPosition(VisionPosition& outPos);

        /// <summary>
        /// 비전 위치 목록에 새 위치 정보를 추가합니다.
        /// </summary>
        void AddVisionPosition(const VisionPosition& pos);

        /// <summary>
        /// 비전 위치 목록이 비어 있는지 확인합니다.
        /// </summary>
        bool IsVisionPositionEmpty() const;

    private:
        VisionEventHandlerPtr   m_processor;
        DataRepositoryPtr       m_repo;
        
        mutable std::mutex      m_mutex;
        std::string             m_lastError;
        bool                    m_isStopRequested;
        VatParams               m_params;
    };
} // namespace VMF
