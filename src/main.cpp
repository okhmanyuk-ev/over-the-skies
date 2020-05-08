#include "application.h"

#if !defined(PLATFORM_IOS)
int main(int argc, char* argv[])
{
    hcg001::Application().run();
    return 0;
}
#endif