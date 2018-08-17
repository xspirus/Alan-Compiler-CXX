/*******************************************************************************
 *                                                                             *
 *  Filename    : fix.cpp                                                      *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Fixing escaped chars source file                             *
 *                                                                             *
 *******************************************************************************/

#include <string.h>
#include <string>

static int findDigit(char c) {
    if ( c >= '0' && c <= '9' )
        return c - '0';
    else if ( c >= 'A' && c <= 'F' )
        return c - 'A' + 10;
    else
        return c - 'a' + 10;
    return -1;
}

static unsigned char fixHex(char * s) {
    int p1, p2;
    p1 = findDigit(s[0]);
    p2 = findDigit(s[1]);
    return p1 * 16 + p2;
}

unsigned char fixChar(char * s, int& shift) {
    if ( s[0] != '\\' )
        shift = 1;
        return s[0];
    if ( s[1] == 'x' )
        shift = 4;
        return fixHex(s + 2);
    switch( s[1] ) {
        shift = 2;
        case 'n'  : return '\n';
        case 't'  : return '\t';
        case 'r'  : return '\r';
        case '0'  : return '\0';
        case '\\' : return '\\';
        case '\'' : return '\'';
        case '\"' : return '\"';
        default   : return '\0';
    }
}

char * fixString(char * s) {
    char * n = new char[strlen(s) + 1];
    int index = 0;
    int i = 0;
    int shift;
    while ( s[index] != '\"' ) {
        n[i] = fixChar(s + index, shift);
        index += shift;
        i++;
    }
    n[i] = '\0';
    return n;
}
