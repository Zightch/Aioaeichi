#include "Screw/Logger.h"

#ifdef _WIN32
#define DEMO extern "C" __declspec(dllexport)
#elif __linux__
#define DEMO extern "C"
#endif

DEMO void init() {}
DEMO void start() {
    LOG(Info, "Hello, Aioaeichi");
}
DEMO void unload() {}
