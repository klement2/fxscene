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

#ifndef _GR3D_H_ 
#define _GR3D_H_

#include "camera.h"

TColor VToC(VECTOR v); // return RGB(255*v[X],255*v[Y],255*v[Z]);

#define CRIT_DISTANCE 0.00000002

class TG3D : public TCamera
{
    friend class Rend;
private:
    REAL tMinimum, tMaximum;
    REAL cone_height, xyasp, yxasp, yxasp_cone_height;
    RECT viewRect;
    REAL widthr, heightr;
    int width, height;
    bool pqClip(REAL directedProjection, REAL directedDistance);
    bool ClipLine(VECTOR pa, VECTOR pb, POINT *p2a, POINT *p2b);
protected:
    POINT mp;
public:
    virtual ~TG3D()
    {
    }

    TG3D()
    {
        mp.x=0;
        mp.y=0;
    }

    void Set2DView(RECT r);

    RECT GetViewRect()
    {
        return viewRect;
    }

    virtual void ProjPoint(VECTOR *p3, POINT *p2);
    void JustifyProjection(VECTOR p3, POINT *p2);
    virtual void ProjLine(VECTOR *p3a, VECTOR *p3b, POINT *p2a, POINT *p2b);

    virtual void MSetColor(TColor color) { }
    virtual void MPixel(POINT p, TColor color) { }
    virtual void MLineTo(POINT p) { }
    virtual void MMoveTo(POINT p) { }
    virtual void MLine(POINT p1, POINT p2) { }
    virtual POINT MGetPosition()
    {
        return mp;
    }
    virtual void MRect(RECT r) { }

    virtual void DSetColor(TColor color) { }
    virtual void DPixel(POINT p, TColor color) { }
    virtual void DLineTo(POINT p) { }
    virtual void DMoveTo(POINT p) { }
    virtual void DLine(POINT p1, POINT p2) { }
    virtual void DRect(RECT r) { }

    void MPixel3D(VECTOR p, TColor color);
    void MLine3D(VECTOR p1, VECTOR p2);
    void MLineTo3D(VECTOR p);
    void MMoveTo3D(VECTOR p);

};

class TG3DPrl : public TG3D
{
public:
    virtual void ProjPoint(VECTOR *p3, POINT *p2);
};

#endif // _GR3D_H_
