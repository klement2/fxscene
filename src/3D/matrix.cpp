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

#include <math.h>
#include <memory.h>
#include "base.h"
#include "matrix.h"

int MError = MNoErr;

void M0(MATRIX m)
{
    register int i, j;
    for(i = 0; i < MATRIX_COLUMNS; i++)
        for(j = 0; j < MATRIX_ROWS; j++)
            m[j][i] = 0.;
}

void M1(MATRIX m)
{
    register int i, j;
    for(i = 0; i < MATRIX_COLUMNS; i++)
        for(j = 0; j < MATRIX_ROWS; j++)
            if(i == j)
                m[j][i] = 1.;
            else
                m[j][i] = 0.;
}

void MDiag(MATRIX m, REAL m11, REAL m22, REAL m33)
{
    M0(m);
    m[0][0] = m11;
    m[1][1] = m22;
    m[2][2] = m33;
}

void MDiagT(MATRIX m, REAL m11, REAL m22, REAL m33,
            REAL m41, REAL m42, REAL m43)
{
    M0(m);
    m[0][0] = m11;
    m[1][1] = m22;
    m[2][2] = m33;
    m[3][0] = m41;
    m[3][1] = m42;
    m[3][2] = m43;
}

void MT(MATRIX m, REAL m41, REAL m42, REAL m43)
{
    m[3][0] = m41;
    m[3][1] = m42;
    m[3][2] = m43;
}

/*
    Vynasob matice: result=a*b;
   POZOR ! result musi byt rozny od a,b
*/
void MMultiply(MATRIX result, MATRIX a, MATRIX b)
{
    result[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0];
    result[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1];
    result[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2];
    result[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0];
    result[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1];
    result[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2];
    result[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0];
    result[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1];
    result[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2];
    result[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + b[3][0];
    result[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + b[3][1];
    result[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + b[3][2];
}

/*
  result = inverse matrix from a;
*/
bool MInverse(MATRIX result, MATRIX a)
{
    REAL det;
    MError = MNoErr;
    det = a[0][0] * (a[1][1] * a[2][2] - a[1][2] * a[2][1]) -
          a[0][1] * (a[1][0] * a[2][2] - a[1][2] * a[2][0]) +
          a[0][2] * (a[1][0] * a[2][1] - a[1][1] * a[2][0]);
    if(det == 0) {
        MError = MInvErr;
        return false;
    } else {
        det = 1 / det;
        result[0][0] = (a[1][1] * a[2][2] - a[1][2] * a[2][1]) * det;
        result[0][1] = -(a[0][1] * a[2][2] - a[0][2] * a[2][1]) * det;
        result[0][2] = (a[0][1] * a[1][2] - a[0][2] * a[1][1]) * det;
        result[1][0] = -(a[1][0] * a[2][2] - a[1][2] * a[2][0]) * det;
        result[1][1] = (a[0][0] * a[2][2] - a[0][2] * a[2][0]) * det;
        result[1][2] = -(a[0][0] * a[1][2] - a[0][2] * a[1][0]) * det;
        result[2][0] = (a[1][0] * a[2][1] - a[1][1] * a[2][0]) * det;
        result[2][1] = -(a[0][0] * a[2][1] - a[0][1] * a[2][0]) * det;
        result[2][2] = (a[0][0] * a[1][1] - a[0][1] * a[1][0]) * det;
        result[3][0] = -a[3][0] * result[0][0] - a[3][1] * result[1][0] - a[3][2] * result[2][0];
        result[3][1] = -a[3][0] * result[0][1] - a[3][1] * result[1][1] - a[3][2] * result[2][1];
        result[3][2] = -a[3][0] * result[0][2] - a[3][1] * result[1][2] - a[3][2] * result[2][2];
    }
    return true;
}

void MRotate(MATRIX m, INT axis, REAL angle)
{
    REAL cosa = cos(angle), sina = sin(angle);
    MATRIX r, tmp;
    memcpy(r, M1110, sizeof(r));
    memcpy(tmp, m, sizeof(tmp));
    switch (axis) {
    case X :
        r[1][1] = cosa;
        r[1][2] = -sina;
        r[2][1] = sina;
        r[2][2] = cosa;
        break;
    case Y :
        r[0][0] = cosa;
        r[2][0] = -sina;
        r[0][2] = sina;
        r[2][2] = cosa;
        break;
    case Z :
        r[0][0] = cosa;
        r[0][1] = -sina;
        r[1][0] = sina;
        r[1][1] = cosa;
    }
    MMultiply(m, tmp, r);
}
