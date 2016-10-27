/*****************************************************************************
 * 
 * Copyright (C) 1997-2016 Karol Gajdos <klement2@azet.sk>
 *
 * This file is part of the FXScene, a software for 3D modeling and
 * raytracing.     
 *
 * FXScene is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FXScene is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FXScene.  If not, see <http://www.gnu.org/licenses/>.
 *   
 *****************************************************************************/
 
#ifndef _BASE_H_
#define _BASE_H_

#define INFOSTR printf

#define ADPI(t,o) ((t*)(o.getItem()))
#define ADPD(t,o) ((t*)(o->getData()))

enum {
    X=0,
    Y,
    Z
};

#define MATRIX_ROWS 4
#define MATRIX_COLUMNS 3
#define VECTOR_SPACE 3

typedef double REAL;
typedef signed char CHAR;
typedef unsigned char UCHAR;
typedef signed int INT;
typedef signed long LONG;
typedef unsigned int UINT;
typedef unsigned long ULONG;

typedef ULONG TFlags;

enum TBoolOper {
    O_PLUS=0,
    O_LOCALPLUS, // MINUS and INTERSECTION after this object is local to this object
    O_MINUS,
    O_INTERSECTION,
    O_LAST
};

typedef INT TColor;

typedef struct POINT {
    INT x;
    INT y;
}
POINT;

typedef struct RECT {
    INT left;
    INT top;
    INT right;
    INT bottom;
}
RECT;

#define shrinkRectBy1(r) do { r.top++; r.left++; r.bottom--; r.right--; } while(0)

#ifndef NULL
#define NULL 0
#endif

//enum bool {false, true}

// this const is related to (bit) precision of GDI
// seems that 32-bit windows has 16-bit precision
#define REAL_MAX_INT4 32767.0

//#define REAL_MAX_INT4 2147483648.0
//#define REAL_MAX_INT8 18446744073709551616.0

#define MAXINT 32767
#define REND_MAX 1.7e38

// used only in mtxt.cpp
#define RMIN 0.000000001
#define RMAX 10000000.0

#define M_PI_   3.1415926535897932384626433832795
#define M_PI_2_ 1.5707963267948966192313216916398
#define M_PI180 0.01745329251994
#define M_PI90  0.03490658503989

#endif // _BASE_H_
