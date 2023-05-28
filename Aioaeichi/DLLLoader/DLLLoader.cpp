#include "DLLLoader.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <dlfcn.h>
#endif

[[maybe_unused]]
void *loadDLL(const char *file) {
#ifdef _WIN32
    return LoadLibrary(file);
#elif __linux__
    return dlopen(file, RTLD_LAZY);
#endif
}

[[maybe_unused]]
FunPtr getFunction(void *handle, const char *funName) {
#ifdef _WIN32
    return (FunPtr) GetProcAddress((HINSTANCE) handle, funName);
#elif __linux__
    return (FunPtr) dlsym(handle, funName);
#endif
}

[[maybe_unused]]
void unloadDLL(void *handle) {
#ifdef _WIN32
    FreeLibrary((HINSTANCE) handle);
#elif __linux__
    dlclose(handle);
#endif
}
