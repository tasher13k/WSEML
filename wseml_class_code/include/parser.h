#ifndef PARSER_H
#define PARSER_H
#include "WSEML.h"

WSEML parse(const std::string& text);
std::string pack(const WSEML& wseml);

#endif