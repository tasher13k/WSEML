#include <dlfcn.h>
#include "WSEML.h"

#include "stringconverter.h"

typedef WSEML (*func)(const WSEML &);

WSEML callFunc(const wchar_t *dllName, const wchar_t *funcName, const WSEML &Args) {
    std::wstring dllName_wstring_view = dllName;
    std::wstring funcName_wstring_view = funcName;

    void *lib = dlopen(strconverter.to_bytes(dllName_wstring_view).c_str(), RTLD_LAZY);
    if (!lib) {
        func ProcAddr = (func) dlsym(lib, strconverter.to_bytes(funcName_wstring_view).c_str());
        WSEML res;
        if (!ProcAddr)
            res = ProcAddr(Args);
        dlclose(lib);
        return res;
    }
    return {};
}