/*******************************************************************************
 *                                                                             *
 *  Filename    : lib.c                                                        *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Library Functions Implementation in C (to convert to asm)    *
 *                                                                             *
 *******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static void __alan_FATAL(const char *fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, "Alan runtime error: ");
    vfprintf(stderr, fmt, arg);
    fprintf(stderr, "\n");
    exit(1);
}

static void skipLine() {
    return;
}

void writeInteger(int32_t n) {
    printf("%d", n);
}

void writeByte(uint8_t b) {
    printf("%hhu", b);
}

void writeChar(uint8_t b) {
    printf("%c", b);
}

void writeString(uint8_t *s) {
    printf("%s", s);
}

int32_t readInteger() {
    int32_t n;
    if ( scanf("%d", &n) != 1 )
        __alan_FATAL("cannot read integer");
    return n;
}

uint8_t readByte() {
    uint8_t b;
    if ( scanf("%hhu", &b) != 1 )
        __alan_FATAL("cannot read byte");
    return b;
}

uint8_t readChar() {
    uint8_t b;
    if ( scanf("%c", &b) != 1 )
        __alan_FATAL("cannot read char");
    return b;
}

void readString(int32_t n, uint8_t *s) {
    for ( int32_t i = 0; i < n; i++ ) {
        int8_t c = ( i + 1 < n ) ? getchar() : '\0';
        if ( c == '\n' || c == EOF ) {
            *s++ = '\0';
            return;
        }
        *s++ = (uint8_t) c;
    }
    return;
}

int32_t extend(uint8_t b) {
    return (int32_t) b;
}

uint8_t shrink(int32_t i) {
    return (uint8_t)(i & 0xFF);
}

int32_t strlen(uint8_t *s) {
    int32_t size = 0;
    while ( *s != '\0' ) {
        size++;
        *s++;
    }
    return size;
}

int32_t strcmp(uint8_t *s1, uint8_t *s2) {
    for (int32_t i = 0; ; i++) {
        if ( s1[i] != s2[i] )
            return ( s1[i] < s2[i] ) ? -1 : 1;
        if ( s1[i] == '\0' )
            return 0;
    }
    return 42;
}

void strcpy(uint8_t *trg, uint8_t *src) {
    for ( int32_t i = 0; ; i++ ) {
        trg[i] = src[i];
        if ( src[i] == '\0' )
            return;
    }
    return;
}

void strcat(uint8_t *trg, uint8_t *src) {
    int32_t i, j;
    for ( i = 0; trg[i] != '\0'; i++ )
        ;
    for ( j = 0; ; j++ ) {
        trg[i + j] = src[j];
        if ( src[j] == '\0' )
            return;
    }
    return;
}
