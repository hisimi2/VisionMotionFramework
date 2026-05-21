#pragma once

#include <cstring> // std::memset, strncpy_s

namespace DVH_VAT
{
    constexpr size_t STR_LEN   = 64;
    constexpr size_t DATA_SIZE = 10;

    struct CPacketBody_S2F41
    {
        // C++11: 멤버 기본 초기화
        int nCmd = 0;
        int nParamCount = 0;
        char szParam[DATA_SIZE][STR_LEN];

        void Clear()
        {
            std::memset(this, 0, sizeof(*this));
        }

        void SetParam(int index, const char* pValue)
        {
            // C++11: NULL -> nullptr, index 비교 시 size_t 변환
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue != nullptr)
            {
                strncpy_s(szParam[index], STR_LEN, pValue, _TRUNCATE);
            }
        }
    };

    struct CPacketBody_S2F42 {
        int nDataID = 0;
        int nIndex = 0;
        int nRCMDACK = 0;
        
        CPacketBody_S2F42() {
            std::memset(this, 0, sizeof(*this));
        }
    };

    struct CPacketBody_S2F3 {
        int nDataID = 0;
        int nStatus = 0;
        int nDataCount = 0;
        char cData[DATA_SIZE][STR_LEN];
        
        CPacketBody_S2F3() {
            std::memset(this, 0, sizeof(*this));
        }
    };

    struct CPacketBody_S2F4 {
        int nDataID = 0;
        int nStatus = 0;
        int nDataCount = 0;
        char cData[DATA_SIZE][STR_LEN];
        
        CPacketBody_S2F4() {
            std::memset(this, 0, sizeof(*this));
        }
    };

    struct CPacketBody_S107F9
    {
        int  nDataID = 0;
        int  nStatus = 0;
        char cData[DATA_SIZE][STR_LEN];

        void Clear()
        {
            std::memset(this, 0, sizeof(*this));
        }

        void SetData(int index, const char* pValue)
        {
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue != nullptr)
            {
                strncpy_s(cData[index], STR_LEN, pValue, _TRUNCATE);
            }
        }
    };
}
