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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <memory.h>

#include "m2txt.h"

static MATRIX mtmp, mitmp;
static VECTOR vtmp;

INT FloatCheck(REAL *vysl, char *s, REAL Min, REAL Max)
{
    char* lastchar;
    char **endptr = &lastchar;
    REAL r;
    r = strtod(s, endptr);
    if(((r < Min) && (r > 0)) || (fabs(r) > Max)) {
        strcpy(s, "Illegal");
        return 0;
    }
    if(*endptr == s || errno == ERANGE) {
        strcpy(s, "Error");
        return 0;
    }
    *vysl = r;
    return 1;
}

void Vector2Txt(VECTOR v, VTxt vt)
{
    INT i;
    for (i = 0; i < VECTOR_SPACE; i++)
        sprintf(vt[i], "%.10f", v[i]);
}

void Matrix2Txt(MATRIX m, MTxt mt)
{
    INT i, j;
    for (i = 0; i < MATRIX_ROWS; i++)
        for (j = 0; j < MATRIX_COLUMNS; j++)
            sprintf(mt[i][j], "%.10f", m[i][j]);
}

INT VectorTxt2Num(VTxt vt, VECTOR v)
{
    INT i, FOk = 1;
    REAL r;

    for (i = 0; i < VECTOR_SPACE; i++) {
        FOk = FloatCheck(&r, vt[i], RMIN, RMAX);
        if (FOk == 0)
            break;
        vtmp[i] = r;
    }
    if (FOk) {
        VAssign(v, vtmp);
        return 1;
    } else
        return 0;
}

INT MatrixTxt2Num(MTxt mt, MATRIX m)
{
    INT FOk = 1;
    INT i, j;
    REAL r;

    for (i = 0; i < MATRIX_ROWS; i++)
        for (j = 0; j < MATRIX_COLUMNS; j++) {
            r = 0;
            FOk = FloatCheck(&r, mt[i][j], RMIN, RMAX);
            if (FOk == 0)
                break;
            mtmp[i][j] = r;
        }
    if (FOk) {
        MInverse(mitmp, mtmp);
        if (MError != MInvErr) {
            memcpy(m, mtmp, sizeof(MATRIX));
            return 1;
        }
    }
    return 0;
}

