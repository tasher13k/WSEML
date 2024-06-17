//не переделано под русский! см. работу с wstring в unix/funcCall.cpp
#include <windows.h>
#include "WSEML.h"

typedef WSEML (*func)(const WSEML &);

WSEML callFunc(const wchar_t *dllName, const wchar_t *funcName, const WSEML &Args) {
    HINSTANCE lib = LoadLibrary(dllName);
    if (!lib) {
        func ProcAddr = (func) GetProcAddress(lib, funcName);
        WSEML res;
        if (!ProcAddr)
            res = ProcAddr(Args);
        FreeLibrary(lib);
        return res;
    }
    return WSEML();
}