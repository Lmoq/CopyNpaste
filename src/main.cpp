#define _MAIN_SRC_
#include <iostream>
#include <thread>
#include <hotkey.h>
#include <callback.h>

static std::string text = "";

static BOOL running = TRUE;
static DWORD threadID;

static std::thread keythread;

int main()
{
    FileStream::loadFiles( "copynsave/saved.txt", "copynsave/copy.txt" );
    std::cout << "\nToggle copypaste with ( ctrl + F9 )\n"
              << "Terminate app with ( ctrl + F10 )\n"
              << "Pressing copy ( ctrl + c ), text will be saved into copynpaste/saved.txt\n"
              << "Pressing paste ( ctrl + v / shift + insert ) will paste each lines inside copynpaste/copy.txt\n\n"
              << "To toggle a window to Top Most Level, press console key ( OEM_3 ) / ( ~ )\n\n";
    Hotkey::add_hotkey( { VK_F9, VK_LCONTROL } , FileStream::toggleCopyPaste, NULL, TRUE );

    Hotkey::add_hotkey( { VK_LCONTROL, 0x43 }, postSaveMessage       , NULL, FALSE );
    Hotkey::add_hotkey( { VK_RCONTROL, 0x43 }, postSaveMessage       , NULL, FALSE );

    Hotkey::add_hotkey( { VK_LCONTROL, 0x56 }, FileStream::cyclePaste, NULL, FALSE );
    Hotkey::add_hotkey( { VK_RCONTROL, 0x56 }, FileStream::cyclePaste, NULL, FALSE );

    Hotkey::add_hotkey( { VK_LSHIFT, VK_INSERT }, FileStream::cyclePaste, NULL, FALSE );
    Hotkey::add_hotkey( { VK_RSHIFT, VK_INSERT }, FileStream::cyclePaste, NULL, FALSE );

    Hotkey::add_hotkey( { VK_OEM_3 }            , setFGTopLevel         , NULL, TRUE );
    Hotkey::add_hotkey( { VK_LCONTROL, VK_F10 } , Hotkey::terminate     , NULL, TRUE );

    Hotkey::run();
    Hotkey::wait();
    printf("EOF\n");
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
        if ( msg.message == WM_SAVECLIP && FileStream::copyPasteOn )
        {
            Sleep(10);
            std::string saveText = FileStream::getClipText();
            FileStream::saveText( saveText );
        }
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
}

void Hotkey::terminate()
{
    revertTopMost();
    PostThreadMessage( threadID, WM_EXIT, 0, 0 );
}





