#ifndef _MAIN_
#define _MAIN_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _MAIN_SRC_
    LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam );
#endif


#endif