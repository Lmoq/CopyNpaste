#define _MAIN_SRC_
#include <iostream>
#include <main.h>
#include <hotkey.h>
#include <callback.h>


static HHOOK hook;
static KBDLLHOOKSTRUCT *kbd;

static Hotkey HotKey;
static std::string text = "";

int main()
{
    INIT_HOTKEY( HotKey );
    FileStream::loadFiles( "data/saved.txt", "data/copy.txt" );

    hook = SetWindowsHookEx( WH_KEYBOARD_LL, &LowLevelKeyboardProc, NULL, 0 );
    if ( !hook ) {
        std::cout << "Hook failed\n";
        return 1;
    }

    MSG msg;
    while( GetMessage( &msg, NULL, 0, 0 ) > 0 )
    {
        TranslateMessage( &msg );
        DispatchMessage( &msg );

        if ( msg.message == WM_SAVECLIP ) {
            Sleep( 10 );
            text = FileStream::getClipText();
            FileStream::saveText( text );
        }
    }
    UnhookWindowsHookEx( hook );
    std::cout << "Exited Normally\n";
    return 0;
}

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    if ( nCode == HC_ACTION ) {
        kbd = reinterpret_cast<KBDLLHOOKSTRUCT *>( lParam );

        switch ( wParam ) {
            case WM_KEYDOWN:
                if ( kbd->vkCode == VK_RSHIFT ) {
                    PostQuitMessage( 0 );
                }
                HotKey.keydown( kbd->vkCode );
                break;

            case WM_KEYUP:
                HotKey.keyup( kbd->vkCode );
                break;
            
            default:
                break;
        }
    }
    return CallNextHookEx( NULL, nCode, wParam, lParam );
} 
