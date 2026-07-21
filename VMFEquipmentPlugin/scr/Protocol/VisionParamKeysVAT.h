#pragma once

namespace VMF_PLUGIN
{
    // ── VAT Request Keys ──
    namespace VAT
    {
        constexpr const char* RecipeName       = "recipe_name";
        constexpr const char* PcdMode          = "pcd_mode";
        constexpr const char* DeviceSizeX      = "device_size_x";
        constexpr const char* DeviceSizeY      = "device_size_y";
        constexpr const char* CokType          = "cok_type";
        constexpr const char* PickerPitchX     = "picker_pitch_x";
        constexpr const char* PickerPitchY     = "picker_pitch_y";
        constexpr const char* CameraId         = "CameraID";
        constexpr const char* InspectionType   = "InspectionType";
        constexpr const char* MovePart         = "nMovePart";
        constexpr const char* SaveImage        = "bSaveImage";
        constexpr const char* FovDirection     = "nFovDirection";
    }

    // ── VAT Response Keys ──
    namespace VATResult
    {
        constexpr const char* Result           = "Result";
        constexpr const char* ServerIndex      = "ServerIndex";
        constexpr const char* CamStatus        = "CamStatus";
        constexpr const char* CamType          = "CamType";
        constexpr const char* ZFocusValue      = "ZFocusValue";
        constexpr const char* XOffset          = "XOffset";
        constexpr const char* YOffset          = "YOffset";
        constexpr const char* Angle            = "Angle";
        constexpr const char* AutoVisionSetting = "AutoVisionSetting";
    }
}