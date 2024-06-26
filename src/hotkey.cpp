#include <iostream>
#include <numeric>
#include <algorithm>
#include <hotkey.h>
#include <callback.h>
#include <utils.h>


void INIT_HOTKEY( Hotkey &HotKey )
{
    HotKey.addhotkey( { VK_OEM_PERIOD, 0x31 }, &terminate ); // Press ". + 1" to terminate program
    HotKey.addhotkey( { VK_LCONTROL,   0x43 }, &postSaveMessage );
    HotKey.addhotkey( { VK_LCONTROL,   0x56 }, &FileStream::cyclePaste );
}

void Hotkey::addhotkey( std::unordered_set<DWORD> vkCodes, void ( *callback )() )
{
    hotkey hkey;
    hkey.vkCodeSet = vkCodes;
    hkey.callback = callback;
    
    hotkeyMap[ hashKeys( vkCodes ) ] = hkey;
}

DWORD Hotkey::hashKeys( std::unordered_set<DWORD> vkCodes )
{
    DWORD hashed_value = 0;
    for ( auto item : vkCodes ) {
        hashed_value += static_cast<DWORD>( ( power( item, item ) ^ sample_prime ) / item );
    }
    return hashed_value % 1999;
}

bool Hotkey::keyExists( std::unordered_set<DWORD> vkCodes, unsigned int &hashed_value )
{
    hashed_value = hashKeys( vkCodes );
    if ( hotkeyMap.find( hashed_value ) != hotkeyMap.end() ) {
        return true;
    }
    return false;
}

void Hotkey::keydown( DWORD vkCode )
{
    unsigned int hashed_value;
    recorded_keys.insert( vkCode );

    if ( !keyExists( recorded_keys, hashed_value ) ) {
        return;
    }

    hotkey &hkey = hotkeyMap[ hashed_value ];
    if ( hkey.active == false )
    {
        hkey.callback();
        hkey.active = true;
        
        active_hotkey = &hkey;
    }
}

void Hotkey::keyup( DWORD vkCode )
{
    if ( recorded_keys.count( vkCode ) != 0 ) {
        recorded_keys.erase( vkCode );
    }

    if ( active_hotkey != NULL ) {
        active_hotkey->active = false;
        active_hotkey = NULL;
    }

}
