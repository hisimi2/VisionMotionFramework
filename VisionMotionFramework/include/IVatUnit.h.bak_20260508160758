#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace DVH_VAT
{
    struct VatRunParams;
    class VisionMemoryProcessor;
    class IDataRepository;

    class IVatUnit
    {
    public:
        typedef boost::function<void(int /*requestId*/, int /*VisionStatus*/, const std::vector<std::string>& /*results*/)> VisionResultCallback;

        virtual ~IVatUnit() {}

        virtual void Initialize(boost::shared_ptr<DVH_VAT::VisionMemoryProcessor> pVision,
                                boost::shared_ptr<DVH_VAT::IDataRepository> pRepo,
                                const DVH_VAT::VatRunParams& params) = 0;

        virtual bool RunSequence(int seqType, const DVH_VAT::VatRunParams& params, std::string& errorMsg) = 0;

        virtual void Stop() = 0;

        virtual void SetVisionResultCallback(VisionResultCallback cb) = 0;
    };
}

