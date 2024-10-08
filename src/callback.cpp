#include <iostream>
#include <callback.h>
#include <algorithm>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

std::string FileStream::save_text_filename;
std::ofstream FileStream::save_text_output_stream;

std::ifstream FileStream::save_text_input_stream;
std::ifstream FileStream::copy_text_input_stream;

DWORD FileStream::threadID;
BOOL FileStream::copyPasteOn = FALSE;

void FileStream::loadFiles( const char *savetextFile, const char *copytextFile )
{
    checkExistenceAndCreate( savetextFile );
    checkExistenceAndCreate( copytextFile );
    
    save_text_output_stream.open( savetextFile, std::ios_base::app | std::ios_base::out );
    save_text_input_stream.open( savetextFile, std::ios_base::in );
    copy_text_input_stream.open( copytextFile, std::ios_base::in );
    
    save_text_filename = savetextFile;

    if ( save_text_output_stream.is_open() && copy_text_input_stream.is_open() ) 
    {
        std::cout << "Input and output filestreams successfully loaded : " << savetextFile << " | " << copytextFile << '\n';
    }
}

bool FileStream::checkExistenceAndCreate( const char *filepath )
{
    fs::path file{ filepath };
    if ( !fs::exists( file ) ) 
    {
        std::cout << "Creating file : " << filepath << '\n';
        if ( !fs::exists( file.parent_path() ) ) 
        {
            fs::create_directory( file.parent_path() );
        }
        std::ofstream ofs{ filepath };
        return false;
    }
    return true;
}

std::string FileStream::getClipText()
{
    HGLOBAL hglbMem;
    std::string text = "";
    UINT8 retries = 0;

    if ( !IsClipboardFormatAvailable( CF_TEXT ) ) {
        std::cout << "No text available\n";
        return text;
    }

    while ( !OpenClipboard( NULL ) ) {
        retries ++;
        if ( retries > 49 ) {
            std::cout << "OpenCliboard failed\n";
            return text;
        }
    }
    hglbMem = GetClipboardData( CF_TEXT );

    if ( hglbMem != NULL ) {
        text = static_cast<char *>( GlobalLock( hglbMem ) );
        GlobalUnlock( hglbMem );
    }

    CloseClipboard();
    return text;
}

void FileStream::saveText( std::string &text )
{
    save_text_input_stream.seekg( -1, std::ios_base::end );
    if ( save_text_input_stream.get() != '\n' && save_text_input_stream.tellg() != -1 )
    {
        save_text_output_stream << '\n';
    }

    // Remove all '\r' as it get converted to '\n'
    auto search = std::find( text.begin(), text.end(), '\r' );
    while ( search != text.end() )
    {
        text.erase( search );
        search = std::find( text.begin(), text.end(), '\r' );
    }

    if ( !text.empty() ) {
        save_text_output_stream << text;
        save_text_output_stream.flush();
    }
    // Clear state in order to continue io operations
    save_text_input_stream.clear();
}

void FileStream::pasteTextClip( std::string &text )
{
    HGLOBAL hglbMem;
    UINT8 retries = 0;

    while ( !OpenClipboard( NULL ) ) {
        retries ++;
        if ( retries > 49 )
        {
            std::cout << "Clipboard failed to open\n";
            return;
        }
    }
    EmptyClipboard();

    hglbMem = GlobalAlloc( GMEM_MOVEABLE, text.length() + 1 );
    if ( hglbMem != NULL )
    {
        char *text_ = static_cast<char *>( GlobalLock( hglbMem ) );
        memcpy( text_, text.c_str(), text.length() + 1 );

        text_[ text.length() ] = '\0';
        GlobalUnlock( text_ );

        SetClipboardData( CF_TEXT, hglbMem );
        GlobalFree( hglbMem );
    }
    CloseClipboard();
}

void FileStream::cyclePaste()
{
    if ( !copyPasteOn ) {
        return;
    }
    std::string text;
    std::getline( copy_text_input_stream, text );

    if ( copy_text_input_stream.tellg() == -1 ) 
    {
        // Clear state in order to continue io operations
        copy_text_input_stream.clear();
        copy_text_input_stream.seekg( 0 );
    }
    pasteTextClip( text );
}

void FileStream::toggleCopyPaste()
{
    copyPasteOn = !copyPasteOn;
    std::cout << "Copy Paste " << (( copyPasteOn == TRUE ) ? "On" : "Off") << '\n';
}

void postSaveMessage()
{
    DWORD threadId = GetCurrentThreadId();
    PostThreadMessage( threadId, WM_SAVECLIP, 0, 0 );
}

#include <unordered_map>
static std::unordered_map<HWND, HWND> WindowsMap;

void setTopLevel( HWND hWnd )
{
    CHAR winName[MAX_PATH];

    if ( IsWindowVisible( hWnd ) && GetWindowTextA( hWnd, winName, MAX_PATH ) ) 
    {
        auto item = WindowsMap.find( hWnd );
        if ( item != WindowsMap.end() )
        {
            if ( item->second == HWND_TOPMOST )
            {
                if ( SetWindowPos( hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE ) )
                {
                    WindowsMap[ hWnd ] = HWND_NOTOPMOST;
                    std::cout << "NOTOPMOST : [ " << winName << " ]\n";
                }
            }
            else if ( item->second == HWND_NOTOPMOST )
            {
                if ( SetWindowPos( hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE ) )
                {
                    WindowsMap[ hWnd ] = HWND_TOPMOST;
                    std::cout << "TOPMOST : [ " << winName << " ]\n";
                }
            }
        }
        else
        {
            if ( SetWindowPos( hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE ) )
            {
                WindowsMap[ hWnd ] = HWND_TOPMOST;
                std::cout << "TOPMOST : [ " << winName << " ]\n";
            }
        }
    }  
}

void setFGTopLevel()
{
    setTopLevel( GetForegroundWindow() );
}

void revertTopMost()
{
    CHAR winName[MAX_PATH];
    for ( auto &item : WindowsMap )
    {
        if ( item.second == HWND_TOPMOST )
        {
            GetWindowTextA( item.first, winName, MAX_PATH );
            SetWindowPos( item.first, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
            WindowsMap[ item.first ] = HWND_NOTOPMOST;
        }
    }
}



