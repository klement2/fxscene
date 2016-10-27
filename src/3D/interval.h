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
 
#ifndef _INTERVAL_H_
#define _INTERVAL_H_

#include "base.h"

/* using 'solid' approach - prefer older objects */
/* ************************************************

              dest            source
  1,    |---------------|  |++++++++++|

  2,    |--------|======|+++|

  3, |++|=======|-------|

  4,    |--|==========|-|

  5, |++|===============|++++++|   (source is wider than in 1,)

  6, |++++++++++|  |---------------|

  U-union
  S-subtraction
  I-intersection

**************************************************** */

typedef struct tmin_tmax {
    REAL tmin;
    REAL tmax;
    void* data_min;
    void* data_max;
} tmin_tmax;

typedef struct IvT {
    void clear()
    {
        t.tmin = REND_MAX;
        t.tmax = -REND_MAX;
        t.data_min = NULL;
        t.data_max = NULL;
    }
    tmin_tmax t;
    INT nextfree;
    INT next;
} IvT;

typedef struct IvtArr {
#define MAX_PARTS 10000
    IvT globIvArray[MAX_PARTS];
    INT firstFreeIv;
    INT maxglobIvArray; // just for statistics
    INT currglobIvArray; // just for statistics
    inline void clear(INT firstIv)
    {
        INT nxt;
        nxt = firstIv;
        while (nxt != 0 && globIvArray[nxt].nextfree == 0) {
            globIvArray[nxt].nextfree = firstFreeIv;
            firstFreeIv = nxt;
            nxt = globIvArray[firstFreeIv].next;
            globIvArray[firstFreeIv].next = 0;
            currglobIvArray--;
        }
    }
} IvtArr;

extern void ivArrayInit(IvtArr *ia);

typedef struct IvList {
    inline void clear()
    {
        ia->clear(firstIv);
        start(ia);
    }
    inline bool isLink()
    {
        return currIv != 0;
    }
    inline bool isFirst()
    {
        return firstIv != 0;
    }
    inline INT getFree()
    {
        return ia->firstFreeIv;
    }
    inline INT getNext()
    {
        return ia->globIvArray[currIv].next;
    }
    inline IvT* getCurr()
    {
        return &ia->globIvArray[currIv];
    }
    inline IvT* getCurrNext()
    {
        return &ia->globIvArray[ia->globIvArray[currIv].next];
    }
    inline IvT* getFirst()
    {
        return &ia->globIvArray[firstIv];
    }
    inline REAL get_tMin()
    {
        return ia->globIvArray[currIv].t.tmin;
    }
    inline REAL get_tMax()
    {
        return ia->globIvArray[currIv].t.tmax;
    }
    inline void goFirst()
    {
        currIv = firstIv;
    }
    inline void goNext()
    {
        if (currIv!=0) currIv = ia->globIvArray[currIv].next;
    }
    inline void start(IvtArr *p_ia)
    {
        ia = p_ia;
        firstIv = 0;
        currIv = 0;
    }
    inline IvList()
    {
        start(NULL);
    }
    IvtArr *ia;
    INT firstIv;
    INT currIv;
} IvList;

bool iv_append(IvList * l);
bool iv_remove(INT i, INT iprev);
void iv_union (IvList *pdest, IvList *psource);
void iv_subtraction (IvList *pdest, IvList *psource);
void iv_intersection (IvList *pdest, IvList *psource);

#endif // _INTERVAL_H_
