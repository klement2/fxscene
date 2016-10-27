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

#include <string.h>
#include <math.h>
#include "base.h"
#include "vector.h"
#include "matrix.h"
#include "camera.h"

MATRIX M1110 = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 0}};

void TCamera::CameraReset()
{
    // OBS - the observer - this is 3D point, not vector
    Vector(observer, 0, 0, 0)
    // POI - this is vector, orientation of the observer in the space,
    // actual position of POI=(observer+poi)
    Vector(poi, -1, 0, 0)
    distance = 1;
    Vector(right, 0, 1, 0)
    Vector(up, 0, 0, 1)
    M1(tmpM);
    //tmpM=NULL;
    M1(t.M);
    M1(t.IM);
}

void TCamera::StoreSight(MATRIX tmpM)
{
    memcpy(this->tmpM, t.M, sizeof(t.M));
    memcpy(tmpM, t.M, sizeof(t.M));
}

void TCamera::RestoreSight(MATRIX tmpM)
{
    memcpy(this->tmpM, tmpM, sizeof(t.M));
    memcpy(t.M, tmpM, sizeof(t.M));
}

void TCamera::GetTmpSight(MATRIX tmpM)
{
    memcpy(tmpM, this->tmpM, sizeof(t.M));
}

void TCamera::StoreTmpSight(MATRIX tmpM)
{
    memcpy(this->tmpM, tmpM, sizeof(t.M));
}

void TCamera::RestoreTmpSight()
{
    memcpy(t.M, tmpM, sizeof(t.M));
}

void TCamera::StoreTmpSight()
{
    memcpy(this->tmpM, t.M, sizeof(t.M));
}

void TCamera::NewSight(MATRIX m)
{
    MMultiply(t.M, m, tmpM);
}

// Move observer so that the distance from poi is distance + distance * c_distance
void TCamera::ZoomCamera(REAL c_distance)
{
    VECTOR v, poi1;
    VAssign(poi1, poi);
    VNormalize(poi1);
    c_distance = distance + distance * c_distance;
    VSMultV(poi1, c_distance);
    VAssign(v, poi);
    VAssign(poi, poi1);
    VSub(poi1, poi1, v);
    VReverse(poi1);
    distance = c_distance;
    MoveCamera(poi1);
}

void TCamera::MoveCamera(VECTOR v)
{
    VAdd(observer, v)
    MTAddV(t.IM, v)
    MInverse(t.M, t.IM);
}

void TCamera::RotateCamera(int axis, REAL angle, INT rotate_camera)
{
    REAL cosa = cos(angle), sina = sin(angle);
    MATRIX r;
    VECTOR v, vz;

    memcpy(r, M1110, sizeof(r));
    switch (axis) {
    case X :
        r[1][1] = cosa;
        r[1][2] = sina;
        r[2][1] = -sina;
        r[2][2] = cosa;
        break;
    case Y :
        r[0][0] = cosa;
        r[2][0] = sina;
        r[0][2] = -sina;
        r[2][2] = cosa;
        break;
    case Z :
        r[0][0] = cosa;
        r[0][1] = sina;
        r[1][0] = -sina;
        r[1][1] = cosa;
    }
    MTSubV(t.IM, observer);
    MMultiply(t.M, t.IM, r);
    MTAddV(t.M, observer);
    memcpy(t.IM, t.M, sizeof(t.IM));
    MInverse(t.M, t.IM);
    VAssign(vz, poi);
    VMultM(poi, vz, r);
    VAssign(v, right);
    VMultM(right, v, r)
    VAssign(v, up);
    VMultM(up, v, r)
    if (rotate_camera) {
        VSub(v, vz, poi);
        MoveCamera(v);
    }
}

INT TCamera::SetCamera(VECTOR c_obs, VECTOR c_poi)
{
    MATRIX m, n, tmpM;
    VECTOR tmp;
    REAL a, b, c, v;

    if(c_obs[X] == c_poi[X] && c_obs[Y] == c_poi[Y] && c_obs[Z] == c_poi[Z])
        return 0;
    VAssign(observer, c_obs);
    VSub(poi, c_poi, c_obs);
    VMultM(c_poi, poi, t.M);
    VDistance(distance, c_poi);
    VNormalize(c_poi);
    VAssign(tmp, c_obs);
    PMultM(c_obs, tmp, t.M);
    a = -c_poi[X];
    b = c_poi[Y];
    c = c_poi[Z];
    v = sqrt(a * a + b * b);
    if (v != 0) {    /* Line of the sight is not parallel with Z axis */
        a = a / v;
        b = b / v;
        m[0][0] = a;
        m[0][1] = b;
        m[0][2] = 0; /* axis Z */
        m[1][0] = -b;
        m[1][1] = a;
        m[1][2] = 0;
        m[2][0] = 0;
        m[2][1] = 0;
        m[2][2] = 1;
        m[3][0] = -c_obs[X] * a + c_obs[Y] * b;
        m[3][1] = -c_obs[X] * b - c_obs[Y] * a;
        m[3][2] = -c_obs[Z];
    } else {
        m[0][0] = 0;
        m[0][1] = -c;
        m[0][2] = 0; /* Swap X and Y */
        m[1][0] = -c;
        m[1][1] = 0;
        m[1][2] = 0;
        m[2][0] = 0;
        m[2][1] = 0;
        m[2][2] = 1;
        m[3][0] = c_obs[Y] * c;
        m[3][1] = c_obs[X] * c;
        m[3][2] = -c_obs[Z];
    };
    MMultiply(tmpM, t.M, m);
    n[0][0] = v;
    n[0][1] = 0;
    n[0][2] = c;   /* axis Y */
    n[1][0] = 0;
    n[1][1] = 1;
    n[1][2] = 0;
    n[2][0] = -c;
    n[2][1] = 0;
    n[2][2] = v;
    n[3][0] = 0;
    n[3][1] = 0;
    n[3][2] = 0;
    MMultiply(t.M, tmpM, n);
    MInverse(t.IM, t.M);
    Vector(tmp, 0, 1, 0);
    VMultM(right, tmp, t.IM);
    Vector(tmp, 0, 0, 1);
    VMultM(up, tmp, t.IM);
    return 1;
}
