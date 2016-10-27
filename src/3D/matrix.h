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
 
#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "base.h"
#include "vector.h"

typedef REAL MATRIX [MATRIX_ROWS][MATRIX_COLUMNS];
typedef struct {
    MATRIX M,IM;
}
TRANSF;

#define MNoErr 0
#define MInvErr 1
extern int MError;
extern MATRIX M1110;

/*
    Warning ! v must be different from result
*/
#define VMultM(result,v,m) \
 { (result)[X]=(v)[X]*(m)[0][0]+(v)[Y]*(m)[1][0]+(v)[Z]*(m)[2][0]; \
   (result)[Y]=(v)[X]*(m)[0][1]+(v)[Y]*(m)[1][1]+(v)[Z]*(m)[2][1]; \
   (result)[Z]=(v)[X]*(m)[0][2]+(v)[Y]*(m)[1][2]+(v)[Z]*(m)[2][2]; }

/*
    Warning ! v must be different from result
*/
#define PMultM(result,v,m) \
 { (result)[X]=(v)[X]*(m)[0][0]+(v)[Y]*(m)[1][0]+(v)[Z]*(m)[2][0]+(m)[3][0]; \
   (result)[Y]=(v)[X]*(m)[0][1]+(v)[Y]*(m)[1][1]+(v)[Z]*(m)[2][1]+(m)[3][1]; \
   (result)[Z]=(v)[X]*(m)[0][2]+(v)[Y]*(m)[1][2]+(v)[Z]*(m)[2][2]+(m)[3][2]; }

#define MTAddV(m,v) { (m)[3][0]+=(v)[X];(m)[3][1]+=(v)[Y];(m)[3][2]+=(v)[Z]; }
#define MTSubV(m,v) { (m)[3][0]-=(v)[X];(m)[3][1]-=(v)[Y];(m)[3][2]-=(v)[Z]; }
#define MTtoV(v,m)  { (v)[X]=(m)[3][0]; (v)[Y]=(m)[3][1]; (v)[Z]=(m)[3][2]; }

void M0(MATRIX m);
void M1(MATRIX m);
void MDiag(MATRIX m, REAL m11, REAL m22, REAL m33);
void MDiagT(MATRIX m, REAL m11, REAL m22, REAL m33,
            REAL m41, REAL m42, REAL m43);
void MT(MATRIX m,REAL m41, REAL m42, REAL m43);
void MMultiply(MATRIX result, MATRIX a, MATRIX b);
bool MInverse(MATRIX result, MATRIX a);
void MRotate(MATRIX m, INT axis, REAL angle);

#endif // _MATRIX_H_

