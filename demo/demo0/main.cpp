#include "Logger.h"

extern "C"
{
    void init() {}
    void start()
    {
        LOG(Info, "Hello, Aioaeichi");
    }
    void unload() {}
}
