#pragma once
#include "DVH_VAT_API.h"
#include "Types.h"

#include "IVisionProcessor.h"
#include "IDataRepository.h"

#include <cstdlib>
#include <sstream>

namespace DVH_VAT 
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

                char* endPtr = NULL;
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

                char* endPtr = NULL;
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
                return value != NULL ? std::string(value) : std::string();
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
        VisionPosition(std::vector<double> _pos, int _locateId, int _visionRequestId)
            : pos(_pos), locateId(_locateId), visionRequestId(_visionRequestId) {}
    };

    struct VatParams
    {
        StringMap                       seqParams;
        StringMap                       visionParams;
        std::vector<VisionPosition>     visionPositions;
    };

    class DVH_VAT_API VAT_Context : public boost::enable_shared_from_this<VAT_Context>
    {
    public:
        /// <summary>
        /// VAT 실행에 필요한 전체 파라미터 집합을 설정합니다.
        /// </summary>
        /// <param name="params">시퀀스 파라미터, 비전 파라미터, 비전 위치 목록을 포함한 파라미터 집합입니다.</param>
        void SetVatParams(const VatParams& params);

        /// <summary>
        /// 시퀀스 파라미터에 정수 값을 문자열 형태로 저장합니다.
        /// </summary>
        /// <param name="key">저장할 시퀀스 파라미터 키입니다.</param>
        /// <param name="value">저장할 정수 값입니다.</param>
        void SetSeqParam(const std::string& key, int value);

        /// <summary>
        /// 지정한 키의 시퀀스 파라미터 문자열 값을 반환합니다.
        /// </summary>
        /// <param name="key">조회할 시퀀스 파라미터 키입니다.</param>
        /// <returns>해당 키의 문자열 값입니다. 없으면 빈 문자열입니다.</returns>
        std::string GetSeqParam(const std::string& key) const;

        /// <summary>
        /// 지정한 키의 비전 파라미터 문자열 값을 반환합니다.
        /// </summary>
        /// <param name="key">조회할 비전 파라미터 키입니다.</param>
        /// <returns>해당 키의 문자열 값입니다. 없으면 빈 문자열입니다.</returns>
        std::string GetVisionParam(const std::string& key) const;








        /// <summary>
        /// VAT 실행 컨텍스트를 초기화합니다.
        /// 비전 프로세서, 저장소, 파라미터, 에러 상태 및 중지 요청 상태의 기본값을 준비합니다.
        /// </summary>
        VAT_Context();

        /// <summary>
        /// VAT 실행 컨텍스트를 정리합니다.
        /// </summary>
        ~VAT_Context();

        /// <summary>
        /// 비전 명령 실행에 사용할 비전 프로세서 인터페이스를 설정합니다.
        /// </summary>
        /// <param name="vp">등록할 비전 프로세서 객체입니다.</param>
        void SetVisionProcessor(VisionEventHandlerPtr vp);

        /// <summary>
        /// 현재 등록된 비전 프로세서 인터페이스를 반환합니다.
        /// </summary>
        /// <returns>등록된 비전 프로세서 객체입니다. 없으면 null 포인터입니다.</returns>
        VisionEventHandlerPtr GetVisionProcessorInterface() const;

        /// <summary>
        /// 데이터 조회 및 저장에 사용할 저장소 인터페이스를 설정합니다.
        /// </summary>
        /// <param name="repo">등록할 데이터 저장소 객체입니다.</param>
        void SetDataRepository(DataRepositoryPtr repo);

        /// <summary>
        /// 현재 등록된 데이터 저장소 인터페이스를 반환합니다.
        /// </summary>
        /// <returns>등록된 데이터 저장소 객체입니다. 없으면 null 포인터입니다.</returns>
        DataRepositoryPtr getRepository() const;

        /// <summary>
        /// 마지막 오류 메시지를 저장합니다.
        /// </summary>
        /// <param name="error">저장할 오류 메시지입니다.</param>
        void SetLastError(const std::string& error);

        /// <summary>
        /// 마지막으로 저장된 오류 메시지를 반환합니다.
        /// </summary>
        /// <returns>현재 저장된 마지막 오류 메시지입니다.</returns>
        const std::string& GetLastError() const;

        /// <summary>
        /// 작업 중지 요청 상태를 설정합니다.
        /// </summary>
        /// <param name="stop">중지 요청 여부입니다.</param>
        void SetStopRequested(bool stop);

        /// <summary>
        /// 현재 작업 중지 요청 상태를 반환합니다.
        /// </summary>
        /// <returns>중지 요청 상태이면 true, 아니면 false입니다.</returns>
        bool GetStopRequested() const;

        /// <summary>
        /// 현재 저장된 비전 파라미터를 사용하여 지정한 비전 명령을 실행합니다.
        /// </summary>
        /// <param name="cmd">실행할 비전 명령입니다.</param>
        /// <returns>명령 요청에 성공하면 true, 실패하면 false입니다.</returns>
        bool ExecuteVisionCommand(VatCommand cmd);

        /// <summary>
        /// 지정한 시퀀스 파라미터를 원하는 타입으로 변환하여 반환합니다.
        /// 키가 없거나 변환에 실패하면 기본값을 반환합니다.
        /// </summary>
        /// <typeparam name="T">반환받을 대상 타입입니다.</typeparam>
        /// <param name="key">조회할 시퀀스 파라미터 키입니다.</param>
        /// <param name="defaultValue">키가 없거나 변환 실패 시 반환할 기본값입니다.</param>
        /// <returns>변환된 시퀀스 파라미터 값 또는 기본값입니다.</returns>
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
        /// 키가 없거나 변환에 실패하면 기본값을 반환합니다.
        /// </summary>
        /// <typeparam name="T">반환받을 대상 타입입니다.</typeparam>
        /// <param name="key">조회할 비전 파라미터 키입니다.</param>
        /// <param name="defaultValue">키가 없거나 변환 실패 시 반환할 기본값입니다.</param>
        /// <returns>변환된 비전 파라미터 값 또는 기본값입니다.</returns>
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
        /// <typeparam name="T">저장할 값의 타입입니다.</typeparam>
        /// <param name="key">저장할 시퀀스 파라미터 키입니다.</param>
        /// <param name="value">저장할 값입니다.</param>
        template <typename T>
        void SetSeqParamAs(const std::string& key, const T& value)
        {
            LockGuardType guard(m_mutex);
            m_params.seqParams[key] = detail::ParamFormatter<T>::Format(value);
        }

        /// <summary>
        /// 비전 파라미터에 지정한 타입의 값을 문자열로 변환하여 저장합니다.
        /// </summary>
        /// <typeparam name="T">저장할 값의 타입입니다.</typeparam>
        /// <param name="key">저장할 비전 파라미터 키입니다.</param>
        /// <param name="value">저장할 값입니다.</param>
        template <typename T>
        void SetVisionParamAs(const std::string& key, const T& value)
        {
            LockGuardType guard(m_mutex);
            m_params.visionParams[key] = detail::ParamFormatter<T>::Format(value);
        }

        /// <summary>
        /// 현재 저장된 비전 위치 목록 전체를 복사하여 반환합니다.
        /// </summary>
        /// <returns>비전 위치 목록입니다.</returns>
        std::vector<VisionPosition> GetVisionPositions() const;

        /// <summary>
        /// 비전 위치 목록의 첫 번째 항목을 꺼내어 반환하고 목록에서 제거합니다.
        /// </summary>
        /// <param name="outPos">꺼낸 비전 위치를 저장할 출력 변수입니다.</param>
        /// <returns>꺼낼 항목이 있으면 true, 없으면 false입니다.</returns>
        bool PopVisionPosition(VisionPosition& outPos);

        /// <summary>
        /// 비전 위치 목록의 마지막 항목을 제거하지 않고 조회합니다.
        /// </summary>
        /// <param name="outPos">조회한 비전 위치를 저장할 출력 변수입니다.</param>
        /// <returns>조회할 항목이 있으면 true, 없으면 false입니다.</returns>
        bool PeekVisionPosition(VisionPosition& outPos);

        /// <summary>
        /// 비전 위치 목록에 새 위치 정보를 추가합니다.
        /// </summary>
        /// <param name="pos">추가할 비전 위치 정보입니다.</param>
        void AddVisionPosition(const VisionPosition& pos);

        /// <summary>
        /// 비전 위치 목록이 비어 있는지 확인합니다.
        /// </summary>
        /// <returns>비어 있으면 true, 아니면 false입니다.</returns>
        bool IsVisionPositionEmpty() const;

    private:
        VisionEventHandlerPtr   m_processor;
        DataRepositoryPtr       m_repo;
        
        mutable boost::mutex    m_mutex;
        std::string             m_lastError;
        bool                    m_isStopRequested;
        VatParams               m_params;
    };
} // namespace DVH_VAT
