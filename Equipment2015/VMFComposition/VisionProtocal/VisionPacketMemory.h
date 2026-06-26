#pragma once

#include <cstring> // std::memset, strncpy_s

namespace VMF
{
    constexpr size_t STR_LEN   = 64;
    constexpr size_t DATA_SIZE = 10;

    struct CPacketBody_S2F41
    {
        int nCmd = 0;
        int nParamCount = 0;
        char szParam[DATA_SIZE][STR_LEN];

        void Clear()
        {
            std::memset(this, 0, sizeof(*this));
        }

        void SetParam(int index, const char* pValue)
        {
            if (index >= 0 && static_cast<size_t>(index) < DATA_SIZE && pValue != nullptr)
            {
                strncpy_s(szParam[index], STR_LEN, pValue, STR_LEN - 1);
            }
        }
    };

    struct CPacketBody_S107F9
    {
        int  nDataID = 0; // 카메라 id
        int  nStatus = 0; // 검사 유형 (Picker, Plate Jig Upper / C-Tray검사 등)
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
