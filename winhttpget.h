#pragma once
#ifndef _INC_WINHTTPGET
#define _INC_WINHTTPGET

#include "winhttp.h"
#include <string>
#pragma comment(lib, "winhttp.lib")

std::string winhttpget(LPCWSTR host, LPCWSTR object, bool isHTTPS);

#endif