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

#include "gr3D.h" 
#include "matrix.h"

void TG3D::Set2DView(RECT r)
{
    int s, v;
    viewRect = r;
    s = r.right - r.left;
    v = r.bottom - r.top;
    if ((s != 0) && (v != 0)) {
        yxasp = REAL(v) / REAL(s);
        xyasp = REAL(s) / REAL(v);
    } else {
        yxasp = 0;
        xyasp = 0;
    }
    /* alpha=aprox 21 deg 48 min
     * that's aproximately the angle related to human eye
     */
    cone_height = REAL(s) * 1.25;
    yxasp_cone_height = yxasp * cone_height;
    widthr = REAL(s) / 2;
    heightr = REAL(v) / 2;
    width = s / 2;
    height = v / 2;
}

static const POINT p2minus1 = {
    -1, -1
};


inline bool TG3D::pqClip(REAL directedProjection, REAL directedDistance)
{
    if (directedProjection == 0) {
        if (directedDistance < 0)
            return false;
    } else {
        REAL amount = directedDistance / directedProjection;
        if (directedProjection < 0) {
            if (amount > tMaximum)
                return false;
            else if (amount > tMinimum)
                tMinimum = amount;
        } else {
            if (amount < tMinimum)
                return false;
            else if (amount < tMaximum)
                tMaximum = amount;
        }
    }
    return true;
}


bool TG3D::ClipLine(VECTOR pa, VECTOR pb, POINT *p2a, POINT *p2b)
{
    REAL lom1X, lom1Y, lom2X, lom2Y;

    if (pa[X] == 0 || pb[X] == 0) {
        pa[X] = -1;
        pb[X] = -1;
    }
    lom1X = pa[Y] / pa[X];
    lom1X *= cone_height;
    lom1X = widthr - lom1X;
    lom1Y = pa[Z] / pa[X];
    lom1Y *= yxasp_cone_height;
    lom1Y = heightr + lom1Y;

    lom2X = pb[Y] / pb[X];
    lom2X *= cone_height;
    lom2X = widthr - lom2X;
    lom2Y = pb[Z] / pb[X];
    lom2Y *= yxasp_cone_height;
    lom2Y = heightr + lom2Y;

    // Liang-Barsky algorithm

    Vector2 P;
    P.x = lom2X - lom1X;
    P.y = lom2Y - lom1Y;
    tMinimum = 0;
    tMaximum = 1;

    if (pqClip(-P.x, lom1X - viewRect.left)) {
        if (pqClip(P.x, viewRect.right - lom1X)) {
            if (pqClip(-P.y, lom1Y - viewRect.top)) {
                if (pqClip(P.y, viewRect.bottom - lom1Y)) {
                    if (tMaximum < 1) {
                        lom2X = lom1X + tMaximum * P.x;
                        lom2Y = lom1Y + tMaximum * P.y;
                    }
                    if (tMinimum > 0) {
                        lom1X += tMinimum * P.x;
                        lom1Y += tMinimum * P.y;
                    }
                    p2a->x = (INT)floor(lom1X);
                    p2a->y = (INT)floor(lom1Y);
                    p2b->x = (INT)floor(lom2X);
                    p2b->y = (INT)floor(lom2Y);
                    return true;
                }
            }
        }
    }
    if (lom1X < viewRect.left)
        p2a->x = viewRect.left - 1;
    else
        p2a->x = viewRect.right + 1;
    if (lom1Y < viewRect.top)
        p2a->y = viewRect.top - 1;
    else
        p2a->y = viewRect.bottom + 1;
    *p2b = *p2a;

    //*p2a=p2minus1;
    //*p2b=p2minus1;
    return false;
}


void TG3D::ProjLine(VECTOR *p3a, VECTOR *p3b, POINT *p2a, POINT *p2b)
{
    REAL ti;
    VECTOR pa, pb, u;
    PMultM(pa, *p3a, t.M);
    PMultM(pb, *p3b, t.M);

    if ((pa[X] > 0) && (pb[X] > 0)) {
        /* both points are behind the observer */
        *p2a = p2minus1;
        *p2b = p2minus1;
    } else {
        if ((pa[X] > 0) && (pb[X] < 0)) {
            //fxwarning("1. pa=[%f,%f,%f], pb=[%f,%f,%f]\n", pa[X],pa[Y],pa[Z],pb[X],pb[Y],pb[Z]);
            VSub(u, pa, pb);
            ti = (-0.01 - pb[X]) / u[X];
            pa[X] = -0.01;
            pa[Y] = pb[Y] + (u[Y] * ti);
            pa[Z] = pb[Z] + (u[Z] * ti);

            //fxwarning("1a. pa=[%f,%f,%f]\n", pa[X],pa[Y],pa[Z]);
            ClipLine(pa, pb, p2a, p2b);
            //fxwarning("1b. p2a=[%d,%d] p2b=[%d,%d]\n", p2a->x, p2a->y, p2b->x, p2b->y);
        } else if ((pa[X] < 0) && (pb[X] > 0)) {
            //fxwarning("2. pa=[%f,%f,%f], pb=[%f,%f,%f]\n", pa[X],pa[Y],pa[Z],pb[X],pb[Y],pb[Z]);
            VSub(u, pb, pa);
            ti = (-0.01 - pa[X]) / u[X];
            pb[X] = -0.01;
            pb[Y] = pa[Y] + (u[Y] * ti);
            pb[Z] = pa[Z] + (u[Z] * ti);

            //fxwarning("2a. pb=[%f,%f,%f]\n", pb[X],pb[Y],pb[Z]);
            ClipLine(pa, pb, p2a, p2b);
            //fxwarning("2b. p2a=[%d,%d] p2b=[%d,%d]\n", p2a->x, p2a->y, p2b->x, p2b->y);
        } else {
            ClipLine(pa, pb, p2a, p2b);
        }
    }
}

void TG3D::ProjPoint(VECTOR *p3, POINT *p2)
{
    VECTOR p3out;

    /* POI=[-1,0,0] OBS=[0,0,0] */
    /* see void TCamera::CameraReset() */
    PMultM(p3out, *p3, t.M);
    /* Now project rect [-w,-h,w,h] to [0,0,w*2,h*2] */
    JustifyProjection(p3out, p2);
}

void TG3D::JustifyProjection(VECTOR p, POINT *p2)
{
    REAL lom1, lom2, lom;

    /* points behind the observer cannot be displayed
    * but here we make a little trick to estimate their
    * projection "as good as possible"... - the main reason
    * is a little performance gain when using combination
    * of 'moveto p1'+'lineto p2'+'lineto p3'+...
    * instead of 'line p1 p2'+'line p2 p3'+...
    * while still performing 'an acceptable level' of projection
    */
    if (p[X] >= 0) {
        p[X] += 1;
        p[Y] *= p[X];
        p[Z] *= p[X];
        p[X] = -0.01;
    }

    lom1 = p[Y] / p[X];
    lom1 *= cone_height;
    lom2 = p[Z] / p[X];
    lom2 *= yxasp_cone_height;


    if (fabs(lom1) > REAL_MAX_INT4) {
        //fxwarning("lom1=%f\n", lom1);
        lom =  fabs((REAL_MAX_INT4 - width) / lom1);
        lom1 *= lom;
        lom2 *= lom;
        if (lom2 > REAL_MAX_INT4)
            lom2 = REAL_MAX_INT4 - height;
        if (lom2 < -REAL_MAX_INT4)
            lom2 = -REAL_MAX_INT4;
    } else if (fabs(lom2) > REAL_MAX_INT4) {
        //fxwarning("lom2=%f\n", lom2);
        lom =  fabs((REAL_MAX_INT4 - height) / lom2);
        lom1 *= lom;
        lom2 *= lom;
        if (lom1 > REAL_MAX_INT4)
            lom1 = REAL_MAX_INT4;
        if (lom1 < -REAL_MAX_INT4)
            lom1 = - REAL_MAX_INT4 + width;
    }

    p2->x = (INT)floor((width - lom1));
    p2->y = (INT)floor((height + lom2));
}

void TG3DPrl::ProjPoint(VECTOR *p3, POINT *p2)
{
    p2->x = 0;
    p2->y = 0;
    // todo
    *p3[X] = 0;
}

void TG3D::MPixel3D(VECTOR p, TColor color)
{
    POINT tmpp2;
    ProjPoint((VECTOR*)p, &tmpp2);
    MPixel(tmpp2, color);
}

void TG3D::MLine3D(VECTOR p1, VECTOR p2)
{
    POINT tmpp21, tmpp22;
    ProjLine((VECTOR*)p1, (VECTOR*)p2, &tmpp21, &tmpp22);
    MLine(tmpp21, tmpp22);
}

void TG3D::MLineTo3D(VECTOR p)
{
    POINT tmpp2;
    ProjPoint((VECTOR*)p, &tmpp2);
    MLineTo(tmpp2);
}

void TG3D::MMoveTo3D(VECTOR p)
{
    POINT tmpp2;
    ProjPoint((VECTOR*)p, &tmpp2);
    MMoveTo(tmpp2);
}
