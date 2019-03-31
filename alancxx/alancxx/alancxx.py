#!/usr/bin/env python3

import os
import subprocess as sp
import argparse
import shutil


def parse_arguments():
    parser = argparse.ArgumentParser(
        prog="acc", description="Compile a Alan file to executable."
    )
    parser.add_argument(
        "--version", action="version", version="Alan Compiler v0.0.2"
    )
    parser.add_argument(
        "filename", type=str, metavar="FILENAME", help="file to compile"
    )
    parser.add_argument(
        "-o",
        type=str,
        metavar="OUTPUT",
        dest="output",
        help="output file",
        default="a.out",
    )
    intermediate = parser.add_argument_group(
        title="intermediate compilations"
    ).add_mutually_exclusive_group()
    intermediate.add_argument(
        "-L", action="store_true", help="compile to LLVM IR"
    )
    intermediate.add_argument(
        "-S", action="store_true", help="compile to assembly"
    )
    opts = parser.add_argument_group(
        title="optimization options"
    ).add_mutually_exclusive_group()
    opts.add_argument(
        "-O0",
        dest="opts",
        action="store_const",
        const="-O0",
        help="disable all optimizations (default)",
        default="-O0",
    )
    opts.add_argument(
        "-O1",
        dest="opts",
        action="store_const",
        const="-O1",
        help="enable some optimizations",
    )
    opts.add_argument(
        "-O2",
        dest="opts",
        action="store_const",
        const="-O2",
        help="enable more optimizations",
    )
    opts.add_argument(
        "-O3",
        dest="opts",
        action="store_const",
        const="-O3",
        help="enable all optimizations",
    )
    return parser.parse_args()


def updir(path, num=1):
    """Go up num dirs.

    Parameters
    ----------

    path: str
        Original path.

    num: int (default: 1)
        Number of directories to go up.

    Returns
    -------

    path: str
        Result directory.
    """
    for _ in range(num):
        path = os.path.dirname(path)
    return path


def cleanup(*args):
    """Remove directories.

    Parameters
    ----------

    *args: iterable
        Directories to remove.
    """
    for arg in args:
        if os.path.isdir(arg):
            shutil.rmtree(arg)


def compile_llvm(filename: str, cmd: str, temp: str):
    """Function to compile to LLVM IR.

    Parameters
    ----------

    filename: str
        File to compile.

    cmd: str
        Command to run.

    temp: str
        Path to save temporary files.

    Returns
    -------

    filename: str
        Filename where code was saved.
    """
    llvm = os.path.basename(filename).partition(".")[0]
    llvm = os.path.join(temp, f"{llvm}.ll")
    with open(filename, "r") as fp:
        with open(llvm, "w") as gp:
            sp.run([cmd, filename], stdin=fp, stdout=gp)
    return llvm


def compile_optimizations(filename: str, cmd: str, opts: str):
    """Function to apply optimizations to LLVM IR.

    Parameters
    ----------

    filename: str
        File to optimize.

    cmd: str
        Command to run.

    opts: str
        Optimizations to apply.
    """
    sp.run([cmd, "-S", opts, filename, "-o", filename])


def compile_assembly(filename: str, cmd: str, temp: str):
    """Function to compile to assembly.

    Parameters
    ----------

    filename: str
        File to compile.

    cmd: str
        Command to run.

    temp: str
        Path to save temporary files.

    Returns
    -------

    filename: str
        Filename where code was saved.
    """
    assembly = os.path.basename(filename).partition(".")[0]
    assembly = os.path.join(temp, f"{assembly}.s")
    with open(assembly, "w") as fp:
        sp.run([cmd, filename], stdout=fp)
    return assembly


def compile_executable(
    filename: str, cmd: str, lib: str, output: str, execs: str
):
    """Function to produce executable.

    Parameters
    ----------

    filename: str
        File to compile.

    cmd: str
        Command to run.

    lib: str
        Library path.

    output: str
        Name of output file.
    
    execs: str
        Path to save executable files.
    """
    output = os.path.join(execs, output)
    sp.run([cmd, filename, lib, "-o", output])


if __name__ == "__main__":
    args = parse_arguments()
    root = updir(os.path.abspath(__file__), 3)
    compiler = os.path.join(root, "bin", "ALAN")
    lib = os.path.join(root, "libs", "libalanstd.a")
    opt = "opt-6.0"
    llc = "llc-6.0"
    linker = "clang-6.0"
    temp = os.path.join(root, "tmp")
    execs = os.path.join(root, "execs")
    os.makedirs(temp, exist_ok=True)
    os.makedirs(execs, exist_ok=True)
    llvm = compile_llvm(args.filename, compiler, temp)
    compile_optimizations(llvm, opt, args.opts)
    if args.L:
        with open(llvm, "r") as fp:
            print(fp.read())
        cleanup(temp)
        exit(0)
    assembly = compile_assembly(llvm, llc, temp)
    if args.S:
        with open(assembly, "r") as fp:
            print(fp.read())
        cleanup(temp)
        exit(0)
    compile_executable(assembly, linker, lib, args.output, execs)
    cleanup(temp)
