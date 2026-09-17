# AurOS

AurOS is a small operating system I'm building from scratch as a way to learn more about how operating systems actually work.

The goal isn't to create a replacement for Linux or Windows. Instead, I want to understand what happens underneath the software I normally write: how a machine boots, how the CPU is managed, how memory is allocated, how processes are created and scheduled, how hardware is accessed, and eventually how things like filesystems and networking are implemented.

The project will start very small and grow over time.

## What I want to learn

Some of the main areas I want to explore are:

* x86-64 architecture
* Boot and system initialization
* C at a low level
* x86-64 assembly
* Interrupts and exceptions
* Memory management and paging
* Processes and scheduling
* System calls
* Filesystems
* Hardware drivers
* Networking
* User/kernel separation
* Operating system security

I'm deliberately building these components myself rather than relying on an existing operating system kernel.

## Current approach

AurOS currently targets **x86-64** and will initially run inside **QEMU**.

The kernel is being written primarily in **C**, with x86-64 assembly used where direct interaction with the CPU is required.

The development environment runs through **WSL2/Ubuntu**, while the project itself can be edited normally from Windows using VS Code.

## Project status

This project is still at the beginning.

The first milestone is to get a minimal AurOS kernel booting and printing output. From there, the system will gradually gain more functionality.

Planned areas include:

1. Booting and kernel initialization
2. CPU setup and interrupts
3. Memory management
4. Processes and scheduling
5. System calls
6. Basic userland and shell
7. Filesystem
8. Networking
9. Security features

The roadmap will probably change as I learn more and discover interesting things to experiment with.

## Why I'm building this

I've worked on projects involving C, Linux and lower-level system information before, but most of the operating system itself is normally hidden behind APIs and libraries.

I want to go one level deeper and understand what those abstractions are actually doing.

This is primarily a learning project, so some parts will probably be inefficient, incomplete or completely broken along the way. That's part of the point.

## Development

The project is developed on an x86-64 PC using:

* C
* x86-64 Assembly
* GCC
* GNU Binutils
* Make
* GDB
* QEMU
* Git
* WSL2 / Ubuntu

No external operating-system framework is being used for the kernel itself.

## License

License to be decided.
