#ifndef PARSER_H
#define PARSER_H
#include "WSEML.h"

WSEML parse(const std::wstring& text);
std::wstring pack(const WSEML& wseml);

#endif