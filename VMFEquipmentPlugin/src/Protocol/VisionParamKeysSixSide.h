#pragma once

namespace VMF_PLUGIN
{
    // ── SixSide Request Keys ──
    namespace SixSide
    {
        constexpr const char* VisionType     = "Side6VisionType";
        // 0=PC1, 1=PC2, 2=PC3, 3=PC4
        constexpr const char* CamPosition    = "CameraIndex";      // Builder/Task에서 실제 사용 키
        constexpr const char* FacePosition   = "FacePosition";     // Builder/Task에서 실제 사용 키
        constexpr const char* SelectCount    = "SelectCount";      // Builder/Task에서 실제 사용 키
        constexpr const char* Skip           = "Side6Skip";        // Builder/Task에서 실제 사용 키 (일부 Task에서 사용)
        constexpr const char* BarcodeId      = "BarcodeID";        // Builder/Task에서 실제 사용 키
        constexpr const char* LotId          = "LotID";            // Builder/Task에서 실제 사용 키
        
        // 호환성을 위한 별칭 (기존 코드 지원)
        constexpr const char* Side6VisionType = "Side6VisionType";
        constexpr const char* Side6CamPosition = "Side6CamPosition";
        constexpr const char* Side6FacePosition = "Side6FacePosition";
        constexpr const char* Side6SelectCount = "Side6SelectCount";
        constexpr const char* Side6Skip = "Side6Skip";
        constexpr const char* Side6BarcodeId = "Side6BarcodeId";
        constexpr const char* Side6LotId = "Side6LotId";
    }

    // ── SixSide Response Keys ──
    namespace SixSideResult
    {
        constexpr const char* GrabCheck  = "Side6GrabCheck";
        // "1"=Grab OK, "2"=Grab Fail
        constexpr const char* InspResult = "Side6InspResult";
        // "1"=OK, "2"=NG
        constexpr const char* ResultFace = "Side6ResultFace";
        // 결과 면 번호 echo
    }
}