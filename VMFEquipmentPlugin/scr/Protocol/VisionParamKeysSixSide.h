#pragma once

namespace VMF_PLUGIN
{
    // ── SixSide Request Keys ──
    namespace SixSide
    {
        constexpr const char* VisionType     = "Side6VisionType";
        // 0=PC1, 1=PC2, 2=PC3, 3=PC4
        constexpr const char* CamPosition    = "Side6CamPosition";
        // 0=CamIndex#1, 1=CamIndex#2
        constexpr const char* FacePosition   = "Side6FacePosition";
        // 1=Left, 2=Right, 3=Front, 4=Top, 5=Rear, 6=Bottom
        constexpr const char* SelectCount    = "Side6SelectCount";
        // 검사할 면 총 개수
        constexpr const char* Skip           = "Side6Skip";
        // 0=검사, 1=Skip
        constexpr const char* BarcodeId      = "Side6BarcodeId";
        constexpr const char* LotId          = "Side6LotId";
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