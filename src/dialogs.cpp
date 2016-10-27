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

#include "dialogs.h" 
#include "icons.h"

FXDEFMAP(FXOptionsDialog) FXOptionsDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND, FXOptionsDialog::ID_PANEL, FXOptionsDialog::onCmdPanel),
    FXMAPFUNC(SEL_UPDATE, FXOptionsDialog::ID_RTHREADS, FXOptionsDialog::onUpdRThreads),
    FXMAPFUNC(SEL_COMMAND, FXOptionsDialog::ID_ACCEPT, FXOptionsDialog::onCmdAccept),
};

// FXOptionsDialog implementation
FXIMPLEMENT(FXOptionsDialog, FXDialogBox, FXOptionsDialogMap, ARRAYNUMBER(FXOptionsDialogMap))

// Active panel switched
long FXOptionsDialog::onCmdPanel(FXObject* o, FXSelector s, void* ptr)
{
    FXTRACE((1, "Panel = %d\n", (FXint)(long long)ptr));
    return 1;
}

long FXOptionsDialog::onCmdAccept(FXObject* o, FXSelector s, void* ptr)
{
    if (*(FXchar*)roptDrawPixel.getData() != 0)
        *(FXchar*)roptPBar.getData() = 0;

    return ((FXDialogBox*)this)->onCmdAccept(o, s, ptr);
}

long FXOptionsDialog::onUpdRThreads(FXObject* o, FXSelector s, void* ptr)
{
    FXString text = proptThreads->getText();
    if (text == "") {
        text.fromInt(*(FXint*)roptThreads.getData(), 10);
        proptThreads->setText(text);
    }
    bool ok = false;
    FXint value = text.toInt(10, &ok);
    if (!ok || value < 0 || value > 64) {
        text.fromInt(*(FXint*)roptThreads.getData(), 10);
        proptThreads->setText(text);
        return 0;
    }
    *(FXint*)roptThreads.getData() = value;
    if (value != 0) {
        *(FXchar*)roptDrawPixel.getData() = 0;
        *(FXchar*)roptClearBefore.getData() = 0;
        proptDrawPixel->setCheck(false);
        proptDrawPixel->disable();
        proptClearBefore->setCheck(false);
        proptClearBefore->disable();
    } else {
        proptDrawPixel->enable();
        proptClearBefore->enable();
    }
    return 1;
}

// Construct a dialog box
FXOptionsDialog::FXOptionsDialog(FXWindow* owner):
    FXDialogBox(owner, "Options Dialog Box", DECOR_TITLE | DECOR_BORDER)
{
    //FXHorizontalFrame *boxframe;

    // Bottom buttons
    buttons = new FXHorizontalFrame(this, LAYOUT_SIDE_BOTTOM | FRAME_NONE | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH); //,0,0,0,0,20,20,10,10);

    // Separator
    new FXHorizontalSeparator(this, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | SEPARATOR_GROOVE);

    // Contents
    contents = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP | FRAME_NONE | LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_WIDTH);

    // Switcher
    tabbook = new FXTabBook(contents, this, ID_PANEL, PACK_UNIFORM_WIDTH | PACK_UNIFORM_HEIGHT | LAYOUT_FILL_X | LAYOUT_FILL_Y | LAYOUT_RIGHT);
    tabRender = new FXTabItem(tabbook, "Render Options", NULL);

    FXHorizontalFrame *hframe = new FXHorizontalFrame(tabbook, FRAME_THICK | FRAME_RAISED);
    FXMatrix *matrix = new FXMatrix(hframe, 2, MATRIX_BY_COLUMNS | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_THICK); //|FRAME_RAISED);
    new FXCheckButton(matrix, "Shades", &roptShades, FXDataTarget::ID_VALUE, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXLabel(matrix, "Step X", NULL, LAYOUT_SIDE_LEFT | JUSTIFY_LEFT | FRAME_RAISED | LAYOUT_CENTER_Y | LAYOUT_LEFT | JUSTIFY_LEFT | LAYOUT_FILL_ROW);
    FXSpinner *spinner1 = new FXSpinner(matrix, 5, &roptStepX, FXDataTarget::ID_VALUE, SPIN_NORMAL | FRAME_SUNKEN | FRAME_THICK | JUSTIFY_RIGHT | LAYOUT_CENTER_Y | LAYOUT_CENTER_X | LAYOUT_FILL_ROW);
    spinner1->setRange(1, 20);

    new FXLabel(matrix, "Step Y", NULL, LAYOUT_SIDE_LEFT | JUSTIFY_LEFT | FRAME_RAISED | LAYOUT_CENTER_Y | LAYOUT_LEFT | JUSTIFY_LEFT | LAYOUT_FILL_ROW);
    FXSpinner *spinner2 = new FXSpinner(matrix, 5, &roptStepY, FXDataTarget::ID_VALUE, SPIN_NORMAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_LEFT);
    spinner2->setRange(1, 20);

    new FXCheckButton(matrix, "Shift odd lines", &roptShiftOddLines, FXDataTarget::ID_VALUE, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    proptClearBefore = new FXCheckButton(matrix, "Clear before rendering", &roptClearBefore, FXDataTarget::ID_VALUE, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXLabel(matrix, "Background Color", NULL, LAYOUT_SIDE_LEFT | LAYOUT_FILL_ROW);
    new FXColorWell(matrix, 0, &roptBgColor, FXDataTarget::ID_VALUE, LAYOUT_CENTER_Y | LAYOUT_FILL_X | LAYOUT_FILL_ROW, 0, 0, 0, 0, 0, 0, 0, 0);

    proptDrawPixel = new FXCheckButton(matrix, "Draw every pixel", &roptDrawPixel, FXDataTarget::ID_VALUE, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXCheckButton(matrix, "Progress bar", &roptPBar, FXDataTarget::ID_VALUE, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXCheckButton(matrix, "Autorender", &roptAutoRender, FXDataTarget::ID_VALUE, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXFrame(matrix, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    FXGroupBox *group1 = new FXGroupBox(matrix, "Rendering threads", GROUPBOX_TITLE_LEFT | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    proptThreads = new FXTextField(group1, 5, this, FXOptionsDialog::ID_RTHREADS/*FXDataTarget::ID_VALUE*/, TEXTFIELD_INTEGER | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP);
    proptThreads->setText("");

    tabDraw = new FXTabItem(tabbook, "Draw Options", NULL);
    FXHorizontalFrame *hframe1 = new FXHorizontalFrame(tabbook, FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_X | LAYOUT_FILL_Y);

    FXMatrix *matrix1 = new FXMatrix(hframe1, 2, MATRIX_BY_COLUMNS | LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y | FRAME_THICK); //|FRAME_RAISED);

    new FXLabel(matrix1, "Line Style", NULL, LAYOUT_SIDE_LEFT);

    // Even option menus can be hooked up
    popup = new FXPopup(this);
    new FXOption(popup, "LINE_SOLID", NULL, &doptLineStyle, FXDataTarget::ID_OPTION + 0, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
    new FXOption(popup, "LINE_ONOFF_DASH", NULL, &doptLineStyle, FXDataTarget::ID_OPTION + 1, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
    new FXOption(popup, "LINE_DOUBLE_DASH", NULL, &doptLineStyle, FXDataTarget::ID_OPTION + 2, JUSTIFY_HZ_APART | ICON_AFTER_TEXT);

    FXOptionMenu *options = new FXOptionMenu(matrix1, popup, LAYOUT_TOP | FRAME_RAISED | FRAME_THICK | JUSTIFY_HZ_APART | ICON_AFTER_TEXT);
    options->setTarget(&doptLineStyle);
    options->setSelector(FXDataTarget::ID_VALUE);

    new FXCheckButton(matrix1, "Accurate Wire Frame", &doptAccurateWireFrane, FXDataTarget::ID_VALUE, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXFrame(matrix1, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    new FXCheckButton(matrix1, "Auto re-draw on selection change", &doptAutoDrawSelected, FXDataTarget::ID_VALUE, ICON_BEFORE_TEXT | LAYOUT_SIDE_LEFT);
    new FXFrame(matrix1, LAYOUT_FILL_COLUMN | LAYOUT_FILL_ROW);

    // Accept
    new FXButton(buttons, "&Accept", NULL, this, ID_ACCEPT, BUTTON_DEFAULT | BUTTON_INITIAL | FRAME_RAISED | FRAME_THICK | LAYOUT_CENTER_X/*LAYOUT_RIGHT*/ | LAYOUT_CENTER_Y);

    // Cancel
    new FXButton(buttons, "&Cancel", NULL, this, ID_CANCEL, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_CENTER_X | LAYOUT_CENTER_Y);
}


// Must delete the menus
FXOptionsDialog::~FXOptionsDialog()
{
    delete popup;
}


// FXOptionsDialog implementation
FXIMPLEMENT(FXCameraDialog, FXDialogBox, NULL, 0)

// Construct a dialog box
FXCameraDialog::FXCameraDialog(FXWindow* owner):
    FXDialogBox(owner, "Camera Dialog Box", DECOR_TITLE | DECOR_BORDER)
{
    // Bottom buttons
    buttons = new FXHorizontalFrame(this, LAYOUT_SIDE_BOTTOM | FRAME_NONE | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH, 0, 0, 0, 0, 40, 40, 20, 20);
    // Separator
    new FXHorizontalSeparator(this, LAYOUT_SIDE_BOTTOM | LAYOUT_FILL_X | SEPARATOR_GROOVE);
    // Contents
    contents = new FXHorizontalFrame(this, LAYOUT_SIDE_TOP | FRAME_NONE | LAYOUT_FILL_X | LAYOUT_FILL_Y | PACK_UNIFORM_WIDTH);

    FXGroupBox *group1 = new FXGroupBox(contents, "Observer[X,Y,Z]", GROUPBOX_TITLE_CENTER | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    FXGroupBox *group2 = new FXGroupBox(contents, "POI[X,Y,Z]", GROUPBOX_TITLE_CENTER | FRAME_THICK | LAYOUT_FILL_X | LAYOUT_FILL_Y);
    new FXTextField(group1, 20, &obs_x, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP);
    new FXTextField(group1, 20, &obs_y, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP);
    new FXTextField(group1, 20, &obs_z, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP);

    new FXTextField(group2, 20, &poi_x, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP);
    new FXTextField(group2, 20, &poi_y, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP);
    new FXTextField(group2, 20, &poi_z, FXDataTarget::ID_VALUE, TEXTFIELD_REAL | FRAME_SUNKEN | FRAME_THICK | LAYOUT_SIDE_TOP);

    // Accept
    new FXButton(buttons, "&Accept", NULL, this, ID_ACCEPT, BUTTON_DEFAULT | BUTTON_INITIAL | FRAME_RAISED | FRAME_THICK | LAYOUT_CENTER_X | LAYOUT_CENTER_Y);
    // Cancel
    new FXButton(buttons, "&Cancel", NULL, this, ID_CANCEL, BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_CENTER_X | LAYOUT_CENTER_Y);
}

// Must delete the menus
FXCameraDialog::~FXCameraDialog()
{}
