#pragma once

#define PROJECT_CODE "hcg001"
#define PROJECT_NAME "OVER THE SKY"

#define BUILD_DEVELOPER

#if defined(ANDROID_RELEASE)
#undef BUILD_DEVELOPER
#endif