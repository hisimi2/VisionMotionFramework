#pragma once

namespace EC
{
    class ActivityBuilderBase;
    class Context;
    class Load1Parts;

    /// <summary>
    /// Load1 Activity Builder
    /// Load1TaskPick + Load1TaskPlace로 Activity 구성
    /// 
    /// Load1은 XY축 + Setplate/Transfer 실린더를 사용합니다.
    /// </summary>
    class CLoad1ActivityBuilder : public ActivityBuilderBase
    {
        std::shared_ptr<Load1Parts> m_parts;

    public:
        void SetParts(std::shared_ptr<Load1Parts> parts) { m_parts = parts; }

    protected:
        ActivityPtr Build() override;
        void ConfigureParams(ContextPtr ctx) override;
    };
}
