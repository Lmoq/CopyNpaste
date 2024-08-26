#define _MAIN_SRC_
#include <iostream>
#include <thread>
#include <main.h>
#include <hotkey.h>
#include <callback.h>

static std::string text = "";

static BOOL running = TRUE;
static DWORD threadID;

static std::thread keythread;

int main()
{
    FileStream::loadFiles( "copynsave/saved.txt", "copynsave/copy.txt" );

    Hotkey::add_hotkey( {VK_LCONTROL, 0x43}, postSaveMessage       , NULL, FALSE );
    Hotkey::add_hotkey( {VK_LCONTROL, 0x56}, FileStream::cyclePaste, NULL, FALSE );
    Hotkey::add_hotkey( {VK_F10}           , Hotkey::terminate     , NULL, TRUE );

    Hotkey::run();
    Hotkey::wait();
    return 0;
}

void Hotkey::msgLoop()
{
    MSG msg;
    while ( GetMessage( &msg, NULL, 0,0 ) > 0 )
    {
        if ( msg.message == WM_EXIT ) {
            PostQuitMessage(0);
        }
        if ( msg.message == WM_SAVECLIP )
        {
            Sleep(10);
            std::string saveText = FileStream::getClipText();
            FileStream::saveText( saveText );
        }
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
}





