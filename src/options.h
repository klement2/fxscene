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

#ifndef _OPTIONS_H_ 
#define _OPTIONS_H_

#include "3D/base.h"
#include <fx.h>

#define CFG_FILENAME "xscene.cfg"

// Render options
#define OPTION1 roptShades
#define TOPTION1 FXchar

#define OPTION2 roptShiftOddLines
#define TOPTION2 FXchar

#define OPTION3 roptStepX
#define TOPTION3 FXint

#define OPTION4 roptStepY
#define TOPTION4 FXint

#define OPTION5 roptClearBefore
#define TOPTION5 FXchar

#define OPTION6 roptBgColor
#define TOPTION6 FXColor

#define OPTION10 roptPBar
#define TOPTION10 FXchar

#define OPTION11 roptThreads
#define TOPTION11 FXint

#define OPTION12 roptDrawPixel
#define TOPTION12 FXchar

#define OPTION13 roptAutoRender
#define TOPTION13 FXchar

// Drawing options
#define OPTION7 doptLineStyle
#define TOPTION7 FXint

#define OPTION8 doptAccurateWireFrane
#define TOPTION8 FXchar

#define OPTION9 doptAutoDrawSelected
#define TOPTION9 FXchar

#define ALL_OPTIONS_OP(t) \
t OPTION1; \
t OPTION2; \
t OPTION3; \
t OPTION4; \
t OPTION5; \
t OPTION6; \
t OPTION7; \
t OPTION8; \
t OPTION9; \
t OPTION10; \
t OPTION11; \
t OPTION12; \
t OPTION13; \

#define DECL_O(t,o) t o

#define ALL_OPTIONS_DECL \
DECL_O(TOPTION1,OPTION1); \
DECL_O(TOPTION2,OPTION2); \
DECL_O(TOPTION3,OPTION3); \
DECL_O(TOPTION4,OPTION4); \
DECL_O(TOPTION5,OPTION5); \
DECL_O(TOPTION6,OPTION6); \
DECL_O(TOPTION7,OPTION7); \
DECL_O(TOPTION8,OPTION8); \
DECL_O(TOPTION9,OPTION9); \
DECL_O(TOPTION10,OPTION10); \
DECL_O(TOPTION11,OPTION11); \
DECL_O(TOPTION12,OPTION12); \
DECL_O(TOPTION13,OPTION13); \

#define CONNECT_O(a,b,o) a.o.connect(b.o)

#define ALL_OPTIONS_CONNECT(a,b) \
CONNECT_O(a,b,OPTION1); \
CONNECT_O(a,b,OPTION2); \
CONNECT_O(a,b,OPTION3); \
CONNECT_O(a,b,OPTION4); \
CONNECT_O(a,b,OPTION5); \
CONNECT_O(a,b,OPTION6); \
CONNECT_O(a,b,OPTION7); \
CONNECT_O(a,b,OPTION8); \
CONNECT_O(a,b,OPTION9); \
CONNECT_O(a,b,OPTION10); \
CONNECT_O(a,b,OPTION11); \
CONNECT_O(a,b,OPTION12); \
CONNECT_O(a,b,OPTION13); \

typedef struct TOptions {
    ALL_OPTIONS_DECL
} TOptions;


#endif /*OPTIONS_H_*/
