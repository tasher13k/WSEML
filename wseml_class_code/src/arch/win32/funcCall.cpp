#include <windows.h>
#include "../../../include/WSEML.h"
typedef WSEML (*func)(const WSEML&);
WSEML callFunc(const char* dllName, const char* funcName, const WSEML& Args) {
    HINSTANCE lib = LoadLibrary(dllName);
    if (!lib) {
        func ProcAddr = (func)GetProcAddress(lib, funcName);
        WSEML res;
        if (!ProcAddr)
            res = ProcAddr(Args);
        FreeLibrary(lib);
        return res;
    }
    return WSEML();
}