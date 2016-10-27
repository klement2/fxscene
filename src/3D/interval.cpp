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
 
#include <memory>
#include <stdio.h>
#include "interval.h"

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

***************************************************** */

void ivArrayInit(IvtArr *ia)
{
    INT i;
    for (i = 0; i < MAX_PARTS; i++) {
        ia->globIvArray[i].clear();
        if (i + 1 < MAX_PARTS)
            ia->globIvArray[i].nextfree = i + 1;
        else
            ia->globIvArray[i].nextfree = 1;
        ia->globIvArray[i].next = 0;
    }
    ia->globIvArray[0].nextfree = -1;
    ia->firstFreeIv = 1;
    ia->maxglobIvArray = 0;
    ia->currglobIvArray = 0;
}

bool iv_append(IvList *l)
{
    INT tmp;
    if (l == NULL)
        return false;
    IvtArr *ia = l->ia;
    if  (ia->firstFreeIv != 0 && ia->globIvArray[ia->firstFreeIv].nextfree != 0) {
        tmp = ia->globIvArray[ia->firstFreeIv].nextfree;
        ia->globIvArray[ia->firstFreeIv].nextfree = 0;
        if (l->currIv != 0)
            ia->globIvArray[ia->firstFreeIv].next = ia->globIvArray[l->currIv].next;
        ia->globIvArray[l->currIv].next = ia->firstFreeIv;
        l->currIv = ia->firstFreeIv;
        if (l->firstIv == 0)
            l->firstIv = ia->firstFreeIv;
        ia->firstFreeIv = tmp;
        ia->currglobIvArray++;
        if (ia->currglobIvArray > ia->maxglobIvArray)
            ia->maxglobIvArray = ia->currglobIvArray;
    } else {
        //INFOSTR("Warning - no more free intervals, l=%p, firstFree=%d, nextfree=%d\n",
        //l, ia->firstFreeIv, ia->globIvArray[ia->firstFreeIv].nextfree);
        return false;
    }
    return true;
}

bool iv_remove(IvList *l, INT iprev)
{
    INT i;
    if (l == NULL || l->currIv == 0) {
        INFOSTR("iv_removeW: could not remove interval from list l=%p\n", l);
        return false;
    }
    IvtArr *ia = l->ia;
    i = l->currIv;
    if  (ia->globIvArray[i].nextfree == 0) {
        if (i == l->firstIv) {
            if (ia->globIvArray[i].next != 0) {
                l->firstIv = ia->globIvArray[i].next;
                l->currIv = l->firstIv;
            } else {
                l->currIv = 0;
                l->firstIv = 0;
            }
        } else {
            ia->globIvArray[iprev].next = ia->globIvArray[i].next;
            l->currIv = iprev;
        }
        ia->globIvArray[i].next = 0;
        ia->globIvArray[i].nextfree = ia->firstFreeIv;
        ia->firstFreeIv = i;
        ia->currglobIvArray--;
    } else {
        INFOSTR("iv_removeE: could not remove interval, i=%d, iprev=%d\n", i, iprev);
        return false;
    }
    return true;
}

void iv_union (IvList *pdest, IvList *psource)
{
    INT dest_prev;
    IvList dest = *pdest, source = *psource;
    bool append_copy;

    if (source.firstIv == 0 || dest.firstIv == source.firstIv)
        return;

    if (dest.firstIv == 0) {
        dest = source;
        *pdest = dest;
        psource->start(psource->ia);
        return;
    }

    source.currIv = source.firstIv;
    while (source.isLink()) {
        dest.currIv = dest.firstIv;
        dest_prev = dest.currIv;
        while (dest.isLink()) {
            if (dest.getCurr()->t.tmin < source.getCurr()->t.tmin) {
                if (dest.getCurr()->t.tmax > source.getCurr()->t.tmin) {
                    if (dest.getCurr()->t.tmax >= source.getCurr()->t.tmax) {
                        // U4
                        // this is 'solid approach' - prefer older object
                        break;
                    } else {
                        // U2;
                        if (dest.getNext() == 0 || dest.getCurr()->t.tmax < dest.getCurrNext()->t.tmin) {
                            dest_prev = dest.currIv;
                            if (iv_append(&dest)) {
                                dest.currIv = dest_prev;
                                // this is 'solid approach' - prefer older object
                                dest.getCurrNext()->t.tmax = source.getCurr()->t.tmax;
                                dest.getCurrNext()->t.data_max = source.getCurr()->t.data_max;

                                dest.getCurrNext()->t.tmin = dest.getCurr()->t.tmax;
                                dest.getCurrNext()->t.data_min = dest.getCurr()->t.data_max;
                                dest.goNext(); //i++;
                            } else {
                                //INFOSTR("Errr on U2\n");
                                dest.getCurr()->t.tmax = source.getCurr()->t.tmax;
                                dest.getCurr()->t.data_max = source.getCurr()->t.data_max;
                            }
                        }
                    }
                } else {
                    // U1
                    if (dest.getNext() == 0) {
                        if (iv_append(&dest)) {
                            dest.getCurr()->t = source.getCurr()->t;
                            //dest.goNext(); // i++; // means i should be dest.N-1 after this
                            break;
                        } else {
                            ;//INFOSTR("Errr on U1\n");
                        }
                    }
                }
            } else {

                if (dest.getCurr()->t.tmax <= source.getCurr()->t.tmax) {
                    // U5

                    // this is 'solid approach' - prefer older object
                    dest.getCurr()->t = source.getCurr()->t;

                    /*if (iv_append(dest,i+1))
                      {
                        // this is 'object approach' - prefer newer object
                          dest.L[i+1]=dest.L[i];
                          dest.getCurr()->t.tmin=source.getCurr()->t.tmin;
                          dest.getCurr()->t.data_min=source.getCurr()->t.data_min;
                          dest.getCurr()->t.tmax=dest.L[i+1].tmin;
                          dest.getCurr()->t.data_max=dest.L[i+1].data_min;
                          i++;
                          if (iv_append(dest,i+1))
                          {
                              dest.L[i+1].tmin=dest.getCurr()->t.tmax;
                              dest.L[i+1].data_min=source.getCurr()->t.data_max;
                              dest.L[i+1].tmax=source.getCurr()->t.tmax;
                              dest.L[i+1].data_max=source.getCurr()->t.data_max;
                              i++;
                          }
                      }
                    */
                } else {

                    append_copy = (dest.currIv == dest_prev);

                    if (dest.getCurr()->t.tmin < source.getCurr()->t.tmax) {
                        // U3
                        dest.currIv = dest_prev;
                        if (iv_append(&dest)) {

                            if (append_copy) {
                                dest.currIv = dest_prev;
                                dest.getCurrNext()->t = dest.getCurr()->t;
                            }

                            // this is 'solid approach' - prefer older object
                            dest.getCurr()->t.tmin = source.getCurr()->t.tmin;
                            dest.getCurr()->t.data_min = source.getCurr()->t.data_min;

                            dest.getCurr()->t.tmax = dest.getCurrNext()->t.tmin;
                            dest.getCurr()->t.data_max = dest.getCurrNext()->t.data_min;
                            break; //i=dest.N; // i++;
                        } else {
                            //INFOSTR("Errr on U3\n");
                            dest.getCurr()->t.tmin = source.getCurr()->t.tmin;
                            dest.getCurr()->t.data_min = source.getCurr()->t.data_min;
                            break; //i=dest.N;
                        }
                    } else {
                        // U6
                        dest.currIv = dest_prev;
                        if (iv_append(&dest)) {
                            if (append_copy) {
                                dest.currIv = dest_prev;
                                dest.getCurrNext()->t = dest.getCurr()->t;
                            }
                            dest.getCurr()->t = source.getCurr()->t;
                            break; // i=dest.N;
                            //i++;
                        } else {
                            //INFOSTR("Errr on U6\n");
                            dest.getCurr()->t = source.getCurr()->t;
                            break; //i=dest.N;
                        }
                    }
                }
            }
            dest_prev = dest.currIv;
            dest.goNext();
        }
        source.goNext();
    }
    *pdest = dest;
    psource->clear();
}

void iv_subtraction (IvList *pdest, IvList *psource)
{
    IvList dest = *pdest, source = *psource;
    INT dest_prev;

    if (dest.firstIv == 0) {
        psource->clear();
        return;
    }

    if (dest.firstIv == source.firstIv) {
        pdest->clear();
        psource->clear();
        return;
    }

    source.currIv = source.firstIv;
    while (source.isLink()) {
        dest.currIv = dest.firstIv;
        dest_prev = dest.currIv;
        while (dest.isLink()) {
            if (dest.getCurr()->t.tmin < source.getCurr()->t.tmin) {
                if (dest.getCurr()->t.tmax > source.getCurr()->t.tmin) {
                    if (dest.getCurr()->t.tmax > source.getCurr()->t.tmax) {
                        // S4
                        dest_prev = dest.currIv;
                        if (iv_append(&dest)) {
                            dest.currIv = dest_prev;
                            dest.getCurrNext()->t.tmin = source.getCurr()->t.tmax;
                            dest.getCurrNext()->t.data_min = source.getCurr()->t.data_max;

                            dest.getCurrNext()->t.tmax = dest.getCurr()->t.tmax;
                            dest.getCurrNext()->t.data_max = dest.getCurr()->t.data_max;

                            dest.getCurr()->t.tmax = source.getCurr()->t.tmin;
                            dest.getCurr()->t.data_max = source.getCurr()->t.data_min;
                            dest.goNext(); //i++;
                        } else
                            ;//INFOSTR("Errr on S4\n");

                    } else {
                        // S2
                        dest.getCurr()->t.tmax = source.getCurr()->t.tmin;
                        dest.getCurr()->t.data_max = source.getCurr()->t.data_min;
                    }
                } else {
                    ; // S1 - other dest-s can be impacted
                }
            } else {
                if (dest.getCurr()->t.tmax <= source.getCurr()->t.tmax) {
                    // S5
                    if (iv_remove(&dest, dest_prev)) {
                        //i--;
                        if (dest.currIv != dest_prev) {
                            dest_prev = dest.currIv;
                            continue;
                        }
                    }

                } else {
                    if (dest.getCurr()->t.tmin <= source.getCurr()->t.tmax) {
                        // S3
                        dest.getCurr()->t.tmin = source.getCurr()->t.tmax;
                        dest.getCurr()->t.data_min = source.getCurr()->t.data_max;
                    } else {
                        break; // i=dest.N; // S6 - finish dest and go to next source interval
                    }
                }
            }
            dest_prev = dest.currIv;
            dest.goNext();
        }
        source.goNext();
    }
    *pdest = dest;
    psource->clear();
}

void iv_intersection (IvList *pdest, IvList *psource)
{
    IvList dest = *pdest, source = *psource;
    INT dest_prev;

    if (source.firstIv == 0) {
        pdest->clear();
        return;
    } else if (dest.firstIv == source.firstIv) {
        return;
    }

    source.currIv = source.firstIv;
    while (source.isLink()) {
        dest.currIv = dest.firstIv;
        dest_prev = dest.currIv;
        while (dest.isLink()) {
            if (dest.getCurr()->t.tmin < source.getCurr()->t.tmin) {
                if (dest.getCurr()->t.tmax > source.getCurr()->t.tmin) {
                    if (dest.getCurr()->t.tmax > source.getCurr()->t.tmax) {
                        // I4
                        dest.getCurr()->t = source.getCurr()->t;
                    } else {
                        // I2
                        dest.getCurr()->t.tmin = source.getCurr()->t.tmin;
                        dest.getCurr()->t.data_min = source.getCurr()->t.data_min;
                    }
                } else {
                    // I1
                    if (iv_remove(&dest, dest_prev)) {
                        //i--;
                        if (dest.currIv != dest_prev) {
                            dest_prev = dest.currIv;
                            continue;
                        }
                    }
                }
            } else {
                if (dest.getCurr()->t.tmax <= source.getCurr()->t.tmax) {
                    // I5
                    // destination interval untouched
                } else {
                    if (dest.getCurr()->t.tmin <= source.getCurr()->t.tmax) {
                        // I3
                        dest.getCurr()->t.tmax = source.getCurr()->t.tmax;
                        dest.getCurr()->t.data_max = source.getCurr()->t.data_max;
                    } else {
                        // I6
                        if (iv_remove(&dest, dest_prev)) {
                            //i--;
                            if (dest.currIv != dest_prev) {
                                dest_prev = dest.currIv;
                                continue;
                            }
                        }
                    }
                }
            }
            dest_prev = dest.currIv;
            dest.goNext();
        }
        source.goNext();
    }
    *pdest = dest;
    psource->clear();
}
