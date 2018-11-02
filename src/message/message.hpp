/*******************************************************************************
 *                                                                             *
 *  Filename    : message.hpp                                                  *
 *  Project     : Alan Compiler                                                *
 *  Version     : 1.0                                                          *
 *  Author      : Spiros Dontas                                                *
 *  Email       : spirosdontas@gmail.com                                       *
 *                                                                             *
 *  Description : Messager class header file                                   *
 *                                                                             *
 *******************************************************************************/

#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

#include <iostream>
#include <string>
#include <deque>

typedef std::deque<std::string> stringStack;

extern int    linecount;
extern char * filename;

/*******************************************************************************
 * Variadic template functions must be declared and implemented in header
 * file, or else the compiler does not know what is going on.
 * Also C++17 folds are OP.
 *******************************************************************************/

template<typename ... Args>
void display(Args&& ... args) {
    (std::cerr << ... << args) << std::endl;
}

template<typename ... Args>
void error(Args&& ... args) {
    std::cerr << "*ERROR*" << std::endl;
    std::cerr << "File: " << filename << std::endl;
    std::cerr << "Line " << linecount << ": ";
    display(args...);
    exit(-1);
}

template<typename ... Args>
void fatal(Args&& ... args) {
    std::cerr << "*FATAL*" << std::endl;
    std::cerr << "File: " << filename << std::endl;
    std::cerr << "Line " << linecount << ": ";
    display(args...);
    exit(-2);
}

template<typename ... Args>
void internal(Args&& ... args) {
    std::cerr << "*INTERNAL*" << std::endl;
    std::cerr << "File: " << filename << std::endl;
    std::cerr << "Line " << linecount << ": ";
    display(args...);
    exit(-3);
}

template<typename ... Args>
void warning(Args&& ... args) {
    std::cerr << "*WARNING*" << std::endl;
    std::cerr << "File: " << filename << std::endl;
    std::cerr << "Line " << linecount << ": ";
    display(args...);
}

template<typename ... Args>
void debug(Args&& ... args) {
#ifndef NDEBUG
    display(args...);
#endif
}

/*******************************************************************************
 * Debugger class
 *   > Helps with printing the ast.
 *   > It does this while performing semantic analysis.
 *   > It prints types too ( so cool ).
 *******************************************************************************/
class Debugger {
    private :
        std::string prefix = "| ";
        stringStack prefixes;
    public :
        void newLevel();
        void restoreLevel();
        template<typename ... Args>
            void show(Args&& ... args){
#ifndef NDEBUG
                std::cerr << this->prefix;
                display(args...);
#endif
            }
};

#endif
