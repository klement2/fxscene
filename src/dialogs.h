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

#ifndef _DIALOGS_H_ 
#define _DIALOGS_H_

#include "3D/rend.h"
#include "options.h"

class FXOptionsDialog : public FXDialogBox
{
    FXDECLARE(FXOptionsDialog)
protected:
    FXHorizontalFrame *contents;
    FXHorizontalFrame *buttons;
    FXTabBook         *tabbook;
    FXTabItem         *tabRender;
    FXTabItem         *tabDraw;
    FXPopup           *popup;
private:
    FXOptionsDialog()
    {}
public:
    ALL_OPTIONS_OP(FXDataTarget)
    FXTextField *proptThreads;
    FXCheckButton *proptDrawPixel;
    FXCheckButton *proptClearBefore;

    enum {
        ID_TABS_TOP=FXDialogBox::ID_LAST,
        ID_PANEL,
        ID_RTHREADS,
    };
    FXOptionsDialog(FXWindow* owner);
    long onCmdPanel(FXObject*,FXSelector,void*);
    long onCmdAccept(FXObject* o, FXSelector s, void* ptr);
    long onUpdRThreads(FXObject*,FXSelector,void*);
    virtual ~FXOptionsDialog();
};

class FXCameraDialog : public FXDialogBox
{
    FXDECLARE(FXCameraDialog)
protected:
    FXHorizontalFrame *contents;
    FXHorizontalFrame *buttons;
private:
    FXCameraDialog()
    {}
public:
    FXDataTarget obs_x,obs_y,obs_z,poi_x,poi_y,poi_z;
    FXCameraDialog(FXWindow* owner);
    virtual ~FXCameraDialog();
};


#endif /*DIALOGS_H_*/
