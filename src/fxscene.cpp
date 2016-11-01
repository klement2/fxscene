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

#include "fxscene.h" 
#include "icons.h"
#include "utils.h"

#ifdef HAVE_PNG_H
#include "FXPNGImage.h"
#endif
#ifdef HAVE_JPEG_H
#include "FXJPGImage.h"
#endif
#ifdef HAVE_TIFF_H
#include "FXTIFImage.h"
#endif

const char *magic_header = "KGAPPX3D";
const char magic_char1 = magic_header[0];
const char magic_char2 = magic_header[1];
const char magic_char3 = magic_header[2];
const char magic_char4 = magic_header[3];
const char magic_char5 = magic_header[4];
const char magic_char6 = magic_header[5];
const char magic_char7 = magic_header[6];
const char magic_char8 = magic_header[7];

#define app_name "FXScene"
const app_version_t app_version = {
major:
    0,
minor:
    15,
bugfix:
    2
};

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
TXSCWindow::TXSCWindow(FXApp *app, const char *title) :
    FXMainWindow(app, title, NULL, NULL, DECOR_ALL, 0, 0, 1024, 530)
{
    this->dropEnable();

    // Make some icons
    bigicon = new FXGIFIcon(getApp(), bigxscene);
    smallicon = new FXGIFIcon(getApp(), tinyxscene);

    folder_openL = new FXGIFIcon(getApp(), minifolderopenL); // , FXRGB(192,192,192),IMAGE_KEEP | IMAGE_OWNED);
    folder_openL->create();
    folder_closedL = new FXGIFIcon(getApp(), minifolderclosedL);
    folder_closedL->create();
    folder_openS = new FXGIFIcon(getApp(), minifolderopenS);
    folder_openS->create();
    folder_closedS = new FXGIFIcon(getApp(), minifolderclosedS);
    folder_closedS->create();
    folder_open = new FXGIFIcon(getApp(), minifolderopen);
    folder_open->create();
    folder_closed = new FXGIFIcon(getApp(), minifolderclosed);
    folder_closed->create();
    object_icon = new FXGIFIcon(getApp(), tinyxscene);
    object_icon->create();

    ic_sphere = new FXGIFIcon(getApp(), icn_sphere);
    ic_sphere->create();
    ic_cone = new FXGIFIcon(getApp(), icn_cone);
    ic_cone->create();
    ic_cylinder = new FXGIFIcon(getApp(), icn_cylinder);
    ic_cylinder->create();
    ic_cube = new FXGIFIcon(getApp(), icn_cube);
    ic_cube->create();
    ic_halfspace = new FXGIFIcon(getApp(), icn_halfspace);
    ic_halfspace->create();

    // Application icons
    setIcon(bigicon);
    setMiniIcon(smallicon);

    filename = "new1.xsc";

    options.roptShades = 1;
    options.roptShiftOddLines = 0;
    options.roptStepX = 1;
    options.roptStepY = 1;
    options.roptClearBefore = 0;
    options.roptBgColor = 0;
    options.doptLineStyle = LINE_SOLID;
    options.doptAccurateWireFrane = 0;
    options.doptAutoDrawSelected = 0;
    options.roptPBar = 1;
    options.roptThreads = FXThread::processors();
    options.roptDrawPixel = 0;
    options.roptAutoRender = 0;

    FXFileStream cfgfile;
    if (cfgfile.open(CFG_FILENAME, FXStreamLoad)) {
        cfgfile.setBigEndian(XSC_BIGENDIAN);
        ALL_OPTIONS_OP(cfgfile >> options.)
    }
    getRenderOptions();

    // Status bar
    statusbar = new FXStatusBar(this, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | STATUSBAR_WITH_DRAGCORNER);
    // Show kamera info on status bar
    statusbarCamera = new FXTextField(statusbar, 80, NULL, 0, FRAME_SUNKEN | JUSTIFY_RIGHT | LAYOUT_RIGHT | LAYOUT_CENTER_Y | TEXTFIELD_READONLY, 0, 0, 0, 0, 2, 2, 1, 1);
    statusbarCamera->setBackColor(statusbar->getBackColor());

    bitmap = new FXImage(getApp());
    // Menubar
    menubar = new FXMenuBar(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X);

    // File Menu
    filemenu = new FXMenuPane(this);
    scenamenu = new FXMenuPane(this);
    new FXMenuCommand(scenamenu, "Cudo", NULL, this, TXSCWindow::ID_CUDO);
    new FXMenuCommand(scenamenu, "Ring", NULL, this, TXSCWindow::ID_PRSTEN);
    new FXMenuCommand(scenamenu, "Lampa", NULL, this, TXSCWindow::ID_LAMPA);
    new FXMenuCommand(scenamenu, "GKVK", NULL, this, TXSCWindow::ID_GKVK);
    new FXMenuCascade(filemenu, "Scene", NULL, scenamenu);
    new FXMenuSeparator(filemenu);
    new FXMenuCommand(filemenu, "Open &File...\tCtl-F", NULL, this, TXSCWindow::ID_FILE_OPEN);
    new FXMenuCommand(filemenu, "Save As...\tCtl-S", NULL, this, TXSCWindow::ID_SAVE_AS);
    new FXMenuCommand(filemenu, "Save bitmap...", NULL, this, TXSCWindow::ID_SAVE_BITMAP);
    new FXMenuSeparator(filemenu);
    new FXMenuCommand(filemenu, "&Options...\tCtl-O", NULL, this, TXSCWindow::ID_OPTIONS);
    new FXMenuCommand(filemenu, "Save Options", NULL, this, TXSCWindow::ID_SAVE_OPTIONS);
    new FXMenuSeparator(filemenu);
    new FXMenuCommand(filemenu, "&Render\tCtl-R", NULL, this, TXSCWindow::ID_RENDER);
    new FXMenuCommand(filemenu, "&PointRender\tCtl-P", NULL, this, TXSCWindow::ID_PRENDER);
    new FXMenuCommand(filemenu, "&Stop Rendering\tEsc", NULL, this, TXSCWindow::ID_STOPRENDER);
    new FXMenuCommand(filemenu, "&Quit\tCtl-Q", NULL, this, TXSCWindow::ID_QUIT); //getApp(), FXApp::ID_QUIT);
    new FXMenuTitle(menubar, "&File", NULL, filemenu);

    // Camera Menu
    cameramenu = new FXMenuPane(this);
    new FXMenuRadio(cameramenu, "Rotate\tR", this, TXSCWindow::ID_C_ROTATE);
    new FXMenuRadio(cameramenu, "Move (all directions)\tM", this, TXSCWindow::ID_C_MOVE);
    new FXMenuRadio(cameramenu, "Move (just in the POI direction)", this, TXSCWindow::ID_C_MOVE_POI);
    new FXMenuRadio(cameramenu, "&Zoom\tZ", this, TXSCWindow::ID_C_DISTANCE);
    new FXMenuRadio(cameramenu, "Rotate (poi vector)", this, TXSCWindow::ID_ROTATE);
    new FXMenuSeparator(cameramenu);
    new FXMenuCommand(cameramenu, "Reset", NULL, this, TXSCWindow::ID_C_RESET);
    new FXMenuCommand(cameramenu, "Set...", NULL, this, TXSCWindow::ID_C_SET);
    new FXMenuTitle(menubar, "&Camera", NULL, cameramenu);

    // Objects Menu
    objectsmenu = new FXMenuPane(this);
    new FXMenuCheck(objectsmenu, "Visible toggle\tInsert", this, TXSCWindow::ID_OBJECT_VISIBLE);
    new FXMenuCommand(objectsmenu, "Make all visible", NULL, this, TXSCWindow::ID_OBJECTS_VISIBLE);
    new FXMenuCommand(objectsmenu, "Make all unvisible", NULL, this, TXSCWindow::ID_OBJECTS_UNVISIBLE);
    new FXMenuCommand(objectsmenu, "Move all visible under current branch", NULL, this, TXSCWindow::ID_OBJECTS_MOVE_VISIBLE);
    new FXMenuTitle(menubar, "&Objects", NULL, objectsmenu);

    // Help Menu
    helpmenu = new FXMenuPane(this);
    new FXMenuCommand(helpmenu, "About", NULL, this, TXSCWindow::ID_ABOUT);
    new FXMenuTitle(menubar, "&Help", NULL, helpmenu);

    FXSplitter *splitter1 = new FXSplitter(this, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y | SPLITTER_REVERSED | SPLITTER_TRACKING);
    canvasFrame = new FXHorizontalFrame(splitter1, FRAME_SUNKEN, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);
    canvas = new FXCanvas(canvasFrame, this, ID_PAINT, FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_FILL_ROW | LAYOUT_FILL_COLUMN);
    objectsFrame = new FXHorizontalFrame(splitter1, FRAME_SUNKEN, LAYOUT_SIDE_TOP | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0, 0, 100, 0, 0, 0, 0, 0);
    objectsFrame->setWidth(getWidth() / 2);

    // Bottom buttons
    //buttons=new FXHorizontalFrame(objectsFrame,LAYOUT_SIDE_BOTTOM|FRAME_NONE|LAYOUT_FILL_X|PACK_UNIFORM_WIDTH,0,0,0,0,40,40,20,20);

    // Separator
    //new FXHorizontalSeparator(objectsFrame,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|SEPARATOR_GROOVE);

    // Contents
    contents = new FXHorizontalFrame(objectsFrame, LAYOUT_SIDE_TOP | FRAME_NONE | LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_WIDTH);

    //tabObjects=new FXVerticalFrame(contents);

    FXSplitter *splitter = new FXSplitter(contents, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y | SPLITTER_REVERSED | SPLITTER_TRACKING);
    FXVerticalFrame *boxframe1 = new FXVerticalFrame(splitter, FRAME_SUNKEN | FRAME_THICK | LAYOUT_FIX_WIDTH | LAYOUT_FILL_Y, 0, 0, 100, 0, 0, 0, 0, 0);
    FXVerticalFrame *boxframe2 = new FXVerticalFrame(splitter, FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);

    objectlist = new FXTreeList(boxframe1, this, ID_OTREE_LIST, FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_TOP | LAYOUT_RIGHT |
                                TREELIST_SHOWS_BOXES | TREELIST_ROOT_BOXES | TREELIST_SHOWS_LINES | TREELIST_BROWSESELECT, 0, 0, 200, 0); //|TREELIST_EXTENDEDSELECT);
    objectlist->dropEnable();

    // common properties (for branches and all the objects)
    FXVerticalFrame *objprop1 = new FXVerticalFrame(boxframe2, FRAME_THICK | FRAME_RAISED);
    FXMatrix *matrix1 = new FXMatrix(objprop1, 2, MATRIX_BY_COLUMNS |/*LAYOUT_SIDE_TOP|*/LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXLabel(matrix1, "Branch Name", NULL, LAYOUT_SIDE_LEFT | FRAME_RAISED | LAYOUT_LEFT | JUSTIFY_LEFT);
    fx_pname = new FXTextField(matrix1, 35, &dt_pname, FXDataTarget::ID_VALUE, FRAME_SUNKEN | FRAME_THICK | LAYOUT_RIGHT);
    new FXLabel(matrix1, "Object Name", NULL, LAYOUT_SIDE_LEFT | FRAME_RAISED | LAYOUT_LEFT | JUSTIFY_LEFT);
    new FXTextField(matrix1, 35, &dt_oname, FXDataTarget::ID_VALUE, FRAME_SUNKEN | FRAME_THICK | LAYOUT_RIGHT);

    //FXHorizontalFrame *opframe2=new FXHorizontalFrame(boxframe2,FRAME_NONE); //FRAME_THICK|FRAME_RAISED);
    new FXCheckButton(matrix1/*opframe2*/, "Visible", &dt_visible, FXDataTarget::ID_VALUE);
    new FXFrame(matrix1, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    // specific properties

    // Switcher
    tabbook = new FXTabBook(boxframe2, this, TXSCWindow::ID_PANEL, PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    tabObjects = new FXTabItem(tabbook, "Object", NULL);

    //FXVerticalFrame *objprop1=new FXVerticalFrame(tabbook,FRAME_THICK|FRAME_RAISED);
    FXScrollWindow *objprop2 = new FXScrollWindow(tabbook); //,HSCROLLING_OFF);
    FXVerticalFrame *objprop = new FXVerticalFrame(objprop2, FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_X | LAYOUT_FILL_Y);

    FXHorizontalFrame *opframe3 = new FXHorizontalFrame(objprop, FRAME_NONE); //FRAME_THICK|FRAME_RAISED);
    new FXLabel(opframe3, "Surface", NULL, LAYOUT_SIDE_LEFT | FRAME_RAISED | LAYOUT_LEFT | JUSTIFY_LEFT);
    surfbox = new FXComboBox(opframe3, 25, this, TXSCWindow::ID_SURFACE, COMBOBOX_INSERT_LAST | FRAME_SUNKEN | FRAME_THICK | ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    surfbox->setNumVisible(5);

    gbox_boolean = new FXGroupBox(objprop, "Boolean", FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    FXHorizontalFrame *opframe = new FXHorizontalFrame(gbox_boolean, FRAME_NONE); //FRAME_THICK|FRAME_RAISED);
    new FXLabel(opframe, "Boolean operation", NULL, LAYOUT_SIDE_LEFT | FRAME_RAISED | LAYOUT_LEFT | JUSTIFY_LEFT);
    new FXRadioButton(opframe, "Add +", &dt_oper, FXDataTarget::ID_OPTION + O_PLUS, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXRadioButton(opframe, "Intersect *", &dt_oper, FXDataTarget::ID_OPTION + O_INTERSECTION, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXRadioButton(opframe, "Subtract -", &dt_oper, FXDataTarget::ID_OPTION + O_MINUS, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);

    FXHorizontalFrame *opframe1 = new FXHorizontalFrame(gbox_boolean, FRAME_NONE); //FRAME_THICK|FRAME_RAISED);
    new FXLabel(opframe1, "Boolean tag", NULL, LAYOUT_SIDE_LEFT | FRAME_RAISED | LAYOUT_LEFT | JUSTIFY_LEFT);
    new FXRadioButton(opframe1, "Local object #", &dt_oper, FXDataTarget::ID_OPTION + O_LOCALPLUS, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);

    /*FXHorizontalFrame *opframe3=new FXHorizontalFrame(boxframe2,FRAME_NONE); //FRAME_THICK|FRAME_RAISED);
    new FXLabel(opframe3,"Surface",NULL,LAYOUT_SIDE_LEFT|FRAME_RAISED|LAYOUT_LEFT|JUSTIFY_LEFT);
    new FXComboBox(opframe1,"Local object #",&dt_oper,FXDataTarget::ID_OPTION+O_LOCALPLUS,ICON_BEFORE_TEXT|LAYOUT_SIDE_LEFT);*/


    chkbt_matrix = new FXCheckButton(objprop, "Transformation matrix", &dt_has_matrix, FXDataTarget::ID_VALUE);

    gbox_matrix = new FXGroupBox(objprop, "Matrix", FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    //FXGroupBox *group1=new FXGroupBox(matrix,"",FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y);

    matrix = new FXMatrix(gbox_matrix, 4, MATRIX_BY_COLUMNS | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y); //|FRAME_RAISED);

    new FXLabel(matrix, "Column[0]", NULL, LAYOUT_CENTER_X | FRAME_RAISED | LAYOUT_CENTER_Y | LAYOUT_LEFT | JUSTIFY_LEFT | LAYOUT_FILL_ROW);
    new FXLabel(matrix, "Column[1]", NULL, LAYOUT_CENTER_X | FRAME_RAISED | LAYOUT_CENTER_Y | LAYOUT_LEFT | JUSTIFY_LEFT | LAYOUT_FILL_ROW);
    new FXLabel(matrix, "Column[2]", NULL, LAYOUT_CENTER_X | FRAME_RAISED | LAYOUT_CENTER_Y | LAYOUT_LEFT | JUSTIFY_LEFT | LAYOUT_FILL_ROW);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    fx_m[0][0] = new FXTextField(matrix, 15, &dt_m[0][0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_m[0][1] = new FXTextField(matrix, 15, &dt_m[0][1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_m[0][2] = new FXTextField(matrix, 15, &dt_m[0][2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    fx_m[1][0] = new FXTextField(matrix, 15, &dt_m[1][0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_m[1][1] = new FXTextField(matrix, 15, &dt_m[1][1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_m[1][2] = new FXTextField(matrix, 15, &dt_m[1][2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    fx_m[2][0] = new FXTextField(matrix, 15, &dt_m[2][0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_m[2][1] = new FXTextField(matrix, 15, &dt_m[2][1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_m[2][2] = new FXTextField(matrix, 15, &dt_m[2][2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    fx_m[3][0] = new FXTextField(matrix, 15, &dt_m[3][0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_m[3][1] = new FXTextField(matrix, 15, &dt_m[3][1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_m[3][2] = new FXTextField(matrix, 15, &dt_m[3][2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXButton(matrix, "R-X", NULL, this, TXSCWindow::ID_ROTATE_X, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXButton(matrix, "R-Y", NULL, this, TXSCWindow::ID_ROTATE_Y, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXButton(matrix, "R-Z", NULL, this, TXSCWindow::ID_ROTATE_Z, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXButton(matrix, "R-X,T", NULL, this, TXSCWindow::ID_ROTATE_TRANSLATE_X, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXButton(matrix, "R-Y,T", NULL, this, TXSCWindow::ID_ROTATE_TRANSLATE_Y, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXButton(matrix, "R-Z,T", NULL, this, TXSCWindow::ID_ROTATE_TRANSLATE_Z, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXButton(matrix, "R-X,TS", NULL, this, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_X, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXButton(matrix, "R-Y,TS", NULL, this, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_Y, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXButton(matrix, "R-Z,TS", NULL, this, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_Z, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    FXMatrix *stepmatrix1 = new FXMatrix(objprop, 5, MATRIX_BY_COLUMNS | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXLabel(stepmatrix1, "Rs", NULL);
    FXSpinner *spin1 = new FXSpinner(stepmatrix1, 4, &dt_rotation_angle, FXDataTarget::ID_VALUE, SPIN_CYCLIC | FRAME_SUNKEN | FRAME_THICK);
    spin1->setRange(-90, 90);
    new FXCheckButton(stepmatrix1, "Rel\\Absolute", &dt_rot_ignore_transf, FXDataTarget::ID_VALUE);
    new FXButton(stepmatrix1, "Reverse all transforms", NULL, this, TXSCWindow::ID_REVERSE_TRANSFORMS, FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);


    FXMatrix *stepmatrix = new FXMatrix(objprop, 6, MATRIX_BY_COLUMNS | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y);

    new FXLabel(stepmatrix, "TXs", NULL);
    new FXTextField(stepmatrix, 10, &dt_translation_step[X], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXLabel(stepmatrix, "TYs", NULL);
    new FXTextField(stepmatrix, 10, &dt_translation_step[Y], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXLabel(stepmatrix, "TZs", NULL);
    new FXTextField(stepmatrix, 10, &dt_translation_step[Z], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);

    new FXLabel(stepmatrix, "SX", NULL);
    new FXTextField(stepmatrix, 10, &dt_size_aspect[X], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXLabel(stepmatrix, "SY", NULL);
    new FXTextField(stepmatrix, 10, &dt_size_aspect[Y], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXLabel(stepmatrix, "SZ", NULL);
    new FXTextField(stepmatrix, 10, &dt_size_aspect[Z], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);


    FXHorizontalFrame *opframe2 = new FXHorizontalFrame(objprop, FRAME_NONE); //FRAME_THICK|FRAME_RAISED);
    new FXCheckButton(opframe2, "Add object copy with each transformation", &dt_copy_on_transf, FXDataTarget::ID_VALUE);

    tabSurfaces = new FXTabItem(tabbook, "Surface", NULL);
    FXHorizontalFrame *hframe4 = new FXHorizontalFrame(tabbook, FRAME_THICK | FRAME_RAISED); //|LAYOUT_FILL_X|LAYOUT_FILL_Y);
    //FXHorizontalFrame *boxframe=new FXHorizontalFrame(hframe4,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);

    FXMatrix *matrix2 = new FXMatrix(hframe4, 4, MATRIX_BY_COLUMNS | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y); //|FRAME_RAISED);

    new FXLabel(matrix2, "Ia", NULL, LAYOUT_SIDE_LEFT);
    fx_Ia[0] = new FXTextField(matrix2, 15, &dt_Ia[0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_Ia[1] = new FXTextField(matrix2, 15, &dt_Ia[1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_Ia[2] = new FXTextField(matrix2, 15, &dt_Ia[2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);

    new FXLabel(matrix2, "kd", NULL, LAYOUT_SIDE_LEFT);
    fx_kd[0] = new FXTextField(matrix2, 15, &dt_kd[0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_kd[1] = new FXTextField(matrix2, 15, &dt_kd[1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_kd[2] = new FXTextField(matrix2, 15, &dt_kd[2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);

    new FXLabel(matrix2, "ks", NULL, LAYOUT_SIDE_LEFT);
    fx_ks[0] = new FXTextField(matrix2, 15, &dt_ks[0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_ks[1] = new FXTextField(matrix2, 15, &dt_ks[1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_ks[2] = new FXTextField(matrix2, 15, &dt_ks[2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);

    new FXLabel(matrix2, "n", NULL, LAYOUT_SIDE_LEFT);
    fx_n = new FXTextField(matrix2, 15, &dt_n, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix2, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);
    new FXFrame(matrix2, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXLabel(matrix2, "metalic", NULL, LAYOUT_SIDE_LEFT);
    fx_metalic = new FXTextField(matrix2, 15, &dt_metalic, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    new FXFrame(matrix2, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);
    new FXFrame(matrix2, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);


    tabLights = new FXTabItem(tabbook, "Light", NULL);
    FXHorizontalFrame *hframe5 = new FXHorizontalFrame(tabbook, FRAME_THICK | FRAME_RAISED); //|LAYOUT_FILL_X|LAYOUT_FILL_Y);
    FXMatrix *matrix3 = new FXMatrix(hframe5, 4, MATRIX_BY_COLUMNS | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y); //|FRAME_RAISED);

    new FXLabel(matrix3, "Position", NULL, LAYOUT_SIDE_LEFT);
    fx_pl[0] = new FXTextField(matrix3, 15, &dt_pl[0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_pl[1] = new FXTextField(matrix3, 15, &dt_pl[1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_pl[2] = new FXTextField(matrix3, 15, &dt_pl[2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);

    new FXLabel(matrix3, "Intensity", NULL, LAYOUT_SIDE_LEFT);
    fx_pl_i[0] = new FXTextField(matrix3, 15, &dt_pl_i[0], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_pl_i[1] = new FXTextField(matrix3, 15, &dt_pl_i[1], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);
    fx_pl_i[2] = new FXTextField(matrix3, 15, &dt_pl_i[2], FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);

    new FXLabel(matrix3, "S", NULL, LAYOUT_SIDE_LEFT);
    fx_pl_s = new FXTextField(matrix3, 15, &dt_pl_s, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_CENTER_X);

    objects_popupmenu = new FXMenuPane(this);
    newobjectmenu = new FXMenuPane(this);
    new FXMenuCommand(newobjectmenu, tr("Branch"), NULL, this, TXSCWindow::ID_OTREE_NEW_BRANCH);
    new FXMenuSeparator(newobjectmenu);
    new FXMenuCommand(newobjectmenu, tr("Sphere"), NULL, this, TXSCWindow::ID_OTREE_NEW_SPHERE);
    new FXMenuCommand(newobjectmenu, tr("Cylinder"), NULL, this, TXSCWindow::ID_OTREE_NEW_CYLINDER);
    new FXMenuCommand(newobjectmenu, tr("Toroid"), NULL, this, TXSCWindow::ID_OTREE_NEW_TOROID);
    new FXMenuCommand(newobjectmenu, tr("Cone"), NULL, this, TXSCWindow::ID_OTREE_NEW_CONE);
    new FXMenuCommand(newobjectmenu, tr("Cube"), NULL, this, TXSCWindow::ID_OTREE_NEW_CUBE);
    new FXMenuCommand(newobjectmenu, tr("HalfSpace"), NULL, this, TXSCWindow::ID_OTREE_NEW_HALFSPACE);
    new FXMenuSeparator(newobjectmenu);
    new FXMenuCommand(newobjectmenu, tr("Surface"), NULL, this, TXSCWindow::ID_OTREE_NEW_SURFACE);
    new FXMenuSeparator(newobjectmenu);
    new FXMenuCommand(newobjectmenu, tr("Point light"), NULL, this, TXSCWindow::ID_OTREE_NEW_LIGHT);
    new FXMenuCascade(objects_popupmenu, tr("New obect"), NULL, newobjectmenu);
    new FXMenuCommand(objects_popupmenu, tr("Cut\tCtl-X"), NULL, this, TXSCWindow::ID_OTREE_CUT);
    new FXMenuCommand(objects_popupmenu, tr("Copy\tCtl-C"), NULL, this, TXSCWindow::ID_OTREE_COPY);
    new FXMenuCommand(objects_popupmenu, tr("Delete\tDelete"), NULL, this, TXSCWindow::ID_OTREE_DELETE);
    new FXMenuCommand(objects_popupmenu, tr("Paste Before"), NULL, this, TXSCWindow::ID_OTREE_PASTE_B);
    new FXMenuCommand(objects_popupmenu, tr("Paste After\tCtl-V"), NULL, this, TXSCWindow::ID_OTREE_PASTE_A);
    new FXMenuCommand(objects_popupmenu, tr("Paste Under"), NULL, this, TXSCWindow::ID_OTREE_PASTE_U);


    g = NULL;
    cameraID = ID_K_NOACTION;
    //Mouse=POINT(0,0);
    RButtD = false;
    LButtD = false;
    drawXsc = false;
    //ClientR=RECT(0,0,0,0);
    xscOpened = false;
    canvasUpdated = true;
    canvasResized = false;
    accelRotation = 0;
    rendering = false;

    dt_oname.connect((FXString&)(oname));
    dt_pname.connect((FXString&)(poname));

    dt_rotation_angle.connect(rotation_angle);
    rotation_angle = 90;

    dt_rot_ignore_transf.connect(rot_ignore_transf);
    rot_ignore_transf = false;

    translation_step[X] = 0;
    translation_step[Y] = 0;
    translation_step[Z] = 0;
    dt_translation_step[X].connect(translation_step[X]);
    dt_translation_step[Y].connect(translation_step[Y]);
    dt_translation_step[Z].connect(translation_step[Z]);

    size_aspect[X] = 1;
    size_aspect[Y] = 1;
    size_aspect[Z] = 1;
    dt_size_aspect[X].connect(size_aspect[X]);
    dt_size_aspect[Y].connect(size_aspect[Y]);
    dt_size_aspect[Z].connect(size_aspect[Z]);


    dt_copy_on_transf.connect(copy_on_transf);
    copy_on_transf = false;
    cyclic_num = 1;

    dt_has_matrix.connect(has_matrix);
    has_matrix = false;

    dt_visible.connect(visible);
    visible = false;

    clippboardBr = NULL;
    cleanUp();

    InitLUT(); // obj3D.cpp

    // Make thread pool (1 thread) for managing render threads
    rPool = new FXThreadPool(1);
    rPool->setMaximumThreads(1);
    rPoolStarted = rPool->start(1);

    // This class (method run) defines the code to run in the thread pool above
    pRunner = new Runner();

    // Message channel for an asynchronous communication between threads
    mC = new FXMessageChannel(getApp());
}

#if 0
// Message map
FXDEFMAP(TXSCApp) TXSCAppMap[] = {
    FXMAPFUNC(SEL_SIGNAL,    TXSCApp::ID_CLOSEALL,  TXSCApp::onCloseAll),
    FXMAPFUNC(SEL_COMMAND,    TXSCApp::ID_CLOSEALL,  TXSCApp::onCloseAll)
};
// Object implementation
FXIMPLEMENT(TXSCApp, FXApp, TXSCAppMap, ARRAYNUMBER(TXSCAppMap))
#endif

// Message map
FXDEFMAP(TXSCWindow) TXSCWindowMap[] = {
    FXMAPFUNC(SEL_CLOSE,    0, TXSCWindow::onCloseAll),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_QUIT, TXSCWindow::onCloseAll),

    FXMAPFUNC(SEL_PAINT,    TXSCWindow::ID_PAINT,  TXSCWindow::onPaintCanvas),
    FXMAPFUNC(SEL_CONFIGURE, TXSCWindow::ID_PAINT,  TXSCWindow::onResizeCanvas),
    FXMAPFUNC(SEL_MOTION,   TXSCWindow::ID_PAINT,  TXSCWindow::onMouseMove),
    //FXMAPFUNC(SEL_LEFTBUTTONPRESS,TXSCWindow::ID_PAINT,  TXSCWindow::onMouseLDown),
    //FXMAPFUNC(SEL_LEFTBUTTONRELEASE, TXSCWindow::ID_PAINT,  TXSCWindow::onMouseLUp),
    //FXMAPFUNC(SEL_KEYPRESS, TXSCWindow::ID_PAINT,  TXSCWindow::onKeyDown),
    //FXMAPFUNC(SEL_KEYRELEASE,TXSCWindow::ID_PAINT,  TXSCWindow::onKeyUp),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_CUDO,  TXSCWindow::onCmOpenDemo),
    FXMAPFUNC(SEL_UPDATE,   TXSCWindow::ID_CUDO,  TXSCWindow::onUpdRender),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_PRSTEN,  TXSCWindow::onCmOpenDemo),
    FXMAPFUNC(SEL_UPDATE,   TXSCWindow::ID_PRSTEN,  TXSCWindow::onUpdRender),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_LAMPA,  TXSCWindow::onCmOpenDemo),
    FXMAPFUNC(SEL_UPDATE,   TXSCWindow::ID_LAMPA,  TXSCWindow::onUpdRender),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_GKVK,  TXSCWindow::onCmOpenDemo),
    FXMAPFUNC(SEL_UPDATE,   TXSCWindow::ID_GKVK,  TXSCWindow::onUpdRender),

    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_FILE_OPEN,  TXSCWindow::onCmdFileOpen),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_SAVE_AS,  TXSCWindow::onCmdSaveAs),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_SAVE_BITMAP, TXSCWindow::onCmdSaveBitmap),

    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_OPTIONS,  TXSCWindow::onOptions),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_SAVE_OPTIONS,  TXSCWindow::onSaveOptions),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_RENDER,  TXSCWindow::onCmRender),
    FXMAPFUNC(SEL_IO_READ,  TXSCWindow::ID_RENDER_PROGRESS, TXSCWindow::onMcRenderProgress),
    FXMAPFUNC(SEL_IO_READ,  TXSCWindow::ID_RENDER_FINISHED,  TXSCWindow::onMcRenderDone),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_PRENDER,  TXSCWindow::onCmPRender),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_STOPRENDER,  TXSCWindow::onCmdStopRender),
    FXMAPFUNC(SEL_UPDATE,   TXSCWindow::ID_RENDER,  TXSCWindow::onUpdRender),
    FXMAPFUNC(SEL_UPDATE,   TXSCWindow::ID_PRENDER,  TXSCWindow::onUpdRender),
    FXMAPFUNC(SEL_UPDATE,   TXSCWindow::ID_STOPRENDER,  TXSCWindow::onUpdStopRender),

    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_C_ROTATE,  TXSCWindow::onCmKamera),
    FXMAPFUNC(SEL_UPDATE,  TXSCWindow::ID_C_ROTATE,  TXSCWindow::onUpdKamera),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_C_MOVE,  TXSCWindow::onCmKamera),
    FXMAPFUNC(SEL_UPDATE,  TXSCWindow::ID_C_MOVE,  TXSCWindow::onUpdKamera),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_C_MOVE_POI,  TXSCWindow::onCmKamera),
    FXMAPFUNC(SEL_UPDATE,  TXSCWindow::ID_C_MOVE_POI,  TXSCWindow::onUpdKamera),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_C_DISTANCE,  TXSCWindow::onCmKamera),
    FXMAPFUNC(SEL_UPDATE,  TXSCWindow::ID_C_DISTANCE,  TXSCWindow::onUpdKamera),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_ROTATE,  TXSCWindow::onCmKamera),
    FXMAPFUNC(SEL_UPDATE,  TXSCWindow::ID_ROTATE,  TXSCWindow::onUpdKamera),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_C_RESET,  TXSCWindow::onCmCameraReset),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_C_SET,  TXSCWindow::onCmCameraNastav),

    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_OBJECT_VISIBLE,  TXSCWindow::onCmObjectToggleVisible),
    FXMAPFUNC(SEL_UPDATE,  TXSCWindow::ID_OBJECT_VISIBLE,  TXSCWindow::onUpdObjectToggleVisible),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_OBJECTS_VISIBLE,  TXSCWindow::onCmObjectsMakeVisible),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_OBJECTS_UNVISIBLE,  TXSCWindow::onCmObjectsMakeUnvisible),
    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_OBJECTS_MOVE_VISIBLE,  TXSCWindow::onCmObjectsMoveVisible),

    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_SURFACE, TXSCWindow::onCmdSurfaceChanged),

    FXMAPFUNC(SEL_COMMAND,  TXSCWindow::ID_ABOUT,  TXSCWindow::onCmdAbout),

    // objects properties
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_PANEL, TXSCWindow::onCmdPanel),
    FXMAPFUNC(SEL_UPDATE , TXSCWindow::ID_PANEL, TXSCWindow::onUpdPanel),
    FXMAPFUNC(SEL_CHANGED, TXSCWindow::ID_OTREE_LIST, TXSCWindow::onCmdObjectSelected),
    FXMAPFUNC(SEL_EXPANDED, TXSCWindow::ID_OTREE_LIST, TXSCWindow::onCmdExpandObjectTreeItem),
    FXMAPFUNC(SEL_COLLAPSED, TXSCWindow::ID_OTREE_LIST, TXSCWindow::onCmdCollapseObjectTreeItem),

    FXMAPFUNC(SEL_RIGHTBUTTONPRESS, TXSCWindow::ID_OTREE_LIST, TXSCWindow::onCmdTreeItemPopupMenu),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_BRANCH, TXSCWindow::onCmdNewBranch),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_SPHERE, TXSCWindow::onCmdNewSphere),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_CYLINDER, TXSCWindow::onCmdNewCylinder),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_TOROID, TXSCWindow::onCmdNewToroid),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_CONE, TXSCWindow::onCmdNewCone),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_CUBE, TXSCWindow::onCmdNewCube),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_HALFSPACE, TXSCWindow::onCmdNewHalfspace),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_SURFACE, TXSCWindow::onCmdNewSurface),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_NEW_LIGHT, TXSCWindow::onCmdNewLight),

    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_CUT, TXSCWindow::onCmdTreeItemCut),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_OTREE_CUT, TXSCWindow::onUpdTreeItemCut),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_COPY, TXSCWindow::onCmdTreeItemCopy),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_OTREE_COPY, TXSCWindow::onUpdTreeItemCopy),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_DELETE, TXSCWindow::onCmdTreeItemDelete),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_OTREE_DELETE, TXSCWindow::onUpdTreeItemDelete),

    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_PASTE_B, TXSCWindow::onCmdTreeItemPasteBefore),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_OTREE_PASTE_B, TXSCWindow::onUpdTreeItemPasteBefore),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_PASTE_A, TXSCWindow::onCmdTreeItemPasteAfter),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_OTREE_PASTE_A, TXSCWindow::onUpdTreeItemPasteAfter),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_OTREE_PASTE_U, TXSCWindow::onCmdTreeItemPasteUnder),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_OTREE_PASTE_U, TXSCWindow::onUpdTreeItemPasteUnder),

    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_REVERSE_TRANSFORMS, TXSCWindow::onCmdReverseTransforms),

    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_X, TXSCWindow::onCmdRotateX),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_Y, TXSCWindow::onCmdRotateY),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_Z, TXSCWindow::onCmdRotateZ),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_X, TXSCWindow::onUpdRotate),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_Y, TXSCWindow::onUpdRotate),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_Z, TXSCWindow::onUpdRotate),

    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_TRANSLATE_X, TXSCWindow::onCmdRotateTranslateX),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_TRANSLATE_Y, TXSCWindow::onCmdRotateTranslateY),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_TRANSLATE_Z, TXSCWindow::onCmdRotateTranslateZ),

    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_TRANSLATE_X, TXSCWindow::onUpdRotate),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_TRANSLATE_Y, TXSCWindow::onUpdRotate),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_TRANSLATE_Z, TXSCWindow::onUpdRotate),

    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_X, TXSCWindow::onCmdRotateTranslateSizeX),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_Y, TXSCWindow::onCmdRotateTranslateSizeY),
    FXMAPFUNC(SEL_COMMAND, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_Z, TXSCWindow::onCmdRotateTranslateSizeZ),

    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_X, TXSCWindow::onUpdRotate),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_Y, TXSCWindow::onUpdRotate),
    FXMAPFUNC(SEL_UPDATE, TXSCWindow::ID_ROTATE_TRANSLATE_SIZE_Z, TXSCWindow::onUpdRotate),


    FXMAPFUNC(SEL_DND_ENTER/*SEL_DND_REQUEST*/, TXSCWindow::ID_OTREE_LIST, TXSCWindow::onDnDRequest),

};

// Object implementation
FXIMPLEMENT(TXSCWindow, FXMainWindow, TXSCWindowMap, ARRAYNUMBER(TXSCWindowMap))


// MyDataTarget MAP
FXDEFMAP(FXMyDataTarget) FXMyDataTargetMap[] = {
    FXMAPFUNC(SEL_COMMAND,  FXDataTarget::ID_VALUE,  FXMyDataTarget::onCmdValue),
    FXMAPFUNCS(SEL_COMMAND, FXDataTarget::ID_OPTION - 10001, FXDataTarget::ID_OPTION + 10000, FXMyDataTarget::onCmdOption),
};

FXIMPLEMENT(FXMyDataTarget, FXDataTarget, FXMyDataTargetMap, ARRAYNUMBER(FXMyDataTargetMap))

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long FXMyDataTarget::onCmdValue(FXObject* sender, FXSelector sel, void* ptr)
{
    FXDataTarget::onCmdValue(sender, sel, ptr);
    action = true;
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long FXMyDataTarget::onCmdOption(FXObject* sender, FXSelector sel, void* ptr)
{
    FXDataTarget::onCmdOption(sender, sel, ptr);
    action = true;
    return 1;
}
// End of MyDataTarget MAP

/*void FXMyTreeList::dropFinished(FXDragAction action)
{
    return;
}*/


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onSomeChange(FXObject*, FXSelector, void*)
{
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// Clean up
TXSCWindow::~TXSCWindow()
{
    delete scenamenu;
    delete filemenu;
    delete cameramenu;
    delete objectsmenu;
    delete helpmenu;
    delete bitmap;
    delete objects_popupmenu;
    delete newobjectmenu;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// Create
void TXSCWindow::create()
{
    FXMainWindow::create();
    bitmap->create();
    objects_popupmenu->create();
    show(PLACEMENT_SCREEN);
}


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// Detach
void TXSCWindow::detach()
{
    FXMainWindow::detach();
    bitmap->detach();
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onPaintCanvas(FXObject* sender, FXSelector sel, void* ptr)
{
    FXCanvas *c = ((FXCanvas*)sender);
    FXDCWindow dc(c); //(FXCanvas*)sender, (FXEvent*)ptr);

    if (g == NULL) {
        clientR.left = 0;
        clientR.top = 0;
        clientR.right = c->getWidth();
        clientR.bottom = c->getHeight();
        bitmap->resize(clientR.right, clientR.bottom);

        FXDCWindow mdc(bitmap);

        g = new TG(&dc, &mdc);

        mdc.setForeground(c->getBackColor());
        mdc.fillRectangle(0, 0, clientR.right, clientR.bottom);
        //dc.drawImage(bitmap,0,0);
        dc.setForeground(c->getBackColor());
        dc.fillRectangle(0, 0, clientR.right, clientR.bottom);

    } else {
        if (canvasResized) {
            bitmap->resize(clientR.right, clientR.bottom);

            if (xscOpened) {
                g->Set2DView(clientR);
                if (!options.roptAutoRender)
                    WireFrame();
            }
            if (!xscOpened || options.roptAutoRender) {
                FXDCWindow mdc(bitmap);
                mdc.setForeground(c->getBackColor());
                mdc.fillRectangle(0, 0, clientR.right, clientR.bottom);
            }
        }
        dc.drawImage(bitmap, 0, 0);
    }
    canvasUpdated = true;
    canvasResized = false;
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onResizeCanvas(FXObject* sender, FXSelector sel, void* ptr)
{
    FXCanvas *c = ((FXCanvas*)sender);

    if (rendering) {
        rendering = false;
    }

    clientR.left = 0;
    clientR.top = 0;
    clientR.right = c->getWidth();
    clientR.bottom = c->getHeight();

    canvasResized = true;
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::WireFrame()
{
    FXDCWindow mdc(bitmap);
    MATRIX tmpM;
    TDTreeBranchItem *restore;

    if (options.roptAutoRender) {
        CmRender(clientR);
        canvasUpdated = false;
        accelRotation = 0;
        return;
    }

    if (rendering)
        return;

    g->UpdateMDC(&mdc);
    g->mdc->setForeground(canvas->getBackColor());
    g->mdc->fillRectangle(0, 0, clientR.right, clientR.bottom);

    Net3DXY(g, 10, 0.5);

    g->mdc->setLineStyle(FXLineStyle(options.doptLineStyle));
    o.Top();
    g->StoreSight(tmpM);

    TDBase *c;
    while (o.isValid()) {
        c = ADPI(TDBase, o);
        if (o.Curr()->getChildrenCount() > 0) {
            if (c->hasMatrix() && c->isVisible()) {
                g->GetTmpSight(c->mi);
                g->NewSight(c->m);
                g->StoreTmpSight();
            }
        }
        if (((c->getOtype() == TDLIGHT) || (c->getOtype() >= TD_FIRST && c->getOtype() <= TD_LAST)) && c->isVisible()) {
            if (o.getItem() == o_selectedItem) {
                int lwidth = g->mdc->getLineWidth();
                g->mdc->setLineWidth(3);
                if (options.doptAccurateWireFrane)
                    c->WfAcc((TG3D*)g);
                else
                    c->Wf((TG3D*)g);
                g->mdc->setLineWidth(lwidth);
            } else {
                if (options.doptAccurateWireFrane)
                    c->WfAcc((TG3D*)g);
                else
                    c->Wf((TG3D*)g);
            }
        }
        if (!c->isVisible())
            o.setWalkDirectionUp();
        o.Walk();
        restore = NULL;
        while (o.isValid() && !o.isWalkDirectionDown()) {
            c = ADPI(TDBase, o);
            if (o.Curr()->getChildrenCount() > 0 &&
                c->hasMatrix() && c->isVisible()) {
                restore = (TDTreeBranchItem*) o.Curr();
            }
            o.Walk();
        }
        // we have to restore actual sight as we go up
        if (restore != NULL) {
            g->RestoreSight(ADPD(TDBase, restore)->mi);
        }
    }
    g->RestoreSight(tmpM);

    canvasUpdated = false;
    accelRotation = 0;
}


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::Net3DXY(TG *g, INT n, REAL step)
{
    VECTOR p, p1;
    VECTOR v000 = {0, 0, 0};
    VECTOR v100 = {1, 0, 0};
    VECTOR v010 = {0, 1, 0};
    VECTOR v001 = {0, 0, 1};
    REAL ll = n * step, li;

    //g.MDC->FillRgn(TRegion(&Net[0],4,0),TBrush(RGB(150,190,200)));

    g->MSetColor(FXRGBA(255, 0, 255, 0));
    p[Z] = 0;
    p1[Z] = 0;
    for (INT i = 1; i <= n; i++) {
        li = i * step;
        p[X] = li;
        p[Y] = 0;
        p1[X] = li;
        p1[Y] = ll;
        g->MLine3D(p, p1);
        p[X] = 0;
        p[Y] = li;
        p1[X] = ll;
        p1[Y] = li;
        g->MLine3D(p, p1);
    }
    v010[X] = 0;
    v100[Y] = 0;

    v100[X] = ll;
    v010[Y] = ll;
    v001[Z] = ll;
    g->MSetColor(FXRGBA(255, 0, 0, 0));
    g->MLine3D(v000, v100);
    g->MSetColor(FXRGBA(0, 255, 0, 0));
    g->MLine3D(v000, v010);
    g->MSetColor(FXRGBA(0, 0, 255, 0));
    g->MLine3D(v000, v001);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::CmCamera(UINT id)
{
    if (cameraID != id)
        cameraID = id;
    else
        cameraID = ID_K_NOACTION;
}

//long TXSCWindow::onMouseLDown(FXObject*,FXSelector,void*) { LButtD = true; return 1;}
//long TXSCWindow::onMouseLUp(FXObject*,FXSelector,void*) { LButtD = false; return 1;}
//long onKeyDown(FXObject*,FXSelector,void*); {}
//long onKeyUp(FXObject*,FXSelector,void*);  {}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onMouseMove(FXObject* sender, FXSelector, void* ptr)
{
    FXEvent *evt = (FXEvent*)ptr;
    POINT dlt;
    VECTOR v;
    bool ymov, otoc;
    REAL af;

    mouse_x = evt->last_x;
    mouse_y = evt->last_y;

    if (canvasUpdated) {
        onKameraStatusBar(0, 0, 0);
        if (xscOpened && (cameraID != ID_K_NOACTION) && (evt->state & LEFTBUTTONMASK) && evt->moved)
            // (dlt.x !=0 || dlt.y !=0))
        {
            if (rendering)
                rendering = 0;

            dlt.x = evt->win_x - evt->last_x;
            dlt.y = evt->win_y - evt->last_y;
            switch (cameraID) {
            case ID_C_DISTANCE:
                if (abs(dlt.x) > abs(dlt.y))
                    dlt.x = dlt.y;
                //g->ZoomCamera(g->distance+(g->distance*REAL(dlt.x)/20));
                g->ZoomCamera(REAL(dlt.x) / 20);
                break;
            case ID_C_MOVE_POI:
                VAssign(v, g->observer);
                VSubV(v, g->poi);
                //VNormalizuj(v);
                if (abs(dlt.x) > abs(dlt.y))
                    dlt.x = dlt.y;
                VSMultV(v, (REAL)(dlt.x) / 20);
                VAdd(g->poi, v);
                g->MoveCamera(v);
                break;
            case ID_C_MOVE:
                Vector(v, 0, 0, 0);
                if (evt->state & RIGHTBUTTONMASK) {
                    v[Z] += 0.1 * (dlt.x) + 0.1 * (dlt.y);
                } else {
                    v[X] += 0.1 * (dlt.x);
                    v[Y] += 0.1 * (dlt.y);
                }
                g->MoveCamera(v);
                break;
            case ID_ROTATE:
            case ID_C_ROTATE:
                af = M_PI180 + accelRotation;
                ymov = false;
                otoc = false;
                if (cameraID == ID_C_ROTATE)
                    otoc = true;
                if (abs(dlt.x) < abs(dlt.y)) {
                    dlt.x = dlt.y;
                    ymov = true;
                }
                if (evt->state & SHIFTMASK) {
                    if (dlt.x < 0)
                        af = -af; // af=M_PI180; else af=-M_PI180;
                    g->RotateCamera(X, af, otoc);
                } else {
                    if (dlt.x < 0)
                        af = -af; // af=M_PI180; else af=-M_PI180;
                    if (ymov)
                        g->RotateCamera(Y, af, otoc);
                    else
                        g->RotateCamera(Z, af, otoc);
                }
                break;
            }
            WireFrame();
            if (!options.roptAutoRender) {
                canvas->update();
                onKameraStatusBar(0, 0, 0);
            }
        }
    } else {
        accelRotation += M_PI180;
    }

    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdStopRender(FXObject* sender, FXSelector, void* ptr)
{
    if (rendering) {
        rendering = false;
    }
    return 1;
}

typedef struct {
    TColor *pixC;
    RECT r;
    struct timeval rTimeEnd;
    long renderId;
} TRendResult;

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
FXint Runner::run()
{
    struct timeval rTimeStart, rTimeEnd;
    gettimeofday(&rTimeStart, NULL);

    // This calculates mainly 2D borders (rectangles) of 3D objects
    render->SomePrecalculations(g, o, l);

    //XscOpened = false;
    INT rWidth = r.right - r.left + 1;
    INT rHeight = r.bottom - r.top + 1;

    INT cpus = ro.rThreads;

    TColor *pixC = new TColor[rWidth * rHeight];
    if (pixC != NULL) {

        for (INT i = 0; i < rWidth * rHeight; i++)
            pixC[i] = ro.bgColor;

        if (cpus == 0) {
            render->Render(parent, g, *o, *l, pixC, r, ro);
        } else {

            INT vSlice = rHeight / cpus;

            #pragma omp parallel num_threads(ro.rThreads)
            {
                #pragma omp for
                for (INT i = 0; i < cpus; i++) {
                    RECT rP = r;

                    rP.top = r.top + i * vSlice;
                    rP.bottom = rP.top + vSlice;
                    if (rP.bottom > r.bottom)
                        rP.bottom = r.bottom;

                    if (render->Render(parent, g, *o, *l, pixC + (i * vSlice * rWidth), rP, ro) != 0)
                        ro.renderId = 0;

                }
            }
        }
    }

    gettimeofday(&rTimeEnd, NULL);
    timeval_subtract(&rTimeEnd, &rTimeEnd, &rTimeStart);

    TRendResult result;
    result.pixC = pixC;
    result.r = r;
    result.rTimeEnd = rTimeEnd;
    result.renderId = ro.renderId + 1;

    TXSCWindow *w = (TXSCWindow*)parent;
    if (ro.rThreads > 0) {
        if (!w->mC->message(w, FXSEL(SEL_IO_READ, TXSCWindow::ID_RENDER_FINISHED), &result, sizeof(result))) {
        }
    } else {
        w->onMcRenderDone(w, 0, &result);
    }

#if 1
    if (g != w->g)
        delete g;
#endif

    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::CmRender(RECT r)
{
    if (xscOpened) {

        FXDCWindow mdc(bitmap);
        g->UpdateMDC(&mdc);
        FXDCWindow dc(canvas);
        g->UpdateDC(&dc);

        if (ro.drawEveryPixel) {
            if (options.roptClearBefore) {
                g->mdc->setForeground(ro.bgColor);
                g->mdc->fillRectangle(r.left, r.top, r.right, r.bottom);
            }
        }

        Runner *pR = (Runner*)this->pRunner;

        if (ro.rThreads == 0) {
            pR->update((void*)this, &this->r, g, &o, &l, r, ro);
            ro.renderId++;
            rendering = true;
            pR->run();
            rendering = false;
        } else {
            TG *gCopy = new TG(g);
            rPool->wait();
            pR->update((void*)this, &this->r, gCopy, &o, &l, r, ro);
            ro.renderId++;
            rendering = true;
            if (rPool->execute(pR)) {
            } else {
                rendering = false;
                delete gCopy;
            }
        }
    }
}

/*
void* TXSCWindow::loadPointer(FXFileStream *fs, FXHash *h)
{
    FXulong sptr;
    (*fs) >> sptr;

    void *ptr=h->find((void*)sptr);
    return ptr;
}
*/

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::savePointer(FXFileStream *fs, FXHash *h, void *ptr, FXuval *glob_ind)
{
    void *c = h->at(ptr);
    if (c == NULL && ptr != NULL) {
        (*glob_ind)++;
        h->insert(ptr, (void*)(*glob_ind));
        (*fs) << (FXulong)(FXptr)(*glob_ind);
    } else
        (*fs) << (FXulong)(FXptr)c;
}


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdFileOpen(FXObject*, FXSelector, void*)
{
    if (g == NULL)
        return 0;

    FXFileDialog open(this, "File open");
    open.setFilename(filename);
    open.setPatternList("FXScene file (*.xsc)");
    if (open.execute()) {
        if (rendering) {
            FXMessageBox::information(this, MBOX_OK, "File open", "Not possible during rendering");
            return 1;
        }

        filename = open.getFilename();
        //filelist->setCurrentFile(filename);
        //mrufiles.appendFile(filename);

        FXFileStream xscfile;
        if (xscfile.open(filename, FXStreamLoad)) {
            xscfile.setBigEndian(XSC_BIGENDIAN);
            if (xscfile.getSpace() <= 8) {
                xscfile.close();
                // TODO - a message
                return 1;
            }
            app_version_t version;

            FXchar mch1, mch2, mch3, mch4, mch5, mch6, mch7, mch8;

            xscfile >> mch1;
            xscfile >> mch2;
            xscfile >> mch3;
            xscfile >> mch4;
            xscfile >> mch5;
            xscfile >> mch6;
            xscfile >> mch7;
            xscfile >> mch8;

            if (mch1 != magic_char1 || mch2 != magic_char2 ||
                mch3 != magic_char3 || mch4 != magic_char4  ||
                mch5 != magic_char5 || mch6 != magic_char6  ||
                mch7 != magic_char7 || mch8 != magic_char8) {

                if (FXMessageBox::warning(this, MBOX_OK_CANCEL, "File open",
                                          "Bad header. Click ok only in case your data is saved!") != MBOX_CLICKED_OK) {
                    xscfile.close();
                    return 1;
                }
                xscfile.close();
                version.major = 0;
                version.minor = 8;
                version.bugfix = 99;
                xscfile.open(filename, FXStreamLoad);
            } else {
                xscfile >> version.major;
                xscfile >> version.minor;
                xscfile >> version.bugfix;
            }

            if (version.isG(app_version.major, app_version.minor, app_version.bugfix)) {
                if (FXMessageBox::warning(this, MBOX_OK_CANCEL, "File open",
                                          "Version of the file is higher than version of application! Continue?") != MBOX_CLICKED_OK) {
                    xscfile.close();
                    return 1;
                }
            }

            xscOpened = false;
            g->Set2DView(clientR);
            o.Done();
            l.Done();
            g->CameraReset();

            FXint i, j;

            for (i = 0; i < MATRIX_ROWS; i++) {
                for (j = 0; j < MATRIX_COLUMNS; j++) {
                    xscfile >> ((FXdouble&)g->t.M[i][j]);
                    xscfile >> ((FXdouble&)g->t.IM[i][j]);
                }
            }
            xscfile >> ((FXdouble&)g->distance);
            xscfile >> ((FXdouble&)g->observer[X]);
            xscfile >> ((FXdouble&)g->observer[Y]);
            xscfile >> ((FXdouble&)g->observer[Z]);
            xscfile >> ((FXdouble&)g->poi[X]);
            xscfile >> ((FXdouble&)g->poi[Y]);
            xscfile >> ((FXdouble&)g->poi[Z]);
            xscfile >> ((FXdouble&)g->right[X]);
            xscfile >> ((FXdouble&)g->right[Y]);
            xscfile >> ((FXdouble&)g->right[Z]);
            xscfile >> ((FXdouble&)g->up[X]);
            xscfile >> ((FXdouble&)g->up[Y]);
            xscfile >> ((FXdouble&)g->up[Z]);

            FXint otype;
            FXHash ptrHT;
            TDTreeBranchItem *br = NULL, *d_p = NULL;
            FXulong c, p;

            while (!xscfile.eof()) {
                TBoolOper oper = O_PLUS;
                FXulong flags = F_VISIBLE;
                FXString name;
                MATRIX m;
                TDSurf *surface;

                xscfile >> otype;

                if (otype >= TDGROUP && otype <= TD_LAST) {
                    FXint op;
                    xscfile >> op;
                    oper = TBoolOper(op);
                    xscfile >> name;

                    if (version.isGE(0, 9, 0)) {
                        xscfile >> flags;
                    }

                    if ((version.isGE(0, 11, 1) && (flags & F_MATRIX)) ||
                        ((!version.isGE(0, 11, 1) && (otype != TDGROUP)))) {
                        for (i = 0; i < MATRIX_ROWS; i++) {
                            for (j = 0; j < MATRIX_COLUMNS; j++) {
                                xscfile >> ((FXdouble&)m[i][j]);
                            }
                        }
                    }
                }

                if (otype == TDGROUP) {
                    xscfile >> c;
                    xscfile >> p;
                    d_p = (TDTreeBranchItem*)ptrHT.at((void*)(FXptr)p);

                    FXString* d_n = new FXString(name);
                    br = o.insertLastBranch(new TDGroup(new MYFXName(d_n), oper, flags), d_p);
                    ptrHT.insert((void*)(FXptr)c, br);
                    ((TDGroup*)(o.Curr()->getData()))->flags = flags;
                } else if (otype == TDBASE) {
                    xscfile >> c;
                    xscfile >> p;
                    d_p = (TDTreeBranchItem*)ptrHT.at((void*)(FXptr)p);

                    FXString* d_n = new FXString(name);
                    br = o.insertLastBranch(new TDBase(new MYFXName(d_n), m, oper, flags), d_p);
                    ptrHT.insert((void*)(FXptr)c, br);
                    if (!version.isGE(0, 11, 1))
                        flags |= F_MATRIX;
                    ((TDBase*)(o.Curr()->getData()))->flags = flags;

                } else if (otype == TDSURF) {
                    TDSurf surf;

                    xscfile >> c;
                    surface = (TDSurf*)ptrHT.at((void*)(FXptr)c);

                    xscfile >> p;
                    d_p = (TDTreeBranchItem*)ptrHT.at((void*)(FXptr)p);

                    /*if (d_p!=NULL)
                    {
                        ADPD(TDGroup, d_p)->flags |= F_SURFACE;
                    }*/

                    if (version.isGE(0, 12, 2)) {
                        xscfile >> ((FXdouble&)surf.s.metalic);
                    }
                    xscfile >> ((FXdouble&)surf.s.n);
                    xscfile >> ((FXdouble&)surf.s.Ia[X]);
                    xscfile >> ((FXdouble&)surf.s.Ia[Y]);
                    xscfile >> ((FXdouble&)surf.s.Ia[Z]);
                    xscfile >> ((FXdouble&)surf.s.kd[X]);
                    xscfile >> ((FXdouble&)surf.s.kd[Y]);
                    xscfile >> ((FXdouble&)surf.s.kd[Z]);
                    xscfile >> ((FXdouble&)surf.s.ks[X]);
                    xscfile >> ((FXdouble&)surf.s.ks[Y]);
                    xscfile >> ((FXdouble&)surf.s.ks[Z]);

                    FXString* d_n = new FXString(name);
                    TDSurf *s = new TDSurf(surf);
                    s->setName(new MYFXName(d_n));

                    o.insertLast(s, d_p);

                    //if (surface!=NULL)
                    ptrHT.insert((void*)(FXptr)c, s);
                    //surface=s;
                    ((TDSurf*)(o.Curr()->getData()))->flags = flags;
                } else if (otype == TDLIGHT) {
                    TDLight l;

                    xscfile >> c;
                    xscfile >> p;
                    d_p = (TDTreeBranchItem*)ptrHT.at((void*)(FXptr)p);

                    xscfile >> ((FXdouble&)l.pl[X]);
                    xscfile >> ((FXdouble&)l.pl[Y]);
                    xscfile >> ((FXdouble&)l.pl[Z]);
                    xscfile >> ((FXdouble&)l.i[X]);
                    xscfile >> ((FXdouble&)l.i[Y]);
                    xscfile >> ((FXdouble&)l.i[Z]);
                    xscfile >> ((FXdouble&)l.s);

                    FXString* d_n = new FXString(name);
                    TDLight *ln = new TDLight(l);
                    ln->setName(new MYFXName(d_n));

                    o.insertLast(ln, d_p);
                    ptrHT.insert((void*)(FXptr)c, ln);
                    ((TDLight*)(o.Curr()->getData()))->flags = flags;
                } else if (otype >= TD_FIRST && otype < TD_LAST) {
                    if (!version.isGE(0, 11, 1)) {
                        flags |= F_MATRIX;
                    }
                    if (version.isGE(0, 10, 0)) {
                        xscfile >> c;
                        if (c != 0) {
                            surface = (TDSurf*)ptrHT.at((void*)(FXptr)c);
                            if (surface == NULL) {
                                surface = (TDSurf*)(FXptr)c;
                                flags |= F_WRONG_SURFACE;
                            }
                        } else
                            surface = (TDSurf*)&defSurface;
                        xscfile >> p;
                        d_p = (TDTreeBranchItem*)ptrHT.at((void*)(FXptr)p);
                    } else { // before 0,10,0
                        TDSurf surf;

                        xscfile >> p;
                        d_p = (TDTreeBranchItem*)ptrHT.at((void*)(FXptr)p);

                        xscfile >> ((FXdouble&)surf.s.n);
                        xscfile >> ((FXdouble&)surf.s.Ia[X]);
                        xscfile >> ((FXdouble&)surf.s.Ia[Y]);
                        xscfile >> ((FXdouble&)surf.s.Ia[Z]);
                        xscfile >> ((FXdouble&)surf.s.kd[X]);
                        xscfile >> ((FXdouble&)surf.s.kd[Y]);
                        xscfile >> ((FXdouble&)surf.s.kd[Z]);
                        xscfile >> ((FXdouble&)surf.s.ks[X]);
                        xscfile >> ((FXdouble&)surf.s.ks[Y]);
                        xscfile >> ((FXdouble&)surf.s.ks[Z]);
                        surface = new TDSurf(surf);
                    }

                    switch (otype) {
                    case SPHERE:
                        o.insertLast(new Sphere(oper, surface, m), d_p);
                        break;
                    case CYLINDER:
                        o.insertLast(new Cylinder(oper, surface, m), d_p);
                        break;
                    case CONE:
                        o.insertLast(new Cone(oper, surface, m), d_p);
                        break;
                    case CUBE:
                        o.insertLast(new Cube(oper, surface, m), d_p);
                        break;
                    case HALFSPACE:
                        o.insertLast(new HalfSpace(oper, surface, m), d_p);
                        break;
                    }
                    if (!name.empty())
                        ((TDBase*)(o.Curr()->getData()))->setName(new MYFXName(name.text()));
                    ((TDBase*)(o.Curr()->getData()))->flags = flags;
                } else if (otype >= LIGHT_FIRST && otype < LIGHT_LAST) {
                    VECTOR pl, li;
                    REAL s;
                    xscfile >> ((FXdouble&)pl[X]);
                    xscfile >> ((FXdouble&)pl[Y]);
                    xscfile >> ((FXdouble&)pl[Z]);
                    xscfile >> ((FXdouble&)li[X]);
                    xscfile >> ((FXdouble&)li[Y]);
                    xscfile >> ((FXdouble&)li[Z]);
                    xscfile >> ((FXdouble&)s);
                    l.Add(new PLight(pl, li, s));
                }
            } // while (!xscfile.eof())
            xscfile.close();
            //stringsHT.clear();
            o.Top();
            while (o.isValid()) {
                TDGroup *ci = ADPI(TDGroup, o);
                if (ci->flags & F_WRONG_SURFACE) {
                    ci->flags &= -1 ^ F_WRONG_SURFACE;
                    ci->surface = (TDSurf*)ptrHT.at((void*)(FXptr)ci->surface);
                    if (ci->surface == NULL)
                        ci->surface = (TDSurf*)&defSurface;
                }
                o.Down();
            }
            xscOpened = true;
            FillTreeList(o.getFirst());
            WireFrame();
            canvas->update();
            onKameraStatusBar(0, 0, 0);

            FXString s(app_name);
            s.append(" \"");
            s.append(filename);
            s.append('"');
            this->setTitle(s.text());
        }
    }

#if 0
    o.Top();
    while (o.isValid()) {
        o.Walk();
    }
#endif
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdSaveAs(FXObject*, FXSelector, void*)
{
    if (xscOpened) {

        FXFileDialog savedialog(this, "Save " app_name);
        savedialog.setFilename(filename);
        if (savedialog.execute()) {
            if (FXStat::exists(savedialog.getFilename())) {
                if (MBOX_CLICKED_NO == FXMessageBox::question(this, MBOX_YES_NO, "Overwrite " app_name, "Overwrite existing file?"))
                    return 1;
            }
            filename = savedialog.getFilename();
            //filelist->setCurrentFile(filename);
            //mrufiles.appendFile(filename);

            FXFileStream xscfile;
            if (xscfile.open(filename, FXStreamSave)) {
                FXint i, j;

                xscfile.setBigEndian(XSC_BIGENDIAN);

                xscfile << magic_char1;
                xscfile << magic_char2;
                xscfile << magic_char3;
                xscfile << magic_char4;
                xscfile << magic_char5;
                xscfile << magic_char6;
                xscfile << magic_char7;
                xscfile << magic_char8;

                xscfile << app_version.major;
                xscfile << app_version.minor;
                xscfile << app_version.bugfix;

                //camera
                for (i = 0; i < MATRIX_ROWS; i++) {
                    for (j = 0; j < MATRIX_COLUMNS; j++) {
                        xscfile << ((FXdouble&)g->t.M[i][j]);
                        xscfile << ((FXdouble&)g->t.IM[i][j]);
                    }
                }
                xscfile << ((FXdouble&)g->distance);
                xscfile << ((FXdouble&)g->observer[X]);
                xscfile << ((FXdouble&)g->observer[Y]);
                xscfile << ((FXdouble&)g->observer[Z]);
                xscfile << ((FXdouble&)g->poi[X]);
                xscfile << ((FXdouble&)g->poi[Y]);
                xscfile << ((FXdouble&)g->poi[Z]);
                xscfile << ((FXdouble&)g->right[X]);
                xscfile << ((FXdouble&)g->right[Y]);
                xscfile << ((FXdouble&)g->right[Z]);
                xscfile << ((FXdouble&)g->up[X]);
                xscfile << ((FXdouble&)g->up[Y]);
                xscfile << ((FXdouble&)g->up[Z]);

                // variable part
                FXHash ptrHT;
                FXuval glob_ptr_index = 0;
                FXint otype;
                o.Top();
                while (o.isValid()) {
                    TDBase* oi = ADPI(TDBase, o);
                    otype = oi->getOtype();
                    xscfile << otype;
                    FXint op = FXint(oi->oper);
                    xscfile << op;
                    FXString s;
                    if (oi->getName() == NULL)
                        s.assign("");
                    else
                        s.assign(*(FXString*)oi->getName());
                    xscfile << s;

                    xscfile << (FXulong) oi->flags;

                    if (oi->hasMatrix()) {
                        for (i = 0; i < MATRIX_ROWS; i++) {
                            for (j = 0; j < MATRIX_COLUMNS; j++) {
                                xscfile << ((FXdouble&)oi->m[i][j]);
                            }
                        }
                    }


                    if (otype == TDGROUP) {
                        savePointer(&xscfile, &ptrHT, o.Curr(), &glob_ptr_index);
                        savePointer(&xscfile, &ptrHT, o.Curr()->getParent(), &glob_ptr_index);
                    } else if (otype == TDBASE) {
                        savePointer(&xscfile, &ptrHT, o.Curr(), &glob_ptr_index);
                        savePointer(&xscfile, &ptrHT, o.Curr()->getParent(), &glob_ptr_index);
                    } else if (otype == TDSURF) {
                        TDSurf* si = ADPI(TDSurf, o);

                        savePointer(&xscfile, &ptrHT, o.Curr()->getData(), &glob_ptr_index);
                        savePointer(&xscfile, &ptrHT, o.Curr()->getParent(), &glob_ptr_index);

                        xscfile << ((FXdouble&)si->s.metalic);
                        xscfile << ((FXdouble&)si->s.n);
                        xscfile << ((FXdouble&)si->s.Ia[X]);
                        xscfile << ((FXdouble&)si->s.Ia[Y]);
                        xscfile << ((FXdouble&)si->s.Ia[Z]);
                        xscfile << ((FXdouble&)si->s.kd[X]);
                        xscfile << ((FXdouble&)si->s.kd[Y]);
                        xscfile << ((FXdouble&)si->s.kd[Z]);
                        xscfile << ((FXdouble&)si->s.ks[X]);
                        xscfile << ((FXdouble&)si->s.ks[Y]);
                        xscfile << ((FXdouble&)si->s.ks[Z]);
                    } else if (otype == TDLIGHT) {
                        TDLight* li = ADPI(TDLight, o);

                        savePointer(&xscfile, &ptrHT, o.Curr()->getData(), &glob_ptr_index);
                        savePointer(&xscfile, &ptrHT, o.Curr()->getParent(), &glob_ptr_index);

                        xscfile << ((FXdouble&)li->pl[X]);
                        xscfile << ((FXdouble&)li->pl[Y]);
                        xscfile << ((FXdouble&)li->pl[Z]);
                        xscfile << ((FXdouble&)li->i[X]);
                        xscfile << ((FXdouble&)li->i[Y]);
                        xscfile << ((FXdouble&)li->i[Z]);
                        xscfile << ((FXdouble&)li->s);
                    } else if (otype >= TD_FIRST && otype <= TD_LAST) {
                        savePointer(&xscfile, &ptrHT, oi->surface, &glob_ptr_index);
                        savePointer(&xscfile, &ptrHT, o.Curr()->getParent(), &glob_ptr_index);
                    }
                    o.Down();
                }
                /* old code ...
                l.Top();
                while (l.isValid())
                {
                    otype=l.GetItem()->OType();
                    xscfile << ((FXint&)otype);
                    xscfile << ((FXdouble&)l.GetItem()->pl[X]);
                    xscfile << ((FXdouble&)l.GetItem()->pl[Y]);
                    xscfile << ((FXdouble&)l.GetItem()->pl[Z]);
                    xscfile << ((FXdouble&)l.GetItem()->i[X]);
                    xscfile << ((FXdouble&)l.GetItem()->i[Y]);
                    xscfile << ((FXdouble&)l.GetItem()->i[Z]);
                    xscfile << ((FXdouble&)l.GetItem()->s);
                    l.Down();
                }*/
                xscfile << ((FXint)OBJ_LAST);
                xscfile.close();

                FXString s(app_name);
                s.append(" \"");
                s.append(filename);
                s.append('"');
                this->setTitle(s.text());
            }
        }
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// Save file
FXbool TXSCWindow::saveBitmap(const FXString& file)
{
    FXFileStream bitmapfile;
    FXint width, height, i, j;
    FXColor color;
    if (bitmapfile.open(file, FXStreamSave)) {
        bitmap->restore();
        width = bitmap->getWidth();
        height = bitmap->getHeight();
        FXBMPImage img(getApp(), NULL, IMAGE_OWNED | IMAGE_SHMI | IMAGE_SHMP, width, height);
        for (i = 0; i < width; i++)
            for (j = 0; j < height; j++) {
                color =  bitmap->getPixel(i, j);
                img.setPixel(i, j, color);
            }
        img.savePixels(bitmapfile);
        bitmapfile.close();
    }

#if 0
//#ifdef HAVE_TIFF_H
    FXBitmap *img;
    FXint width, height, size, scanline, i;
    FXuchar *data, *pa;
    TIFF *tif;

    img = bitmap;
    width = img->getWidth();
    height = img->getHeight();
    scanline = (width + 7) >> 3;
    size = height * scanline;

    FXMALLOC(&data, FXuchar, size);
    memcpy(data, img->getData(), size);
    for (i = 0; i < size; i++) {
        data[i] = FXBITREVERSE(data[i]);
    }

    tif = TIFFOpen(file.text(), "w");
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 1);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, 1);

    pa = data;
    for (i = 0; i < height; i++) {
        TIFFWriteScanline(tif, (tdata_t)pa, i, 0);
        pa += scanline;
    }
    TIFFClose(tif);
    FXFREE(&data);
#endif
    return true;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// Save
long TXSCWindow::onCmdSaveBitmap(FXObject*, FXSelector, void*)
{
    FXFileDialog savedialog(this, "Save Image (in BMP format)");
    FXString bitmapname;
    bitmapname = filename.before('.');
    bitmapname.append(".bmp");
    savedialog.setFilename(bitmapname);
    if (savedialog.execute()) {
        if (FXStat::exists(savedialog.getFilename())) {
            if (MBOX_CLICKED_NO == FXMessageBox::question(this, MBOX_YES_NO, "Overwrite Image", "Overwrite existing image?")) return 1;
        }
        filename = savedialog.getFilename();
        //filelist->setCurrentFile(filename);
        //mrufiles.appendFile(filename);
        saveBitmap(filename);
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::formatTreeObjectName(FXString *s, TDTreeLeafItem *current)
{
    TDBase *o = ADPD(TDBase, current);

    s->assign(o->getOperAsChar());
    if (!current->isParent()) {
        s->append(o->getOName());
        if (o->getName() != NULL) {
            s->append("(");
            s->append(*(FXString*)(o->getName()));
            s->append(")");
        }
    } else if (o->getName() != NULL)
        s->append(*(FXString*)(o->getName()));
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
FXTreeItem* TXSCWindow::appendFXTreeItem(FXTreeItem *branch, TDTreeLeafItem *current)
{
    FXString s;
    formatTreeObjectName(&s, current);

    if (current->isParent()) {
        TDGroup *gr = ADPD(TDGroup, current);
        if (gr->flags & F_LIGHT)
            branch = objectlist->appendItem(branch, s.text(), folder_openL, folder_closedL, current, true);
        else if (gr->flags & F_SURFACE)
            branch = objectlist->appendItem(branch, s.text(), folder_openS, folder_closedS, current, true);
        else {
            branch = objectlist->appendItem(branch, s.text(), folder_open, folder_closed, current, true);
        }
        branch->setHasItems(true);
    } else {
        switch (ADPD(TDBase, current)->getOtype()) {
        case SPHERE:
            branch = objectlist->appendItem(branch, s.text(), ic_sphere, ic_sphere, current, true);
            break;
        case CUBE:
            branch = objectlist->appendItem(branch, s.text(), ic_cube, ic_cube, current, true);
            break;
        case CYLINDER:
            branch = objectlist->appendItem(branch, s.text(), ic_cylinder, ic_cylinder, current, true);
            break;
        case CONE:
            branch = objectlist->appendItem(branch, s.text(), ic_cone, ic_cone, current, true);
            break;
        case HALFSPACE:
            branch = objectlist->appendItem(branch, s.text(), ic_halfspace, ic_halfspace, current, true);
            break;
        default:
            branch = objectlist->appendItem(branch, s.text(), object_icon, object_icon, current, true);
        }
        branch->setDraggable(true);
    }
    return branch;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::FillTreeList(TDTreeLeafItem *ol)
{
    TDTreeLeafItem *c = ol;

    FillSurfaceList();

    objectlist->clearItems(true);
    while (c != NULL) {
        appendFXTreeItem(NULL, c);
        c = c->getNext();
    }

    if (objectlist->getNumItems() > 0) {
        onCmdObjectSelected(NULL, 0, objectlist->getFirstItem());
        objectlist->selectItem(objectlist->getFirstItem(), true);
    }
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::FillSurfaceList()
{
    surfbox->clearItems();
    o.Top();
    {
        while (o.isValid()) {
            TDGroup* g = ADPI(TDGroup, o);
            if (g->getOtype() == TDSURF) {
                if (g->getName() == NULL)
                    surfbox->appendItem("", g);
                else
                    surfbox->appendItem(*(FXString*)g->getName(), g);
            }
            o.Down();
        }
    }
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdExpandObjectTreeItem(FXObject*o, FXSelector s, void* ptr)
{
    FXTreeItem *item = (FXTreeItem*)ptr;
    FXTreeItem *branch = item;
    TDTreeLeafItem *n, *c;

    if (item == NULL)
        return 1;

    if (item->getFirst() != NULL)
        return 1;

    n = (TDTreeBranchItem*)(item->getData());

    if (n == NULL)
        return 0;

    c = n->getFirstChild();
    while ((c != NULL)) {
        appendFXTreeItem(branch, c);
        c = c->getNext();
    }

    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdTreeItemPopupMenu(FXObject*o, FXSelector s, void* ptr)
{
    FXEvent* event = (FXEvent*)ptr;
    if (!event->moved) {
        objects_popupmenu->popup(NULL, event->root_x, event->root_y);
        getApp()->runModalWhileShown(objects_popupmenu);
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdCollapseObjectTreeItem(FXObject*o, FXSelector s, void* ptr)
{
    FXTreeItem *item = (FXTreeItem*)ptr;
    FXTreeItem *child, *rem_child;

    if (item == NULL)
        return 1;
    if (item->getFirst() == NULL)
        return 0;

    child = item->getFirst();
    while ((child != NULL) && (child->getParent() == item)) {
        rem_child = child;
        child = child->getNext();
        objectlist->removeItem(rem_child);
    }

    onCmdObjectSelected(NULL, 0, item);
    //objectlist->selectItem(item, true);

    return 1;
}


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdPanel(FXObject* o, FXSelector s, void* ptr)
{
    int i, j;

    if (objectlist->getNumItems() == 0)
        return 1;

    if (dt_visible.isAction()) {
        dt_visible.HandleAction();

        o_selectedItem->flags &= -1 ^ F_VISIBLE;
        o_selectedItem->flags |= (visible ? F_VISIBLE : 0);

        WireFrame();
        canvas->update();
        return 1;
    }

    if (dt_has_matrix.isAction()) {
        dt_has_matrix.HandleAction();
        if (!rendering && o_selectedItem->getOtype() == TDGROUP && has_matrix) {
            // convert TDGroup to TDBase
            TDBase *obj = new TDBase((TDGroup*)o_selectedItem);

            delete o_selectedItem;
            o_selectedItem = (TDBase*)obj;
            selectedItem->setData(obj);

            FXint *op = (FXint*)(void*)&o_selectedItem->oper;
            dt_oper.connect((FXint&)*op);

            for (i = 0; i < MATRIX_ROWS; i++)
                for (j = 0; j < MATRIX_COLUMNS; j++) {
                    dt_m[i][j].connect((FXdouble&)o_selectedItem->m[i][j]);
                    fx_m[i][j]->enable();
                }
            gbox_matrix->enable();

            return 1;
        }
        if (!rendering && o_selectedItem->getOtype() == TDBASE && (!has_matrix)) {

            // convert TDBase to TDGroup
            TDGroup *obj = new TDGroup();
            obj->importData(o_selectedItem);

            for (i = 0; i < MATRIX_ROWS; i++)
                for (j = 0; j < MATRIX_COLUMNS; j++) {
                    dt_m[i][j].connect();
                    fx_m[i][j]->disable();
                }
            gbox_matrix->disable();

            delete o_selectedItem;
            o_selectedItem = (TDBase*)obj;
            selectedItem->setData(obj);

            FXint *op = (FXint*)(void*)&o_selectedItem->oper;
            dt_oper.connect((FXint&)*op);

            WireFrame();
            canvas->update();
            return 1;
        }
    }
    if (dt_oper.isAction() || dt_oname.isAction()) {
        if (oname.empty())
            o_selectedItem->setName(NULL);
        else
            o_selectedItem->setName(new MYFXName(oname.text()));

        FXString s;
        formatTreeObjectName(&s, selectedItem);
        objectlist->getCurrentItem()->setText(s);
        objectlist->updateItem(objectlist->getCurrentItem());

        if (dt_oname.isAction() && o_selectedItem->getOtype() == TDSURF) {
            FXint ind = surfbox->findItemByData(o_selectedItem);
            if (ind > 0)
                surfbox->setItemText(ind, oname);
        }

        dt_oper.HandleAction();
        dt_oname.HandleAction();
        return 1;
    }

    for (i = 0; i < MATRIX_ROWS; i++) {
        for (j = 0; j < MATRIX_COLUMNS; j++) {
            if (dt_m[i][j].isAction()) {
                dt_m[i][j].HandleAction();
                MATRIX tmpM;
                if (MInverse(tmpM, o_selectedItem->m)) {
                    WireFrame();
                    canvas->update();
                } else {
                    FXMessageBox::information(this, MBOX_OK, "Math Error", "The Matrix is singular!");
                }
                i = MATRIX_ROWS;
                break;
            }
        }
    }

    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// Object selection changed
long TXSCWindow::onCmdObjectSelected(FXObject* o, FXSelector s, void* ptr)
{
    int i, j;

    selectedTreeItem = (FXTreeItem*)ptr;

    if (selectedTreeItem == NULL)
        return 1;

    //selectedTreeItem->setSelected(true);
    //selectedTreeItem->setFocus(true);

    selectedItem = (TDTreeBranchItem*)(selectedTreeItem->getData());
    if (selectedItem == NULL)
        return 0;

    o_selectedItem = (TDBase*)selectedItem->getData();

    if (o_selectedItem == NULL)
        return 0;

    tabObjects->disable();
    tabSurfaces->disable();
    tabLights->disable();

    surfbox->enable();
    gbox_boolean->disable();

    TDSurf *sf = (TDSurf*)o_selectedItem;
    TDLight *pl = (TDLight*)o_selectedItem;

    dt_oper.connect();

    switch (o_selectedItem->getOtype()) {
    case TDSURF:
        surfbox->disable();
        tabSurfaces->enable();
        tabObjects->enable();
        tabbook->handle(tabSurfaces, FXSEL(SEL_COMMAND, FXTabBar::ID_OPEN_ITEM), NULL);
        //tabbook->setCurrent(2);
        dt_Ia[0].connect(sf->s.Ia[0]);
        dt_Ia[1].connect(sf->s.Ia[1]);
        dt_Ia[2].connect(sf->s.Ia[2]);
        dt_kd[0].connect(sf->s.kd[0]);
        dt_kd[1].connect(sf->s.kd[1]);
        dt_kd[2].connect(sf->s.kd[2]);
        dt_ks[0].connect(sf->s.ks[0]);
        dt_ks[1].connect(sf->s.ks[1]);
        dt_ks[2].connect(sf->s.ks[2]);
        dt_n.connect(sf->s.n);
        dt_metalic.connect((FXdouble&)sf->s.metalic);
        break;
    case TDLIGHT:
        surfbox->disable();
        tabLights->enable();
        tabObjects->enable();
        tabbook->handle(tabLights, FXSEL(SEL_COMMAND, FXTabBar::ID_OPEN_ITEM), NULL);

        dt_pl[0].connect(pl->pl[0]);
        dt_pl[1].connect(pl->pl[1]);
        dt_pl[2].connect(pl->pl[2]);
        dt_pl_i[0].connect(pl->i[0]);
        dt_pl_i[1].connect(pl->i[1]);
        dt_pl_i[2].connect(pl->i[2]);
        dt_pl_s.connect(pl->s);
        break;
    default:
        tabObjects->enable();
        tabbook->handle(tabObjects, FXSEL(SEL_COMMAND, FXTabBar::ID_OPEN_ITEM), NULL);
        //tabbook->setCurrent(0);
        INT type = o_selectedItem->getOtype();
        if (!(o_selectedItem->flags & F_SPECIAL_BRANCH) &&
            ((type >= TD_FIRST && type <= TD_LAST) || type == TDGROUP || type == TDBASE)) {
            void* data = o_selectedItem->surface;
            FXint ind = surfbox->findItemByData(data);
            surfbox->setCurrentItem(ind);
            gbox_boolean->enable();
            FXint *op = (FXint*)(void*)&o_selectedItem->oper;
            dt_oper.connect((FXint&)*op);
        } else {
            surfbox->disable();
            surfbox->setCurrentItem(-1);
        }
    }

    visible = o_selectedItem->isVisible();

    has_matrix = o_selectedItem->hasMatrix() && !(o_selectedItem->flags & F_SPECIAL_BRANCH);
    has_matrix ? gbox_matrix->enable() : gbox_matrix->disable();

    // INT type = o_selectedItem->getOtype();
    // ((type >= TD_FIRST && type <= TD_LAST) || (o_selectedItem->flags & F_SPECIAL_BRANCH)) ?

    if (has_matrix) {
        for (i = 0; i < MATRIX_ROWS; i++)
            for (j = 0; j < MATRIX_COLUMNS; j++) {
                dt_m[i][j].connect((FXdouble&)o_selectedItem->m[i][j]);
                fx_m[i][j]->enable();
            }
    } else {
        // e.g. otype == TDGROUP
        for (i = 0; i < MATRIX_ROWS; i++)
            for (j = 0; j < MATRIX_COLUMNS; j++) {
                fx_m[i][j]->disable();
                dt_m[i][j].connect();
            }
    }

    poname.length(0);
    if ((selectedItem->getParent() == NULL) ||
        (selectedItem->getParent()->getData() == NULL)) {
        poname = FXString("**ROOT**");
        fx_pname->disable();
    } else {
        if (ADPD(TDBase, selectedItem->getParent())->getName() != NULL)
            poname.assign(*(FXString*)ADPD(TDBase, selectedItem->getParent())->getName());
        fx_pname->disable();
    }
    oname.length(0);
    if (o_selectedItem->getName() != NULL)
        oname.assign(*(FXString*)o_selectedItem->getName());

    if (options.doptAutoDrawSelected) {
        WireFrame();
        canvas->update();
    }


    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdSurfaceChanged(FXObject*o, FXSelector s, void* ptr)
{
    //selectedItem=(FXCombobox*)ptr;
    if (o_selectedItem->getOtype() >= TDBASE) {
        TDSurf *surf = (TDSurf*)surfbox->getItemData(surfbox->getCurrentItem());
        o_selectedItem->surface = surf;
        WireFrame(); // draw defined scene
        canvas->update();
    } else
        surfbox->setCurrentItem(-1);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewBranch(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    clippboardBr = new TDTreeBranchItem(new TDGroup(new MYFXName("Branch")));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewSphere(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    MATRIX m;
    M1(m);
    clippboardBr = new TDTreeLeafItem(new Sphere(O_PLUS, (TDSurf*)&defSurface, m));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewCylinder(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    MATRIX m;
    M1(m);
    clippboardBr = new TDTreeLeafItem(new Cylinder(O_PLUS, (TDSurf*)&defSurface, m));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewToroid(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    MATRIX m;
    M1(m);
    clippboardBr = new TDTreeLeafItem(new Toroid(O_PLUS, (TDSurf*)&defSurface, m));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewCone(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    MATRIX m;
    M1(m);
    clippboardBr = new TDTreeLeafItem(new Cone(O_PLUS, (TDSurf*)&defSurface, m));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewCube(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    MATRIX m;
    M1(m);
    clippboardBr = new TDTreeLeafItem(new Cube(O_PLUS, (TDSurf*)&defSurface, m));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewHalfspace(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    MATRIX m;
    M1(m);
    clippboardBr = new TDTreeLeafItem(new HalfSpace(O_PLUS, (TDSurf*)&defSurface, m));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewSurface(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    clippboardBr = new TDTreeLeafItem(new TDSurf());
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdNewLight(FXObject*, FXSelector, void*)
{
    delete clippboardBr;
    clippboardBr = new TDTreeLeafItem(new TDLight());
    return 1;
}


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdTreeItemCut(FXObject* obj, FXSelector s, void* ptr)
{
    FXTreeItem *new_selection;
    if (selectedTreeItem == NULL)
        return 1;
    if (o_selectedItem->flags & F_SPECIAL_BRANCH)
        return 1;

    objectlist->removeItem(selectedTreeItem);
    new_selection = objectlist->getCurrentItem();
    delete clippboardBr;
    clippboardBr = selectedItem;
    clippboardBr->rip();
    WireFrame(); // draw defined scene
    canvas->update();
    if (new_selection != NULL) {
        objectlist->selectItem(new_selection);
        onCmdObjectSelected(NULL, 0, new_selection);
    } else {
        selectedTreeItem = NULL;
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdTreeItemDelete(FXObject* obj, FXSelector s, void* ptr)
{
    if (o_selectedItem->getOtype() == TDSURF) {
        FXint ind = surfbox->findItemByData(o_selectedItem);
        if (ind > 0)
            surfbox->removeItem(ind);
        o.Top();
        {
            while (o.isValid()) {
                TDBase* b = ADPI(TDBase, o);
                if (b->getOtype() >= TDBASE && b->getOtype() <= TD_LAST &&
                    b->surface == (TDSurf*)o_selectedItem)
                    b->surface = (TDSurf*)&defSurface;
                o.Down();
            }
        }
    }
    objectlist->removeItem(selectedTreeItem);
    selectedTreeItem = objectlist->getCurrentItem();
    selectedItem->rip();
    delete selectedItem;
    WireFrame(); // draw defined scene
    canvas->update();
    if (selectedTreeItem != NULL) {
        objectlist->selectItem(selectedTreeItem);
        onCmdObjectSelected(NULL, 0, selectedTreeItem);
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdTreeItemCopy(FXObject* obj, FXSelector s, void* ptr)
{
    if (selectedTreeItem == NULL)
        return 1;
    if (o_selectedItem->flags & F_SPECIAL_BRANCH)
        return 1;
    delete clippboardBr;
    clippboardBr = o.CopyBranch(selectedItem);

    FXString *str = (FXString*)(clippboardBr->getData()->getName());
    FXString copynum("#");
    copynum.append(FXString::value(cyclic_num, 10));
    cyclic_num++;
    if (str != NULL) {
        str->assign(str->before('#'));
        str->append(copynum);
    } else {
        ((TDGroup*)(clippboardBr->getData()))->setName(new MYFXName(copynum.text()));
    }

    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdTreeItemPaste(FXObject* obj, FXSelector s, void* ptr)
{
    static TDSurf *xscDefSurface = NULL;

    if (clippboardBr == NULL)
        return 0;

    if (xscDefSurface == NULL)
        xscDefSurface = new TDSurf(SurfConst[1], new MYFXName("Green"));

    if (o.getFirst() == NULL) {
        TDTreeBranchItem *br;

        br = o.insertFirstBranch(new TDBase(new MYFXName("Surfaces")));
        ((TDBase*)br->getData())->flags |= F_SURFACE;
        o.insertFirst(xscDefSurface, br);
        br = o.appendBranch(new TDBase(new MYFXName("Lights")), br);
        ((TDBase*)br->getData())->flags |= F_LIGHT;
        o.insertFirst(new TDLight(), br);

        if (clippboardBr->isParent())
            o.appendBranch(clippboardBr->getData(), br);
        else {
            o.append(clippboardBr->getData(), br);
            TDGroup *selobj = ((TDGroup*)clippboardBr->getData());
            if (selobj->getOtype() > TD_FIRST && selobj->getOtype() < TD_LAST)
                selobj->surface = xscDefSurface;
        }
        clippboardBr->setData(NULL);
        delete clippboardBr;
        clippboardBr = NULL;
        g->Set2DView(clientR);
        g->CameraReset();
        //clippboardBr=NULL;
        //cleanUp();
        WireFrame(); // draw defined scene
        canvas->update();
        onKameraStatusBar(0, 0, 0);
        FillTreeList(o.getFirst());
        xscOpened = bool(o.First());
        return 1;
    }

    if (selectedTreeItem == NULL)
        return 0;

    TDGroup *clipBrdObj = ((TDGroup*)clippboardBr->getData());
    if (clipBrdObj != NULL && clipBrdObj->getOtype() > TD_FIRST && clipBrdObj->getOtype() < TD_LAST && clipBrdObj->surface == &defSurface)
        clipBrdObj->surface = xscDefSurface;

    switch (pasteOp) {
    case ID_OTREE_PASTE_A:
        clippboardBr->append(selectedItem);
        break;
    case ID_OTREE_PASTE_B:
        clippboardBr->prepend(selectedItem);
        break;
    case ID_OTREE_PASTE_U:
        if (selectedItem->isParent()) {
            clippboardBr->insert(selectedItem);
        }
        break;
    default:
        throw 1;
    }

    if (pasteOp == ID_OTREE_PASTE_U) {
        objectlist->collapseTree(selectedTreeItem, true);
        if (!clippboardBr->isParent() &&
            ADPD(TDGroup, clippboardBr)->getOtype() == TDSURF) {
            FillSurfaceList();
        }
    } else {
        //objectlist->collapseTree(selectedTreeItem->getParent(),TRUE);
        FXTreeItem *newItem = appendFXTreeItem(selectedTreeItem->getParent(), clippboardBr);
        // FXTreeList.prependItem is only able to insert item as the first child of father...
        // so I have to use moveItem to achieve what I want :)
        if (selectedTreeItem->getNext() != NULL) {
            if (pasteOp == ID_OTREE_PASTE_B)
                objectlist->moveItem(selectedTreeItem,
                                     selectedTreeItem->getParent(), newItem);
            else
                objectlist->moveItem(selectedTreeItem->getNext(),
                                     selectedTreeItem->getParent(), newItem);
        }
        onCmdObjectSelected(0, 0, newItem);
        objectlist->selectItem(newItem);
        objectlist->setCurrentItem(newItem);

        if (!clippboardBr->isParent() &&
            ADPD(TDGroup, clippboardBr)->getOtype() == TDSURF) {
            FillSurfaceList();
        }
    }
    //else
    //{
    //    objectlist->clearItems(TRUE);
    //    FillTreeList(o.getFirst());
    //}

    clippboardBr = NULL;

    WireFrame(); // draw defined scene
    canvas->update();

    pasteOp = TXSCWindow::ID_LAST;

    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdTreeItemPasteBefore(FXObject* obj, FXSelector s, void* ptr)
{
    pasteOp = ID_OTREE_PASTE_B;
    return onCmdTreeItemPaste(obj, s, ptr);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdTreeItemPasteAfter(FXObject* obj, FXSelector s, void* ptr)
{
    pasteOp = ID_OTREE_PASTE_A;
    return onCmdTreeItemPaste(obj, s, ptr);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdTreeItemPasteUnder(FXObject* obj, FXSelector s, void* ptr)
{
    pasteOp = ID_OTREE_PASTE_U;
    return onCmdTreeItemPaste(obj, s, ptr);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdTreeItemPasteBefore(FXObject* obj, FXSelector s, void* ptr)
{
    return onUpdTreeItemPasteAfter(obj, s, ptr);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdTreeItemPasteAfter(FXObject* obj, FXSelector s, void* ptr)
{
    if (clippboardBr != NULL)
        obj->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    else
        obj->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdTreeItemPasteUnder(FXObject* obj, FXSelector s, void* ptr)
{
    if ((clippboardBr != NULL) && (selectedItem != NULL) && (selectedItem->isParent()))
        obj->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    else
        obj->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdTreeItemCut(FXObject* obj, FXSelector s, void* ptr)
{
    if (selectedItem == NULL)
        return 1;
    TDGroup *selobj = ((TDGroup*)selectedItem->getData());
    if (rendering || (selobj != NULL && selobj->getOtype() == TDSURF) ||
        (selobj->flags & F_SPECIAL_BRANCH))
        obj->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    else
        obj->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdTreeItemCopy(FXObject* obj, FXSelector s, void* ptr)
{
    if (selectedItem == NULL)
        return 1;
    TDGroup *selobj = ((TDGroup*)selectedItem->getData());
    if (selobj->flags & F_SPECIAL_BRANCH)
        obj->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    else
        obj->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdTreeItemDelete(FXObject* obj, FXSelector s, void* ptr)
{
    if (rendering) {
        obj->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
        return 1;
    } else
        return onUpdTreeItemCopy(obj, s, ptr);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdRotate(FXObject* obj, FXSelector s, void* ptr)
{
    if (chkbt_matrix->getCheck() == true) //  has_matrix)
        obj->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    else
        obj->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdObjectToggleVisible(FXObject* obj, FXSelector sel, void*)
{
    if (visible)
        obj->handle(this, FXSEL(SEL_COMMAND, ID_CHECK), NULL);
    else
        obj->handle(this, FXSEL(SEL_COMMAND, ID_UNCHECK), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdRender(FXObject*o, FXSelector sel, void* ptr)
{
    rendering ? o->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL) :
    o->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdStopRender(FXObject*o, FXSelector sel, void* ptr)
{
    rendering ? o->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL) :
    o->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmCameraReset(FXObject*, FXSelector sel, void*)
{
    if (xscOpened) {
        g->CameraReset();
        WireFrame();
        canvas->update();
        onKameraStatusBar(0, 0, 0);
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmCameraNastav(FXObject*, FXSelector sel, void*)
{
    VECTOR obs, poi;
    if (xscOpened) {
        FXCameraDialog c(this);
        VAssign(obs, g->observer);
        VSum(poi, g->observer, g->poi);
        c.obs_x.connect((FXdouble&)obs[X]);
        c.obs_y.connect((FXdouble&)obs[Y]);
        c.obs_z.connect((FXdouble&)obs[Z]);
        c.poi_x.connect((FXdouble&)poi[X]);
        c.poi_y.connect((FXdouble&)poi[Y]);
        c.poi_z.connect((FXdouble&)poi[Z]);

        if (c.execute(PLACEMENT_OWNER)) {
            obs[X] = *((REAL*)c.obs_x.getData());
            obs[Y] = *((REAL*)c.obs_y.getData());
            obs[Z] = *((REAL*)c.obs_z.getData());
            poi[X] = *((REAL*)c.poi_x.getData());
            poi[Y] = *((REAL*)c.poi_y.getData());
            poi[Z] = *((REAL*)c.poi_z.getData());
            g->SetCamera(obs, poi);
            WireFrame();
            canvas->update();
            onKameraStatusBar(0, 0, 0);
        }
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmKamera(FXObject*, FXSelector sel, void*)
{
    CmCamera(FXSELID(sel));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onUpdKamera(FXObject* sender, FXSelector sel, void*)
{
    sender->handle(this, (cameraID == FXSELID(sel)) ? FXSEL(SEL_COMMAND, ID_CHECK) : FXSEL(SEL_COMMAND, ID_UNCHECK), NULL);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// Update status bar Kamera info
long TXSCWindow::onKameraStatusBar(FXObject*, FXSelector, void*)
{
    TColor rcol;
    if (g != NULL) {
        VECTOR o, p;
        VAssign(o, g->observer);
        VSum(p, o, g->poi); // g->poi is vector
        FXDCWindow mdc(bitmap);
        rcol = mdc.readPixel(mouse_x, mouse_y);
        FXString s;
        s.format("O[%f,%f,%f] P[%f,%f,%f] M[%d,%d] Pix[0x%x]", o[X], o[Y], o[Z], p[X], p[Y], p[Z], mouse_x, mouse_y, rcol);
        statusbarCamera->setText(s);
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmObjectToggleVisible(FXObject*, FXSelector sel, void*)
{
    if (selectedTreeItem == NULL)
        return 1;
    o_selectedItem->flags ^= F_VISIBLE;
    FXTreeItem *nextItem = selectedTreeItem->getNext();
    if (nextItem == NULL)
        nextItem = selectedTreeItem;

    onCmdObjectSelected(0, 0, nextItem);
    objectlist->selectItem(nextItem);
    objectlist->setCurrentItem(nextItem);

    WireFrame();
    canvas->update();
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdReverseTransforms(FXObject*, FXSelector sel, void*)
{
    if (translation_step[X] != 0)
        translation_step[X] = - translation_step[Y];
    if (translation_step[Y] != 0)
        translation_step[Y] = - translation_step[Y];
    if (translation_step[Z] != 0)
        translation_step[Z] = - translation_step[Z];
    if (size_aspect[X] != 0)
        size_aspect[X] = 1 / size_aspect[X];
    if (size_aspect[Y] != 0)
        size_aspect[Y] = 1 / size_aspect[Y];
    if (size_aspect[Z] != 0)
        size_aspect[Z] = 1 / size_aspect[Z];
    if (rotation_angle != 0)
        rotation_angle = - rotation_angle;
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::RotateTranslateSizeObjects(int axis, VECTOR t, VECTOR s)
{
    VECTOR v;
    VZero(v);
    if (o_selectedItem == NULL)
        return 1;

    if (copy_on_transf) {
        //FXTreeItem *selTreeItem=selectedTreeItem;
        onCmdTreeItemCopy(0, 0, 0);
        //TDTreeLeafItem *tmp_clipp=clippboardBr;

        pasteOp = ID_OTREE_PASTE_A;
        onCmdTreeItemPaste(0, 0, 0);

        //selTreeItem=objectlist->findItemByData(tmp_clipp);
        //objectlist->selectItem(selTreeItem);
        //selectedItem=tmp_clipp;
        //o_selectedItem=(TDBase*)selectedItem->getData();
    }
    if (rotation_angle != 0) {
        REAL angle = (2 * M_PI_ * rotation_angle) / 360;
        if (rot_ignore_transf) {
            MTtoV(v, o_selectedItem->m);
            MTSubV(o_selectedItem->m, v);
        }
        MRotate(o_selectedItem->m, axis, angle);
        if (rot_ignore_transf)
            MTAddV(o_selectedItem->m, v);
    }
    o_selectedItem->m[0][0] *= s[X];
    o_selectedItem->m[1][0] *= s[X];
    o_selectedItem->m[2][0] *= s[X];
    o_selectedItem->m[0][1] *= s[Y];
    o_selectedItem->m[1][1] *= s[Y];
    o_selectedItem->m[2][1] *= s[Y];
    o_selectedItem->m[0][2] *= s[Z];
    o_selectedItem->m[1][2] *= s[Z];
    o_selectedItem->m[2][2] *= s[Z];

    MTAddV(o_selectedItem->m, t);
    WireFrame();
    canvas->update();
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateX(FXObject* obj, FXSelector s, void* ptr)
{
    VECTOR V0, V1;
    VZero(V0);
    VOne(V1);
    return RotateTranslateSizeObjects(X, V0, V1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateY(FXObject* obj, FXSelector s, void* ptr)
{
    VECTOR V0, V1;
    VZero(V0);
    VOne(V1);
    return RotateTranslateSizeObjects(Y, V0, V1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateZ(FXObject* obj, FXSelector s, void* ptr)
{
    VECTOR V0, V1;
    VZero(V0);
    VOne(V1);
    return RotateTranslateSizeObjects(Z, V0, V1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateTranslateX(FXObject* obj, FXSelector s, void* ptr)
{
    VECTOR V1;
    VOne(V1);
    return RotateTranslateSizeObjects(X, translation_step, V1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateTranslateY(FXObject* obj, FXSelector s, void* ptr)
{
    VECTOR V1;
    VOne(V1);
    return RotateTranslateSizeObjects(Y, translation_step, V1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateTranslateZ(FXObject* obj, FXSelector s, void* ptr)
{
    VECTOR V1;
    VOne(V1);
    return RotateTranslateSizeObjects(Z, translation_step, V1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateTranslateSizeX(FXObject* obj, FXSelector s, void* ptr)
{
    return RotateTranslateSizeObjects(X, translation_step, size_aspect);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateTranslateSizeY(FXObject* obj, FXSelector s, void* ptr)
{
    return RotateTranslateSizeObjects(Y, translation_step, size_aspect);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmdRotateTranslateSizeZ(FXObject* obj, FXSelector s, void* ptr)
{
    return RotateTranslateSizeObjects(Z, translation_step, size_aspect);
}


long TXSCWindow::onDnDRequest(FXObject* o, FXSelector s, void* ptr)
{
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// Active panel switched
long TXSCWindow::onCmdPanel(FXObject* o, FXSelector s, void* ptr)
{
    FXTRACE((1, "Panel = %d\n", (FXint)(long long)ptr));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmObjectsMakeVisible(FXObject*, FXSelector sel, void*)
{
    o.Top();
    while (o.isValid()) {
        TDGroup *ci = ADPI(TDGroup, o);
        //if (ci->isVisible())
        ci->flags |= F_VISIBLE;
        o.Down();
    }
    onCmdObjectSelected(0, 0, selectedTreeItem);
    WireFrame();
    canvas->update();
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmObjectsMakeUnvisible(FXObject*, FXSelector sel, void*)
{
    o.Top();
    while (o.isValid()) {
        TDGroup *ci = ADPI(TDGroup, o);
        ci->flags &= -1 ^ F_VISIBLE;
        o.Down();
    }
    onCmdObjectSelected(0, 0, selectedTreeItem);
    WireFrame();
    canvas->update();
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmObjectsMoveVisible(FXObject*, FXSelector sel, void*)
{
    if (!selectedItem->isParent() || (ADPD(TDGroup, selectedItem)->flags & F_SPECIAL_BRANCH)) {
        FXMessageBox::information(this, MBOX_OK, __FUNCTION__, "The current item is not parent!");
        return 0;
    }
    TDTreeLeafItem *br = selectedItem->getParent();
    while (br != NULL) {
        TDGroup *ci = ADPD(TDGroup, br);
        if (ci != NULL && (ci->isVisible() || (ci->flags & F_SPECIAL_BRANCH))) {
            FXMessageBox::information(this, MBOX_OK, __FUNCTION__, "One or more parents are visible!");
            return 0;
        }
        br = br->getParent();
    }
    ADPD(TDGroup, selectedItem)->flags &= -1 ^ F_VISIBLE;
    o.Top();
    while (o.isValid()) {
        TDGroup *ci = ADPI(TDGroup, o);
        if (ci->flags & F_SPECIAL_BRANCH)
            o.Next();
        else if (ci->isVisible()) {
            TDTreeLeafItem *br = o.Curr();
            o.Next();
            br->rip();
            if (selectedItem->getChildrenCount() == 0)
                br->insert(selectedItem);
            else
                br->append(selectedItem->getLastChild());
        } else {
            if (o.Curr() != selectedItem)
                o.Down();
            else
                o.Next();
        }
    }
    ADPD(TDGroup, selectedItem)->flags |= F_VISIBLE;
    FillTreeList(o.getFirst());
    WireFrame();
    canvas->update();
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmRender(FXObject*, FXSelector sel, void*)
{
    if (!rendering) {
        CmRender(clientR);
    }

    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onMcRenderProgress(FXObject*, FXSelector sel, void*ptr)
{
    TRendProgress *pP = (TRendProgress*)ptr;

    FXDCWindow mdc(bitmap);
    FXDCWindow dc(canvas);

    if (ro.rThreads > 0) {
        g->UpdateMDC(&mdc);
        g->UpdateDC(&dc);
    }

    // A little "progress bar"
    if (pP->pbWidth == pP->pbR.right - pP->pbR.left) {
        g->DSetColor(0);
        g->MRect(pP->pbR);
        g->DRect(pP->pbR);
        shrinkRectBy1(pP->pbR);
        g->DSetColor(0xFFFFFFFF);
        g->MRect(pP->pbR);
        g->DRect(pP->pbR);
        shrinkRectBy1(pP->pbR);
    } else {
        POINT p1 = { pP->pbR.left + pP->pbLength, pP->pbR.top };
        POINT p2 = { pP->pbR.left + pP->pbLength, pP->pbR.bottom };
        g->DSetColor(0xAAAAAAAA);
        g->MLine(p1, p2);
        g->DLine(p1, p2);
    }

#if 0
    if (ro.rThreads == 0) {
        //canvas->update();
        if (getApp()->peekEvent()) {
            getApp()->runWhileEvents();
        }
    }
#endif

    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onMcRenderDone(FXObject*, FXSelector sel, void*ptr)
{
    TRendResult *pResult = (TRendResult*)ptr;
    TColor *pixC = pResult->pixC;
    RECT r = pResult->r;
    struct timeval rTimeEnd = pResult->rTimeEnd;
    INT rWidth = r.right - r.left + 1;
    INT rHeight = r.bottom - r.top + 1;

    FXString s;
    s.format("Render time: %lu seconds, %lu microseconds.", rTimeEnd.tv_sec, rTimeEnd.tv_usec);
    statusbarCamera->setText(s);

    FXDCWindow mdc(bitmap);
    FXDCWindow dc(canvas);

    if ((!ro.drawEveryPixel) && (ro.renderId == pResult->renderId) &&
        clientR.top == r.top && clientR.left == r.left && clientR.bottom == r.bottom && clientR.right == r.right) {
        FXImage img(getApp(), (FXColor*)pixC, 0/*IMAGE_OWNED*/, rWidth, rHeight);
        img.create();

        if (ro.rThreads > 0) {
            g->UpdateMDC(&mdc);
            g->UpdateDC(&dc);
        }

        g->mdc->drawImage(&img, r.left, r.top);
        img.destroy();
        canvas->update();
    }
    delete pixC;

    rendering = false;
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmPRender(FXObject*, FXSelector sel, void*ptr)
{
    RECT cr;
    if (canvasUpdated && !rendering) {
        rendering = true;

        cr.left = mouse_x;
        cr.top = mouse_y;
        cr.bottom = cr.top;
        cr.right = cr.left + (cr.top & options.roptShiftOddLines);

        bool pBar = ro.pBar;
        FXint rThreads = ro.rThreads;
        ro.pBar = false;
        ro.rThreads = 0;

        CmRender(cr);

        ro.pBar = pBar;
        ro.rThreads = rThreads;
        fflush(stdout);
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// About
long TXSCWindow::onCmdAbout(FXObject*, FXSelector, void*)
{
    FXMessageBox::information(this, MBOX_OK, "About " app_name,
                              "Version %d.%d.%d\nCopyright Karol Gajdos <klement2@azet.sk> 2007-2016\n"
                              "This software uses the FOX Toolkit (www.fox-toolkit.org)",
                              app_version.major, app_version.minor, app_version.bugfix);
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onOptions(FXObject*, FXSelector, void*)
{
    FXOptionsDialog o(this);
    TOptions od = options;

    ALL_OPTIONS_CONNECT(o, od);

    if (o.execute(PLACEMENT_OWNER)) {
        options = od;
        getRenderOptions();
    }
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onSaveOptions(FXObject*, FXSelector, void*)
{
    FXFileStream cfgfile;
    if (cfgfile.open(CFG_FILENAME, FXStreamSave)) {
        cfgfile.setBigEndian(XSC_BIGENDIAN);
        ALL_OPTIONS_OP(cfgfile << options.)
        cfgfile.close();
    }
    return 1;
}

static void xscRendPBarFct(TRendProgress *pP)
{
    TXSCWindow *w = (TXSCWindow*)pP->pPtr;
    if (w->getRThreads() > 0)
        w->mC->message(w, FXSEL(SEL_IO_READ, TXSCWindow::ID_RENDER_PROGRESS), pP, sizeof(*pP));
    else {
        TRendProgress rp = *pP;
        w->onMcRenderProgress(w, 0, &rp);
    }
}

static bool xscRendStopFct(void *pP, long myRenderId)
{
    TXSCWindow *w = (TXSCWindow*)pP;
#if 0
    if (w->getRThreads() == 0) {
        if (w->getApp()->peekEvent()) {
            w->getApp()->runWhileEvents();
        }
    }
#endif
    return (!w->isRendering()) || (w->getRenderId() != myRenderId + 1);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::getRenderOptions()
{
    ro.shades = options.roptShades;
    ro.rmask = options.roptShiftOddLines;
    ro.stepx = options.roptStepX;
    ro.stepy = options.roptStepY;
    ro.bgColor = options.roptBgColor;
    ro.pBar = options.roptPBar;
    ro.rThreads = options.roptThreads;
    ro.drawEveryPixel = options.roptDrawPixel;

    ro.stopFct = &xscRendStopFct;
    ro.pBarFct = &xscRendPBarFct;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCmOpenDemo(FXObject*, FXSelector sel, void*)
{
    CmOpen(FXSELID(sel));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
long TXSCWindow::onCloseAll(FXObject*o, FXSelector sel, void* ptr)
{
    // my_rendering = false;
    exit(0);
    //while(0<windowlist.no() && windowlist[0]->close(TRUE));
    return 1;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
void TXSCWindow::CmOpen(UINT scena)
{
    VECTOR obs, poi;
    MATRIX m;
    TDTreeBranchItem *n0, *n1, *n2, *n3;
    TDTreeLeafItem *leaf;

    if (g == NULL || rendering)
        return;

    xscOpened = false;

    g->Set2DView(clientR);
    g->CameraReset();
    cleanUp();

    TDSurf *DynSurfConst[5];
    DynSurfConst[0] = new TDSurf(SurfConst[0], new MYFXName("White"));
    DynSurfConst[1] = new TDSurf(SurfConst[1], new MYFXName("Green"));
    DynSurfConst[2] = new TDSurf(SurfConst[2], new MYFXName("Red"));
    DynSurfConst[3] = new TDSurf(SurfConst[3], new MYFXName("Yellow"));
    DynSurfConst[4] = new TDSurf(SurfConst[4], new MYFXName("Pink"));

    // F_SPECIAL_BRANCH is flag that protects branch from beeing deleted
    // for example it covers F_SURFACE or F_LIGHT flags
    n0 = o.insertFirstBranch(new TDBase(new MYFXName("Surfaces")), NULL);
    ((TDBase*)n0->getData())->flags |= F_SURFACE; // protect this branch from beeing deleted
    o.insertFirst(DynSurfConst[0], n0);
    o.insertLast(DynSurfConst[1], n0);
    o.insertLast(DynSurfConst[2], n0);
    o.insertLast(DynSurfConst[3], n0);
    o.insertLast(DynSurfConst[4], n0);

    n0 = o.appendBranch(new TDBase(new MYFXName("Lights")), n0);
    ((TDBase*)n0->getData())->flags |= F_LIGHT; // protect this branch from beeing deleted
    //o.insertFirst(new TDLight(), n0);

    o.ACurr(n0);
    //o.Top();
    //o.Down();
    //o.insertLastBranch(new TDGroup(new MYFXName("Objects")),NULL);

    switch (scena) {
    case ID_CUDO:
#if 0
        Vector(obs, 3, 1.9, 3.6);
        Vector(poi, -1.5, 1.5, 2.5);
        g->SetCamera(obs, poi);
#else
        Vector(obs, 2.4, 2.6, 4);
        g->MoveCamera(obs);
        g->ZoomCamera(5);
        g->RotateCamera(Y, -15 * M_PI180, true);
        g->RotateCamera(Z, 35 * M_PI180, true);
#endif

        Vector(poi, 10, -6, 8);
        Vector(obs, 1, 1, 1);
        o.insertFirst(new TDLight(poi, obs, 0.6), n0);
        //l.Add(new PLight(poi,obs,0.02));
        Vector(poi, 1, 1, 8);
        Vector(obs, 0.8, 0.8, 0.8);
        o.insertLast(new TDLight(poi, obs, 0.3), n0);
        //l.Add(new PLight(poi,obs,0.05));
        Vector(poi, 1, 10, 15);
        Vector(obs, 1, 1, 1);
        o.insertLast(new TDLight(poi, obs, 0.6), n0);
        //l.Add(new PLight(poi,obs,0.04));
        o.ACurr(n0);

        MDiagT(m, 0.2, 3, 3, -1, 1, 3);
        o.append(new Cube(O_PLUS, DynSurfConst[0], m));
        MDiagT(m, 3, 3, 0.2, -1, 1, 3);
        o.append(new Cube(O_PLUS, DynSurfConst[0], m));

        MDiagT(m, 1.8, 1.8, 1.8, 0.1, 0.1, 4);
        o.append(new Cube(O_LOCALPLUS, DynSurfConst[1], m));
        MDiagT(m, 1, 1, 1.2, 1.9, 1.9, 5.8);
        o.append(new Sphere(O_MINUS, DynSurfConst[2], m));
        MDiagT(m, 0.7, 0.7, 2, 1, 1, 3.9);
        o.append(new Cylinder(O_MINUS, DynSurfConst[0], m));
        MDiagT(m, 0.7, 0.7, 2, 0, 0, 0);
        MRotate(m, X, -M_PI_2_);
        m[3][0] += 1;
        m[3][1] += 2;
        m[3][2] += 4.9;
        o.append(new Cylinder(O_MINUS, DynSurfConst[0], m));
        MDiagT(m, 0.7, 0.7, 2, 0, 0, 0);
        MRotate(m, Y, -M_PI_2_);
        m[3][1] += 1;
        m[3][2] += 4.9;
        o.append(new Cylinder(O_MINUS, DynSurfConst[0], m));
        MDiagT(m, 0.3, 0.3, 0.6, 1.5, 2.5, 3.5);
        o.append(new Cone(O_PLUS, DynSurfConst[2], m));
        MDiagT(m, 0.4, 0.4, 0.42, 1, 1, 5);
        o.append(new Sphere(O_PLUS, DynSurfConst[1], m));
        break;

    case ID_LAMPA:
        Vector(obs, 2.5, 0.4, 2.2);
        Vector(poi, 0.8, 0.7, 1);
        g->SetCamera(obs, poi);

        Vector(poi, 0.2, 0.54, 1.6);
        Vector(obs, 0.6, 0.6, 0.6);
        o.insertFirst(new TDLight(poi, obs, 0.62), n0);
        //l.Add(new PLight(poi,obs,0.22));
        Vector(poi, 0.1, 2.7, 3.5);
        Vector(obs, 1, 1, 1);
        o.insertLast(new TDLight(poi, obs, 0.37), n0);
        //l.Add(new PLight(poi,obs,0.37));
        Vector(poi, 2.3, 2.4, 1.4);
        Vector(obs, 1, 1, 1);
        o.insertLast(new TDLight(poi, obs, 0.27), n0);
        //l.Add(new PLight(poi,obs,0.27));
        o.ACurr(n0);

        // Stol
        n1 = o.appendBranch(new TDGroup(new MYFXName("Workroom")), n0);

        n2 = o.insertFirstBranch(new TDBase(new MYFXName("Table")), n1);
        MDiagT(m, 0.8, 1.46, 0.08, 0, 0, 1);
        o.insertFirst(new Cube(O_PLUS, DynSurfConst[3], m), n2);

        MDiagT(m, 0.03, 0.04, 1.46, 0, 0, 0);
        MRotate(m, X, M_PI_2_);
        m[3][0] += 0;
        m[3][1] += 0;
        m[3][2] += 1.04;
        o.append(new Cylinder(O_PLUS, DynSurfConst[3], m));
        m[3][0] += 0.8;
        m[3][1] += 0;
        m[3][2] += 0;
        o.append(new Cylinder(O_PLUS, DynSurfConst[3], m));

        MDiagT(m, 0.06, 0.06, 1, 0, 0, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[3], m));
        MT(m, 0.74, 0, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[3], m));
        MT(m, 0.74, 1.40, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[3], m));
        MT(m, 0, 1.40, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[3], m));

        // Veci na stole
        n3 = o.insertLastBranch(new TDBase(new MYFXName("Things on the table")), n2);
        MDiagT(m, 0.05, 0.05, 0.05, 0.3, 0.8, 1.13);
        o.insertFirst(new Sphere(O_PLUS, DynSurfConst[1], m), n3);
        MDiagT(m, 0.1, 0.1, 0.12, 0.5, 1.1, 1.08);
        o.append(new Cube(O_PLUS, DynSurfConst[0], m));
        MDiagT(m, 0.06, 0.06, 0.18, 0.5, 0.25, 1.08);
        o.append(new Cone(O_PLUS, DynSurfConst[2], m));

        // Lampa
        n3 = o.appendBranch(new TDBase(new MYFXName("Lamp")), n3);
        MDiagT(m, 0.12, 0.12, 0.04, 0.2, 0.4, 1.08);
        o.insertLast(new Cylinder(O_PLUS, DynSurfConst[2], m), n3);
        MDiagT(m, 0.01, 0.01, 0.5, 0.2, 0.38, 1.12);
        o.append(new Cylinder(O_PLUS, DynSurfConst[2], m));
        MT(m, 0.2, 0.42, 1.12);
        o.append(new Cylinder(O_PLUS, DynSurfConst[2], m));

        MDiagT(m, 0.02, 0.02, 0.2, 0, 0, 0);
        MRotate(m, X, -M_PI_2_);
        m[3][0] += 0.2;
        m[3][1] += 0.54;
        m[3][2] += 1.45;
        o.append(new Cylinder(O_PLUS, DynSurfConst[2], m));

        MDiagT(m, 0.1, 0.1, 0.25, 0.2, 0.54, 1.32);
        o.append(new Cone(O_LOCALPLUS, DynSurfConst[2], m));
        MT(m, 0.2, 0.54, 1.30);
        o.append(new Cone(O_MINUS, DynSurfConst[1], m));
        // Stolicka
        n3 = o.insertLastBranch(new TDBase(new MYFXName("Chair")), n1);
        MDiagT(m, 0.4, 0.4, 0.04, 0.6, 0.6, 0.6);
        o.insertLast(new Cube(O_PLUS, DynSurfConst[2], m), n3);
        MDiagT(m, 0.04, 0.04, 0.6, 0.6, 0.6, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[2], m));
        MT(m, 0.96, 0.6, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[2], m));
        MT(m, 0.96, 0.96, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[2], m));
        MT(m, 0.6, 0.96, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[2], m));
        MDiagT(m, 0.16, 0.18, 0.05, 0.82, 0.80, 0.64);
        o.append(new Sphere(O_PLUS, DynSurfConst[0], m));
        m[0][0] = 0;
        m[1][1] = 0.2;
        m[2][2] = 0;
        m[0][2] = -0.4;
        m[2][0] = 0.04;
        MT(m, 0.96, 0.8, 0.8);
        o.append(new Cylinder(O_LOCALPLUS, DynSurfConst[2], m));
        m[1][1] = 0.16;
        m[0][2] = -0.4;
        m[2][0] = 0.1;
        m[3][0] -= 0.04;
        o.append(new Cylinder(O_MINUS, DynSurfConst[1], m));
        MDiagT(m, 0.08, 0.4, 0.64, 0.94, 0.6, 0);
        o.append(new Cube(O_MINUS, DynSurfConst[2], m));
        MDiagT(m, 0.02, 0.02, 0.51, 0.99, 0.72, 0.65);
        o.append(new Cylinder(O_PLUS, DynSurfConst[2], m));
        m[3][1] = 0.86;
        o.append(new Cylinder(O_PLUS, DynSurfConst[2], m));

        //o.Add(new Branch("","Workroom"));
        MDiagT(m, 1, 1, -1, 0, 0, 0);
        leaf = o.append(new HalfSpace(O_PLUS, DynSurfConst[4], m), n1);
        o.prependBranch(new TDBase(new MYFXName("Empty...")), leaf);


        break;

    case ID_GKVK:
        Vector(obs, 10, 8, 8);
        Vector(poi, 2, 2, 2);
        g->SetCamera(obs, poi);

        Vector(poi, 4.5, -1.5, 6);
        Vector(obs, 1, 1, 1);
        o.insertFirst(new TDLight(poi, obs, 0.3), n0);
        //l.Add(new PLight(poi,obs,0.01));
        Vector(poi, -1, 2.5, 8);
        Vector(obs, 1, 1, 1);
        o.insertLast(new TDLight(poi, obs, 0.4), n0);
        //l.Add(new PLight(poi,obs,0.04));
        o.ACurr(n0);

        MDiagT(m, 4, 4, 2, 0, 0, 0);
        o.append(new Cube(O_PLUS, DynSurfConst[2], m));
        MDiagT(m, 1.8, 1.8, 2, 2, 2, 2);
        o.append(new Sphere(O_PLUS, DynSurfConst[1], m));
        MDiagT(m, 1.2, 1.2, 3.8, 2, 3.8, 0);
        o.append(new Cylinder(O_PLUS, DynSurfConst[0], m));
        MDiagT(m, 2, 2, 5.5, 2.5, 1.8, 0);
        o.append(new Cone(O_PLUS, DynSurfConst[3], m));

        MDiagT(m, 1, 1, -1, 0, 0, 0);
        o.append(new HalfSpace(O_PLUS, DynSurfConst[4], m));

        break;

    case ID_PRSTEN:
        Vector(obs, 2, 1, 8);
        Vector(poi, 0, 0, 4);
        g->SetCamera(obs, poi);

        Vector(poi, 4.5, 0.5, 6);
        Vector(obs, 1, 1, 1);
        o.insertFirst(new TDLight(poi, obs, 0.02), n0);
        //l.Add(new PLight(poi,obs,0.02));
        o.ACurr(n0);

        MDiagT(m, 0.9, 0.9, 0.9, 0, 0, 4);
        o.append(new Sphere(O_LOCALPLUS, DynSurfConst[1], m));
        MT(m, 0, 0, 5);
        o.append(new Sphere(O_MINUS, DynSurfConst[3], m));
        MDiagT(m, 0.6, 0.6, 1.5, 0, 0, 3);
        o.append(new Cylinder(O_MINUS, DynSurfConst[0], m));
        MDiagT(m, 1, 1, 1.5, 0, 0, 3);
        o.append(new Cube(O_MINUS, DynSurfConst[2], m));

#if 0
        /* this code is a test of tree.cpp */
        /*---------------------------------*/
        /*
         Root
          |-->A
          |   |-->B
          |   |   |
          |   |   |-->C
          |   |       |--<>D
          |   |       |--<>E
          |   |--<>G
          |
          |--<>F
          */
        using namespace STree;
        Tree *t;
        Branch *b1, *b2;
        Leaf *l;
        bool down = true;
        t = new Tree(/*new KG::Branch((void*)"Root")*/);
        b2 = t->insertFirstBranch(new Name("A-Branch1"));
        b2 = new Branch(new Name("B-Branch2"), b2);
        b1 = new Branch(new Name("C-Branch3"), b2);
        // this inserts the leaf D under C as its first child
        l = new Leaf(new Name("D-Leaf under C"));
        l->insert(b1);
        // this appends the leaf D after branch C as a child of B
        // l=new KG::Leaf((void*)"DLeaf under B after C",b1);
        new Leaf(new Name("E-Leaf after leaf C"), l, true);
        // this would throw an exception - calling wrong virtual fnc.
        //(new KG::Leaf((void*)"ELeaf after leaf C"))->insert(l);
        new Leaf(new Name("F-Leaf after Branch A"), t->getFirst(), SIBLING);
        new Leaf(new Name("G-Leaf in Branch B"), b2, SIBLING);

        t->Top();
        while (t->isValid()) {
            l = t->Curr();
            char *ch = (char*)((Name*)l->getData())->getName();
            long cc = l->getChildrenCount();
            INFOSTR("Tree item mark=%s, Children Count=%ld\n", ch, cc);
            t->Down();
        }

        INFOSTR("Rip and delete branch C...\n");
        b1->rip();
        delete b1;


        l = t->getFirst();
        down = true;
        while (l != NULL) {
            char *ch = (char*)((Name*)l->getData())->getName();
            long cc = l->getChildrenCount();
            INFOSTR("Tree item mark=%s, Children Count=%ld\n", ch, cc);
            do {
                l = t->TreeWalk(l, &down);
                if (l != NULL) {
                    ch = (char*)((Name*)l->getData())->getName();
                    cc = l->getChildrenCount();
                    INFOSTR("  After Walk item=%s, Children Count=%ld, down=%d\n", ch, cc, down);
                }
            } while (l != NULL && !down);
        }
        /*---------------------------------*/
#endif
        break;

    }

    FillTreeList(o.getFirst());
    xscOpened = bool(o.First());

    WireFrame(); // draw defined scene
    canvas->update();
    onKameraStatusBar(0, 0, 0);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
// The entry point where the program starts running
int main(int argc, char **argv)
{
    // globIvArrayInit(); // init buffer for intervals

    FXApp application(app_name, app_name " raytracer");

    application.init(argc, argv);

    new TXSCWindow(&application, app_name);

    // This "realizes" the widget tree.  This is necessary because GUI's are
    // a client-server system, i.e. there are actually two programs involved,
    // a client (in this case, "hello world"), and a server (The X11 server or
    // Windows GDI).  We can build our C++ widgets but something extra is needed
    // to tell the server that we want windows on the screen.  Besides windows,
    // there are various other resources that need to be created, such as icons,
    // fonts, and so on.  This call recurses through the entire widget tree and
    // creates them all, insofar as it can know about them.
    application.create();

    // Now, we actually run the application.  This does not return until we
    // quit. The function run() is a simple loop which gets events from the
    // user, executes them, and then waits for the next event, and so on until
    // we hit the button.
    return application.run();
}
