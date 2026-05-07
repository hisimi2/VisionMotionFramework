#pragma once

namespace DVH_VAT
{

    // C++11/14: #define 대신 타입 안정성을 제공하는 constexpr const char* 사용 권장
    // Request parameter keys
    constexpr const char* RECIPE_NAME           = "recipe_name";
    constexpr const char* PCD_MODE              = "pcd_mode";
    constexpr const char* DEVICE_SIZE_X         = "device_size_x";
    constexpr const char* DEVICE_SIZE_Y         = "device_size_y";
    constexpr const char* COK_TYPE              = "cok_type";
    constexpr const char* PICKER_PITCH_X        = "picker_pitch_x";
    constexpr const char* PICKER_PITCH_Y        = "picker_pitch_y";

    constexpr const char* CAMERA_ID             = "CameraID";
    constexpr const char* INSPECTION_TYPE       = "InspectionType";
    constexpr const char* MOVE_PART             = "nMovePart";
    constexpr const char* SAVE_IMAGE            = "bSaveImage";
    constexpr const char* FOV_DIRECTION         = "nFovDirection";

    // Response/result keys
    constexpr const char* RESULT                = "Result";
    constexpr const char* SERVER_INDEX          = "ServerIndex";
    constexpr const char* CAM_STATUS            = "CamStatus";
    constexpr const char* CAM_TYPE              = "CamType";

    constexpr const char* Z_FOCUS_VALUE         = "ZFocusValue";
    constexpr const char* X_OFFSET              = "XOffset";
    constexpr const char* Y_OFFSET              = "YOffset";
    constexpr const char* ANGLE                 = "Angle";
    constexpr const char* AUTO_VISION_SETTING   = "AutoVisionSetting";

}
