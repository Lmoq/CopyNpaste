#ifndef _HOTKEY_
#define _HOTKEY_

#include <unordered_set>
#include <unordered_map>
#include <main.h>

class Hotkey {

public:
    typedef struct {
        std::unordered_set<DWORD> vkCodeSet;
        void ( *callback )();
        bool active = false;
    } hotkey;
    hotkey *active_hotkey = NULL;

    std::unordered_map<unsigned int, hotkey> hotkeyMap;
    std::unordered_set<DWORD> recorded_keys;

    unsigned int sample_prime = 16777619;

    void addhotkey( std::unordered_set<DWORD> vkCodes, void ( *callback )() );
    unsigned int hashKeys( std::unordered_set<DWORD> vkCodes );
    bool keyExists( std::unordered_set<DWORD> vkCodes, unsigned int &hashed_value );
    
    void keydown( DWORD vkCode );
    void keyup( DWORD vkCode );
};
void INIT_HOTKEY( Hotkey &HotKey );

#endif