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
 
#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <math.h>
#include "base.h"

typedef REAL VECTOR [VECTOR_SPACE];
typedef struct Vector2 {
    REAL x;
    REAL y;
} Vector2;

#define VZero(result) \
 {(result)[X]=0; (result)[Y]=0; (result)[Z]=0;}

#define VOne(result) \
 {(result)[X]=1; (result)[Y]=1; (result)[Z]=1;}

#define Vector(result, x, y, z) \
 {(result)[X]=x; (result)[Y]=y; (result)[Z]=z;}

/* result=a */
#define VAssign(result, a) \
 {(result)[X]=(a)[X]; (result)[Y]=(a)[Y]; (result)[Z]=(a)[Z];}

/* result=-result */
#define VReverse(result) \
 {(result)[X]=-(result)[X]; (result)[Y]=-(result)[Y]; (result)[Z]=-(result)[Z];}

/* result=-a */
#define VAssignReversed(result, a) \
 {(result)[X]=-(a)[X]; (result)[Y]=-(a)[Y]; (result)[Z]=-(a)[Z];}

/* a=b+c */
#define VSum(a, b, c) \
 {(a)[X]=(b)[X]+(c)[X];(a)[Y]=(b)[Y]+(c)[Y];(a)[Z]=(b)[Z]+(c)[Z];}

/* a+=b */
#define VAdd(a, b) \
 {(a)[X]+=(b)[X];(a)[Y]+=(b)[Y];(a)[Z]+=(b)[Z];}

/* a=b-c */
#define VSub(a, b, c) \
 {(a)[X]=(b)[X]-(c)[X];(a)[Y]=(b)[Y]-(c)[Y];(a)[Z]=(b)[Z]-(c)[Z];}

/* a-=b */
#define VSubV(a, b) \
 {(a)[X]-=(b)[X];(a)[Y]-=(b)[Y];(a)[Z]-=(b)[Z];}

/* a=b*c */
#define VMult(a, b, c) \
 {(a)[X]=(b)[X]*(c)[X];(a)[Y]=(b)[Y]*(c)[Y];(a)[Z]=(b)[Z]*(c)[Z];}

/* a*=b */
#define VMultV(a, b) \
 {(a)[X]*=(b)[X];(a)[Y]*=(b)[Y];(a)[Z]*=(b)[Z];}

/* a=b/c */
#define VDivide(a, b, c) \
 {(a)[X]=(b)[X]/(c)[X];(a)[Y]=(b)[Y]/(c)[Y];(a)[Z]=(b)[Z]/(c)[Z];}

#define VDivideV(a, b) \
 {(a)[X]/=(b)[X];(a)[Y]/=(b)[Y];(a)[Z]/=(b)[Z];}

/* a=b*k; a,b=vektory, k=skalar */
#define VSMult(a, b, k) \
 {(a)[X]=(b)[X]*(k);(a)[Y]=(b)[Y]*(k);(a)[Z]=(b)[Z]*(k);}

/* a*=k; a=vektor, k=skalar */
#define VSMultV(a, k) \
 {(a)[X]*=(k);(a)[Y]*=(k);(a)[Z]*=(k);}

/* a=b/k; a,b=vektory, k=skalar */
#define VSDivide(a, b, k) \
 {(a)[X]=(b)[X]/(k);(a)[Y]=(b)[Y]/(k);(a)[Z]=(b)[Z]/(k);}

/* a/=k; a=vektor, k=skalar */
#define VSDivideV(a, k) \
 {(a)[X]/=(k);(a)[Y]/=(k);(a)[Z]/=(k);}

/* a=b*c; a=skalar, b,c=vektory, Skalarny sucin (Dot Product) */
#define VSDotP(a, b, c) \
 {a=(b)[X]*(c)[X]+(b)[Y]*(c)[Y]+(b)[Z]*(c)[Z];}

/* a=b*c; a,b,c=vektory, Vektorovy sucin (Cross Product)
   Pozor:  a musi byt rozne ako b a c */
#define VCrossP(a, b, c) \
 {(a)[X]=(b)[Y]*(c)[Z]-(b)[Z]*(c)[Y]; \
  (a)[Y]=(b)[Z]*(c)[X]-(b)[X]*(c)[Z]; \
  (a)[Z]=(b)[X]*(c)[Y]-(b)[Y]*(c)[X];}

/* vektor a = vektor b*b */
#define VSquare(a, b) \
 {(a)[X]=(b)[X]*(b)[X];(a)[Y]=(b)[Y]*(b)[Y];(a)[Z]=(b)[Z]*(b)[Z];}

/* a=dlzka vektora b */
#define VLength(a, b) \
 {a=sqrt((b)[X]*(b)[X]+(b)[Y]*(b)[Y]+(b)[Z]*(b)[Z]);}

/* a=dlzka vektora b na druhu */
#define VLengthSquared(a, b) \
 {a=(b)[X]*(b)[X]+(b)[Y]*(b)[Y]+(b)[Z]*(b)[Z];}

/* a=vzdialenost medzi dvoma vektormi,bodmi */
#define VVDistance(a, b, c) \
 {VECTOR tmp; VSub(tmp, b, c); VLength(a, tmp);}

/* Normalizuje vektor a - vrati vektor b s dlzkou(velkostou) 1 */
#define VVNormalize(a,b) \
 {REAL VTmp=1./sqrt((b)[X]*(b)[X]+(b)[Y]*(b)[Y]+(b)[Z]*(b)[Z]); \
  (a)[X]=(b)[X]*VTmp;(a)[Y]=(b)[Y]*VTmp;(a)[Z]=(b)[Z]*VTmp;}

#define VDistance(a,b) \
 { a = sqrt(b[X]*b[X]+b[Y]*b[Y]+b[Z]*b[Z]); }

/* Normalizuje vektor a */
#define VNormalize(a) \
 {REAL VTmp=sqrt((a)[X]*(a)[X]+(a)[Y]*(a)[Y]+(a)[Z]*(a)[Z]); \
  (a)[X]/=VTmp;(a)[Y]/=VTmp;(a)[Z]/=VTmp;}

/* V  = V1 + k * V2, k=skalar */
#define VAddVMult(V, V1, k, V2) \
  {(V)[X] = (V1)[X] + (k) * (V2)[X]; \
   (V)[Y] = (V1)[Y] + (k) * (V2)[Y]; \
   (V)[Z] = (V1)[Z] + (k) * (V2)[Z]; }

/* V += k * V2, k=skalar  */
#define VAddVSMult(V, k, V2) \
  {(V)[X] += (k) * (V2)[X]; \
   (V)[Y] += (k) * (V2)[Y]; \
   (V)[Z] += (k) * (V2)[Z]; }

/* V  = V1 - k * V2, k=skalar */
#define VSubVMult(V, V1, k, V2) \
  {(V)[X] = (V1)[X] - (k) * (V2)[X]; \
   (V)[Y] = (V1)[Y] - (k) * (V2)[Y]; \
   (V)[Z] = (V1)[Z] - (k) * (V2)[Z]; }

/* V -= k * V2, k=skalar */
#define VSubVSMult(V, k, V2) \
  {(V)[X] -= (k) * (V2)[X]; \
   (V)[Y] -= (k) * (V2)[Y]; \
   (V)[Z] -= (k) * (V2)[Z]; }

#endif // _VECTOR_H_
