#pragma once

namespace DVH_VAT
{
    #define STR_LEN		(64)
    #define DATA_SIZE	(10)

    struct CPacketBody_S2F41
    {
        int nCmd;
        int nParamCount;
        char szParam[DATA_SIZE][STR_LEN];

        void Clear()
        {
            std::memset(this, 0, sizeof(*this));
        }

        void SetParam(int index, const char* pValue)
        {
            if (index >= 0 && index < (int)DATA_SIZE && pValue != NULL)
            {
                strncpy_s(szParam[index], STR_LEN, pValue, STR_LEN - 1);
            }
        }
    };

    struct CPacketBody_S107F9
    {
        int  nDataID; // 카메라 id
        int  nStatus; // 검사 유형 (Picker, Plate Jig Upper / C-Tray검사 등)
        char cData[DATA_SIZE][STR_LEN];

        void Clear()
        {
            std::memset(this, 0, sizeof(*this));
        }

        void SetData(int index, const char* pValue)
        {
            if (index >= 0 && index < (int)DATA_SIZE && pValue != NULL)
            {
                strncpy_s(cData[index], STR_LEN, pValue, _TRUNCATE);
            }
        }
    };
}
