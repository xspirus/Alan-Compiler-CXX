#!/usr/bin/env python3

################################################################################
#                                                                              #
#   Filename    : alanc.py                                                     #
#   Project     : Alan Compiler                                                #
#   Version     : 1.0                                                          #
#   Author      : Spiros Dontas                                                #
#   Email       : spirosdontas@gmail.com                                       #
#                                                                              #
#   Description : Script to run the whole compiler and produce executable      #
#                                                                              #
################################################################################

import os
import sys
import shutil
import subprocess as sp

ALANDIR = os.path.dirname(os.path.dirname(os.path.realpath(__file__)))
COMPILER = os.path.join(ALANDIR, 'bin', 'ALAN')
ALANLIB = os.path.join(ALANDIR, 'libs', 'libalanstd.a')
LLC = 'llc'
OPT = 'opt'
LINKER = 'clang'

TEMP = os.path.join(ALANDIR, 'tmp')
EXECS = os.path.join(ALANDIR, 'execs')

def args(argv) :
    inFile = ''
    outFile = 'a.out'
    extras = []
    i = 0
    while (i < len(argv)) :
        if ( argv[i].endswith('.alan') ) :
            inFile = argv[i]
        elif ( argv[i] == '-o' ) :
            i = i + 1
            outFile = argv[i]
        else :
            extras.append(argv[i])
        # end if
        i = i + 1
    # end for
    return inFile, outFile, extras
# end args

def makeTempFiles(tempdir, name) :
    tempLL = os.path.join(tempdir, name + '.ll')
    tempS = os.path.join(tempdir, name + '.s')
    return tempLL, tempS

def help() :
    lines = [
            "ALAN COMPILER v1.0.0",
            "",
            "usage: acc [options] file",
            "",
            "Options:",
            "  -o <file>",
            "    Place the output file to <file>",
            "    Output files are placed by default in folder \"execs\"",
            "  -O0",
            "    Compile with no optimisations",
            "  -O1",
            "    Enable basic optimisations",
            "  -O2",
            "    Enable more optimisations",
            "  -O3",
            "    Enable all optimisations",
            "  --help",
            "    Show this menu",
            ""
            ]
    for line in lines :
        print(line)
    sys.exit(0)
# end help

def main(argv) :
    inFile, outFile, extras = args(argv)
    if ( "--help" in extras ) :
        help()
    if ( os.path.isdir(TEMP) ) :
        shutil.rmtree(TEMP)
    os.makedirs(TEMP)
    os.makedirs(EXECS)
    tempLL, tempS = makeTempFiles(TEMP, outFile)
    outFile = os.path.join(EXECS, outFile)
    sp.run([COMPILER, inFile], stdin=open(inFile), stdout=open(tempLL, 'w'))
    OPTCMD = [OPT, '-S', tempLL, '-o', tempLL]
    OPTCMD.extend(extras)
    sp.run(OPTCMD)
    LLCCMD = [LLC, tempLL]
    sp.run(LLCCMD)
    LINKCMD = [LINKER, tempS, ALANLIB, '-o', outFile]
    sp.run(LINKCMD)
    if ( os.path.isdir(TEMP) ) :
        shutil.rmtree(TEMP)

if __name__ == '__main__' :
    main(sys.argv[1:])
