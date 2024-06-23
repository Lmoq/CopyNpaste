#include <iostream>
#include <callback.h>
#include <algorithm>


std::string FileStream::save_text_filename;
std::ofstream FileStream::save_text_output_stream;

std::ifstream FileStream::save_text_input_stream;
std::ifstream FileStream::copy_text_input_stream;

void FileStream::loadFiles( const char *savetextFile, const char *copytextFile )
{
    FileStream::save_text_output_stream.open( savetextFile, std::ios_base::app | std::ios_base::out );
    FileStream:save_text_input_stream.open( savetextFile, std::ios_base::ate | std::ios_base::in );

    FileStream::copy_text_input_stream.open( copytextFile, std::ios_base::in );
    FileStream::save_text_filename = savetextFile;

    if ( !FileStream::save_text_output_stream.is_open() || !FileStream::save_text_input_stream.is_open()) 
    {
        std::cout << "Savefile path : " << savetextFile << '\n';
        return;
    }
    if ( !FileStream::copy_text_input_stream.is_open() )
    {
        std::cout << "Copyfile path : " << copytextFile << '\n';
        return;
    }
    std::cout << "File loaded : " << FileStream::save_text_filename << '\n';
}

std::string FileStream::getClipText()
{
    HGLOBAL hglbMem;
    std::string text = "";

    if ( !IsClipboardFormatAvailable( CF_TEXT ) ) {
        std::cout << "No text available\n";
        return text;
    }
    if ( !OpenClipboard( NULL ) ) {
        std::cout << "Clipboard failed\n";
        return text;
    }

    hglbMem = GetClipboardData( CF_TEXT );
    if ( hglbMem != NULL )
    {
        const char *retrived = static_cast<char *>( GlobalLock( hglbMem ) );
        text = retrived;
        GlobalUnlock( hglbMem );
    }

    CloseClipboard();
    return text;
}

void FileStream::saveText( std::string &text )
{
    FileStream::save_text_input_stream.seekg( -1, std::ios_base::cur );

    if ( FileStream::save_text_input_stream.get() != '\n' ) {
        FileStream::save_text_output_stream << '\n';
    }

    auto search = std::find( text.begin(), text.end(), '\r' );
    while ( search != text.end() )
    {
        text.erase( search );
        search = std::find( text.begin(), text.end(), '\r' );
    }

    if ( !text.empty() ) {
        FileStream::save_text_output_stream << text;
        FileStream::save_text_output_stream.flush();
    }
}

void FileStream::pasteTextClip( std::string &text )
{
    HGLOBAL hglbMem;

    if ( !OpenClipboard( NULL ) ) {
        std::cout << "Clipboard failed to open\n";
        return;
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
    std::string text;
    std::getline( FileStream::copy_text_input_stream, text );

    if ( FileStream::copy_text_input_stream.tellg() == -1 ) 
    {
        FileStream::copy_text_input_stream.clear();
        FileStream::copy_text_input_stream.seekg( 0 );
    }
    FileStream::pasteTextClip( text += '\n' );
}

void postSaveMessage()
{
    DWORD threadId = GetCurrentThreadId();
    PostThreadMessage( threadId, WM_SAVECLIP, 0, 0 );
}

void terminate() {
    PostQuitMessage(0);
}


