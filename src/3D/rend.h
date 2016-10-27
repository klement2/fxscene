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

#ifndef _REND_H_
#define _REND_H_

#include "surf3D.h"
#include "light3D.h"
#include "interval.h"

extern bool ray_debug;

typedef struct {
    VECTOR *P, *V;
} THalfLine;

typedef struct {
    RECT r;
    RECT pbR;
    INT pbHeight;
    INT pbWidth;
    INT pbLength;
    void *pPtr;
} TRendProgress;

typedef bool(*RendCheckStopFct)(void *pP, long myRenderId);
typedef void(*RendPbFct)(TRendProgress *rp);

typedef struct _RendOptions {
    bool shades;
    bool pBar;
    bool drawEveryPixel;
    INT stepx;
    INT stepy;
    INT rmask;
    TColor bgColor;
    INT rThreads;
    RendCheckStopFct stopFct;
    RendPbFct pBarFct;
    long renderId;
} RendOptions;

const RendOptions RendDefaults = {
    true, true, true, 1, 1, 0, 0, 1, NULL, NULL, 0
};

class Rend
{
public:
    Rend()
    {
    }
    void SomePrecalculations(TG3D* g, TDTree *o, PLList *l);
    int Render(void *pPtr, TG3D* g, TDTree &o, PLList &l, TColor *bmp, RECT r, RendOptions opts = RendDefaults);
    void SetCheckStopFunction(RendCheckStopFct fc);

    TDR* Intersection(THalfLine Pol, IvtArr *ai, POINT ri, /*TDLigthList &l,*/ TDRList &o, bool isV, bool isLightRay);
    TColor Light(TDR *RObj, IvtArr *ai, POINT ri, TG3D *g, VECTOR v, TDLigthList &l, TDRList &o);
protected:
private:
    RendOptions options;
};

#endif // _REND_H_
