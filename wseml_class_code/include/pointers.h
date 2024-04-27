#ifndef POINTERS_H
#define POINTERS_H
#include "WSEML.h"
WSEML calc(WSEML& expPtr);
WSEML expand(WSEML& compPtr);
WSEML reduce(WSEML& expPtr);
void to_i(WSEML& expPtr);
void to_k(WSEML& expPtr);
WSEML* extractObj(WSEML& compPtr);
std::string getAddrStr(const WSEML* ptr);
WSEML makePtr(const WSEML& object);
#endif
