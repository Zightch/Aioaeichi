#pragma once

typedef void *(*FunPtr)(...);
[[maybe_unused]] void *loadDLL(const char *);
[[maybe_unused]] FunPtr getFunction(void *, const char *);
[[maybe_unused]] void unloadDLL(void *);
