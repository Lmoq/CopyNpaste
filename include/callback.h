#ifndef _CALLBACK_
#define _CALLBACK_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define WM_SAVECLIP WM_APP + 2
#include <fstream>

class FileStream {

public:

    static std::string save_text_filename;
    static std::ofstream save_text_output_stream;

    static std::ifstream save_text_input_stream;
    static std::ifstream copy_text_input_stream;

    static DWORD threadID;
    static BOOL copyPasteOn;
    
    static void loadFiles( const char *savetextFile, const char* copytextFile );
    static bool checkExistenceAndCreate( const char *filepath );

    static void toggleCopyPaste();

    static std::string getClipText();
    static void saveText( std::string &text );

    static void pasteTextClip( std::string &text );
    static void cyclePaste();
};
void postSaveMessage();

void setTopLevel( HWND hWnd );
void setFGTopLevel();
void revertTopMost();

#endif