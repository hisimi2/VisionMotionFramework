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
                strncpy_s(szParam[index], STR_LEN, pValue, _TRUNCATE);
            }
        }
    };

    struct CPacketBody_S2F42 {
        int nDataID;
        int nIndex;
        int nRCMDACK;
        CPacketBody_S2F42() {
            memset(this, 0x00, sizeof(CPacketBody_S2F42));
        }
    };

    struct CPacketBody_S2F3 {
        int nDataID;
        int nStatus;
        int nDataCount;
        char cData[DATA_SIZE][STR_LEN];
        CPacketBody_S2F3() {
            memset(this, 0x00, sizeof(CPacketBody_S2F3));
        }
    };

    struct CPacketBody_S2F4 {
        int nDataID;
        int nStatus;
        int nDataCount;
        char cData[DATA_SIZE][STR_LEN];
        CPacketBody_S2F4() {
            memset(this, 0x00, sizeof(CPacketBody_S2F4));
        }
    };

    struct CPacketBody_S107F9
    {
        int  nDataID;
        int  nStatus;
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
