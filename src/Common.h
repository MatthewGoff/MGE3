#ifndef COMMON_H
#define COMMON_H

/*
 * Primative data type convension in this project
 *
 * Size (bytes) | Signed | Unsigned
 *            1 | char   | uchar
 *            2 | short  | ushort
 *            4 | int    | uint
 *            8 | int64  | uint64
 * 
 * Other types
 * bool (1 byte)
 * float (4 bytes)
 * double (8 bytes)
 * 
 * Aliases
 * int32 -> int
 * byte -> uchar
*/

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long uint64;
typedef long long int64;
typedef int int32;
typedef unsigned char byte;

#include <math.h>
#include <stdio.h>

#include "Print.h"

#endif