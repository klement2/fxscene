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

#include "rend.h" 
#include "matrix.h"
#include "interval.h"
#include <stdio.h>
#include <stdlib.h>

bool ray_debug; // extern from rend.h

void Rend :: SomePrecalculations(TG3D* g, TDTree *o, PLList *l)
{
    TDBase *ci;
    MATRIX tmpM;
    MATRIX m;

    M1(m);
    TDTreeLeafItem *restore;
    o->Top();
    while (o->Curr() != NULL) {
        ci = (TDBase*)o->getItem();

        if (ci->getOtype() == TDGROUP) {
            ;
        } else if (ci->getOtype() == TDBASE) {
            if (o->Curr()->getChildrenCount() > 0) {
                memcpy(tmpM, m, sizeof(MATRIX));
                memcpy(ci->mi, m, sizeof(MATRIX));
                MMultiply(m, ci->m, tmpM);
            }
        } else if (ci->getOtype() >= TD_FIRST && ci->getOtype() <= TD_LAST) {
            ci->br.right = -MAXINT;
            ci->br.bottom = -MAXINT;
            ci->br.left = MAXINT - 1;
            ci->br.top = MAXINT - 1;

            // Vypocet inverznej matice k m , teda mi
            if ((o->Curr()->getParent() != o->getRoot())) {
                MMultiply(tmpM, ci->m, m);
                MInverse(ci->mi, tmpM);
                ci->Precalc(g, tmpM);
            } else {
                MInverse(ci->mi, ci->m);
                // Poloha pozorovatela pre teleso v zakl. polohe
                // BNasobM(ci->p,g->poloha,ci->mi);
                // Vypocet obdlznikoveho ohranicenia na obr.
                ci->Precalc(g, ci->m);
            }
        }
        // Next object
        // we have to restore current transform. matrix as we go up
        restore = NULL;
        o->Walk();
        while (o->isValid() && !o->isWalkDirectionDown()) {
            ci = (TDBase*)o->getItem();
            if (o->Curr()->getChildrenCount() > 0 && ci->getOtype() == TDBASE) {
                restore = (TDTreeBranchItem*) o->Curr();
            }
            o->Walk();
        }
        if (restore != NULL) {
            memcpy(m, ADPD(TDBase, restore)->mi, sizeof(MATRIX));
        }
    }

    l->Top(); // vypocet polohy svetiel na obrazovke
    while (l->Curr() != NULL) {
        g->ProjPoint(&l->GetItem()->pl, &l->GetItem()->plo);
        l->Down();
    }
}

inline bool RectContainsPoint(RECT r, POINT p)
{
    return (p.x >= r.left && p.x <= r.right && p.y >= r.top && p.y <= r.bottom);
}

#if 0
inline INT Intersect_Code(RECT rect, POINT point)
{
    register INT result = 0;

    if (point.x < rect.left)
        result = 1;
    else if (point.x > rect.right)
        result = 2;
    if (point.y < rect.top)
        result |= 4;
    else if (point.y > rect.bottom)
        result |= 8;
    return result;
}


bool Intersect(RECT r, POINT lp1, POINT lp2)
{
    register INT c1, c2, x, y;
    long a, b, c;
    // Pozor ! moze pretiect aj long (pre vzdialene svetlo)
    POINT ptmp;
    bool von;

    c1 = Intersect_Code(r, lp1);
    if (c1 == 0)
        return true;
    c2 = Intersect_Code(r, lp2);
    if (c2 == 0)
        return true;
    if (c1 & c2)
        return false;
    if (lp1.x > lp2.x) {
        ptmp = lp1;
        lp1 = lp2;
        lp2 = ptmp;
        x = c1;
        c1 = c2;
        c2 = x;
    }
    a = lp2.y - lp1.y;
    b = lp1.x - lp2.x;
    c = -a * lp1.x - b * lp1.y;
    //return true;
    if ((c1 | c2) == 15)
        if (c1 == 5)                                            // rohy 0101,1010
            if (a * r.right + b * r.top + c >= 0 && a * r.left + b * r.bottom + c < 0)
                return true;
            else
                return false;
        else                                                    // rohy 1001,0110
            if (a * r.left + b * r.top + c >= 0 && a * r.right + b * r.bottom + c < 0)
                return true;
            else
                return false;
    von = true;
    if (c2 == 8 || (c2 == 10 && c1 == 1)) {
        x = r.left;
        y = r.bottom;
    }  //a
    else if (c1 == 8 || (c2 == 2 && c1 == 9)) {
        x = r.right;
        y = r.bottom;
    } //d
    else {
        von = false;
        if (c2 == 4 || c1 == 1) {
            x = r.left;
            y = r.top;
        } //b
        else {
            x = r.right;
            y = r.top;
        } //c
    }
    if (a * x + b * y + c < 0)
        return von;
    else
        return !von;
    /*
      1001 | 1000 | 1010
      -----a------d-----
      0001 | 0000 | 0010     0000 - obdlznik r.
      -----b------c-----
      0101 | 0100 | 0110

     orientacia v zmysle hodinovych ruciciek a zlava doprava
    */
}
#endif

TColor Rend :: Light(TDR *RObj, IvtArr *ia, POINT ri, TG3D *g, VECTOR V, TDLigthList &l, TDRList &o)
{
    TDR *LObj;
    TDBase *Obj;
    THalfLine LPol;
    TColor color;
    TSurf S;
    VECTOR SP;
    INT count;
    VECTOR GI;
    VECTOR V1;

    VECTOR P, P1, L, R, H, N, Iv, LV, Mi;
    REAL coss, cosd, k, ld, cosdr, D, F1;

    VOne(GI);

    VAssign(SP, g->observer);
    VAssign(V1, V);

    VZero(Iv);                          // Iv - final intensity (color) of pixel
    count = 0;
    do {
        Obj = RObj->o;
        if (RObj->tmin < 0)
            RObj->tmin = 0;

        RObj->Nvect();
        if (Obj->oper == O_MINUS)
            VReverse(RObj->n);

        S = Obj->surface->s;

        VMultM(N, RObj->n, Obj->m);    // N = n transformed from the basic position
        VNormalize(N);                  // size of N = 1
        VAddVMult(P, SP,                // P - point, for which we calculate its color
                  RObj->tmin, V1);      // V1 - vector from observer to the point of interest
        VReverse(V1);
        VNormalize(V1)                  // size of V1 = 1
        // VSub(vd,SP,P);                  // vd - vector [point on the object, observer]
        // VLengthSquared(dd,vd);          // dd - distance(from observer to the surface)^2

        l.Top();
        while (l.isValid()) {
            TDLight *li = ADPI(TDLight, l);
            VSub(L, li->pl, P);         // L -vector of light ray, pl - position of light
            VLengthSquared(ld, L);
            VAssign(LV, L);             // ld=size of L^2
            VSDivideV(L, sqrt(ld));     // VNormalize(L); // size of L = 1
            VSDotP(cosd, L, N);         // cosd - angle between L and N

            if ((cosd > 0)) {           // calculation of intersection between L and scene
                k = li->s;              // s - coefficient of light intensity decrease
                if (options.shades) {
                    VAddVMult(P1, P, 0.0001, L);
                    LPol.P = &P1;
                    LPol.V = &LV;
                    // RObj != NULL
                    LObj = Intersection(LPol, ia, ri, o, false, true);
                    if (LObj && ((LObj->tmin > 0 && LObj->tmin < 1) || (LObj->tmax > 0 && LObj->tmax < 1))) {
                        k = 0;
                        l.Down();
                        continue;
                    }
                }
            } else {
                k = 0;
            }

            //k=1/(li->s*(ld+dd));       // intensity of light decreases with distance ...
            VSum(H, V1, L);
            VNormalize(H);
            VSDotP(coss, H, N);          // coss - angle between H and N

            // Beckmann distribution (n <==> m)
            D = pow(M_E, -pow(((sin(acos(coss)) / coss) / S.n), 2)) / (4 * S.n * S.n * pow(coss, 4));

            if (ray_debug) {
                INFOSTR("Light: D=%f, coss=%f, S.n=%f\n", D, coss, S.n);
            }

#if 0
            // Cook-Torrance model....
            VSDotP(cosnv, N, V1);

            if (k != 0 && cosnv > 0) {

                VSDotP(cosvh, V1, H);
                VSDotP(cosc, L, H);

                Gm = (2 * coss * cosnv) / cosvh;
                Gs = (2 * coss * cosd) / cosvh;
                G  = 1;
                if (Gm < G)
                    G = Gm;
                if (Gs < G)
                    G = Gs;

                // here instead of 1.5 should be parameters n1/n2 (2 different materials)
                g1[X] = sqrt(pow(1.5/*S.kd[X]/1*/, 2) + cosc * cosc - 1);
                g1[Y] = sqrt(pow(1.5/*S.kd[Y]/1*/, 2) + cosc * cosc - 1);
                g1[Z] = sqrt(pow(1.5/*S.kd[Z]/1*/, 2) + cosc * cosc - 1);

                F[X] = pow(g1[X] - cosc, 2) / (2 * pow(g1[X] + cosc, 2)) * (1 + pow(cosc * (g1[X] + cosc) - 1, 2) / pow(cosc * (g1[X] - cosc) + 1, 2));
                F[Y] = pow(g1[Y] - cosc, 2) / (2 * pow(g1[Y] + cosc, 2)) * (1 + pow(cosc * (g1[Y] + cosc) - 1, 2) / pow(cosc * (g1[Y] - cosc) + 1, 2));
                F[Z] = pow(g1[Z] - cosc, 2) / (2 * pow(g1[Z] + cosc, 2)) * (1 + pow(cosc * (g1[Z] + cosc) - 1, 2) / pow(cosc * (g1[Z] - cosc) + 1, 2));

                DGF[X] = D * G * F[X] / (M_PI * cosd * cosnv);
                DGF[Y] = D * G * F[Y] / (M_PI * cosd * cosnv);
                DGF[Z] = D * G * F[Z] / (M_PI * cosd * cosnv);

                // kd, cosd, k should not be here...
                Iv[X] += GI[X] * li->i[X] * (S.kd[X] * cosd * k + (1 - S.Ia[X] - S.ks[X]) * DGF[X] * cosd + S.ks[X] * D * G * F[X] / (M_PI * cosnv));
                Iv[Y] += GI[Y] * li->i[Y] * (S.kd[Y] * cosd * k + (1 - S.Ia[Y] - S.ks[Y]) * DGF[Y] * cosd + S.ks[Y] * D * G * F[Y] / (M_PI * cosnv));
                Iv[Z] += GI[Z] * li->i[Z] * (S.kd[Z] * cosd * k + (1 - S.Ia[Z] - S.ks[Z]) * DGF[Z] * cosd + S.ks[Z] * D * G * F[Z] / (M_PI * cosnv));
            }

#endif
#if 1
            // Phong model...
            // li->i - intensity of point light
            // ks = koef. specular - specular reflection
            // kd = koef. diffusion - diffuse light

            if (S.metalic > 0.0) {
                F1 = 0.014567225 / pow((fabs(acos(cosd)) / M_PI_2) - 1.12, 2) - 0.011612903;
                if (F1 < 0)
                    F1 = 0;
                if (F1 > 1)
                    F1 = 1;
                Mi[X] = li->i[X] * (1.0 + S.metalic * (1.0 - F1) * (S.Ia[X] - 1.0));
                Mi[Y] = li->i[Y] * (1.0 + S.metalic * (1.0 - F1) * (S.Ia[Y] - 1.0));
                Mi[Z] = li->i[Z] * (1.0 + S.metalic * (1.0 - F1) * (S.Ia[Z] - 1.0));

                Iv[X] += GI[X] * Mi[X] * (S.ks[X] * D + S.kd[X] * cosd * k);
                Iv[Y] += GI[Y] * Mi[Y] * (S.ks[Y] * D + S.kd[Y] * cosd * k);
                Iv[Z] += GI[Z] * Mi[Z] * (S.ks[Z] * D + S.kd[Z] * cosd * k);
            } else {
                Iv[X] += GI[X] * li->i[X] * (S.ks[X] * D + S.kd[X] * cosd * k);
                Iv[Y] += GI[Y] * li->i[Y] * (S.ks[Y] * D + S.kd[Y] * cosd * k);
                Iv[Z] += GI[Z] * li->i[Z] * (S.ks[Z] * D + S.kd[Z] * cosd * k);
            }
#endif
            if (ray_debug) {
                INFOSTR("Light: Iv[X]=%f, Iv[Y]=%f, Iv[Z]=%f, GI[X]=%f, GI[Y]=%f, GI[Z]=%f, cosd=%f, k=%f\n",
                        Iv[X], Iv[Y], Iv[Z], GI[X], GI[Y], GI[Z], cosd, k);
            }

            l.Down();
        }


        Iv[X] += GI[X] * Obj->surface->s.Ia[X];
        Iv[Y] += GI[Y] * Obj->surface->s.Ia[Y];
        Iv[Z] += GI[Z] * Obj->surface->s.Ia[Z];

        GI[X] *= S.ks[X];
        GI[Y] *= S.ks[Y];
        GI[Z] *= S.ks[Z];

        // R1 = 2 * (V * n) * n – V
        VSDotP(cosdr, V1, N);         // cosdr = uhol medzi V a N
        VSMult(R, N, 2 * cosdr);
        VSubV(R, V1);                // R1 - odrazeny vektor luca V, velkost R1 = 1

        VAssign(V1, R);
        VAddVMult(SP, P, 0.0001, V1);
        LPol.P = &SP;
        LPol.V = &V1;
        RObj = Intersection(LPol, ia, ri, o, false, false);

        count++;
    } while (RObj != NULL && count < 16 && (GI[X] > 0.0000001 || GI[Y] > 0.0000001 || GI[Z] > 0.0000001));

    if (Iv[X] < 0)
        Iv[X] = 0;
    else if (Iv[X] > 1)
        Iv[X] = 1;
    if (Iv[Y] < 0)
        Iv[Y] = 0;
    else if (Iv[Y] > 1)
        Iv[Y] = 1;
    if (Iv[Z] < 0)
        Iv[Z] = 0;
    else if (Iv[Z] > 1)
        Iv[Z] = 1;
    color = VToC(Iv);
    return color;
}


TDR* Rend::Intersection(THalfLine hl, IvtArr *ia, POINT ri, TDRList &o, bool isV, bool isLightRay)
{
    TDR* ci;
    TDBase *co;
    REAL tmp;
    char tmp1;
    IvList ivl, ivlg, ivloc;
    IvList* p_ivl;

    ivlg.start(ia);
    ivl.start(ia);
    ivloc.start(ia);
    p_ivl = &ivlg;

    // This can be parallelized...

// #pragma omp parallel num_threads(1)

    o.Top();
    while (o.isValid()) {
        ci = (TDR*)o.getItem();
        co = ci->o;

        if ((co->getOtype() < TD_FIRST || co->getOtype() > TD_LAST /*|| ci==RObj*/)) {
            // || (p_ivl == NULL && ci->oper != O_LOCALPLUS && ci->oper != O_PLUS)) {
            o.Down();
            continue;
        }

#if 0
        pf(parent, ci, hl, ri, isV, o.isBottom() ? true : false);
#else
        ci->tmin = 0;
        ci->tmax = 0;
        if ((!isV) || 1) { // || RectContainsPoint(co->br, ri)) {
            PMultM(ci->p, *hl.P, co->mi);
            VMultM(ci->u, *hl.V, co->mi);
            if ((!isV) || ci->isPoint()) {
                ci->nnmi = 0;
                ci->nnma = 0;
#undef TEST_ISPOINT
#ifdef TEST_ISPOINT
                ci->tmin = 0;
                ci->tmax = 10000;
                ci->nnmi = 1;
                ci->nnma = 1;
#else
                ci->Point();
#endif
            }
        }
#endif
        o.Down();
    }

// #pragma omp single

    o.Top();
    while (o.isValid()) {
        ci = (TDR*)o.getItem();
        co = ci->o;

        if ((co->getOtype() < TD_FIRST || co->getOtype() > TD_LAST /*|| ci==RObj*/) ||
            (p_ivl == NULL && co->oper != O_LOCALPLUS && co->oper != O_PLUS)) {
            o.Down();
            continue;
        }
        if (ci->tmin != 0 || ci->tmax != 0) { // (!isV) || RectContainsPoint(ci->br, ri)) {

            if (ray_debug) {
                INFOSTR("InterSect: tmin=%f, tmax=%f P[%d,%d], nnmi=%d, nnma=%d, object=%s\n",
                        ci->tmin, ci->tmax, ri.x, ri.y, ci->nnmi, ci->nnma, co->getOName());
            }
            if (((ci->tmin != 0) && (ci->nnmi == 0)) || ((ci->tmax != 0) && (ci->nnma == 0))) {
                INFOSTR("Intersect_E1: tmin=%f, tmax=%f P[%d,%d], nnmi=%d, nnma=%d, object=%s\n",
                        ci->tmin, ci->tmax, ri.x, ri.y, ci->nnmi, ci->nnma, co->getOName());
            }
            if ((ci->tmin > ci->tmax) && (ci->tmin != REND_MAX)) { // && ci->tmin>0)
                INFOSTR("Intersect_E2: tmax=%f < tmin=%f P[%d,%d], nnmi=%d, nnma=%d, object=%s\n",
                        ci->tmax, ci->tmin, ri.x, ri.y, ci->nnmi, ci->nnma, co->getOName());

                tmp = ci->tmax;
                ci->tmax = ci->tmin;
                ci->tmin = tmp;
                tmp1 = ci->nnma;
                ci->nnma = ci->nnmi;
                ci->nnmi = tmp1;
            }
            // case: the light is between the object and surface...
            if (isLightRay && (ci->tmin > 1)) {
                ci->tmin = 0;
                ci->tmax = 0;
            }
        }
        if ((ci->nnmi != 0) && (ci->nnma != 0) &&
            ((ci->tmin > 0 || ci->tmax > 0) || ((p_ivl != NULL) && (p_ivl->getFirst()->t.tmin < ci->tmax)))) {
            if (ray_debug) {
                INFOSTR("InterSect: Evaluating...\n");
            }
            ivl.clear();
            iv_append(&ivl);
            ivl.getFirst()->t.tmin = ci->tmin;
            ivl.getFirst()->t.tmax = ci->tmax;
            ivl.getFirst()->t.data_min = ci;
            ivl.getFirst()->t.data_max = ci;
            switch (co->oper) {
            case O_PLUS:
                if (p_ivl == &ivloc) {
                    iv_union(&ivlg, p_ivl);
                }
                p_ivl = &ivlg;
                iv_union(p_ivl, &ivl);
                break;

            case O_MINUS:
                iv_subtraction(p_ivl, &ivl);
                break;

            case O_LOCALPLUS:
                if (p_ivl == &ivloc) {
                    iv_union(&ivlg, p_ivl);
                }
                ivloc.clear();
                iv_append(&ivloc);
                ivloc.getFirst()->t = ivl.getFirst()->t;
                p_ivl = &ivloc;
                break;

            case O_INTERSECTION:
                iv_intersection(p_ivl, &ivl);
                break;

            default:
                ;
            }
            /*
            if (ri.x==295 && ri.y==205)
            {
                INFOSTR("N=%d, otype=%d, tmin=%f, tmax=%f\n", ivlg.N, ci->getOtype(), ci->tmin, ci->tmax);
                INFOSTR("L[0] tmin=%f, tmax=%f\n",ivlg.L[0].tmin, ivlg.L[0].tmax);
                INFOSTR("L[1] tmin=%f, tmax=%f\n",ivlg.L[1].tmin, ivlg.L[1].tmax);
                INFOSTR("L[2] tmin=%f, tmax=%f\n",ivlg.L[2].tmin, ivlg.L[2].tmax);
                INFOSTR("L[3] tmin=%f, tmax=%f\n",ivlg.L[3].tmin, ivlg.L[3].tmax);
                INFOSTR("L[4] tmin=%f, tmax=%f\n",ivlg.L[4].tmin, ivlg.L[4].tmax);
            }*/
        } else {
            switch (co->oper) {
            case O_PLUS:
                if (p_ivl == &ivloc) {
                    iv_union(&ivlg, p_ivl);
                    p_ivl = &ivlg;
                }
                break;
            case O_LOCALPLUS:
                if (p_ivl == &ivloc) {
                    iv_union(&ivlg, p_ivl);
                }
                p_ivl = NULL;
                break;
            case O_INTERSECTION:
                p_ivl->clear();
                break;
            default:
                ;
            }
        }
        if (isLightRay && (p_ivl != NULL) && (p_ivl != &ivloc) && (p_ivl->isFirst())) {
            TDR *LObj = NULL;
            if (ray_debug) {
                p_ivl->goFirst();
                while (p_ivl->isLink()) {
                    if (p_ivl->get_tMin() > 0 || p_ivl->get_tMax() > 0) {
                        LObj = (TDR*)(p_ivl->getCurr()->t.data_min);
                        break;
                    }
                    p_ivl->goNext();
                }
                INFOSTR("Object %s detected between surface and light\n", LObj->o->getOName());
            }
            if (LObj != NULL && ((LObj->tmin > 0 && LObj->tmin < 1) || (LObj->tmax > 0 && LObj->tmax < 1)))
                break;
        }
        o.Down();
    }

    if (p_ivl == &ivloc) {
        iv_union(&ivlg, p_ivl);
        p_ivl = &ivlg;
    } else if (p_ivl == NULL) {
        p_ivl = &ivlg;
    }

    if (p_ivl->isFirst()) {
        if (ray_debug) {
            p_ivl->goFirst();
            INFOSTR("InterSect: Intervals:\n");
            while (p_ivl->isLink()) {
                INFOSTR("tmin=%f, tmax=%f ----> tmin_obj=%s, tmax_obj=%s\n",
                        p_ivl->get_tMin(), p_ivl->get_tMax(),
                        ((TDR*)(p_ivl->getCurr()->t.data_min))->o->getOName(),
                        ((TDR*)(p_ivl->getCurr()->t.data_max))->o->getOName());
                p_ivl->goNext();
            }
        }
        p_ivl->goFirst();
        ci = NULL;
        while (p_ivl->isLink()) {
            if (p_ivl->get_tMin() > 0 || p_ivl->get_tMax() > 0) {
                ci = (TDR*)(p_ivl->getCurr()->t.data_min);
                break;
            }
            p_ivl->goNext();
        }
    } else
        ci = NULL;

    if (ci != NULL) {
        co = ci->o;
        switch (co->oper) {
        case O_MINUS:
            ci->nnmi = ci->nnma;
            ci->tmin = ci->tmax;
            break;
        default:
            ;
        }
    }

    ivlg.clear();
    ivl.clear();
    ivloc.clear();

    if (ray_debug) {
        INFOSTR("InterSect result: --------------\n");
        if (ci == NULL)
            INFOSTR("InterSect: V is %d, L is %d, ci = NULL\n", isV, isLightRay);
        else
            INFOSTR("InterSect: V is %d, L is %d, tmin=%f, tmax=%f P[%d,%d], nnmi=%d, nnma=%d, object=%s\n",
                    isV, isLightRay, ci->tmin, ci->tmax, ri.x, ri.y, ci->nnmi, ci->nnma, ci->o->getOName());
    }

    return ci;
}

int Rend :: Render (void *pPtr, TG3D* g, TDTree &o, PLList &l, TColor *pix, RECT r, RendOptions opts)
{
    POINT ri;
    INT pIy = 0, pIx;

    TColor rcol;
    THalfLine rRay = {NULL, NULL};
    TDR *RObj;

    TDRList ol;
    TDLigthList ll;
    IvtArr ia;

    VECTOR vy, V, V1, V2, V3;
    REAL xx, yy;

    ivArrayInit(&ia);

    #pragma omp critical(copyRObjects)
    {

        l.Top();
        while (l.isValid()) {
            ll.appendNew();
            VAssign(ll.getItem()->pl, l.GetItem()->pl);
            VAssign(ll.getItem()->i, l.GetItem()->i);
            ll.getItem()->s = l.GetItem()->s;
            l.Down();
        }

        o.Top();
        while (o.isValid()) {
            TDBase *ci = ADPI(TDBase, o);
            if (ci->isVisible()) {
                switch (ci->getOtype()) {
                case TDSURF:
                    break;
                case TDLIGHT:
                    ll.appendCopy((TDLight*)ci);
                    break;
                case SPHERE:
                    ol.appendNew(new RSphere((Sphere*)ci));
                    break;
                case CYLINDER:
                    ol.appendNew(new RCylinder((Cylinder*)ci));
                    break;
                case TOROID:
                    ol.appendNew(new RToroid((Toroid*)ci));
                    break;
                case CONE:
                    ol.appendNew(new RCone((Cone*)ci));
                    break;
                case CUBE:
                    ol.appendNew(new RCube((Cube*)ci));
                    break;
                case HALFSPACE:
                    ol.appendNew(new RHalfSpace((HalfSpace*)ci));
                    break;
                default:
                    ; // ol.appendNew((TDR*)ci);
                }
            } else
                o.setWalkDirectionUp();
            o.Walk();
        }

        options = opts;
    }

    Vector(V1, g->t.IM[0][0], g->t.IM[0][1], g->t.IM[0][2]);
    Vector(V2, g->t.IM[1][0], g->t.IM[1][1], g->t.IM[1][2]);
    Vector(V3, g->t.IM[2][0], g->t.IM[2][1], g->t.IM[2][2]);
    VSMultV(V1, -g->cone_height);

    tst1 = 0;
    ia.maxglobIvArray = 0;

    TRendProgress rp;
    INT rWidth, rHeight;

    rWidth = r.right - r.left + 1;
    rHeight = r.bottom - r.top + 1;

    if (opts.pBar) {
        // A little "progress bar"
        rp.r = r;
        rp.pPtr = pPtr;
        rp.pbWidth = rWidth / 8;
        rp.pbHeight = 15 * rHeight / 32;
        rp.pbR.left = r.left + 3 * rp.pbWidth;
        rp.pbR.top = r.top + rp.pbHeight;
        rp.pbR.right = r.right - 3 * rp.pbWidth;
        rp.pbR.bottom = r.bottom - rp.pbHeight;
        rp.pbWidth = rp.pbR.right - rp.pbR.left;
        rp.pbLength = 0;
        opts.pBarFct(&rp);
        shrinkRectBy1(rp.pbR);
        shrinkRectBy1(rp.pbR);
    }

    for (ri.y = r.top; ri.y <= r.bottom; ri.y += opts.stepy) {
        yy = (g->height - ri.y) * g->xyasp;
        VAddVMult(vy, V1, yy, V3);

        // A little "progress bar"
        if (opts.pBar) {
            while ((ri.y - r.top) * 100 / rHeight > (rp.pbLength * 100 / rp.pbWidth)) {
                opts.pBarFct(&rp);
                rp.pbLength += opts.stepy;
            }
        }
        if (opts.stopFct != NULL && opts.stopFct(pPtr, opts.renderId))
            return 1;

        pIx = pIy + (ri.y & opts.rmask);
        pIy += rWidth * opts.stepy;

        for (ri.x = r.left + (ri.y & opts.rmask); ri.x <= r.right; ri.x += opts.stepx) {

            // V - vektor pohladu pre dany bod na obrazovke
            xx = ri.x - g->width;
            VAddVMult(V, vy, xx, V2);

            // for debug purposes....
            if (((r.top == r.bottom) && (r.left + (r.top & opts.rmask) == r.right))) { // || ((ri.x==129) && (ri.y==439)))
                ray_debug = true;
            } else
                ray_debug = false;

#undef VTEST
#ifdef VTEST
            // check if V is ok ....

            VAdd(V, g->observer);
            POINT p;
            g->ProjPoint(&V, &p);
            //POINT p1;
            //g->ProjLine(&g->observer,&V,&p,&p1);
            g->DPixel(p, 0xFFFFFFFF);
            g->MPixel(p, 0xFFFFFFFF);
            VSubV(V, g->observer);
#endif
            rRay.P = &g->observer;
            rRay.V = &V;
            RObj = Intersection(rRay, &ia, ri, ol, true, false);

#if 0
            INT i = 0;
            while (i < MAX_PARTS - 1) {
                if (globIvArray[i].nextfree == 0) {
                    INFOSTR("Render: unreleased Iv, idx=%d\n", i);
                    rcol = 23;
                }
                i++;
            }
#endif
            if (RObj != NULL) { // Vypocitaj farbu }
                //ll.ACurr(NULL);
                rcol = Light(RObj, &ia, ri, g, V, ll, ol);
                if (ray_debug) {
                    INFOSTR("Render: rcol = %x\n", rcol);
                }

#ifndef VTEST
                pix[pIx] = rcol;
                if (opts.rThreads == 0 && opts.drawEveryPixel) {
                    g->DPixel(ri, rcol);
                    g->MPixel(ri, rcol);
                }
#endif
            } else {
#ifndef VTEST
                pix[pIx] = opts.bgColor;
                if (opts.rThreads && opts.drawEveryPixel) {
                    g->DPixel(ri, opts.bgColor);
                    g->MPixel(ri, opts.bgColor);
                }
#endif
            }
            pIx += opts.stepx;
        } // for ri.x
    } // for ri.y

    // INFOSTR("Render: Tst1=%d, currglobIvArray=%d, maxglobIvArray=%d\n", tst1, ia.currglobIvArray, ia.maxglobIvArray);
    
    return 0;
} // Rend :: Render
