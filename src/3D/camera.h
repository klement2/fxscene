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
 
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "base.h"
#include "matrix.h"
#include "vector.h"

/*
    observer,target = 3D points
    right,up = vectors
*/

class TCamera
{
private:
public:
    MATRIX tmpM;
    TRANSF t;
    VECTOR observer,poi,right,up;
    REAL distance;

    TCamera()
    {
        CameraReset();
    }
    void CameraReset();
    void StoreSight(MATRIX tmpM);
    void RestoreSight(MATRIX tmpM);
    void GetTmpSight(MATRIX tmpM);
    void StoreTmpSight();
    void RestoreTmpSight();
    void StoreTmpSight(MATRIX tmpM);
    void NewSight(MATRIX m);
    void MoveCamera(VECTOR v);
    // around=X,Y,Z ;
    // rotate_camera: FALSE-rotate target | TRUE-rotate camera (around the target)
    void RotateCamera(int around, REAL angle, INT rotate_camera);
    void ZoomCamera(REAL c_distance);
    INT SetCamera(VECTOR c_observer, VECTOR c_poi);
};

#endif // _CAMERA_H_
