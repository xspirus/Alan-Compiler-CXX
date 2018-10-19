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

template<typename T>
void display(T t) {
    std::cerr << t;
}

template<typename T, typename ... Args>
void display(T t, Args ... args) {
    std::cerr << t;
    display(args...);
}

template<typename ... Args>
void error(Args ... args) {
    std::cerr << "*ERROR*" << std::endl;
    std::cerr << "File: " << filename << std::endl;
    std::cerr << "Line " << linecount << ": ";
    display(args...);
}

template<typename ... Args>
void fatal(Args ... args) {
    std::cerr << "*FATAL*" << std::endl;
    std::cerr << "File: " << filename << std::endl;
    std::cerr << "Line " << linecount << ": ";
    display(args...);
}

template<typename ... Args>
void internal(Args ... args) {
    std::cerr << "*INTERNAL*" << std::endl;
    std::cerr << "File: " << filename << std::endl;
    std::cerr << "Line " << linecount << ": ";
    display(args...);
}

template<typename ... Args>
void warning(Args ... args) {
    std::cerr << "*WARNING*" << std::endl;
    std::cerr << "File: " << filename << std::endl;
    std::cerr << "Line " << linecount << ": ";
    display(args...);
}

template<typename ... Args>
void debug(Args ... args) {
#ifndef DEBUG
    display(args...);
#endif
}

class Debugger {
    private :
        std::string prefix = "| ";
        stringStack prefixes;
    public :
        void newLevel();
        void restoreLevel();
        template<typename ... Args>
            void show(Args ... args){
                std::cerr << this->prefix;
                display(args...);
            }
};

#endif
