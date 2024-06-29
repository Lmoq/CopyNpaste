#include <iostream>
#include <callback.h>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

std::string FileStream::save_text_filename;
std::ofstream FileStream::save_text_output_stream;

std::ifstream FileStream::save_text_input_stream;
std::ifstream FileStream::copy_text_input_stream;


void FileStream::loadFiles( const char *savetextFile, const char *copytextFile )
{
    if ( !checkExistenceAndCreate( savetextFile ) ) {
        std::cout << "SaveFile created : " << savetextFile << '\n';
    }

    if ( !checkExistenceAndCreate( copytextFile ) ) {
        std::cout << "Copyfile created : " << copytextFile << '\n';
    }
    
    save_text_output_stream.open( savetextFile, std::ios_base::app | std::ios_base::out );
    save_text_input_stream.open( savetextFile, std::ios_base::in );

    copy_text_input_stream.open( copytextFile, std::ios_base::in );
    save_text_filename = savetextFile;

    if ( save_text_input_stream.is_open() && save_text_output_stream.is_open() && copy_text_input_stream.is_open() ) 
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
        if ( !fs::exists( file.parent_path() ) ) {
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

    if ( !IsClipboardFormatAvailable( CF_TEXT ) ) {
        std::cout << "No text available\n";
        return text;
    }
    if ( !OpenClipboard( NULL ) ) {
        std::cout << "Clipboard failed\n";
        return text;
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

    if ( save_text_input_stream.get() != '\n' && !fs::is_empty( fs::path( save_text_filename ) ) ) {
        save_text_output_stream << '\n';
    }

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
    std::getline( copy_text_input_stream, text );

    if ( copy_text_input_stream.tellg() == -1 ) 
    {
        copy_text_input_stream.clear();
        copy_text_input_stream.seekg( 0 );
    }
    pasteTextClip( text += '\n' );
}

void postSaveMessage()
{
    DWORD threadId = GetCurrentThreadId();
    PostThreadMessage( threadId, WM_SAVECLIP, 0, 0 );
}

void terminate() {
    PostQuitMessage(0);
}


