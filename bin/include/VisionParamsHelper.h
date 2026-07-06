#pragma once
#include "Types.h"
#include <sstream>
#include <string>
#include <vector>

namespace VMF
{
    /// <summary>
    /// VisionParams 조립을 위한 정적 헬퍼 유틸리티 클래스.
    /// SequenceBuilderBase에서 분리하여 독립적으로 사용할 수 있습니다.
    /// </summary>
    class VisionParamsHelper
    {
    public:
        /// <summary>
        /// VisionParams에 문자열 키-값 쌍을 설정합니다.
        /// </summary>
        static void SetParam(VisionParams& params, const std::string& key, const std::string& value)
        {
            params.visionParams[key] = value;
        }

        /// <summary>
        /// VisionParams에 double 값을 문자열로 변환하여 설정합니다.
        /// </summary>
        static void SetParam(VisionParams& params, const std::string& key, double value)
        {
            std::ostringstream oss;
            oss << value;
            params.visionParams[key] = oss.str();
        }

        /// <summary>
        /// 3축 비전 검사 위치를 추가합니다. (x, y, z)
        /// </summary>
        static void AddVisionPoint(VisionParams& params, int locateId, int requestId,
                                   double x, double y, double z)
        {
            std::vector<double> pos;
            pos.push_back(x);
            pos.push_back(y);
            pos.push_back(z);
            params.visionPositions.push_back(VisionPosition(pos, locateId, requestId));
        }

        /// <summary>
        /// 5축 비전 검사 위치를 추가합니다. (x, y, z, t1, t2)
        /// </summary>
        static void AddVisionPoint(VisionParams& params, int locateId, int requestId,
                                   double x, double y, double z, double t1, double t2)
        {
            std::vector<double> pos;
            pos.push_back(x);
            pos.push_back(y);
            pos.push_back(z);
            pos.push_back(t1);
            pos.push_back(t2);
            params.visionPositions.push_back(VisionPosition(pos, locateId, requestId));
        }
    };
}