# Ashe Operating System Codes
Ashe Operating System Codes are one byte code values representing known Operating Systems out there in this world.
It is designed to implement my softwares on multiple platforms.
The codes are usually used in macro functions but there's no limitations on testing the codes in runtime.
Compilers could optimise them out.

`OSCode.h` is a header file dedicated to contain the codes in macro definitions and related macro functions.
This file can appear from any project of mine.
Each header will check whether if itself has been included from somewhere else and disables itself if it is.
Also, it will print error if the headers are of different revisions.
Yes, there could be revisions later on.

## The Codes
### 0x01 ~: Unix-like
Implementations with this range of codes will make use of POSIX functionalities to achieve cross-platform portability.
Code values higher than 0x01 can be used for OS-specific components and optimisation.
Using only POSIX APIs can always hinder the flexibilty.
Especially when it comes to network programming.
It is safe to say using the code 0x01 ensures that the code can run on any POSIX based OS.
However, this options is not viable if the output has to be reliable and robust.

- 0x01: Implemented strictly with POSIX only.
- 0x02: Linux
- 0x03: BSD
- 0x04: OSX 

### 0x10: Microsoft Windows
- 0x10: XP
- 0x11: 7
- 0x12: 10

The list consists of the Windows builds that have considerable changes in its API.
WinAPI hasn't been changing in a noticable extent since the release of XP.

Using the code 0x10 guarantees the output to be able to run on any version of Windows above XP.
