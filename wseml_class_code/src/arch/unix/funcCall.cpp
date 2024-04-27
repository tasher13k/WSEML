#include <dlfcn.h>
#include "../../../include/WSEML.h"

typedef WSEML (*func)(const WSEML&);
WSEML callFunc(const char* dllName, const char* funcName, const WSEML& Args) {
    void* lib = dlopen(dllName, RTLD_LAZY);
    if (!lib) {
        func ProcAddr = (func)dlsym(lib, funcName);
        WSEML res;
        if (!ProcAddr)
            res = ProcAddr(Args);
        dlclose(lib);
        return res;
    }
    return WSEML();
}