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

#ifndef _MTXT_H_ 
#define _MTXT_H_

#include "base.h"
#include "vector.h"
#include "matrix.h"

#define REAL_IN_TEXT 15
typedef char MTxt[MATRIX_ROWS][MATRIX_COLUMNS][REAL_IN_TEXT];
typedef char VTxt[VECTOR_SPACE][REAL_IN_TEXT];

INT FloatCheck(REAL *vysl, char *s, REAL Min, REAL Max);
void Vector2Txt(VECTOR v, VTxt vt);
void Matrix2Txt(MATRIX m, MTxt mt);
int VectorTxt2Num(VTxt vt, VECTOR v);
int MatrixTxt2Num(MTxt mt, MATRIX m);

#endif // _MTXT_H_
