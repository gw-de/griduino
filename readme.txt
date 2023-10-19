SGO (SmartGridObserver) 1st semester project
(C) Siemens Technik Akademie Berlin

Intro:
Compiling done via the make command (Makefile driven)


Needed Debian/Ubuntu packages:
avr-libc
gcc-avr
avrdude
gawk


First steps (Linux):
To be able to develop with the equipped board one additional initialization
step is needed. A bootloader needs to be stored as initial program into the
ATTiny84. The bootloader allows one to upload new self programmed code using
the serial port and cheap serial cables, instead of using the more expensive
AVR ISP MKII adapter.
Initializing: Downloading a bootloader to the ATTiny84 microcontroller

    Connect the AVR ISP MKII adapter to the board and your computer.

    Compile the bootloader by using the command with parameter:
	make bootloader

    Store the bootloader in the ATTiny84 by using the command with parameter:
	make program_bootloader

    Already compile the program which makes it possible to download your self
    written programs later by following the following steps:
    Switch to the directory lboot
	cd lboot
    Use the command:
	make

    This first initialization process needs to be done only once or every time
    one exchanges the ATTiny84 with a fresh one. The AVR ISP MKII is not needed
    anymore.


Development process:
    Edit the main.c as main file for your c program and write your code.

    Compile the program using the command:
	make

    If no errors are display the program can be transferred to the ATTiny84 by
    using the command with parameter:
	make program

    One needs to wait until a rotating bar is to see and then shortly press and
    release the reset button on the board. The progress of the download will be
    shown and the new code will be started when the transfer finished.



Description of directories and files (Linux):

bootloader:
    This directory conatins the source code (and its compiled form) of the
    bootloader which is stored in the ATTiny84 as part of the initialization
    process.
    Usually does not need to be touched.

lboot:
    This directory contains the source code (and its compiled form) of the
    program used to download your self written programs using a serial cable.
    Usually does not need to be touched.

main.c:
    This is the main program file of your self written program. It already
    contains by default a frame so that it can be compiled and uploaded
    but it does not do anything usefull.
    This is your main working file.

Makefile:
    This file contains the setup for compiling and downloading so that the
    make command knows which compiler to start for which microcontroller.
    Usually it does not need to be edited.
