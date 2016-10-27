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

#ifndef _OBJ3D_H_
#define _OBJ3D_H_

#include "base.h"
#include "list.h"
#include "vector.h"
#include "matrix.h"
#include "tree.h"
#include "gr3D.h"
#include <memory.h>

extern INT tst1;
extern void InitLUT();

class TDBase;
class TDR;
class PLight;
class TDSurf;
class TDLight;

typedef STree::Object TDObject;
typedef STree::Tree TDTree;
typedef STree::Leaf TDTreeLeafItem;
typedef STree::Branch TDTreeBranchItem;
typedef List2w <PLight> PLList;
typedef List1w <TDBase> TDList;
typedef List1w <TDR> TDRList;
typedef List1w <TDLight> TDLigthList;

#define F_VISIBLE 1
#define F_SURFACE 2
#define F_LIGHT   4
#define F_SPECIAL_BRANCH 6
#define F_WRONG_SURFACE  8
#define F_SPARE2  16
#define F_SPARE3  32
#define F_SPARE4  64
#define F_MATRIX  128

#define TDGROUP 10
#define TDBASE 20
#define TDSURF 30
#define TDLIGHT 40
#define TD_FIRST 100
#define SPHERE 110
#define CYLINDER 120
#define CONE 130
#define CUBE 140
#define HALFSPACE 150
#define TOROID 160
#define TD_LAST 200
#define NumOfPrim 7
#define LIGHT_FIRST 300
#define PLIGHT 310
#define LIGHT_LAST 400
#define NumOfPrimAndLights (NumOfPrim+1)
#define OBJ_LAST 65535
#define commonTDParamDecl TBoolOper o, TDSurf* s, MATRIX m
#define commonTDParams o,s,m


struct TBrd2D {
    REAL xymin, sl1, sl2, h1, h2;
};

/* old code ... */
class PLight
{
protected:
    INT otype;
public:
    virtual ~PLight()
    { }
    PLight(VECTOR c_pl, VECTOR c_i, REAL c_s)
    {
        VAssign(pl, c_pl)
        VAssign(i, c_i)
        s = c_s;
        otype = PLIGHT;
    }
    VECTOR pl, i;
    REAL s;
    POINT plo;
    INT OType()
    {
        return otype;
    }
    void Done()
    {
        delete this;
    }
    virtual const char* getOName()
    {
        return "Point light";
    }
};
/*... old code */

class TDGroup : public TDObject
{
private:
protected:
    TDObject *name;
    INT   otype;

    void cleanUp()
    {
        name = NULL;
        oper = O_PLUS;
        otype = TDGROUP;
        flags = F_VISIBLE;
    }
    void exportDynamicData(TDGroup *o)
    {
        exportName(o);
    }
public:
    TBoolOper oper;
    TFlags flags;
    TDSurf *surface;

    TDGroup()
    {
        cleanUp();
    }
    virtual ~TDGroup()
    {
        delete name;
        name = NULL;
    }
    TDGroup(TDObject *oname, TBoolOper op = O_PLUS, TFlags f = F_VISIBLE)
    {
        cleanUp();
        name = (TDObject*)oname;
        oper = op;
        flags = f;
    }

    TDGroup(TDGroup* o)
    {
        *this = *o;
    }
    void importData(TDGroup* o)
    {
        if (o != NULL) {
            if (o->name != NULL)
                this->name = o->name->newCopy();
            else
                this->name = NULL;
            oper = o->oper;
        }
    }
    void exportData(TDGroup* o)
    {
        if (o != NULL) {
            if (name != NULL)
                o->setName(name->newCopy());
            else
                o->setName(NULL);
            o->oper = oper;
        }
    }
    void exportName(TDGroup* o)
    {
        if (name != NULL && o != NULL)
            o->name = name->newCopy();
    }
    virtual TDObject* newCopy()
    {
        TDObject *o = new TDGroup(this);
        exportDynamicData((TDGroup*)(o));
        return o;
    }
    bool hasMatrix()
    {
        return (flags & F_MATRIX) != 0;
    }

    INT getOtype()
    {
        return otype;
    }
    virtual const char* getOName()
    {
        return "Group";
    }
    void *getName()
    {
        if (name == NULL)
            return NULL;
        else
            return name->getName();
    }
    void setName(TDObject *name)
    {
        delete this->name;
        this->name = name;
    }
    void deleteName()
    {
        if (name != NULL)
            name->deleteName();
        name = NULL;
    }
    char getOperAsChar()
    {
        switch (oper) {
        case O_PLUS:
            return '+';
        case O_MINUS:
            return '-';
        case O_LOCALPLUS:
            return '#';
        case O_INTERSECTION:
            return '*';
        default:
            return '\0';
        }
    }
    bool isVisible()
    {
        return flags & F_VISIBLE;
    }
};

class TDBase : public TDGroup
{
private:
protected:
    void cleanUp()
    {
        TDGroup::cleanUp();
        M1(m);
        otype = TDBASE;
        surface = NULL;
        flags |= F_MATRIX;
    }

public:
    MATRIX m;
    MATRIX  mi;             // inverse matrix
    RECT br;

    TDBase()
    {
        cleanUp();
    }
    TDBase(TDObject *oname)
    {
        cleanUp();
        name = (TDObject*)oname;
    }
    TDBase(TDGroup *o)
    {
        cleanUp();
        importData(o);
    }
    TDBase(TDObject *oname, MATRIX m, TBoolOper op = O_PLUS, TFlags f = F_VISIBLE)
    {
        cleanUp();
        name = (TDObject*)oname;
        oper = op;
        flags = f;
        memcpy(this->m, m, sizeof(MATRIX));
    }

    TDBase(commonTDParamDecl)
    {
        cleanUp();
        oper = o;
        surface = s;
        memcpy(this->m, m, sizeof(this->m));
    }

    TDBase(TDBase* o)
    {
        *this = *o;
    }

    void Border2D(TG3D* g, TBrd2D &b, MATRIX mb, bool drawBorders = false);

    virtual TDObject* newCopy()
    {
        TDObject *o = new TDBase(this);
        exportDynamicData((TDGroup*)(o));
        return o;
    }
    const char* getOName()
    {
        return "Base";
    }

    /*virtual bool hasMatrix()
    {
        return true;
    }*/

    virtual void Wf(TG3D* g)
    { }

    virtual void WfAcc(TG3D* g)
    { }

    virtual void Precalc(TG3D* g, MATRIX mb) { }

    friend class Rend;

    virtual void Done()
    {
        /*delete this;*/
    }
    virtual ~TDBase()
    {
        //name = NULL;
        // empty
    }
};

class Sphere : public TDBase
{
public:
    Sphere()
    {
        cleanUp();
        otype = SPHERE;
    }
    virtual ~Sphere()
    {}
    Sphere(Sphere* o)
    {
        *this = *o;
    }
    virtual TDObject* newCopy()
    {
        TDObject *o = new Sphere(this);
        exportDynamicData((TDGroup*)(o));
        return o;
    }
    const char* getOName()
    {
        return "Sphere";
    }

    Sphere(commonTDParamDecl) : TDBase(commonTDParams)
    {
        otype = SPHERE;
    };
    void Precalc(TG3D* g, MATRIX mb);
    void Wf(TG3D* g);
    void WfAcc(TG3D* g);
};

class Cylinder : public TDBase
{
public:
    Cylinder()
    {
        cleanUp();
        otype = CYLINDER;
    }
    virtual ~Cylinder()
    {}
    Cylinder(Cylinder* o)
    {
        *this = *o;
    }
    virtual TDObject* newCopy()
    {
        TDObject *o = new Cylinder(this);
        exportDynamicData((TDGroup*)(o));
        return o;
    }
    const char* getOName()
    {
        return "Cylinder";
    }
    Cylinder(commonTDParamDecl) : TDBase(commonTDParams)
    {
        otype = CYLINDER;
    };
    void Precalc(TG3D* g, MATRIX mb);
    void Wf(TG3D* g);
    void WfAcc(TG3D* g);
};

class Toroid : public TDBase
{
protected:
    REAL R,r;
    friend class RToroid;

public:
    Toroid()
    {
        cleanUp();
        otype = TOROID;
        R = 2;
        r = 1;
    }
    virtual ~Toroid()
    {}
    Toroid(Toroid* o)
    {
        *this = *o;
    }
    virtual TDObject* newCopy()
    {
        TDObject *o = new Toroid(this);
        exportDynamicData((TDGroup*)(o));
        return o;
    }
    const char* getOName()
    {
        return "Toroid";
    }
    Toroid(commonTDParamDecl) : TDBase(commonTDParams)
    {
        otype = TOROID;
        R = 2;
        r = 1;
    };
    void Precalc(TG3D* g, MATRIX mb);
    void Wf(TG3D* g);
    void WfAcc(TG3D* g);
};

class Cone : public TDBase
{
public:
    Cone()
    {
        cleanUp();
        otype = CONE;
    }
    virtual ~Cone()
    {}
    Cone(Cone* o)
    {
        *this = *o;
    }
    virtual TDObject* newCopy()
    {
        TDObject *o = new Cone(this);
        exportDynamicData((TDGroup*)(o));
        return o;
    }
    const char* getOName()
    {
        return "Cone";
    }
    Cone(commonTDParamDecl) : TDBase(commonTDParams)
    {
        otype = CONE;
    };
    void Precalc(TG3D* g, MATRIX mb);
    void Wf(TG3D* g);
    void WfAcc(TG3D* g);
};

class Cube : public TDBase
{
public:
    Cube()
    {
        cleanUp();
        otype = CUBE;
    }
    virtual ~Cube()
    {}
    Cube(Cube* o)
    {
        *this = *o;
    }
    virtual TDObject* newCopy()
    {
        TDObject *o = new Cube(this);
        exportDynamicData((TDGroup*)(o));
        return o;
    }
    const char* getOName()
    {
        return "Cube";
    }
    Cube(commonTDParamDecl) : TDBase(commonTDParams)
    {
        otype = CUBE;
    };
    void Precalc(TG3D* g, MATRIX mb);
    void Wf(TG3D* g);
    void WfAcc(TG3D* g);
};

class HalfSpace : public TDBase
{
public:
    HalfSpace()
    {
        cleanUp();
        otype = HALFSPACE;
    }
    virtual ~HalfSpace()
    {}
    HalfSpace(HalfSpace* o)
    {
        *this = *o;
    }
    virtual TDObject* newCopy()
    {
        TDObject *o = new HalfSpace(this);
        exportDynamicData((TDGroup*)(o));
        return o;
    }
    const char* getOName()
    {
        return "Halfspace";
    }
    HalfSpace(commonTDParamDecl) : TDBase(commonTDParams)
    {
        otype = HALFSPACE;
    };
    void Precalc(TG3D* g, MATRIX mb);
    void Wf(TG3D* g);
    void WfAcc(TG3D* g);
};

class TDR // : public TDBase
{
private:
protected:
    TDBase *o;

    VECTOR   p, u, n;  // p,u,n - pozorovatel, vektor pohladu, normalovy vektor v zakl. polohe
    REAL     tmin, tmax;
    CHAR     nnmi, nnma;

    void cleanUp();
public:

    TDR()
    {
        // cleanUp();
    }

    virtual ~TDR()
    {
    }

    TDR(TDR* o)
    {
        *this = *o;
    }

    virtual bool isPoint()
    {
        return true;
    }
    virtual void Point() { }
    virtual void Nvect() { }
    friend class Rend;
    virtual void Done() { }
};

class TDR2 : public TDR
{
protected:
    REAL tmin2, tmax2;
};

class RSphere : public TDR
{
public:
    RSphere(Sphere *s) { o = s; }
    virtual ~RSphere() {}
    bool isPoint();
    void Point();
    void Nvect();
};

class RCylinder : public TDR
{
public:
    RCylinder(Cylinder *c) { o = c; }
    virtual ~RCylinder() {}
    bool isPoint();
    void Point();
    void Nvect();
};

class RToroid : public TDR
{
public:
    RToroid(Toroid *c) { o = c; }
    virtual ~RToroid() {}
    bool isPoint();
    void Point();
    void Nvect();
};

class RCone : public TDR
{
public:
    RCone(Cone *c) { o = c; }
    virtual ~RCone() {}
    bool isPoint();
    void Point();
    void Nvect();
};

class RCube : public TDR
{
public:
    RCube(Cube *c) { o = c; }
    virtual ~RCube() {}
    bool isPoint();
    void Point();
    void Nvect();
};

class RHalfSpace : public TDR
{
public:
    RHalfSpace(HalfSpace *h) { o = h; }
    virtual ~RHalfSpace() {}
    bool isPoint();
    void Point();
    void Nvect();
};

/*
typedef void(*TRANSFORMVOID)(MATRIX);

class TD_basic
{
public:
    TRANSFORMVOID t;
};

class TD_1i : public TD_basic
{
protected:
    INT p1;
public:
    TD_1i(INT pp1)
    {
        p1=pp1;
    }
};

class TD_1 : public TD_basic
{
protected:
    REAL p1;
public:
    TD_1(REAL pp1)
    {
        p1=pp1;
    }
    void TSCALE(MATRIX m)
    {
        m[0][0]*=p1;
        m[1][1]*=p1;
        m[2][2]*=p1;
    }
};

class TD_3 : public TD_1
{
protected:
    REAL p2, p3;
public:
    TD_3(REAL pp1, REAL pp2, REAL pp3) : TD_1(pp1)
    {
        p2=pp2;
        p3=pp3;
    }
    void TTRANSLAT(MATRIX m)
    {
        m[3][0]+=p1;
        m[3][1]+=p2;
        m[3][2]+=p3;
    }
    void TSCALE(MATRIX m)
    {
        m[0][0]*=p1;
        m[1][1]*=p2;
        m[2][2]*=p3;
    }
};


class TD_4 : public TD_3
{
protected:
    REAL p4;
public:
    TD_4(REAL pp1, REAL pp2, REAL pp3, REAL pp4) : TD_3(pp1,pp2,pp3)
    {
        p4=pp4;
    }
    void TSCALE(MATRIX m)
    {
        TD_3::TSCALE(m);
        m[3][0]*=p4;
        m[3][1]*=p4;
        m[3][2]*=p4;
    }
};

class TD_6 : public TD_4
{
protected:
    REAL p5, p6;
public:
    TD_6(REAL pp1, REAL pp2, REAL pp3, REAL pp4, REAL pp5, REAL pp6) : TD_4(pp1,pp2,pp3,pp4)
    {
        p5=pp5;
        p6=pp6;
    }
    void TTRANSLAT(MATRIX m)
    {
        m[3][0]+=p4;
        m[3][1]+=p5;
        m[3][2]+=p6;
    }
    void TSCALE_TRANSLAT(MATRIX m)
    {
        TD_3::TSCALE(m);
        TD_6::TTRANSLAT(m);
    }
};
*/


#endif // _OBJ3D_H_
