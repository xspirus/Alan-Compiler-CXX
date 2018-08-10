/*******************************************************************************
 *                                                                             *
 *  Filename    : error.hpp                                                    *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Errors / Warnings / Internals / Fatals header file           *
 *                                                                             *
 *******************************************************************************/

#ifndef __ERROR_HPP__
#define __ERROR_HPP__

/*******************************************************************************
 *********************** Simple Error Handling Functions ***********************
 *******************************************************************************/

void internal(const char * fmt, ...);
void fatal(const char * fmt, ...);
void error(const char * fmt, ...);
void warning(const char * fmt, ...);

extern int    linecount;
extern char * filename;

#endif
