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

#ifndef _FXSCENE_H_
#define _FXSCENE_H_

#include <signal.h>

#include "3D/rend.h"
#include "dialogs.h"
#include "options.h"

#define FXptr FXuval

#if FOX_BIGENDIAN
#define XSC_BIGENDIAN true
#else
#define XSC_BIGENDIAN false
#endif

extern const char* app_name;

typedef struct app_version_t {
    FXchar major;
    FXchar minor;
    FXchar bugfix;
    bool isGE(char maj, char min, char bfx)
    {
        return ((major>maj) ||
                ((major==maj)&&
                 ((minor>min) ||
                  ((minor==min)&&
                   (bugfix>=bfx)))));
    }
    bool isG(char maj, char min, char bfx)
    {
        return ((major>maj) ||
                ((major==maj)&&
                 ((minor>min) ||
                  ((minor==min)&&
                   (bugfix>bfx)))));
    }
}
app_version_t;

extern const app_version_t app_version;

class MYFXName : public TDObject
{
private:
    FXString name;
public:
    MYFXName(const FXchar *s)
    {
        name=FXString(s);
    }
    MYFXName(FXString s)
    {
        name=s;
    }
    MYFXName(FXString *s)
    {
        name=*s;
    }
    MYFXName(MYFXName *o)
    {
        *this=*o;
    }
    virtual MYFXName* newCopy()
    {
        MYFXName *o = new MYFXName(this);
        o->name = FXString(this->name);
        return o;
    }
    virtual void deleteName()
    {
        name.length(0);
    }
    virtual void* getName()
    {
        if (name.empty())
            return NULL;
        else
            return (void*)&name;
    }
    virtual void setName(void *s)
    {
        name.length(0);
        if (s!=NULL)
            name.assign(*(FXString*)s);
    }
    virtual ~MYFXName()
    {
        name.length(0);
    }
};

class TG: public TG3D
{
public:
    FXDCWindow *dc, *mdc;
    TG(FXDCWindow *dcp, FXDCWindow *mdcp)
    {
        dc=dcp;
        mdc=mdcp;
    }
    TG(TG *from)
    {
        *this = *from;
    }
    void UpdateDC(FXDCWindow *dcp)
    {
        dc=dcp;
    }
    void UpdateMDC(FXDCWindow *mdcp)
    {
        mdc=mdcp;
    }
    FXDCWindow* getMDC()
    {
        return mdc;
    }

    void MSetColor(TColor color)
    {
        mdc->setForeground(color);
    }

    void DSetColor(TColor color)
    {
        dc->setForeground(color);
    }

    void MPixel(POINT p, TColor color)
    {
        mdc->setForeground(color);
        //mdc->drawLine(p.x, p.y, p.x+1, p.y);
        mdc->drawPoint(p.x, p.y);
    }
    void DPixel(POINT p, TColor color)
    {
        dc->setForeground(color);
        //dc->drawLine(p.x, p.y, p.x+1, p.y);
        // drawPoint is much faster than drawLine, though it ignores line-width...
        dc->drawPoint(p.x, p.y);
    }
    void MLineTo(POINT p)
    {
        mdc->drawLine(mp.x, mp.y, p.x, p.y);
        mp = p;
    }
    void MLine(POINT p1, POINT p2)
    {
        mdc->drawLine(p1.x, p1.y, p2.x, p2.y);
    }
    void DLine(POINT p1, POINT p2)
    {
        dc->drawLine(p1.x, p1.y, p2.x, p2.y);
    }
    POINT MGetPosition(TG3D *g)
    {
        return mp;
    }
    void MMoveTo(POINT p)
    {
        mp = p;
    }
    void MRect(RECT r)
    {
        mdc->drawRectangle(r.left, r.top, r.right-r.left, r.bottom-r.top);
    }
    void DRect(RECT r)
    {
        dc->drawRectangle(r.left, r.top, r.right-r.left, r.bottom-r.top);
    }

};

TColor VToC(VECTOR v)
{
#if XSC_BIGENDIAN==true
    return FXRGBA(0,255*v[Z],255*v[Y],255*v[X]); // at least for older FOX versions
#else
    return FXRGBA(255*v[X],255*v[Y],255*v[Z],0);
#endif
}

class FXMyDataTarget : public FXDataTarget
{
    FXDECLARE(FXMyDataTarget)
private:
    bool action;
public:
    FXMyDataTarget()
    {
        action = false;
    }
    bool isAction()
    {
        return action;
    }
    void HandleAction()
    {
        action = false;
    }
    void startAction()
    {
        action = true;
    }
    long onCmdValue(FXObject* ,FXSelector ,void*);
    long onCmdOption(FXObject* ,FXSelector ,void*);
};

/*
class FXMyTreeList : public FXTreeList
{
    //FXDECLARE(FXMyTreeList)
    public:
    FXMyTreeList(FXComposite *p,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):
       FXTreeList(p,tgt,sel,opts,x,y,w,h) {}
    void dropFinished(FXDragAction action);
};
*/

//static FXApp *my_app;
//static FXbool my_rendering;

// Job runner
class Runner : public FXThread
{
protected:
    void *parent;
    Rend *render;
    TG3D *g;
    TDTree *o;
    PLList *l;
    RECT r;
    RendOptions ro;
public:
    Runner()
    {
    }

    void update(void *parent, Rend *render, TG3D *g, TDTree *o, PLList *l, RECT r, RendOptions ro)
    {
        this->parent = parent;
        this->render = render;
        this->g = g;
        this->o = o;
        this->l = l;
        this->r = r;
        this->ro = ro;
    }

    virtual FXint run();
};

class TXSCWindow : public FXMainWindow
{
    FXDECLARE(TXSCWindow)
private:
    TXSCWindow()
    {}

    // GUI objects
    //My data targets (per 3D object)
    FXMyDataTarget       dt_m[MATRIX_ROWS][MATRIX_COLUMNS];
    FXTextField          *fx_m[MATRIX_ROWS][MATRIX_COLUMNS];
    FXMyDataTarget       dt_oname;
    FXString             oname;
    FXMyDataTarget       dt_pname;
    FXTextField          *fx_pname;
    FXString             poname;
    FXMyDataTarget       dt_oper;
    FXint                oper;
    FXMyDataTarget       dt_has_matrix;
    FXbool               has_matrix;
    FXMyDataTarget       dt_visible;
    FXbool               visible;

    // Normal data targets (global values)
    FXDataTarget         dt_rotation_angle;
    FXint                rotation_angle;
    FXDataTarget         dt_rot_ignore_transf;
    FXbool               rot_ignore_transf;
    FXDataTarget         dt_translation_step[VECTOR_SPACE];
    FXdouble             translation_step[VECTOR_SPACE];
    FXDataTarget         dt_size_aspect[VECTOR_SPACE];
    FXdouble             size_aspect[VECTOR_SPACE];
    FXDataTarget         dt_copy_on_transf;
    FXbool               copy_on_transf;

    // Surface
    FXDataTarget         dt_Ia[VECTOR_SPACE];
    FXDataTarget         dt_kd[VECTOR_SPACE];
    FXDataTarget         dt_ks[VECTOR_SPACE];
    FXDataTarget         dt_n;
    FXDataTarget         dt_metalic;
    FXTextField          *fx_Ia[VECTOR_SPACE];
    FXTextField          *fx_kd[VECTOR_SPACE];
    FXTextField          *fx_ks[VECTOR_SPACE];
    FXTextField          *fx_n;
    FXTextField          *fx_metalic;

    // Point Light
    FXDataTarget         dt_pl[VECTOR_SPACE];
    FXDataTarget         dt_pl_i[VECTOR_SPACE];
    FXDataTarget         dt_pl_s;
    FXTextField          *fx_pl[VECTOR_SPACE];
    FXTextField          *fx_pl_i[VECTOR_SPACE];
    FXTextField          *fx_pl_s;

    FXHorizontalFrame *contents;
    FXHorizontalFrame *buttons;
    FXTabBook         *tabbook;
    FXTabItem         *tabObjects;
    FXTabItem         *tabSurfaces;
    FXTabItem         *tabLights;
    FXTreeList        *objectlist;
    FXComboBox        *surfbox;
    FXPopup           *objects_popupmenu;
    FXPopup           *newobjectmenu;
    FXTreeItem        *selectedTreeItem;

    // 3D objects
    TDTreeLeafItem    *selectedItem;
    TDBase            *o_selectedItem;
    TDTreeLeafItem    *clippboardBr;

    UINT cameraID;
    RECT clientR;
    //POINT mouse;
    VECTOR obs,poi;
    REAL accelRotation;
    //FXHash stringsHT;
    FXint cyclic_num;

    FXint             pasteOp;

    FXbool            rendering;

    void cleanUp()
    {
        selectedTreeItem=NULL;
        selectedItem=NULL;
        o_selectedItem=NULL;
        delete clippboardBr;
        clippboardBr=NULL;
        rendering=false;
        o.Done();
        l.Done();
    }

    void savePointer(FXFileStream *fs, FXHash *h, void *ptr, FXuval *glob_ind);

protected:
    FXIcon            *bigicon;             // Big application icon
    FXIcon            *smallicon;           // Small application icon

    FXIcon            *object_icon;
    FXIcon            *ic_sphere;
    FXIcon            *ic_cylinder;
    FXIcon            *ic_cube;
    FXIcon            *ic_cone;
    FXIcon            *ic_halfspace;
    FXIcon            *folder_open;
    FXIcon            *folder_closed;
    FXIcon            *folder_openL;
    FXIcon            *folder_closedL;
    FXIcon            *folder_openS;
    FXIcon            *folder_closedS;

    FXStatusBar       *statusbar;           // Status line
    FXTextField       *statusbarCamera;     // Camera info on status bar

    FXMenuBar         *menubar;
    FXMenuPane        *filemenu;
    FXMenuPane        *scenamenu;
    FXMenuPane        *cameramenu;
    FXMenuPane        *objectsmenu;
    FXMenuPane        *helpmenu;
    FXImage           *bitmap;
    FXHorizontalFrame *canvasFrame;
    FXHorizontalFrame *objectsFrame;
    FXGroupBox        *gbox_matrix;
    FXGroupBox        *gbox_boolean;
    FXMatrix          *matrix;
    FXCheckButton     *chkbt_matrix;
    FXCanvas          *canvas;

    FXString          filename;              // File being viewed

    bool              xscOpened;
    bool              drawXsc;
    bool              LButtD;
    bool              RButtD;
    bool              canvasUpdated;
    bool              canvasResized;

    TDTree            o;
    PLList            l;
    Rend              r;
    RendOptions       ro;
    TOptions          options;

    int               mouse_x;
    int               mouse_y;

    //void EvLButtonDown(UINT, POINT) { LButtD=TRUE; }
    //void EvLButtonUp(UINT, POINT) { LButtD=FALSE; }
    //void EvRButtonDown(UINT, POINT) { RButtD=TRUE; }
    //void EvRButtonUp(UINT, POINT) { RButtD=FALSE; }
    //void EvSize(uint sizeType , TSize&  size);
    //void EvPaint();
    //void EvMouseMove(UINT modKeys, POINT point);

    void Net3DXY(TG *g, INT n, REAL step);
    void WireFrame();
    void CmCamera(UINT id);
    void CmOpen(UINT scena);
    FXbool saveBitmap(const FXString& file);
    void formatTreeObjectName(FXString *s, TDTreeLeafItem *current);
    FXTreeItem* appendFXTreeItem(FXTreeItem *branch, TDTreeLeafItem *current);
    void FillTreeList(TDTreeLeafItem *ol);
    void FillSurfaceList();
    long RotateTranslateSizeObjects(int axis, VECTOR t, VECTOR s);
    void getRenderOptions();

public:
    TG* g;

    TXSCWindow(FXApp *app, const char *title);

    virtual void create();
    virtual void detach();
    virtual ~TXSCWindow();

    long onPaintCanvas(FXObject*,FXSelector,void*);
    long onResizeCanvas(FXObject*,FXSelector,void*);
    long onMouseMove(FXObject*,FXSelector,void*);

    //long onMouseLDown(FXObject*,FXSelector,void*);
    //long onMouseLUp(FXObject*,FXSelector,void*);
    //long onKeyDown(FXObject*,FXSelector,void*);
    //long onKeyUp(FXObject*,FXSelector,void*);

    long onCmdFileOpen(FXObject*,FXSelector,void*);
    long onCmdSaveAs(FXObject*,FXSelector,void*);
    long onCmdSaveBitmap(FXObject*,FXSelector,void*);

    long onCmdPanel(FXObject*,FXSelector,void*);
    long onUpdPanel(FXObject*,FXSelector,void*);
    long onCmdObjectSelected(FXObject*,FXSelector,void*);
    long onCmdSurfaceChanged(FXObject*,FXSelector,void*);
    long onCmdExpandObjectTreeItem(FXObject*,FXSelector,void*);
    long onCmdCollapseObjectTreeItem(FXObject*,FXSelector,void*);
    long onCmdTreeItemPopupMenu(FXObject*,FXSelector,void*);

    long onCmdNewBranch(FXObject*,FXSelector,void*);
    long onCmdNewSphere(FXObject*,FXSelector,void*);
    long onCmdNewCylinder(FXObject*,FXSelector,void*);
    long onCmdNewToroid(FXObject*,FXSelector,void*);
    long onCmdNewCone(FXObject*,FXSelector,void*);
    long onCmdNewCube(FXObject*,FXSelector,void*);
    long onCmdNewHalfspace(FXObject*,FXSelector,void*);
    long onCmdNewSurface(FXObject*,FXSelector,void*);
    long onCmdNewLight(FXObject*,FXSelector,void*);

    long onCmdTreeItemCut(FXObject*,FXSelector,void*);
    long onCmdTreeItemCopy(FXObject*,FXSelector,void*);
    long onCmdTreeItemDelete(FXObject*,FXSelector,void*);
    long onCmdTreeItemPaste(FXObject*,FXSelector,void*);
    long onCmdTreeItemPasteBefore(FXObject*,FXSelector,void*);
    long onUpdTreeItemPasteBefore(FXObject*,FXSelector,void*);
    long onCmdTreeItemPasteAfter(FXObject*,FXSelector,void*);
    long onUpdTreeItemPasteAfter(FXObject*,FXSelector,void*);
    long onCmdTreeItemPasteUnder(FXObject*,FXSelector,void*);
    long onUpdTreeItemPasteUnder(FXObject*,FXSelector,void*);
    long onUpdTreeItemCut(FXObject*,FXSelector,void*);
    long onUpdTreeItemCopy(FXObject*,FXSelector,void*);
    long onUpdTreeItemDelete(FXObject*,FXSelector,void*);

    long onUpdObjectToggleVisible(FXObject*,FXSelector,void*);
    long onCmObjectToggleVisible(FXObject*,FXSelector,void*);
    long onCmObjectsMakeVisible(FXObject*,FXSelector,void*);
    long onCmObjectsMakeUnvisible(FXObject*,FXSelector,void*);
    long onCmObjectsMoveVisible(FXObject*,FXSelector,void*);

    long onUpdRender(FXObject*,FXSelector,void*);
    long onUpdStopRender(FXObject*,FXSelector,void*);
    long onCmdStopRender(FXObject*,FXSelector,void*);
    long onMcRenderProgress(FXObject*, FXSelector,void*);
    long onMcRenderDone(FXObject*, FXSelector,void*);
    long onCmRender(FXObject*,FXSelector,void*);
    long onCmPRender(FXObject*,FXSelector,void*);

    long onCmCameraReset(FXObject*,FXSelector,void*);
    long onCmCameraNastav(FXObject*,FXSelector,void*);
    long onCmKamera(FXObject*,FXSelector,void*);
    long onUpdKamera(FXObject*,FXSelector,void*);
    long onKameraStatusBar(FXObject*,FXSelector,void*);

    long onCmdAbout(FXObject*,FXSelector,void*);

    long onOptions(FXObject*,FXSelector,void*);
    long onSaveOptions(FXObject*,FXSelector,void*);
    long onCmOpenDemo(FXObject*,FXSelector,void*);
    long onCloseAll(FXObject*,FXSelector,void*);

    long onCmdReverseTransforms(FXObject*,FXSelector,void*);
    long onUpdRotate(FXObject*,FXSelector,void*);
    long onCmdRotateX(FXObject*,FXSelector,void*);
    long onCmdRotateY(FXObject*,FXSelector,void*);
    long onCmdRotateZ(FXObject*,FXSelector,void*);
    long onCmdRotateTranslateX(FXObject*,FXSelector,void*);
    long onCmdRotateTranslateY(FXObject*,FXSelector,void*);
    long onCmdRotateTranslateZ(FXObject*,FXSelector,void*);
    long onCmdRotateTranslateSizeX(FXObject*,FXSelector,void*);
    long onCmdRotateTranslateSizeY(FXObject*,FXSelector,void*);
    long onCmdRotateTranslateSizeZ(FXObject*,FXSelector,void*);
    long onDnDRequest(FXObject*,FXSelector,void*);
    long onSomeChange(FXObject*,FXSelector,void*);

    FXThreadPool *rPool;
    bool rPoolStarted;
    Runner *pRunner;
    FXMessageChannel *mC;
    void CmRender(RECT r);
    bool isRendering()
    {
        return rendering;
    }
    long getRenderId()
    {
        return ro.renderId;
    }
    FXint getRThreads()
    {
        return ro.rThreads;
    }

    enum {
        ID_SHAPES=FXMainWindow::ID_LAST,

        ID_FILE_OPEN,
        ID_SAVE_AS,
        ID_SAVE_BITMAP,
        ID_QUIT,

        ID_SCENA,
        ID_CUDO,
        ID_PRSTEN,
        ID_LAMPA,
        ID_GKVK,

        ID_PAINT,

        ID_OPTIONS,
        ID_SAVE_OPTIONS,
        ID_RENDER,
        ID_RENDER_PROGRESS,
        ID_RENDER_FINISHED,
        ID_PRENDER,
        ID_STOPRENDER,

        ID_C_ROTATE,
        ID_C_MOVE,
        ID_C_MOVE_POI,
        ID_C_DISTANCE,
        ID_ROTATE,
        ID_K_NOACTION,

        ID_C_SET,
        ID_C_RESET,

        //ID_OBJECTS,
        ID_OBJECT_VISIBLE,
        ID_OBJECTS_VISIBLE,
        ID_OBJECTS_UNVISIBLE,
        ID_OBJECTS_MOVE_VISIBLE,

        ID_SURFACE,

        ID_ABOUT,

        ID_REVERSE_TRANSFORMS,

        ID_ROTATE_X,
        ID_ROTATE_Y,
        ID_ROTATE_Z,

        ID_ROTATE_TRANSLATE_X,
        ID_ROTATE_TRANSLATE_Y,
        ID_ROTATE_TRANSLATE_Z,

        ID_ROTATE_TRANSLATE_SIZE_X,
        ID_ROTATE_TRANSLATE_SIZE_Y,
        ID_ROTATE_TRANSLATE_SIZE_Z,

        ID_PANEL,
        ID_OTREE_LIST,
        ID_OTREE_NEW_BRANCH,
        ID_OTREE_NEW_SPHERE,
        ID_OTREE_NEW_CYLINDER,
        ID_OTREE_NEW_TOROID,
        ID_OTREE_NEW_CONE,
        ID_OTREE_NEW_CUBE,
        ID_OTREE_NEW_HALFSPACE,
        ID_OTREE_NEW_SURFACE,
        ID_OTREE_NEW_LIGHT,
        ID_OTREE_CUT,
        ID_OTREE_COPY,
        ID_OTREE_DELETE,
        ID_OTREE_PASTE_B,
        ID_OTREE_PASTE_A,
        ID_OTREE_PASTE_U,

        ID_NONE,
        ID_LAST
    };

};

class TXSCApp : public FXApp
{
    friend class TXSCWindow;
protected:
    FXIcon            *object_icon;
    FXIcon            *folder_open;
    FXIcon            *folder_closed;
    FXIcon            *folder_openL;
    FXIcon            *folder_closedL;
    FXIcon            *folder_openS;
    FXIcon            *folder_closedS;

    virtual ~TXSCApp()
    {
        delete object_icon;
        delete folder_open;
        delete folder_closed;
        delete folder_openL;
        delete folder_closedL;
        delete folder_openS;
        delete folder_closedS;
    }
};


#endif /* FXSCENE_H_ */
