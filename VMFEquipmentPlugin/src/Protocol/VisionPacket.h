#pragma once


namespace VMF_PLUGIN
{
    constexpr size_t STR_LEN = 64;
    constexpr size_t DATA_SIZE = 10;

    // ================================================================
    // [VAT / 공통] S2/F41 — SET COK, INSPECT READY
    // ================================================================
    struct CPacketBody_S2F41
    {
        int  nCmd = 0;
        int  nParamCount = 0;
        char szParam[DATA_SIZE][STR_LEN];

        void Clear() { std::memset(this, 0, sizeof(*this)); }
        void SetParam(int index, const char* pValue)
        {
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue)
                strncpy_s(szParam[index], STR_LEN, pValue, STR_LEN - 1);
        }
    };

    // ================================================================
    // [VAT] S107/F9 — REQ_MEASURE (H→V) / REQ_RESULT (V→H)
    // nDataID : Camera ID (3028, 3128 등)
    // nStatus : 검사 종류(요청) / 결과 코드(응답)
    // cData[0] : nMovePart
    // cData[1] : bSaveImage
    // cData[2] : (응답) Offset X
    // cData[3] : (응답) Offset Y / AutoVisionSetting
    // cData[4] : (응답) Angle / AutoVisionSetting
    // ================================================================
    struct CPacketBody_S107F9
    {
        int  nDataID = 0;
        int  nStatus = 0;
        char cData[DATA_SIZE][STR_LEN];

        void Clear() { std::memset(this, 0, sizeof(*this)); }
        void SetData(int index, const char* pValue)
        {
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue)
                strncpy_s(cData[index], STR_LEN, pValue, _TRUNCATE);
        }
    };

    // ================================================================
    // [PLVI] S107/F5 — REQ_MEASURE (H→V)
    // nDataID : PLVI 요청 ID
    // nStatus : 0 (미사용)
    // cData[0] : PLVI 위치 (0=SetPlate1, 1=SetPlate2, 11=Shuttle 등)
    // cData[1] : PKG 명칭
    // cData[2] : C-Tray 크기 (문자열 "X,Y" 형태로 인코딩)
    //            [2][0] = CTrayX, [2][1] = CTrayY
    // cData[3][n] : 개별 Device 유무 (0=없음, 99=존재)
    //              n = CTrayX * CTrayY 개수
    // ================================================================
    struct CPacketBody_S107F5
    {
        int  nDataID = 0;
        int  nStatus = 0;
        char cData[DATA_SIZE][STR_LEN];

        void Clear() { std::memset(this, 0, sizeof(*this)); }
        void SetData(int index, const char* pValue)
        {
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue)
                strncpy_s(cData[index], STR_LEN, pValue, _TRUNCATE);
        }
    };

    // ================================================================
    // [PLVI] S107/F6 — REQ_RESULT (V→H)
    // nDataID : 요청 ID + 1000
    // nStatus : 0=ERROR / 1=SUCCESS
    // cData[0] : Error Description
    //            0=BUSY, 1=CAM_DISCONNECT, 2=CAM_TIMEOUT,
    //            3=CAM_STATUS_FAIL, 4=LIGHT_ERROR, 5=NO_TEACHING_DATA
    // cData[1] : 전체 결과 ('0':OK / '1':NG)
    // cData[2] : PLVI 위치 echo
    // cData[3][n] : 개별 Pocket 상태
    //              0=없음, 99=정상, 1=Leave, 2=Double,
    //              11=Missing, 12=Mismatch
    // ================================================================
    struct CPacketBody_S107F6
    {
        int  nDataID = 0;
        int  nStatus = 0;
        char cData[DATA_SIZE][STR_LEN];

        void Clear() { std::memset(this, 0, sizeof(*this)); }
        void SetData(int index, const char* pValue)
        {
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue)
                strncpy_s(cData[index], STR_LEN, pValue, _TRUNCATE);
        }
    };

    // ================================================================
    // [6SIDE] S107/F1 — CMD_6_SIDE_INSPECTION_MEASURE (H→V)
    // nDataID : CMD ID = 1102
    // nStatus : VisionType (0=PC1, 1=PC2, 2=PC3, 3=PC4)
    // cData[0] : CamPosition (0=CamIndex#1, 1=CamIndex#2)
    // cData[1] : 6면 Position (1=Left, 2=Right, 3=Front,
    //                           4=Top, 5=Rear, 6=Bottom)
    // cData[2] : Select Position Count (검사할 면 총 개수)
    // cData[3] : Position Skip (0=검사, 1=Skip)
    // cData[4] : Barcode ID (S/N)
    // cData[5] : Lot ID (YYYYMMDD_HHMM)
    // ================================================================
    struct CPacketBody_S107F1_6Side
    {
        int  nDataID = 0;   // 1102
        int  nStatus = 0;   // VisionType
        char cData[DATA_SIZE][STR_LEN];

        void Clear() { std::memset(this, 0, sizeof(*this)); }
        void SetData(int index, const char* pValue)
        {
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue)
                strncpy_s(cData[index], STR_LEN, pValue, _TRUNCATE);
        }
    };

    // ================================================================
    // [6SIDE] S107/F2 — CMD_6_SIDE_INSPECTION_MEASURE 응답 (V→H)
    // nDataID : CMD ID = 1102 (echo)
    // nStatus : VisionType (echo)
    // cData[0] : CamPosition (echo)
    // cData[1] : 6면 Position (echo)
    // cData[2] : Grab Check (1=OK, 2=Fail)
    // cData[3] : Inspection Result (1=OK, 2=NG)
    // cData[4] : Barcode ID (echo)
    // cData[5] : Lot ID (echo)
    // ================================================================
    struct CPacketBody_S107F2_6Side
    {
        int  nDataID = 0;
        int  nStatus = 0;
        char cData[DATA_SIZE][STR_LEN];

        void Clear() { std::memset(this, 0, sizeof(*this)); }
        void SetData(int index, const char* pValue)
        {
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue)
                strncpy_s(cData[index], STR_LEN, pValue, _TRUNCATE);
        }
    };

} // namespace VMF_PLUGIN
