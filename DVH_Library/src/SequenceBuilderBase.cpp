#include "StdAfx.h"

#include "VAT_Context.h"
#include "SequenceBuilderBase.h"
#include "VatSequence.h"
#include "IVatSequence.h"

#include "SqliteDataRepository.h"
#include "AsyncDataRepository.h"
#include "CompatUtils.h"

#include <string>
#include <memory> // boost::make_shared 등 대신 표준 라이브러리 사용
#include <direct.h>

namespace DVH_VAT
{
    // C++11/14: 명시적 구현보다 default 지정자가 효율적이나 헤더 정의와 일치를 위해 유지
    SequenceBuilderBase::SequenceBuilderBase() {}
    SequenceBuilderBase::~SequenceBuilderBase() {}

    static void EnsureDirExists(const std::string& path) 
    {
        _mkdir(path.c_str());
    }

    // boost::unique_ptr -> std::unique_ptr 변경. 
    // Types.h에 선언된 VatSequencePtr (std::unique_ptr<IVatSequence>)로 반환 형태 일치
    std::unique_ptr<IVatSequence> SequenceBuilderBase::CreateSequence(std::string sequenceName)
    {
        return BuildSequence(sequenceName);
    }

} // namespace DVH_VAT
