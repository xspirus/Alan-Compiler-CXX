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

def main(argv) :
    inFile, outFile, extras = args(argv)
    if ( os.path.isdir(TEMP) ) :
        shutil.rmtree(TEMP)
    os.makedirs(TEMP)
    tempLL, tempS = makeTempFiles(TEMP, outFile)
    sp.run([COMPILER, inFile], stdin=open(inFile), stdout=open(tempLL, 'w'))
    OPTCMD = [OPT, '-S', tempLL, '-o', tempLL]
    OPTCMD.extend(extras)
    sp.run(OPTCMD)
    LLCCMD = [LLC, tempLL]
    sp.run(LLCCMD)
    LINKCMD = [LINKER, tempS, ALANLIB, '-o', outFile]
    sp.run(LINKCMD)

if __name__ == '__main__' :
    main(sys.argv[1:])
