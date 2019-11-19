#include <Platform/defines.h>

#include "application.h"

#if defined(PLATFORM_WINDOWS)
void main(int argc, char* argv[])
{
	hcg001::Application().run();
}
#elif defined(PLATFORM_ANDROID)
#include <android_native_app_glue.h>
#include <Platform/system_android.h>

void android_main(android_app* app)
{
	Platform::SystemAndroid::run(app, [] { hcg001::Application().run(); });
}
#endif