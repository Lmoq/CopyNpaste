#include <utils.h>


int power( int base, int exp )
{
    int result = base;
    for ( int i = 1; i < exp; i ++ ) 
    {
        result *= base;
    }
    return result;
}