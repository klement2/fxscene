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
#include "obj3D.h"
#include "surf3D.h"
//---------------------------------------------------------------------------

void TDR :: cleanUp()
{
#if 0
    TDBase::cleanUp();
    otype = TD_FIRST;
    surface = (TDSurf*)&defSurface; //&SurfConst[0];
#endif
}

static const TBrd2D Brd2D[NumOfPrim] = {
    {0, 1, 1, -1, 1}, // sphere
    {0, 1, 1, 0, 1}, // cylinder
    {0, 1, 0, 0, 1}, // cone
    {0.5, 0.5, 0.5, 0, 1}, // cube
    {0, RMAX, RMAX, 0, 0} // half-space
}
; // xymin,sl1,sl2,h1,h2;

void TDBase :: Border2D(TG3D* g, TBrd2D& b, MATRIX mb, bool drawBorders)
{
    static const REAL square [4][2] = { { -1, -1 } , {1, -1}, {1, 1}, { -1, 1} };
    VECTOR p3, op;
    POINT p; //, obs_p;
    REAL k;
    INT i, j;
    for (i = 0; i <= 3; i++)
        for (j = 0; j <= 1; j++) {
            if (j == 1) {
                p3[Z] = b.h1;
                k = b.sl1;
            } else {
                p3[Z] = b.h2;
                k = b.sl2;
            }
            p3[X] = square[i][0] * k + b.xymin;
            p3[Y] = square[i][1] * k + b.xymin;
            /* Transform a point of the rounding object
             * from the base position
             * to the position defined by the object's matrix m
             */
            PMultM(op, p3, mb);
            //g->ProjLine(&g->observer,&op,&obs_p, &p);
            g->ProjPoint(&op, &p);
            if (p.x < br.left)
                br.left = p.x;
            if (p.y < br.top)
                br.top = p.y;
            if (p.x > br.right)
                br.right = p.x;
            if (p.y > br.bottom)
                br.bottom = p.y;
        }
#if 1
    //br.InflateRect(2,2);
    //diagnostic: draw borders
    if (drawBorders) {
        g->DSetColor(VToC(surface->s.Ia));
        g->DRect(br);
        g->MSetColor(VToC(surface->s.Ia));
        g->MRect(br);
    }
#endif
}


void Sphere :: Precalc(TG3D* g, MATRIX mb)
{
    Border2D(g, (TBrd2D&)(Brd2D[0]), mb);
}

void Cylinder :: Precalc(TG3D* g, MATRIX mb)
{
    Border2D(g, (TBrd2D&)(Brd2D[1]), mb);
}

void Toroid:: Precalc(TG3D* g, MATRIX mb)
{
    br = g->GetViewRect();
}

void Cone :: Precalc(TG3D* g, MATRIX mb)
{
    Border2D(g, (TBrd2D&)(Brd2D[2]), mb);
}

void Cube :: Precalc(TG3D* g, MATRIX mb)
{
    Border2D(g, (TBrd2D&)(Brd2D[3]), mb);
}

void HalfSpace :: Precalc(TG3D* g, MATRIX mb)
{
    br = g->GetViewRect();
}

void RSphere :: Nvect()
{
    n[X] = p[X] + u[X] * tmin;
    n[Y] = p[Y] + u[Y] * tmin;
    n[Z] = p[Z] + u[Z] * tmin;
}

void RCone :: Nvect()
{
    if (nnmi != 1) {
        n[X] = p[X] + u[X] * tmin;
        n[Y] = p[Y] + u[Y] * tmin;
        n[Z] = 1 - p[Z] - u[Z] * tmin;
        if (n[Z] == 0)
            n[Z] = 1;
    } else {
        n[X] = 0;
        n[Y] = 0;
        n[Z] = -1;
    }
}

void RCylinder :: Nvect()
{
    if (nnmi < 3) {
        n[X] = p[X] + u[X] * tmin;
        n[Y] = p[Y] + u[Y] * tmin;
        n[Z] = 0;
    } else if (nnmi == 3) {
        n[X] = 0;
        n[Y] = 0;
        n[Z] = -1;
    } else {
        n[X] = 0;
        n[Y] = 0;
        n[Z] = 1;
    }
}

void RToroid:: Nvect()
{
}

static VECTOR CubeNVect[7] = {
    {
        0, 0, 0
    }
    , {1, 0, 0}, { -1, 0, 0}, {0, 1, 0},
    {0, -1, 0}, {0, 0, 1}, {0, 0, -1}
};

void RCube :: Nvect()
{
    VAssign(n, CubeNVect[nnmi])
}

void RHalfSpace :: Nvect()
{
    n[X] = 0;
    n[Y] = 0;
    if (nnmi == 1)
        n[Z] = -1;
    else if (nnma == 1)
        n[Z] = 1;
    else
        n[Z] = 0;
}

#define ISPNT1 1

// static REAL t,a,b,c,d,od,x,y,z,x1,y1,z1;

bool RSphere :: isPoint()
{
#if ISPNT1
    if (p[X] < -1 && (p[X] + u[X] < -1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[Z] < -1 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;
    if (p[X] > 1 && (p[X] + u[X] > 1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[Z] < -1 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0))) 
        return false;

    if (p[Y] < -1 && (p[Y] + u[Y] < -1 || (p[X] < -1 && u[Y] <= 0) || (p[X] > 1 && u[X] >= 0) || (p[Z] < -1 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;
    if (p[Y] > 1 && (p[Y] + u[Y] > 1 || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0) || (p[Z] < -1 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;

    if (p[Z] < -1 && (p[Z] + u[Z] < -1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0)))
        return false;
    if (p[Z] > 1 && (p[Z] + u[Z] > 1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0))) 
        return false;

    return true;
#else
    if (p[X] < -1 && p[X] + u[X] < -1)
        return false;
    if (p[X] > 1 && p[X] + u[X] > 1)
        return false;

    if (p[Y] < -1 && p[Y] + u[Y] < -1)
        return false;
    if (p[Y] > 1 && p[Y] + u[Y] > 1)
        return false;

    if (p[Z] < -1 && p[Z] + u[Z] < -1)
        return false;
    if (p[Z] > 1 && p[Z] + u[Z] > 1)
        return false;
    return true;
#endif
}

// Basic position: r = 1, S = [0,0,0]
void RSphere :: Point()
{
    REAL a, b, c, d, od;
    a = u[X] * u[X] + u[Y] * u[Y] + u[Z] * u[Z];
    b = u[X] * p[X] + u[Y] * p[Y] + u[Z] * p[Z];
    c = p[X] * p[X] + p[Y] * p[Y] + p[Z] * p[Z] - 1;
    od = b * b;
    d = od - a * c;
    // this test will be true for cases (tmin<0 && tmax<0)
    if ((d < 0) || (od >= d && ((b < 0 && a <= 0) || (b > 0 && a >= 0))))
        return;
    //if (d>=0 && a!=0)
    {
        od = sqrt(d);
        tmin = (-b - od) / a;
        tmax = (-b + od) / a;
        nnmi = 1;
        nnma = 1;
        /*if (b>0 && tmax >0)
            INFOSTR("Dbgx b=%f, od=%f, a=%f, tmax=%f, tmin=%f, p=[%f,%f,%f], u=[%f,%f,%f]\n",
                   b,od,a, tmax, tmin, p[X],p[Y],p[Z],u[X],u[Y],u[Z]);*/
    }
}

bool RCone :: isPoint()
{
#if ISPNT1
    if (p[X] < -1 && (p[X] + u[X] < -1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;
    if (p[X] > 1 && (p[X] + u[X] > 1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0))) 
        return false;

    if (p[Y] < -1 && (p[Y] + u[Y] < -1 || (p[X] < -1 && u[Y] <= 0) || (p[X] > 1 && u[X] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;
    if (p[Y] > 1 && (p[Y] + u[Y] > 1 || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;

    if (p[Z] < 0 && (p[Z] + u[Z] < 0 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0)))
        return false;
    if (p[Z] > 1 && (p[Z] + u[Z] > 1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0))) 
        return false;

    return true;
#else
    if (p[X] < -1 && p[X] + u[X] < -1)
        return false;
    if (p[X] > 1 && p[X] + u[X] > 1)
        return false;

    if (p[Y] < -1 && p[Y] + u[Y] < -1)
        return false;
    if (p[Y] > 1 && p[Y] + u[Y] > 1)
        return false;

    if (p[Z] < 0 && p[Z] + u[Z] < 0)
        return false;
    if (p[Z] > 1 && p[Z] + u[Z] > 1)
        return false;
    return true;
#endif
}

// Basic position: r = 1, v = 1, S = [0,0,0]
void RCone :: Point()
{
    REAL t, a, b, c, d, od, x, y, z1, z2;

    a = u[X] * u[X] + u[Y] * u[Y] - u[Z] * u[Z];
    b = p[X] * u[X] + p[Y] * u[Y] - p[Z] * u[Z] + u[Z];
    c = p[X] * p[X] + p[Y] * p[Y] - p[Z] * p[Z] - 1 + p[Z] + p[Z];
    od = b * b;
    d = od - a * c;

    if (!((d < 0) || (od >= d && ((b < 0 && a <= 0) || (b > 0 && a >= 0)))))
        //if (d>=0 && a!=0)
    {
        od = sqrt(d);
        t = (-b - od) / a;
        z1 = p[Z] + u[Z] * t;
        if (z1 >= 0 && z1 <= 1) {
            tmin = t;
            nnmi = 2;
        }
        t = (-b + od) / a;
        z2 = p[Z] + u[Z] * t;
        if (z2 >= 0 && z2 <= 1) {
            tmax = t;
            nnma = 3;
        }
        if (z1 < 0 && z2 < 0)
            return;
        if (z1 > 1 && z2 > 1)
            return;
    } else
        return;

    if (nnmi + nnma > 3)
        return;

    if (u[Z] != 0) {
        y = p[Y] - u[Y] * p[Z] / u[Z];
        x = p[X] - u[X] * p[Z] / u[Z];
        if (y * y + x * x <= 1) {
            if (u[Z] > 0) {
                tmin = -p[Z] / u[Z];
                nnmi = 1;
                if (nnma == 0) {
                    tmax = tmin;
                    nnma = 1;
                }
            } else {
                tmax = -p[Z] / u[Z];
                nnma = 1;
                if (nnmi == 0) {
                    tmin = tmax;
                    nnmi = 1;
                }
            }
        }
    }
}

bool RCylinder :: isPoint()
{
#if ISPNT1
    if (p[X] < -1 && (p[X] + u[X] < -1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;
    if (p[X] > 1 && (p[X] + u[X] > 1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0))) 
        return false;

    if (p[Y] < -1 && (p[Y] + u[Y] < -1 || (p[X] < -1 && u[Y] <= 0) || (p[X] > 1 && u[X] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;
    if (p[Y] > 1 && (p[Y] + u[Y] > 1 || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;

    if (p[Z] < 0 && (p[Z] + u[Z] < 0 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0)))
        return false;
    if (p[Z] > 1 && (p[Z] + u[Z] > 1 || (p[Y] < -1 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[X] < -1 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0))) 
        return false;

    return true;
#else
    if (p[X] < -1 && p[X] + u[X] < -1)
        return false;
    if (p[X] > 1 && p[X] + u[X] > 1)
        return false;

    if (p[Y] < -1 && p[Y] + u[Y] < -1)
        return false;
    if (p[Y] > 1 && p[Y] + u[Y] > 1)
        return false;

    if (p[Z] < 0 && p[Z] + u[Z] < 0)
        return false;
    if (p[Z] > 1 && p[Z] + u[Z] > 1)
        return false;
    return true;
#endif
}

// Basic position: r = 1, v = 1 , S = [0,0,0]
void RCylinder :: Point()
{
    REAL t, a, b, c, d, od, x, y, z1, z2;

    if ((p[Z] > 1 && u[Z] >= 0) || (p[Z] < 0 && u[Z] <= 0))
        return;
    a = u[X] * u[X] + u[Y] * u[Y];
    b = p[X] * u[X] + p[Y] * u[Y];
    c = p[X] * p[X] + p[Y] * p[Y] - 1;
    od = b * b;
    d = od - a * c;

    if (!((d < 0) || (od >= d && ((b < 0 && a <= 0) || (b > 0 && a >= 0)))))
        //if (d>=0 && a!=0)
    {
        od = sqrt(d);
        t = (-b - od) / a;
        z1 = p[Z] + u[Z] * t;
        if (z1 >= 0 && z1 <= 1) {
            tmin = t;
            nnmi = 1;
        }
        t = (-b + od) / a;
        z2 = p[Z] + u[Z] * t;
        if (z2 >= 0 && z2 <= 1) {
            tmax = t;
            nnma = 1;
        }
        if (z1 < 0 && z2 < 0)
            return;
        if (z1 > 1 && z2 > 1)
            return;
    } else
        return;

    if (nnmi + nnma < 2 && u[Z] != 0) {
        t = -p[Z] / u[Z];
        y = p[Y] + u[Y] * t;
        x = p[X] + u[X] * t;
        if (y * y + x * x <= 1) {
            if (u[Z] > 0) {
                tmin = t;
                nnmi = 3;
            } else {
                tmax = t;
                nnma = 3;
            }
        }
        if (nnmi + nnma <= 3) {
            t = (1 - p[Z]) / u[Z];
            y = p[Y] + u[Y] * t;
            x = p[X] + u[X] * t;
            if (y * y + x * x <= 1) {
                if (u[Z] < 0) {
                    tmin = t;
                    nnmi = 4;
                    if (nnma == 0) {
                        tmax = t;
                        nnma = 4;
                    }
                } else {
                    tmax = t;
                    nnma = 4;
                    if (nnmi == 0) {
                        tmin = t;
                        nnmi = 4;
                    }
                }
            }
        }
    }
}

bool RToroid :: isPoint()
{
    return true;
}

void RToroid :: Point()
{
    REAL a,b,c;
    Toroid *t = (Toroid*)o;
    if (u[X] == 0) {
        return; // TODO: a special case
    }
    a = p[Y] * p[Y] * ((u[Z] * u[Z] / (u[X] * u[X])) + 1);
    b = -2 * p[Z] * u[Z] * p[Y] / u[X];
    c = p[Z] * p[Z] + p[Y] * p[Y] + t->R * t->R - t->r * t->r;
    if (b * b - 4 * a * c > 0) {
        tmax = 1;
    }
    
}

INT tst1 = 0;

#if 1
#define C_CODE(fc, fnum, Xx, Yy, Zz, R, c1, c2, c3, c4, t_out, nn_out, num) \
    if (fc & fnum) \
    { \
        tst1++; \
        y=p[Yy]+u[Yy]*(R-p[Xx])/u[Xx]; \
        z=p[Zz]+u[Zz]*(R-p[Xx])/u[Xx]; \
        if (y<0) \
           { if (nnmi==0 && u[Yy]<0) return; fc &= -1^(fnum|c1); } \
        else if (y>1) \
           { if (nnmi==0 && u[Yy]>0) return; fc &= -1^(fnum|c2); } \
        else \
           { fc &= -1^(c1|c2); } \
        if (z<0) \
           { if (nnmi==0 && u[Zz]<0) return; fc &= -1^(fnum|c3); } \
        else if (z>1) \
           { if (nnmi==0 && u[Zz]>0) return; fc &= -1^(fnum|c4); } \
        else \
           { fc &= -1^(c3|c4); } \
        if (!(fc & 63)) \
            return; \
        if (fc & fnum) \
        { \
            t_out=(R-p[Xx])/u[Xx]; \
            nn_out=num; \
            fc=0; \
        } \
    } \

bool RCube :: isPoint()
{
    return true;
#if XISPNT1
    if (p[X] < 0 && (p[X] + u[X] < 0 || (p[Y] < 0 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;
    if (p[X] > 1 && (p[X] + u[X] > 1 || (p[Y] < 0 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0))) 
        return false;

    if (p[Y] < 0 && (p[Y] + u[Y] < 0 || (p[X] < 0 && u[Y] <= 0) || (p[X] > 1 && u[X] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;
    if (p[Y] > 1 && (p[Y] + u[Y] > 1 || (p[X] < 0 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0) || (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0)))
        return false;

    if (p[Z] < 0 && (p[Z] + u[Z] < 0 || (p[Y] < 0 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[X] < 0 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0)))
        return false;
    if (p[Z] > 1 && (p[Z] + u[Z] > 1 || (p[Y] < 0 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) || (p[X] < 0 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0))) 
        return false;

    return true;
#else 
    if (p[X] < 0 && p[X] + u[X] < 0)
        return false;
    if (p[X] > 1 && p[X] + u[X] > 1)
        return false;

    if (p[Y] < 0 && p[Y] + u[Y] < 0)
        return false;
    if (p[Y] > 1 && p[Y] + u[Y] > 1)
        return false;

    if (p[Z] < 0 && p[Z] + u[Z] < 0)
        return false;
    if (p[Z] > 1 && p[Z] + u[Z] > 1)
        return false;
    return true;
#endif
}

// Base position:  v = 1, S = [0.5,0.5,0.5]
void RCube :: Point()
{
    REAL y, z;
    INT f = -1, f1 = -1;

#if 1
    // the following speeds up the detection about 2x
    if ((p[X] < 0 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0) ||
        (p[Y] < 0 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) ||
        (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0))
        return;
#endif

    if (u[X] > 0)
        f ^= 1;
    else if (u[X] < 0)
        f ^= 2;
    else {
        f ^= 3;
        f1 ^= 3;
    }

    if (u[Y] > 0)
        f ^= 4;
    else if (u[Y] < 0)
        f ^= 8;
    else {
        f ^= 12;
        f1 ^= 12;
    }

    if (u[Z] > 0)
        f ^= 16;
    else if (u[Z] < 0)
        f ^= 32;
    else {
        f ^= 48;
        f1 ^= 48;
    }

    f1 ^= f;

    C_CODE(f, 1, X, Y, Z, 1, 4, 8, 16, 32, tmin, nnmi, 1);
    C_CODE(f, 2, X, Y, Z, 0, 4, 8, 16, 32, tmin, nnmi, 2);

    C_CODE(f, 4, Y, X, Z, 1, 1, 2, 16, 32, tmin, nnmi, 3);
    C_CODE(f, 8, Y, X, Z, 0, 1, 2, 16, 32, tmin, nnmi, 4);

    C_CODE(f, 16, Z, X, Y, 1, 1, 2, 4, 8,  tmin, nnmi, 5);
    C_CODE(f, 32, Z, X, Y, 0, 1, 2, 4, 8,  tmin, nnmi, 6);

    if (f != 0)
        return;

    C_CODE(f1, 1, X, Y, Z, 1, 4, 8, 16, 32, tmax, nnma, 1);
    C_CODE(f1, 2, X, Y, Z, 0, 4, 8, 16, 32, tmax, nnma, 2);

    C_CODE(f1, 4, Y, X, Z, 1, 1, 2, 16, 32, tmax, nnma, 3);
    C_CODE(f1, 8, Y, X, Z, 0, 1, 2, 16, 32, tmax, nnma, 4);

    C_CODE(f1, 16, Z, X, Y, 1, 1, 2, 4, 8,  tmax, nnma, 5);
    C_CODE(f1, 32, Z, X, Y, 0, 1, 2, 4, 8,  tmax, nnma, 6);
}
#endif

#if 0
#define RIN01(x) (x>=0 && x<=1)
// #define sqr(x) (x*x)
#define code(x,m, s1,s2,s3) { if (x<0) m |= s1; else if (x<=1) m |= s2; else m |= s3; }

#define C_CODE(m, value1, value2, Xx, Yy, Zz, nn, nn1, nn2, t_out, ret) \
    if ((m & (value1))==value1) \
    { \
        if (m & value2) \
        {   if (u[Xx]==0) \
               u[Xx]=0.00000001; \
            tst1++; \
            y=p[Yy]-p[Xx]*u[Yy]/u[Xx]; \
            z=p[Zz]-p[Xx]*u[Zz]/u[Xx]; \
            if (y>=0 && y<=1 && z>=0 && z<=1) \
            { \
                nn=nn2; \
                t1=-(p[Xx]/u[Xx]); \
                t_out=t1; \
            } \
            else \
              ret; \
        } \
        else \
        {   if (u[Xx]==0) \
               u[Xx]=0.00000001; \
            tst1++; \
            y=p[Yy]+(u[Yy]-u[Yy]*p[Xx])/u[Xx]; \
            z=p[Zz]+(u[Zz]-u[Zz]*p[Xx])/u[Xx]; \
            if (y>=0 && y<=1 && z>=0 && z<=1) \
            { \
                nn=nn1; \
                t1=(1-p[Xx])/u[Xx]; \
                t_out=t1; \
            } \
            else \
              ret; \
        } \
    } \


// Base position:  v = 1, S = [0.5,0.5,0.5]
void Cube :: Point()
{
    REAL t1, t2, x1, y1, z1, a, b, y, z;
    INT f1;

    if ((p[X] < 0 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0) ||
        (p[Y] < 0 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) ||
        (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0))
        return;

    a = u[X] * u[X] + u[Y] * u[Y] + u[Z] * u[Z];
    b = u[X] * p[X] + u[Y] * p[Y] + u[Z] * p[Z];

    t1 = 0;
    t2 = 0;

    if (u[X] < 0)
        t1 = u[X];
    else
        t2 = u[X];

    if (u[Y] < 0)
        t1 += u[Y];
    else
        t2 += u[Y];

    if (u[Z] < 0)
        t1 += u[Z];
    else
        t2 += u[Z];

    t1 = (t1 - b) / a;
    tst1++;

    x1 = p[X] + (t1 * u[X]);
    y1 = p[Y] + (t1 * u[Y]);
    z1 = p[Z] + (t1 * u[Z]);

    f1 = 0;

    code(x1, f1, 1, 2, 4);
    code(y1, f1, 8, 16, 32);
    code(z1, f1, 64, 128, 256);

    C_CODE(f1, (16 + 128), 1, X, Y, Z, nnmi, 1, 2, tmin, return)
    else
        C_CODE(f1, (2 + 128),  8, Y, X, Z, nnmi, 3, 4, tmin, return)
        else
            C_CODE(f1, (2 + 16),  64, Z, X, Y, nnmi, 5, 6, tmin, return)
            else {
                C_CODE(f1, 2, 8, Y, X, Z, nnmi, 3, 4, tmin,
                       C_CODE(f1, 2, 64, Z, X, Y, nnmi, 5, 6, tmin, return))
                else
                    C_CODE(f1, 16, 1, X, Y, Z, nnmi, 1, 2, tmin,
                           C_CODE(f1, 16, 64, Z, X, Y, nnmi, 5, 6, tmin, return))
                    else
                        C_CODE(f1, 128, 1, X, Y, Z, nnmi, 1, 2, tmin,
                               C_CODE(f1, 128, 8, Y, X, Z, nnmi, 3, 4, tmin, return))
                    }

    if (tmin == 0)
        return;

    t2 = (t2 - b) / a;
    tst1++;

    x1 = p[X] + (t2 * u[X]);
    y1 = p[Y] + (t2 * u[Y]);
    z1 = p[Z] + (t2 * u[Z]);

    f1 = 0;

    code(x1, f1, 1, 2, 4);
    code(y1, f1, 8, 16, 32);
    code(z1, f1, 64, 128, 256);

    C_CODE(f1, (16 + 128), 1, X, Y, Z, nnma, 1, 2, tmax, return)
    else
        C_CODE(f1, (2 + 128),  8, Y, X, Z, nnma, 3, 4, tmax, return)
        else
            C_CODE(f1, (2 + 16),  64, Z, X, Y, nnma, 5, 6, tmax, return)
            else {
                C_CODE(f1, 2, 8, Y, X, Z, nnma, 3, 4, tmax,
                       C_CODE(f1, 2, 64, Z, X, Y, nnma, 5, 6, tmax, return))
                else
                    C_CODE(f1, 16, 1, X, Y, Z, nnma, 1, 2, tmax,
                           C_CODE(f1, 16, 64, Z, X, Y, nnma, 5, 6, tmax, return))
                    else
                        C_CODE(f1, 128, 1, X, Y, Z, nnma, 1, 2, tmax,
                               C_CODE(f1, 128, 8, Y, X, Z, nnma, 3, 4, tmax, return))
                        else // only because of some floating point inaccuracies
                            C_CODE(f1, (1 + 8 + 64), 1, X, Y, Z, nnma, 1, 2, tmax,
                                   C_CODE(f1, (1 + 8 + 64), 8, Y, X, Z, nnma, 3, 4, tmax,
                                          C_CODE(f1, (1 + 8 + 64), 64, Z, X, Y, nnma, 5, 6, tmax, return)));
            }
}
#endif

#if 0
// Base position:  v = 1, S = [0.5,0.5,0.5]
void RCube :: Point()
{
    REAL x, y, z, x1, y1, z1;
    if ((p[X] < 0 && u[X] <= 0) || (p[X] > 1 && u[X] >= 0) ||
        (p[Y] < 0 && u[Y] <= 0) || (p[Y] > 1 && u[Y] >= 0) ||
        (p[Z] < 0 && u[Z] <= 0) || (p[Z] > 1 && u[Z] >= 0))
        return;
    if (u[X] != 0) {
        //d=1/u[X];
        //t=-p[X]/u[X]; //*d;
        tst1++;
        y = p[Y] + u[Y] * -p[X] / u[X]; //*t;
        z = p[Z] + u[Z] * -p[X] / u[X]; //*t;
        if (y >= 0 && y <= 1 && z >= 0 && z <= 1) {
            if (u[X] > 0) {
                tmin = -p[X] / u[X]; //t;
                nnmi = 1;
            } else {
                tmax = -p[X] / u[X]; //t;
                nnma = 1;
            }
        }
        tst1++;
        y1 = y + u[Y] / u[X]; //*d;
        z1 = z + u[Z] / u[X]; //*d;
        /*if ((nnmi+nnma==0) &&
                (((z1>1) && (z>1)) || ((z1<0) && (z<0)) ||
                 ((y1>1) && (y>1)) || ((y1<0) && (y<0))))
            return;*/
        if (y1 >= 0 && y1 <= 1 && z1 >= 0 && z1 <= 1) {
            if (u[X] < 0) {
                tmin = (-p[X] + 1) / u[X]; //d;
                nnmi = 2;
            } else {
                tmax = (-p[X] + 1) / u[X]; //d;
                nnma = 2;
            }
        }
        if (nnmi + nnma == 3)
            return;
    }
    if (u[Y] != 0) {
        //d=1/u[Y];
        //t=-p[Y]/u[Y]; //*d;
        tst1++;
        x = p[X] + u[X] * -p[Y] / u[Y]; //*t;
        z = p[Z] + u[Z] * -p[Y] / u[Y]; //*t;
        if (x >= 0 && x <= 1 && z >= 0 && z <= 1) {
            if (u[Y] > 0) {
                tmin = -p[Y] / u[Y]; //t;
                nnmi = 3;
            } else {
                tmax = -p[Y] / u[Y]; //t;
                nnma = 3;
            }
        }
        if (nnmi + nnma > 3)
            return;
        tst1++;
        x1 = x + u[X] / u[Y]; //*d;
        z1 = z + u[Z] / u[Y]; //*d;
        /*if (((z1>1) && (z>1)) || ((z1<0) && (z<0)) ||
                ((x1>1) && (x>1)) || ((x1<0) && (x<0)))
            return;*/
        if (x1 >= 0 && x1 <= 1 && z1 >= 0 && z1 <= 1) {
            if (u[Y] < 0) {
                tmin = (-p[Y] + 1) / u[Y]; //d;
                nnmi = 4;
            } else {
                tmax = (-p[Y] + 1) / u[Y]; //d;
                nnma = 4;
            }
        }
        if (nnmi + nnma > 4)
            return;
    }
    if (u[Z] == 0)
        return;
    //d=1/u[Z];
    //t=-p[Z]/u[Z]; //*d;
    tst1++;
    y = p[Y] + u[Y] * -p[Z] / u[Z]; //*t;
    x = p[X] + u[X] * -p[Z] / u[Z]; //*t;
    if (y >= 0 && y <= 1 && x >= 0 && x <= 1) {
        if (u[Z] > 0) {
            tmin = -p[Z] / u[Z]; //t;
            nnmi = 5;
        } else {
            tmax = -p[Z] / u[Z]; //t;
            nnma = 5;
        }
    }
    if (nnmi + nnma > 5)
        return;
    tst1++;
    y = y + u[Y] / u[Z]; //*d;
    x = x + u[X] / u[Z]; //*d;
    if (y >= 0 && y <= 1 && x >= 0 && x <= 1) {
        if (u[Z] < 0) {
            tmin = (-p[Z] + 1) / u[Z]; //d;
            nnmi = 6;
        } else {
            tmax = (-p[Z] + 1) / u[Z]; //d;
            nnma = 6;
        }
    }
}
#endif

bool RHalfSpace :: isPoint()
{
    if (p[Z] < 0 && u[Z] <= 0)
        return false;
    return true;
}

// Base position: plane = xy, normal vector direction = z axis positive part
void RHalfSpace :: Point()
{
    REAL t;

    if (u[Z] != 0) {
        t = -p[Z] / u[Z];
        if (u[Z] > 0) {
            tmin = t;
            tmax = REND_MAX;
            nnmi = 1;
            nnma = 2;
        } else {
            tmax = t;
            tmin = -REND_MAX;
            nnmi = 2;
            nnma = 1;
        }
    } else {
        if (p[Z] > 0) {
            nnmi = 2;
            nnma = 2;
            tmin = -REND_MAX;
            tmax = REND_MAX;
        }
    }
}

REAL sinLUT[361];
REAL cosLUT[361];

void InitLUT()
{
    INT k;
    for (k = 0; k <= 360; k++) {
        sinLUT[k] = sin (2 * M_PI_ * k / 360);
        cosLUT[k] = cos (2 * M_PI_ * k / 360);
    }
}

void Sphere :: Wf(TG3D* g)
{
    VECTOR rb;
    REAL tmp; //,ii,jj;
    INT i, j;

    g->NewSight(m);
    for (i = 0; i <= 180; i += 5) {
        //ii=i*M_PI180;
        rb[Z] = cosLUT[i]; //cos(M_PI90+ii);
        tmp = sinLUT[i]; //sin(ii);
        for (j = 0; j <= 360; j += 10) {
            //jj=j*M_PI180;
            rb[X] = tmp * sinLUT[j]; //sin(jj);
            rb[Y] = tmp * cosLUT[j]; //cos(jj);
            g->MPixel3D(rb, VToC(surface->s.Ia));
        }
    }
}

void Cylinder :: Wf(TG3D* g)
{
    VECTOR rb;
    POINT obrp;
    INT j;
    REAL jj, jj1;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    rb[X] = 0;
    rb[Y] = 1;
    rb[Z] = 0;
    g->MMoveTo3D(rb);
    for (j = 0; j <= 340; j += 20) {
        jj = j * M_PI180;
        jj1 = (j + 20) * M_PI180;
        rb[X] = sin(jj1);
        rb[Y] = cos(jj1);
        g->MLineTo3D(rb);
        obrp = g->MGetPosition();
        rb[Z] = 1;
        g->MLineTo3D(rb);
        rb[X] = sin(jj);
        rb[Y] = cos(jj);
        g->MLineTo3D(rb);
        rb[Z] = 0;
        g->MMoveTo(obrp);
    }
}

void Toroid :: Wf(TG3D* g)
{
    VECTOR rb;
    REAL tmp1; //,ii,jj;
    INT i, j;

    g->NewSight(m);
    for (i = 0; i <= 360; i += 10) {
        for (j = 0; j <= 360; j += 10) {
            tmp1 = R  + (r * cosLUT[j]);
            rb[X] = -sinLUT[i] * (tmp1);
            rb[Y] = cosLUT[i] * (tmp1);
            rb[Z] = r * sinLUT[j]; 
            g->MPixel3D(rb, VToC(surface->s.Ia));
        }
    }
}

void Cone :: Wf(TG3D* g)
{
    VECTOR rb;
    POINT obrp;
    INT j;
    REAL jj;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    rb[X] = 0;
    rb[Y] = 1;
    rb[Z] = 0;
    g->MMoveTo3D(rb);
    for (j = 20; j <= 360; j += 20) {
        jj = j * M_PI180;
        rb[X] = sin(jj);
        rb[Y] = cos(jj);
        g->ProjPoint(&rb, &obrp);
        g->MLineTo(obrp);
        rb[Z] = 1;
        rb[X] = 0;
        rb[Y] = 0;
        g->MLineTo3D(rb);
        rb[Z] = 0;
        g->MMoveTo(obrp);
    }
}

void Cube :: Wf(TG3D* g)
{
    static const REAL sq[5][2] = {
        {
            0, 0
        }
        , {1, 0}, {1, 1}, {0, 1}, {0, 0}
    };
    VECTOR rb;
    POINT obrp;
    INT i;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    for (i = 0; i < 4; i++) {
        rb[X] = sq[i][0];
        rb[Y] = sq[i][1];
        rb[Z] = 0;
        g->ProjPoint(&rb, &obrp);
        g->MMoveTo(obrp);
        rb[X] = sq[i + 1][0];
        rb[Y] = sq[i + 1][1];
        g->MLineTo3D(rb);
        rb[Z] = 1;
        g->MLineTo3D(rb);
        rb[X] = sq[i][0];
        rb[Y] = sq[i][1];
        g->MLineTo3D(rb);
        g->MMoveTo(obrp);
    }
}

void HalfSpace :: Wf(TG3D* g)
{
    VECTOR rb;
    POINT obrp;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    rb[X] = 0;
    rb[Y] = 0;
    rb[Z] = 0;
    g->MMoveTo3D(rb);
    rb[Z] = 1;
    g->ProjPoint(&rb, &obrp);
    g->MLineTo(obrp);
    rb[Z] = 0.8;
    rb[Y] = 0.1;
    g->MLineTo3D(rb);
    g->MMoveTo(obrp);
    rb[Z] = 0.8;
    rb[Y] = -0.1;
    g->MLineTo3D(rb);
    rb[X] = -1;
    rb[Y] = -1;
    rb[Z] = 0;
    g->ProjPoint(&rb, &obrp);
    g->MMoveTo(obrp);
    rb[X] = 1;
    rb[Y] = -1;
    g->MLineTo3D(rb);
    rb[X] = 1;
    rb[Y] = -1;
    g->MLineTo3D(rb);
    rb[X] = 1;
    rb[Y] = 1;
    g->MLineTo3D(rb);
    rb[X] = -1;
    rb[Y] = 1;
    g->MLineTo3D(rb);
    g->MLineTo(obrp);
}

void Sphere :: WfAcc(TG3D* g)
{
    INT xysteps = 20;
    INT zsteps = 20;

    INT xysteps360 = 360 / xysteps;
    INT zsteps360 = 180 / zsteps;

    VECTOR rb, rb1;
    //REAL stepxy=M_PI_/xysteps;
    //REAL stepz=2*M_PI_/zsteps;
    REAL zal, zal1, jj1s, jj1c, rb1z; //ii,jj
    INT i, j, ii, jj;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    for (i = 0; i < xysteps; i++) {
        //ii=i*stepxy;
        ii = i * xysteps360;
        rb[Z] = cosLUT[ii]; //cos(M_PI90+ii);
        rb1z = cosLUT[ii + xysteps360]; //cos(M_PI90+ii+stepxy);
        rb1[Z] = rb[Z];
        zal = sinLUT[ii]; //;sin(ii);
        zal1 = sinLUT[ii + xysteps360]; //sin(ii+stepxy);
        for (j = 0; j < zsteps; j++) {
            //jj=j*stepz;
            jj = j * zsteps360;
            jj1s = sinLUT[jj + zsteps360]; //sin((jj+stepz));
            jj1c = cosLUT[jj + zsteps360]; //cos((jj+stepz));
            rb[X] = zal * sinLUT[jj]; //sin(jj);
            rb[Y] = zal * cosLUT[jj]; //cos(jj);
            rb1[X] = zal * jj1s;
            rb1[Y] = zal * jj1c;
            g->MLine3D(rb, rb1);
            rb[X] = zal1 * jj1s;
            rb[Y] = zal1 * jj1c;
            rb1[Z] = rb[Z];
            rb[Z] = rb1z;
            g->MLine3D(rb, rb1);
            rb[Z] = rb1[Z];
        }
    }
}

void Cylinder :: WfAcc(TG3D* g)
{
    INT xysteps = 25;
    INT zsteps = 1;

    INT xysteps360 = 360 / (xysteps - 1);

    VECTOR rb, rb1;
    //REAL stepxy=2*M_PI_/xysteps;
    REAL stepz = 1.0 / zsteps;
    REAL cos1, sin1; //ii
    INT i, j, ii;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    for (j = 0; j < zsteps; j++) {
        for (i = 0; i < xysteps; i++) {
            //ii=i*stepxy;
            ii = i * xysteps360;
            cos1 = cosLUT[ii]; //cos(ii);
            sin1 = sinLUT[ii]; //sin(ii);
            rb[X] = cos1;
            rb[Y] = sin1;
            if (ii + xysteps360 > 360)
                ii = ii - 360;
            rb1[X] = cosLUT[ii + xysteps360]; //cos(ii+stepxy);
            rb1[Y] = sinLUT[ii + xysteps360]; //sin(ii+stepxy);
            rb[Z] = j * stepz;
            rb1[Z] = rb[Z];
            g->MLine3D(rb, rb1);
            rb[X] = rb1[X];
            rb[Y] = rb1[Y];
            rb1[Z] += stepz;
            g->MLine3D(rb, rb1);
            if (j + 1 <= zsteps) {
                rb[X] = cos1;
                rb[Y] = sin1;
                rb[Z] = rb1[Z];
                g->MLine3D(rb, rb1);
            }
        }
    }
}

void Toroid :: WfAcc(TG3D* g)
{
    VECTOR rb, rb1;
    REAL tmp1;
    INT i, j;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    for (i = 10; i <= 360; i += 10) {
        tmp1 = R  + (r * cosLUT[0]);
        rb1[X] = -sinLUT[i] * (tmp1);
        rb1[Y] = cosLUT[i] * (tmp1);
        rb1[Z] = r * sinLUT[0]; 
        for (j = 10; j <= 360; j += 10) {
            tmp1 = R  + (r * cosLUT[j]);
            rb[X] = -sinLUT[i] * (tmp1);
            rb[Y] = cosLUT[i] * (tmp1);
            rb[Z] = r * sinLUT[j]; 
            g->MLine3D(rb1, rb);
            rb1[X] = -sinLUT[i-10] * tmp1;
            rb1[Y] = cosLUT[i-10] * tmp1;
            rb1[Z] = rb[Z];
            g->MLine3D(rb1, rb);
            VAssign(rb1, rb);
        }
    }
}

void Cone :: WfAcc(TG3D* g)
{
    INT xysteps = 25;
    INT zsteps = 1;

    INT xysteps360 = 360 / (xysteps - 1);

    VECTOR rb, rb1;
    //REAL stepxy=2*M_PI_/xysteps;
    REAL stepz = 1.0 / zsteps;
    REAL jj, cos1, sin1; // ii
    INT i, j, ii;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    for (j = 0; j < zsteps; j++) {
        for (i = 0; i < xysteps; i++) {
            jj = 1.0 - stepz * j;
            //ii=i*stepxy;
            ii = i * xysteps360;
            rb[X] = jj * cosLUT[ii]; //cos(ii);
            rb[Y] = jj * sinLUT[ii]; //sin(ii);
            if (ii + xysteps360 > 360)
                ii = ii - 360;
            cos1 = cosLUT[ii + xysteps360]; //cos(ii+stepxy);
            sin1 = sinLUT[ii + xysteps360]; //sin(ii+stepxy);
            rb1[X] = jj * cos1;
            rb1[Y] = jj * sin1;
            rb[Z] = j * stepz;
            rb1[Z] = rb[Z];
            g->MLine3D(rb, rb1);
            jj = 1.0 - (stepz * (j + 1));
            rb[X] = jj * cos1; //cos(ii+stepxy);
            rb[Y] = jj * sin1; //sin(ii+stepxy);
            rb[Z] += stepz;
            g->MLine3D(rb, rb1);
        }
    }
}

void Cube :: WfAcc(TG3D* g)
{
    static const VECTOR sq[5] = {
        {
            0, 0, 0
        }
        , {0, 1, 0}, {1, 1, 0}, {1, 0, 0}, {0, 0, 0}
    };
    VECTOR rb, rb1;
    INT i;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    for (i = 0; i < 4; i++) {
        VAssign(rb, sq[i]);
        VAssign(rb1, sq[i + 1]);
        g->MLine3D(rb, rb1);
        rb[Z] = 1;
        rb1[Z] = 1;
        g->MLine3D(rb, rb1);
        VAssign(rb1, sq[i]);
        g->MLine3D(rb, rb1);
    }
}

void HalfSpace :: WfAcc(TG3D* g)
{
    VECTOR rb, rb1;

    g->NewSight(m);
    g->MSetColor(VToC(surface->s.Ia));
    Vector(rb, 0, 0, 0);
    Vector(rb1, 0, 0, 1);
    g->MLine3D(rb, rb1);
    rb[Z] = 0.8;
    rb[Y] = 0.1;
    g->MLine3D(rb, rb1);
    rb[Z] = 0.8;
    rb[Y] = -0.1;
    g->MLine3D(rb, rb1);
    Vector(rb, -1, -1, 0);
    Vector(rb1, 1, -1, 0);
    g->MLine3D(rb, rb1);
    Vector(rb, 1, 1, 0);
    g->MLine3D(rb, rb1);
    Vector(rb1, -1, 1, 0);
    g->MLine3D(rb, rb1);
    Vector(rb, -1, -1, 0);
    g->MLine3D(rb, rb1);
}
