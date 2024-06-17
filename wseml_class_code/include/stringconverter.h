#ifndef WSEML_STRINGCONVERTER_H
#define WSEML_STRINGCONVERTER_H
#include <codecvt>
#include <locale>

using convert_t = std::codecvt_utf8<wchar_t>;
static std::wstring_convert<convert_t, wchar_t> strconverter;

#endif //WSEML_STRINGCONVERTER_H
