#ifndef MY_LIB_h
#define MY_LIB_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef bool
#define bool char
#define true 1
#define false 0
#endif

#ifndef CAST
#define CAST(type, value) *((type*)value)
#endif

#include "dlinked_list.h"
#include "dynamic_array.h"
#include "stack.h"
#include "mystring.h"
#include "set.h"

#define TYPE_String 1
#define TYPE_Int 2
#define TYPE_Char 3
#define TYPE_Float 4
#define TYPE_Struct 5
#endif