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

#ifndef _LIGHT3D_H_ 
#define _LIGHT3D_H_

#include "obj3D.h"


struct TPlight {
    VECTOR pl;
    VECTOR i;
    REAL s;
};

extern const TPlight defPlight;

class TDLight : public TDBase
{
protected:
    void cleanUp()
    {
        TDBase::cleanUp();
        otype = TDLIGHT;
        VAssign(pl, defPlight.pl);
        VAssign(i, defPlight.i);
        s = defPlight.s;
    }
public:
    VECTOR pl, i;
    REAL s;
    POINT plo;

    TDLight();
    TDLight(TDLight* o)
    {
        *this = *o;
    }
    virtual ~TDLight();
    TDLight(VECTOR c_pl, VECTOR c_i, REAL c_s)
    {
        cleanUp();
        VAssign(pl, c_pl)
        VAssign(i, c_i)
        s = c_s;
    }
    virtual TDObject* newCopy()
    {
        TDLight *o = new TDLight(this);

        exportDynamicData((TDGroup*)(o));
        return (TDObject*)o;
    }
    virtual const char* getOName()
    {
        return "Point light";
    }
    virtual void Wf(TG3D* g)
    {
        //((Sphere*)this)->Wf(g);
    }
    virtual void WfAcc(TG3D* g)
    {
        //((Sphere*)this)->Wf(g);
    }
protected:
private:
};



#endif // _LIGHT3D_H_
