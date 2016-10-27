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

#include "surf3D.h"

void TDSurf::cleanUp()
{
    TDBase::cleanUp();
    otype = TDSURF;
    surface = this;
    s = defSurface.s;
}

TDSurf::TDSurf()
{
    cleanUp();
}

TDSurf::TDSurf(TDSurf* s)
{
    *this = *s;
}

TDSurf::TDSurf(const TDSurf& s)
{
    *this = s;
}

TDSurf::TDSurf(VECTOR Ia, VECTOR kd, VECTOR ks, REAL n, TDObject *oname)
{
    cleanUp();
    name = oname;
    VAssign(this->s.Ia, Ia);
    VAssign(this->s.kd, kd);
    VAssign(this->s.ks, ks);
    this->s.n = n;
}

TDSurf::TDSurf(TSurf s, TDObject *oname)
{
    cleanUp();
    name = oname;
    this->s = s;
}

TDObject* TDSurf::newCopy()
{
    TDObject *o = new TDSurf(this);
    exportDynamicData((TDGroup*)(o));
    return o;
}

const TDSurf defSurface(SurfConst[0]);

const TSurf SurfConst[5] = {
    { Ia: { 0.6, 0.6, 0.6} , kd: {0.4, 0.4, 0.4}, {0, 0, 0}, n: 1, 0 }, // white
    {{0.2, 0.6, 0.2}, {0.2, 0.3, 0.2}, {0.6, 0.6, 0.6}, n: 0.05, 0}, // green
    {{0.7, 0.2, 0.25}, {0.6, 0.2, 0.25}, {0, 0, 0}, n: 1, 0}, // red
    {{0.66, 0.64, 0.35}, {0.4, 0.4, 0.2}, {0.2, 0.2, 0.1}, n: 0.08, 1}, // yellow
    {{0.66, 0.5, 0.65}, {0.4, 0.4, 0.6}, {0, 0, 0}, n: 1, 0} // pink
};

