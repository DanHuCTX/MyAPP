// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <atlbase.h>
#include <Psapi.h>
#include <Windows.h>
#include <gtest/gtest.h>

namespace std
{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
};