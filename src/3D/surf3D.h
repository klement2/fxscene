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

#ifndef _SURF3D_H_ 
#define _SURF3D_H_

#include "obj3D.h"

struct TSurf {
    VECTOR Ia, kd, ks;
    REAL n;
    REAL metalic;
};

class TDSurf : public TDBase
{
protected:
    void cleanUp();
public:
    TSurf s;

    TDSurf();
    TDSurf(TDSurf* s);
    TDSurf(const TDSurf& s);
    TDSurf(VECTOR Ia, VECTOR kd, VECTOR ks, REAL n, TDObject *oname = NULL);
    TDSurf(TSurf s, TDObject *oname = NULL);
    virtual ~TDSurf()
    { }
    virtual TDObject* newCopy();

    const char* getOName()
    {
        return "Surface";
    }
};

extern const TDSurf defSurface;
extern const TSurf SurfConst[5];

#endif // _SURF3D_H_
