#pragma once

#include "TaskExecutor.h"

namespace VisionComm
{
    // 하위 호환용 별칭.
    // 향후 신규 코드는 TaskExecutor 사용 권장.
    using WorkerScheduler = TaskExecutor;
}

