/*******************************************************************************
 *                                                                             *
 *  Filename    : error.cpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Errors / Warnings / Internals / Fatals source code           *
 *                                                                             *
 *******************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <error/error.hpp>

/*******************************************************************************
 ******************** Simple Error Handling Functions Code *********************
 *******************************************************************************/

void internal (const char * fmt, ...) {

    va_list ap;

    va_start(ap, fmt);
    if (fmt[0] == '\r')
        fmt++;
    else
        fprintf(stderr, "%s:%d: ", filename, linecount);
    fprintf(stderr, "Internal Error, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(1);

}

void fatal (const char * fmt, ...) {

    va_list ap;

    va_start(ap, fmt);
    if (fmt[0] == '\r')
        fmt++;
    else
        fprintf(stderr, "%s:%d: ", filename, linecount);
    fprintf(stderr, "Fatal Error, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

    exit(1);

}

void error (const char * fmt, ...) {

    va_list ap;

    va_start(ap, fmt);
    if (fmt[0] == '\r')
        fmt++;
    else
        fprintf(stderr, "%s:%d: ", filename, linecount);
    fprintf(stderr, "Error, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fprintf(stderr, "Hello! My name is \"Alan Compiler\".\n");
    fprintf(stderr, "I could fix the problem, but I will not :P\n");
    fprintf(stderr, "Sorry...not sorry :)\n");
    va_end(ap);

    exit(1);

}

void warning (const char * fmt, ...) {

    va_list ap;

    va_start(ap, fmt);
    if (fmt[0] == '\r')
        fmt++;
    else
        fprintf(stderr, "%s:%d: ", filename, linecount);
    fprintf(stderr, "Warning, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);

}
