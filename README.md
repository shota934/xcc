

The xcc C compiler
=====================

Overview
------------
The xcc is a small C compiler. it compiles from a source code written in C
into x64 assembler. the code compiled run on Linux on a x64 architecture.
it is written by C, the goal of the project is to be self-hosting and
interoperability with compiled code by another compiler like clang and gcc. 
The xcc is all hand-written, Any black box does not have such as yacc, lex and LLVM.

Build
------------

  To build this compiler, simply type and run the following command, as usual. 
  Build this compiler with command as follows.

   make

  To run test, run following command, as usual.

   make test


Author
------

  Shota Hisai

License
-------
	
  This software is released under the MIT License.  
  See LICENSE.

Reference
---------
  For the xcc development, I have consulted the following article and implementation for C compiler.
      
  - 8cc C compiler

    https://github.com/rui314/8cc.

  - 低レイヤを知りたい人のためのCコンパイラ作成入門

    https://www.sigbus.info/compilerbook
    
  - C11 standard final draft

    http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf 

  - Intel® 64 and IA-32 Architectures Software Developer Manuals

    https://software.intel.com/sites/default/files/managed/39/c5/325462-sdm-vol-1-2abcd-3abcd.pdf

