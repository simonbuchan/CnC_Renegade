/*
**	Command & Conquer Renegade(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***********************************************************************************************
 ***                            Confidential - Westwood Studios                              ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Commando                                                     *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/Combat/input.cpp                             $*
 *                                                                                             *
 *                      $Author:: Byon_g                                                      $*
 *                                                                                             *
 *                     $Modtime:: 3/15/02 3:22p                                               $*
 *                                                                                             *
 *                    $Revision:: 120                                                        $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "input.h"

#include <algorithm>
#include <array>

#include "slist.h"
#include "assets.h"
#include "ini.h"
#include "debug.h"
#include "timemgr.h"
#include "registry.h"
#include "ffactory.h"
#include "win.h"
#include "translatedb.h"
#include "string_ids.h"
#include "vehicle.h"
#include "combat.h"
#include "ccamera.h"

#include <stdio.h>
#include <string_view>
#include <vector>

////////////////////////////////////////////////////////////////
//	Special virtual keys
////////////////////////////////////////////////////////////////
#define VK_MOUSEWHEEL_UP      0x100
#define VK_MOUSEWHEEL_DOWN    0x101


/*
**
*/
float MouseSensitivity = 0.5f;
float MouseScale = 1;
bool MouseInvert = true;
bool Mouse2DInvert = false;

const char* DEFAULT_INPUT_FILENAME = "DEFAULT_INPUT.CFG";

#define		BUTTON_BIT_HELD		DirectInput::DI_BUTTON_HELD
#define		BUTTON_BIT_HIT			DirectInput::DI_BUTTON_HIT
#define		BUTTON_BIT_RELEASED	DirectInput::DI_BUTTON_RELEASED
#define		BUTTON_BIT_DOUBLE		8
#define		SLIDER_DEAD_ZONE		0.01f

/*
**	Button states
*/
#define		BUTTON_UP				0x0000
#define		BUTTON_HIT				0x0001
#define		BUTTON_RELEASE			0x0002
#define		BUTTON_HELD				0x0003
#define		BUTTON_DOUBLE			0x0004
#define		BUTTON_SHIFT			0x1000
#define		BUTTON_CTRL			0x2000
#define		BUTTON_ALT			0x4000
#define		BUTTON_DEBUG			0x8000

/*
** Database Strings
*/
#define	ENTRY_ACTIVATE_BUTTON		"ActivateButton"
#define	ENTRY_FUNCTION					"Function"
#define	ENTRY_MIN						"Min"
#define	ENTRY_MAX						"Max"
#define	ENTRY_ACCELERATION			"Acceleration"

#define	SECTION_MISC_SETTINGS		"Misc Settings"
#define	ENTRY_DAMAGE_INDICATORS		"DamageIndicatorsEnabled"
#define	ENTRY_MOUSE_SENSITIVITY		"MouseSensitivity"
#define	ENTRY_MOUSE_SCALE				"MouseScale"
#define	ENTRY_MOUSE_INVERT			"MouseInvert"
#define	ENTRY_MOUSE_2D_INVERT		"Mouse2DInvert"
#define	ENTRY_TARGET_STEERING		"TargetSteering"


struct KeyName
{
    InputKey ID;
    const char* Name;
};

#define KEY_NAME(name) {InputKey::name, #name}
#define KEY_NAME_D(name) {InputKey::_ ## name, #name}

KeyName KeyNames[] = {
    KEY_NAME(F1_Key),
    KEY_NAME(F2_Key),
    KEY_NAME(F3_Key),
    KEY_NAME(F4_Key),
    KEY_NAME(F5_Key),
    KEY_NAME(F6_Key),
    KEY_NAME(F7_Key),
    KEY_NAME(F8_Key),
    KEY_NAME(F9_Key),
    KEY_NAME(F10_Key),
    KEY_NAME(F11_Key),
    KEY_NAME(F12_Key),
    KEY_NAME_D(0_Key),
    KEY_NAME_D(1_Key),
    KEY_NAME_D(2_Key),
    KEY_NAME_D(3_Key),
    KEY_NAME_D(4_Key),
    KEY_NAME_D(5_Key),
    KEY_NAME_D(6_Key),
    KEY_NAME_D(7_Key),
    KEY_NAME_D(8_Key),
    KEY_NAME_D(9_Key),
    KEY_NAME(A_Key),
    KEY_NAME(B_Key),
    KEY_NAME(C_Key),
    KEY_NAME(D_Key),
    KEY_NAME(E_Key),
    KEY_NAME(F_Key),
    KEY_NAME(G_Key),
    KEY_NAME(H_Key),
    KEY_NAME(I_Key),
    KEY_NAME(J_Key),
    KEY_NAME(K_Key),
    KEY_NAME(L_Key),
    KEY_NAME(M_Key),
    KEY_NAME(N_Key),
    KEY_NAME(O_Key),
    KEY_NAME(P_Key),
    KEY_NAME(Q_Key),
    KEY_NAME(R_Key),
    KEY_NAME(S_Key),
    KEY_NAME(T_Key),
    KEY_NAME(U_Key),
    KEY_NAME(V_Key),
    KEY_NAME(W_Key),
    KEY_NAME(X_Key),
    KEY_NAME(Y_Key),
    KEY_NAME(Z_Key),
    KEY_NAME(Minus_Key),
    KEY_NAME(Equals_Key),
    KEY_NAME(Backspace_Key),
    KEY_NAME(Tab_Key),
    KEY_NAME(Left_Bracket_Key),
    KEY_NAME(Right_Bracket_Key),
    KEY_NAME(Enter_Key),
    KEY_NAME(Semicolon_Key),
    KEY_NAME(Apostrophe_Key),
    KEY_NAME(Grave_Key),
    KEY_NAME(Backslash_Key),
    KEY_NAME(Comma_Key),
    KEY_NAME(Period_Key),
    KEY_NAME(Slash_Key),
    KEY_NAME(Space_Bar_Key),
    KEY_NAME(Caps_Lock_Key),
    KEY_NAME(Num_Lock_Key),
    KEY_NAME(Scroll_Lock_Key),
    KEY_NAME(Escape_Key),
    KEY_NAME(Keypad_0_Key),
    KEY_NAME(Keypad_1_Key),
    KEY_NAME(Keypad_2_Key),
    KEY_NAME(Keypad_3_Key),
    KEY_NAME(Keypad_4_Key),
    KEY_NAME(Keypad_5_Key),
    KEY_NAME(Keypad_6_Key),
    KEY_NAME(Keypad_7_Key),
    KEY_NAME(Keypad_8_Key),
    KEY_NAME(Keypad_9_Key),
    KEY_NAME(Keypad_Minus_Key),
    KEY_NAME(Keypad_Star_Key),
    KEY_NAME(Keypad_Plus_Key),
    KEY_NAME(Keypad_Period_Key),
    KEY_NAME(Keypad_Enter_Key),
    KEY_NAME(Keypad_Slash_Key),
    KEY_NAME(Home_Key),
    KEY_NAME(Page_Up_Key),
    KEY_NAME(End_Key),
    KEY_NAME(Page_Down_Key),
    KEY_NAME(Insert_Key),
    KEY_NAME(Delete_Key),
    KEY_NAME(Up_Key),
    KEY_NAME(Down_Key),
    KEY_NAME(Left_Key),
    KEY_NAME(Right_Key),
    KEY_NAME(Sys_Req_Key),
    KEY_NAME(Control_Key),
    KEY_NAME(Left_Control_Key),
    KEY_NAME(Right_Control_Key),
    KEY_NAME(Shift_Key),
    KEY_NAME(Left_Shift_Key),
    KEY_NAME(Right_Shift_Key),
    KEY_NAME(Alt_Key),
    KEY_NAME(Left_Alt_Key),
    KEY_NAME(Right_Alt_Key),
    KEY_NAME(Windows_Key),
    KEY_NAME(Left_Windows_Key),
    KEY_NAME(Right_Windows_Key),
    KEY_NAME(App_Menu_Key),

    KEY_NAME(Left_Mouse_Button),
    KEY_NAME(Right_Mouse_Button),
    KEY_NAME(Center_Mouse_Button),

    KEY_NAME(Joystick_Button_A),
    KEY_NAME(Joystick_Button_B),

    KEY_NAME(Mouse_Left),
    KEY_NAME(Mouse_Right),
    KEY_NAME(Mouse_Up),
    KEY_NAME(Mouse_Down),
    KEY_NAME(Mouse_Wheel_Forward),
    KEY_NAME(Mouse_Wheel_Backward),
    KEY_NAME(Joystick_Left),
    KEY_NAME(Joystick_Right),
    KEY_NAME(Joystick_Up),
    KEY_NAME(Joystick_Down),
};

#define	NUM_FUNCTIONS	INPUT_FUNCTION_COUNT

struct StringID
{
    int ID;
    const char* Name;
};

StringID Functions[NUM_FUNCTIONS] = {

    {INPUT_FUNCTION_MOVE_FORWARD, "MoveForward"},
    {INPUT_FUNCTION_MOVE_BACKWARD, "MoveBackward"},
    {INPUT_FUNCTION_MOVE_LEFT, "MoveLeft"},
    {INPUT_FUNCTION_MOVE_RIGHT, "MoveRight"},
    {INPUT_FUNCTION_MOVE_UP, "MoveUp"},
    {INPUT_FUNCTION_MOVE_DOWN, "MoveDown"},

    {INPUT_FUNCTION_WALK_MODE, "WalkMode"},

    {INPUT_FUNCTION_TURN_LEFT, "TurnLeft"},
    {INPUT_FUNCTION_TURN_RIGHT, "TurnRight"},
    {INPUT_FUNCTION_VEHICLE_TURN_LEFT, "VehicleTurnLeft"},
    {INPUT_FUNCTION_VEHICLE_TURN_RIGHT, "VehicleTurnRight"},
    {INPUT_FUNCTION_VEHICLE_TOGGLE_GUNNER, "VehicleToggleGunner"},

    {INPUT_FUNCTION_WEAPON_UP, "WeaponUp"},
    {INPUT_FUNCTION_WEAPON_DOWN, "WeaponDown"},
    {INPUT_FUNCTION_WEAPON_LEFT, "WeaponLeft"},
    {INPUT_FUNCTION_WEAPON_RIGHT, "WeaponRight"},
    {INPUT_FUNCTION_WEAPON_RESET, "WeaponReset"},

    {INPUT_FUNCTION_ZOOM_IN, "ZoomIn"},
    {INPUT_FUNCTION_ZOOM_OUT, "ZoomOut"},

    {INPUT_FUNCTION_ACTION, "Action"}, // Was Ladder
    {INPUT_FUNCTION_JUMP, "Jump"},
    {INPUT_FUNCTION_CROUCH, "Crouch"},

    {INPUT_FUNCTION_DIVE_FORWARD, "DiveForward"},
    {INPUT_FUNCTION_DIVE_BACKWARD, "DiveBackward"},
    {INPUT_FUNCTION_DIVE_LEFT, "DiveLeft"},
    {INPUT_FUNCTION_DIVE_RIGHT, "DiveRight"},

    {INPUT_FUNCTION_TURN_AROUND, "TurnAround"},
    {INPUT_FUNCTION_DROP_FLAG, "DropFlag"},

    {INPUT_FUNCTION_NEXT_WEAPON, "NextWeapon"},
    {INPUT_FUNCTION_PREV_WEAPON, "PrevWeapon"},
    {INPUT_FUNCTION_FIRE_WEAPON_PRIMARY, "FireWeaponPrimary"},
    {INPUT_FUNCTION_FIRE_WEAPON_SECONDARY, "FireWeaponSecondary"},
    {INPUT_FUNCTION_USE_WEAPON, "UseWeapon"},
    {INPUT_FUNCTION_RELOAD_WEAPON, "ReloadWeapon"},

    {INPUT_FUNCTION_SELECT_NO_WEAPON, "SelectNoWeapon"},
    {INPUT_FUNCTION_SELECT_WEAPON_0, "SelectWeapon0"},
    {INPUT_FUNCTION_SELECT_WEAPON_1, "SelectWeapon1"},
    {INPUT_FUNCTION_SELECT_WEAPON_2, "SelectWeapon2"},
    {INPUT_FUNCTION_SELECT_WEAPON_3, "SelectWeapon3"},
    {INPUT_FUNCTION_SELECT_WEAPON_4, "SelectWeapon4"},
    {INPUT_FUNCTION_SELECT_WEAPON_5, "SelectWeapon5"},
    {INPUT_FUNCTION_SELECT_WEAPON_6, "SelectWeapon6"},
    {INPUT_FUNCTION_SELECT_WEAPON_7, "SelectWeapon7"},
    {INPUT_FUNCTION_SELECT_WEAPON_8, "SelectWeapon8"},
    {INPUT_FUNCTION_SELECT_WEAPON_9, "SelectWeapon9"},

    {INPUT_FUNCTION_CYCLE_POG, "CyclePog"},

    //	{	INPUT_FUNCTION_HUD_ZOOM_RADAR_IN,		"HudZoomRadarIn"		},
    //	{	INPUT_FUNCTION_HUD_ZOOM_RADAR_OUT,		"HudZoomRadarOut"		},

    {INPUT_FUNCTION_PANIC, "Panic"},
    {INPUT_FUNCTION_CURSOR_TARGETING, "CursorTargeting"},
    {INPUT_FUNCTION_FIRST_PERSON_TOGGLE, "FirstPersonToggle"},
    {INPUT_FUNCTION_SUICIDE, "Suicide"},

    {INPUT_FUNCTION_VERBOSE_HELP, "VerboseHelp"},

    {INPUT_FUNCTION_BEGIN_PUBLIC_MESSAGE, "BeginPublicMessage"},
    {INPUT_FUNCTION_BEGIN_TEAM_MESSAGE, "BeginTeamMessage"},
    {INPUT_FUNCTION_BEGIN_PRIVATE_MESSAGE, "BeginPrivateMessage"},
    {INPUT_FUNCTION_BEGIN_CONSOLE, "BeginConsole"},

    {INPUT_FUNCTION_HELP_SCREEN, "HelpScreen"},

    {INPUT_FUNCTION_EVA_OBJECTIVES_SCREEN, "ObjectivesScreen"},
    {INPUT_FUNCTION_EVA_MAP_SCREEN, "MapScreen"},

    // DEBUGGING
    {INPUT_FUNCTION_CAMERA_HEADING_LEFT, "CameraHeadingLeft"},
    {INPUT_FUNCTION_CAMERA_HEADING_RIGHT, "CameraHeadingRight"},

    {INPUT_FUNCTION_CAMERA_TRANSTILT_INC, "CameraTransTiltInc"},
    {INPUT_FUNCTION_CAMERA_TRANSTILT_DEC, "CameraTransTiltDec"},
    {INPUT_FUNCTION_CAMERA_VIEWTILT_INC, "CameraViewTiltInc"},
    {INPUT_FUNCTION_CAMERA_VIEWTILT_DEC, "CameraViewTiltDec"},
    {INPUT_FUNCTION_CAMERA_DIST_INC, "CameraDistInc"},
    {INPUT_FUNCTION_CAMERA_DIST_DEC, "CameraDistDec"},
    {INPUT_FUNCTION_CAMERA_FOV_INC, "CameraFOVInc"},
    {INPUT_FUNCTION_CAMERA_FOV_DEC, "CameraFOVDec"},
    {INPUT_FUNCTION_CAMERA_HEIGHT_INC, "CameraHeightInc"},
    {INPUT_FUNCTION_CAMERA_HEIGHT_DEC, "CameraHeightDec"},

    {INPUT_FUNCTION_MENU_SERVERQUICKSTART, "ServerQuickStart"},
    {INPUT_FUNCTION_MENU_CLIENTQUICKSTART, "ClientQuickStart"},
    {INPUT_FUNCTION_MAKE_SCREEN_SHOT, "MakeScreenShot"},
    {INPUT_FUNCTION_TOGGLE_MOVIE_CAPTURE, "ToggleMovieCapture"},
    //{	INPUT_FUNCTION_INCREMENT_ACTIVE_MESSAGE,	"IncrementActiveMessage"	},
    //{	INPUT_FUNCTION_DECREMENT_ACTIVE_MESSAGE,	"DecrementActiveMessage"	},
    //{	INPUT_FUNCTION_INCREMENT_DEBUG_SWITCH_1,	"IncrementDebugSwitch1"	},
    //{	INPUT_FUNCTION_INCREMENT_DEBUG_SWITCH_2,	"IncrementDebugSwitch2"	},
    {INPUT_FUNCTION_DEBUG_SINGLE_STEP, "DebugSingleStep"},
    {INPUT_FUNCTION_DEBUG_SINGLE_STEP_STEP, "DebugSingleStepStep"},
    {INPUT_FUNCTION_DEBUG_RAPID_MOVE, "DebugRapidMove"},
    {INPUT_FUNCTION_DEBUG_GENERIC0, "GenericDebug0"},
    {INPUT_FUNCTION_DEBUG_GENERIC1, "GenericDebug1"},
    {INPUT_FUNCTION_DEBUG_FAR_CLIP_IN, "DebugFarClipIn"},
    {INPUT_FUNCTION_DEBUG_FAR_CLIP_OUT, "DebugFarClipOut"},
    {INPUT_FUNCTION_QUICK_FULL_EXIT, "QuickFullExit"},
    {INPUT_FUNCTION_VIS_UPDATE, "VisUpdate"},
    //	{	INPUT_FUNCTION_TOGGLE_PERFORMANCE_SAMPLING,	"TogglePerformanceSampling"	},
    {INPUT_FUNCTION_TOGGLE_SNAP_SHOT_MODE, "ToggleSnapShotMode"},
    {INPUT_FUNCTION_SNAP_SHOT_ADVANCE, "SnapShotAdvance"},
    //{	INPUT_FUNCTION_DEBUG_OPTIONS_DIALOG,   "DebugOptionsDialog"	},
    {INPUT_FUNCTION_CNC, "CNC"},
    {INPUT_FUNCTION_QUICKSAVE, "Quicksave"},

    // Menu
    {INPUT_FUNCTION_MENU_TOGGLE, "MenuToggle"},

    // In game EVA
    {INPUT_FUNCTION_EVA_MISSION_OBJECTIVES_TOGGLE, "EvaMissionObjectives"},

    {INPUT_FUNCTION_RADIO_CMD_01, "RadioCommand01"},
    {INPUT_FUNCTION_RADIO_CMD_02, "RadioCommand02"},
    {INPUT_FUNCTION_RADIO_CMD_03, "RadioCommand03"},
    {INPUT_FUNCTION_RADIO_CMD_04, "RadioCommand04"},
    {INPUT_FUNCTION_RADIO_CMD_05, "RadioCommand05"},
    {INPUT_FUNCTION_RADIO_CMD_06, "RadioCommand06"},
    {INPUT_FUNCTION_RADIO_CMD_07, "RadioCommand07"},
    {INPUT_FUNCTION_RADIO_CMD_08, "RadioCommand08"},
    {INPUT_FUNCTION_RADIO_CMD_09, "RadioCommand09"},
    {INPUT_FUNCTION_RADIO_CMD_10, "RadioCommand10"},

    {INPUT_FUNCTION_RADIO_CMD_11, "RadioCommand11"},
    {INPUT_FUNCTION_RADIO_CMD_12, "RadioCommand12"},
    {INPUT_FUNCTION_RADIO_CMD_13, "RadioCommand13"},
    {INPUT_FUNCTION_RADIO_CMD_14, "RadioCommand14"},
    {INPUT_FUNCTION_RADIO_CMD_15, "RadioCommand15"},
    {INPUT_FUNCTION_RADIO_CMD_16, "RadioCommand16"},
    {INPUT_FUNCTION_RADIO_CMD_17, "RadioCommand17"},
    {INPUT_FUNCTION_RADIO_CMD_18, "RadioCommand18"},
    {INPUT_FUNCTION_RADIO_CMD_19, "RadioCommand19"},
    {INPUT_FUNCTION_RADIO_CMD_20, "RadioCommand20"},

    {INPUT_FUNCTION_RADIO_CMD_21, "RadioCommand21"},
    {INPUT_FUNCTION_RADIO_CMD_22, "RadioCommand22"},
    {INPUT_FUNCTION_RADIO_CMD_23, "RadioCommand23"},
    {INPUT_FUNCTION_RADIO_CMD_24, "RadioCommand24"},
    {INPUT_FUNCTION_RADIO_CMD_25, "RadioCommand25"},
    {INPUT_FUNCTION_RADIO_CMD_26, "RadioCommand26"},
    {INPUT_FUNCTION_RADIO_CMD_27, "RadioCommand27"},
    {INPUT_FUNCTION_RADIO_CMD_28, "RadioCommand28"},
    {INPUT_FUNCTION_RADIO_CMD_29, "RadioCommand29"},
    {INPUT_FUNCTION_RADIO_CMD_30, "RadioCommand30"},

    {INPUT_FUNCTION_TEAM_INFO_TOGGLE, "TeamInfoToggle"},
    {INPUT_FUNCTION_BATTLE_INFO_TOGGLE, "BattleInfoToggle"},
    {INPUT_FUNCTION_SERVER_INFO_TOGGLE, "ServerInfoToggle"},

    // Profiler Controls
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD0, "ProfileEnterChild0"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD1, "ProfileEnterChild1"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD2, "ProfileEnterChild2"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD3, "ProfileEnterChild3"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD4, "ProfileEnterChild4"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD5, "ProfileEnterChild5"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD6, "ProfileEnterChild6"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD7, "ProfileEnterChild7"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD8, "ProfileEnterChild8"},
    {INPUT_FUNCTION_PROFILE_ENTER_CHILD9, "ProfileEnterChild9"},
    {INPUT_FUNCTION_PROFILE_ENTER_PARENT, "ProfileEnterParent"},
    {INPUT_FUNCTION_PROFILE_RESET, "ProfileReset"},

};


/*
** FunctionKeyStates
*/
int FunctionKeyStates[NUM_FUNCTIONS] =
{
    BUTTON_HELD, //INPUT_FUNCTION_MOVE_FORWARD,
    BUTTON_HELD, //INPUT_FUNCTION_MOVE_BACKWARD,
    BUTTON_HELD, //INPUT_FUNCTION_MOVE_LEFT,
    BUTTON_HELD, //INPUT_FUNCTION_MOVE_RIGHT,
    BUTTON_HELD, //INPUT_FUNCTION_MOVE_UP,
    BUTTON_HELD, //INPUT_FUNCTION_MOVE_DOWN,

    BUTTON_HELD, //INPUT_FUNCTION_WALK_MODE,

    BUTTON_HELD, //INPUT_FUNCTION_TURN_LEFT,
    BUTTON_HELD, //INPUT_FUNCTION_TURN_RIGHT,

    BUTTON_HELD, //INPUT_FUNCTION_VEHICLE_TURN_LEFT,
    BUTTON_HELD, //INPUT_FUNCTION_VEHICLE_TURN_RIGHT,
    BUTTON_HIT, //INPUT_FUNCTION_VEHICLE_TOGGLE_GUNNER,

    BUTTON_HELD, //INPUT_FUNCTION_WEAPON_UP,
    BUTTON_HELD, //INPUT_FUNCTION_WEAPON_DOWN,
    BUTTON_HELD, //INPUT_FUNCTION_WEAPON_LEFT,
    BUTTON_HELD, //INPUT_FUNCTION_WEAPON_RIGHT,
    BUTTON_HELD, //INPUT_FUNCTION_WEAPON_RESET,

    BUTTON_HELD, //INPUT_FUNCTION_ZOOM_IN,
    BUTTON_HELD, //INPUT_FUNCTION_ZOOM_OUT,

    BUTTON_HIT, //INPUT_FUNCTION_ACTION,
    BUTTON_HIT, //INPUT_FUNCTION_JUMP,
    //	BUTTON_HIT, //INPUT_FUNCTION_CROUCH,
    BUTTON_HELD, //INPUT_FUNCTION_CROUCH,

    BUTTON_DOUBLE, //INPUT_FUNCTION_DIVE_FORWARD,
    BUTTON_DOUBLE, //INPUT_FUNCTION_DIVE_BACKWARD,
    BUTTON_DOUBLE, //INPUT_FUNCTION_DIVE_LEFT,
    BUTTON_DOUBLE, //INPUT_FUNCTION_DIVE_RIGHT,

    BUTTON_HIT, //INPUT_FUNCTION_TURN_AROUND,
    BUTTON_HIT, //INPUT_FUNCTION_DROP_FLAG,

    BUTTON_HIT, //INPUT_FUNCTION_NEXT_WEAPON,
    BUTTON_HIT, //INPUT_FUNCTION_PREV_WEAPON,
    BUTTON_HELD, //INPUT_FUNCTION_FIRE_WEAPON_PRIMARY,
    BUTTON_HELD, //INPUT_FUNCTION_FIRE_WEAPON_SECONDARY,
    BUTTON_HIT, //INPUT_FUNCTION_USE_WEAPON,
    BUTTON_HIT, //INPUT_FUNCTION_RELOAD_WEAPON,

    BUTTON_HIT, //INPUT_FUNCTION_SELECT_NO_WEAPON,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_0,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_1,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_2,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_3,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_4,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_5,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_6,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_7,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_8,
    BUTTON_HIT, //INPUT_FUNCTION_SELECT_WEAPON_9,

    BUTTON_HIT, //INPUT_FUNCTION_CYCLE_POG

    //	BUTTON_HIT, //INPUT_FUNCTION_HUD_ZOOM_RADAR_IN,
    //	BUTTON_HIT, //INPUT_FUNCTION_HUD_ZOOM_RADAR_OUT,

    BUTTON_HIT, //INPUT_FUNCTION_PANIC,
    BUTTON_HELD, //INPUT_FUNCTION_CURSOR_TARGETING,
    BUTTON_HIT, //INPUT_FUNCTION_FIRST_PERSON_TOGGLE,
    BUTTON_HIT, //INPUT_FUNCTION_SUICIDE,

    BUTTON_HIT, //INPUT_FUNCTION_VERBOSE_HELP,

    BUTTON_HIT, //INPUT_FUNCTION_BEGIN_PUBLIC_MESSAGE,
    BUTTON_HIT, //INPUT_FUNCTION_BEGIN_TEAM_MESSAGE,
    BUTTON_HIT, //INPUT_FUNCTION_BEGIN_PRIVATE_MESSAGE,
    BUTTON_HIT, //INPUT_FUNCTION_BEGIN_CONSOLE,

    BUTTON_HIT, //INPUT_FUNCTION_HELP_SCREEN,

    BUTTON_HIT, //INPUT_FUNCTION_EVA_OBJECTIVES_SCREEN
    BUTTON_HIT, //INPUT_FUNCTION_EVA_MAP_SCREEN

    // DEBUGGING
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_HEADING_LEFT,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_HEADING_RIGHT,

    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_TRANSTILT_INC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_TRANSTILT_DEC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_VIEWTILT_INC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_VIEWTILT_DEC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_DIST_INC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_DIST_DEC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_FOV_INC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_FOV_DEC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_HEIGHT_INC,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_CAMERA_HEIGHT_DEC,

    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_MENU_SERVERQUICKSTART,
    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_MENU_CLIENTQUICKSTART,
    BUTTON_HIT, //INPUT_FUNCTION_MAKE_SCREEN_SHOT,
    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_TOGGLE_MOVIE_CAPTURE,

    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_DEBUG_SINGLE_STEP,
    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_DEBUG_SINGLE_STEP_STEP,
    BUTTON_HELD, //INPUT_FUNCTION_DEBUG_RAPID_MOVE,
    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_DEBUG_GENERIC0,
    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_DEBUG_GENERIC1,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_DEBUG_FAR_CLIP_IN,
    BUTTON_HELD | BUTTON_DEBUG, //INPUT_FUNCTION_DEBUG_FAR_CLIP_OUT,
    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_QUICK_FULL_EXIT,
    BUTTON_HIT | BUTTON_DEBUG, //INPUT_FUNCTION_VIS_UPDATE,
    BUTTON_HIT, //INPUT_FUNCTION_TOGGLE_SNAP_SHOT_MODE,
    BUTTON_HIT, //INPUT_FUNCTION_SNAP_SHOT_ADVANCE,
    BUTTON_HIT, //INPUT_FUNCTION_CNC,
    BUTTON_HIT, //INPUT_FUNCTION_QUICKSAVE,

    // Menu
    BUTTON_HIT, //INPUT_FUNCTION_MENU_TOGGLE,

    // In game EVA
    BUTTON_HIT, //INPUT_FUNCTION_EVA_MISSION_OBJECTIVES_TOGGLE,

    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_01
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_02
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_03
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_04
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_05
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_06
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_07
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_08
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_09
    BUTTON_HIT | BUTTON_CTRL, // INPUT_FUNCTION_CNC_RADIO_CMD_10

    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_11,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_12,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_13,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_14,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_15,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_16,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_17,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_18,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_19,
    BUTTON_HIT | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_20,

    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_21,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_22,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_23,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_24,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_25,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_26,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_27,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_28,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_29,
    BUTTON_HIT | BUTTON_CTRL | BUTTON_ALT, // INPUT_FUNCTION_RADIO_CMD_30,

    BUTTON_HIT, //INPUT_FUNCTION_TEAM_INFO_TOGGLE,
    BUTTON_HIT, //INPUT_FUNCTION_BATTLE_INFO_TOGGLE,
    BUTTON_HIT, //INPUT_FUNCTION_SERVER_INFO_TOGGLE

    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD0,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD1,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD2,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD3,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD4,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD5,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD6,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD7,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD8,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_CHILD9,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_ENTER_PARENT,
    BUTTON_HIT, //INPUT_FUNCTION_PROFILE_RESET,
};


/*
**
*/
class AcceleratedKeyDef
{
public:
    float Value;
    short Function;
    float Min;
    float Max;
    float Acceleration;
};


/*
**
*/
float Input::Sliders[NUM_SLIDERS];
bool Input::ConsoleMode = false;
bool Input::MenuMode = false;
int Input::Queue[Input::QUEUE_MAX];
int Input::QueueHead;
int Input::QueueTail;
int Input::QueueSize;
Vector2 Input::CursorPos;
float Input::FunctionValue[INPUT_FUNCTION_COUNT];
float Input::FunctionClamp[INPUT_FUNCTION_COUNT];
InputKey Input::FunctionPrimaryKeys[INPUT_FUNCTION_COUNT];
InputKey Input::FunctionSecondaryKeys[INPUT_FUNCTION_COUNT];
bool Input::DamageIndicatorsEnabled = true;

SimpleDynVecClass<AcceleratedKeyDef> AcceleratedKeyList;

int _StatsHelpScreen;
int _StatsObjectives;
int _StatsMap;
int _StatsMenu;

static void enable_raw_input(bool enabled)
{
    DWORD flags = enabled ? 0 : RIDEV_REMOVE;
    auto items = std::array{
        RAWINPUTDEVICE{
            .usUsagePage = 1, // HID_USAGE_PAGE_GENERIC
            .usUsage = 2, // HID_USAGE_GENERIC_MOUSE
            .dwFlags = flags,
        },
        // 4 is joystick, but what lunatic actually uses a joystick for an FPS...
        RAWINPUTDEVICE{
            .usUsagePage = 1, // HID_USAGE_PAGE_GENERIC
            .usUsage = 6, // HID_USAGE_GENERIC_KEYBOARD
            .dwFlags = flags,
        },
    };
    RegisterRawInputDevices(items.data(), items.size(), sizeof(*items.data()));
}

struct KeyState
{
    bool held : 1;
    bool hit : 1;
    bool released : 1;
    bool double_hit : 1;

    void down()
    {
        held = true;
        hit = true;
    }

    void up()
    {
        held = false;
        released = true;
    }

    KeyState operator|(KeyState other) const
    {
        return {
            .held = held || other.held,
            .hit = hit || other.hit,
            .released = released || other.released,
            .double_hit = double_hit || other.double_hit,
        };
    }
};

static std::array<float, (int)InputKey::Button_Count> key_last_hit;
static std::array<KeyState, (int)InputKey::Button_Count> key_states;
static InputKey last_key = InputKey::Unset;

static void raw_keyboard_input(RAWKEYBOARD msg)
{
    auto down = (msg.Flags & RI_KEY_BREAK) == 0;
    auto key = InputKey::Unset;
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#scan-codes
    switch (msg.MakeCode)
    {
    case 0x1E: key = InputKey::A_Key; break;
    case 0x30: key = InputKey::B_Key; break;
    case 0x2E: key = InputKey::C_Key; break;
    case 0x20: key = InputKey::D_Key; break;
    case 0x12: key = InputKey::E_Key; break;
    case 0x21: key = InputKey::F_Key; break;
    case 0x22: key = InputKey::G_Key; break;
    case 0x23: key = InputKey::H_Key; break;
    case 0x17: key = InputKey::I_Key; break;
    case 0x24: key = InputKey::J_Key; break;
    case 0x25: key = InputKey::K_Key; break;
    case 0x26: key = InputKey::L_Key; break;
    case 0x32: key = InputKey::M_Key; break;
    case 0x31: key = InputKey::N_Key; break;
    case 0x18: key = InputKey::O_Key; break;
    case 0x19: key = InputKey::P_Key; break;
    case 0x10: key = InputKey::Q_Key; break;
    case 0x13: key = InputKey::R_Key; break;
    case 0x1F: key = InputKey::S_Key; break;
    case 0x14: key = InputKey::T_Key; break;
    case 0x16: key = InputKey::U_Key; break;
    case 0x2F: key = InputKey::V_Key; break;
    case 0x11: key = InputKey::W_Key; break;
    case 0x2D: key = InputKey::X_Key; break;
    case 0x15: key = InputKey::Y_Key; break;
    case 0x2C: key = InputKey::Z_Key; break;
    case 0x02: key = InputKey::_1_Key; break;
    case 0x03: key = InputKey::_2_Key; break;
    case 0x04: key = InputKey::_3_Key; break;
    case 0x05: key = InputKey::_4_Key; break;
    case 0x06: key = InputKey::_5_Key; break;
    case 0x07: key = InputKey::_6_Key; break;
    case 0x08: key = InputKey::_7_Key; break;
    case 0x09: key = InputKey::_8_Key; break;
    case 0x0A: key = InputKey::_9_Key; break;
    case 0x0B: key = InputKey::_0_Key; break;
    case 0x1C: key = InputKey::Enter_Key; break;
    case 0x01: key = InputKey::Escape_Key; break;
    case 0x0E: key = InputKey::Backspace_Key; break;
    case 0x0F: key = InputKey::Tab_Key; break;
    case 0x39: key = InputKey::Space_Bar_Key; break;
    case 0x0C: key = InputKey::Minus_Key; break;
    case 0x0D: key = InputKey::Equals_Key; break;
    case 0x1A: key = InputKey::Left_Bracket_Key; break;
    case 0x1B: key = InputKey::Right_Bracket_Key; break;
    case 0x2B: key = InputKey::Backslash_Key; break;
    case 0x27: key = InputKey::Semicolon_Key; break;
    case 0x28: key = InputKey::Apostrophe_Key; break;
    case 0x29: key = InputKey::Grave_Key; break;
    case 0x33: key = InputKey::Comma_Key; break;
    case 0x34: key = InputKey::Period_Key; break;
    case 0x35: key = InputKey::Slash_Key; break;
    case 0x3A: key = InputKey::Caps_Lock_Key; break;
    case 0x3B: key = InputKey::F1_Key; break;
    case 0x3C: key = InputKey::F2_Key; break;
    case 0x3D: key = InputKey::F3_Key; break;
    case 0x3E: key = InputKey::F4_Key; break;
    case 0x3F: key = InputKey::F5_Key; break;
    case 0x40: key = InputKey::F6_Key; break;
    case 0x41: key = InputKey::F7_Key; break;
    case 0x42: key = InputKey::F8_Key; break;
    case 0x43: key = InputKey::F9_Key; break;
    case 0x44: key = InputKey::F10_Key; break;
    case 0x57: key = InputKey::F11_Key; break;
    case 0x58: key = InputKey::F12_Key; break;
    case 0x46: key = InputKey::Scroll_Lock_Key; break;
    case 0xE052: key = InputKey::Insert_Key; break;
    case 0xE047: key = InputKey::Home_Key; break;
    case 0xE049: key = InputKey::Page_Up_Key; break;
    case 0xE053: key = InputKey::Delete_Key; break;
    case 0xE04F: key = InputKey::End_Key; break;
    case 0xE051: key = InputKey::Page_Down_Key; break;
    case 0xE04D: key = InputKey::Right_Key; break;
    case 0xE04B: key = InputKey::Left_Key; break;
    case 0xE050: key = InputKey::Down_Key; break;
    case 0xE048: key = InputKey::Up_Key; break;
    case 0x45: key = InputKey::Num_Lock_Key; break;
    case 0xE045: key = InputKey::Num_Lock_Key; break;
    case 0xE035: key = InputKey::Keypad_Slash_Key; break;
    case 0x37: key = InputKey::Keypad_Star_Key; break;
    case 0x4A: key = InputKey::Keypad_Minus_Key; break;
    case 0x4E: key = InputKey::Keypad_Plus_Key; break;
    case 0xE01C: key = InputKey::Keypad_Enter_Key; break;
    case 0x4F: key = InputKey::Keypad_1_Key; break;
    case 0x50: key = InputKey::Keypad_2_Key; break;
    case 0x51: key = InputKey::Keypad_3_Key; break;
    case 0x4B: key = InputKey::Keypad_4_Key; break;
    case 0x4C: key = InputKey::Keypad_5_Key; break;
    case 0x4D: key = InputKey::Keypad_6_Key; break;
    case 0x47: key = InputKey::Keypad_7_Key; break;
    case 0x48: key = InputKey::Keypad_8_Key; break;
    case 0x49: key = InputKey::Keypad_9_Key; break;
    case 0x52: key = InputKey::Keypad_0_Key; break;
    case 0x53: key = InputKey::Keypad_Period_Key; break;
    case 0xE05D: key = InputKey::App_Menu_Key; break;
    case 0x1D: key = InputKey::Left_Control_Key; break;
    case 0x2A: key = InputKey::Left_Shift_Key; break;
    case 0x38: key = InputKey::Left_Alt_Key; break;
    case 0xE05B: key = InputKey::Left_Windows_Key; break;
    case 0xE01D: key = InputKey::Right_Control_Key; break;
    case 0x36: key = InputKey::Right_Shift_Key; break;
    case 0xE038: key = InputKey::Right_Alt_Key; break;
    case 0xE05C: key = InputKey::Right_Windows_Key; break;
    }
    if (key == InputKey::Unset)
        return;
    last_key = key;
    auto index = (int)key;
    if (down)
        key_states[index].down();
    else
        key_states[index].up();
}

struct RawMousePixels
{
    int x, y;

    RawMousePixels operator-(RawMousePixels other) const
    {
        return { .x = x - other.x, .y = y - other.y };
    }

    RawMousePixels operator+=(RawMousePixels other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
};

struct RawMouseState
{
    std::optional<RawMousePixels> absolute;
    RawMousePixels relative;
    int wheel_delta;
    bool eat_mouse_held;
};

static RawMouseState raw_mouse_state = {};

// https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawmouse
static void raw_mouse_input(RAWMOUSE msg)
{
    // This computes the absolute position in pixels. Not sure this makes the most sense for game input?
    // Doesn't seem to make any promises about when it sends absolute vs relative, but common sense says
    // one absolute message when we get focus and then relative messages after that, in which case we can
    // just ignore absolute...
    if (msg.usFlags & MOUSE_MOVE_ABSOLUTE)
    {
        RECT rect;
        if (msg.usFlags & MOUSE_VIRTUAL_DESKTOP)
        {
            rect.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
            rect.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
            rect.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
            rect.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        }
        else
        {
            rect.left = 0;
            rect.top = 0;
            rect.right = GetSystemMetrics(SM_CXSCREEN);
            rect.bottom = GetSystemMetrics(SM_CYSCREEN);
        }

        auto next = RawMousePixels{
            .x = MulDiv(msg.lLastX, rect.right, USHRT_MAX) + rect.left,
            .y = MulDiv(msg.lLastY, rect.bottom, USHRT_MAX) + rect.top,
        };

        if (raw_mouse_state.absolute)
        {
            raw_mouse_state.relative = next - *raw_mouse_state.absolute;
        }
        else
        {
            raw_mouse_state.relative = {};
        }

        raw_mouse_state.absolute = next;
    }
    else
    {
        raw_mouse_state.relative = { .x = msg.lLastX, .y = msg.lLastY };
        if (raw_mouse_state.absolute)
            *raw_mouse_state.absolute += raw_mouse_state.relative;
    }

    auto& left_button = key_states[(int)InputKey::Left_Mouse_Button];
    auto& right_button = key_states[(int)InputKey::Right_Mouse_Button];
    auto& center_button = key_states[(int)InputKey::Center_Mouse_Button];

    if (msg.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
        left_button.down();
    if (msg.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
    {
        left_button.up();
        raw_mouse_state.eat_mouse_held = false;
    }
    if (msg.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
        right_button.down();
    if (msg.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
        right_button.up();
    if (msg.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
        center_button.down();
    if (msg.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
        center_button.up();
    if (msg.usButtonFlags & RI_MOUSE_WHEEL)
        raw_mouse_state.wheel_delta = (short)msg.usButtonData;
    else
        raw_mouse_state.wheel_delta = 0;
}

/*
**
*/
void Input::Init()
{
    ConsoleMode = false;
    MenuMode = false;

    _StatsHelpScreen = 0;
    _StatsObjectives = 0;
    _StatsMap = 0;
    _StatsMenu = 0;

    std::ranges::fill(FunctionValue, 0.0);

    enable_raw_input(true);
}

void Input::Shutdown(void)
{
    enable_raw_input(false);

    Free_Mappings();

    Debug_Say(( "Help Screen Hits: %d\n", _StatsHelpScreen ));
    Debug_Say(( " Objectives Hits: %d\n", _StatsObjectives ));
    Debug_Say(( " Map Screen Hits: %d\n", _StatsMap ));
    Debug_Say(( "Menu Screen Hits: %d\n", _StatsMenu ));

    return;
}


void Input::Free_Mappings(void)
{
    //
    //	Simply reset the key arrays
    //
    std::ranges::fill(FunctionPrimaryKeys, InputKey::Unset);
    std::ranges::fill(FunctionSecondaryKeys, InputKey::Unset);

    //
    //	Free the accelerated key list
    //
    AcceleratedKeyList.Delete_All();
}

float Input::Get_Mouse_Sensitivity(void)
{
    return MouseSensitivity;
}

void Input::Set_Mouse_Sensitivity(float mouse_sensitivity)
{
    // save this in the registry...
    MouseSensitivity = WWMath::Clamp(mouse_sensitivity, 0, 1);

    /*
    When MouseSensitivity == 0, set MouseScale to 0.00025
    When MouseSensitivity == 1, set MouseScale to 0.025
    When MouseSensitivity == x, set MouseScale to 25 * 10^(2x-5)
    */

    MouseScale = 25.0f * pow(10.0f, 2 * MouseSensitivity - 5);
}

bool Input::Get_Mouse_Invert(void)
{
    return MouseInvert;
}

void Input::Set_Mouse_Invert(bool invert)
{
    // save this in the registry...
    MouseInvert = invert;
}

bool Input::Get_Mouse_2D_Invert(void)
{
    return Mouse2DInvert;
}

void Input::Set_Mouse_2D_Invert(bool invert)
{
    // save this in the registry...
    Mouse2DInvert = invert;
}

void Input::Flush()
{
    std::ranges::fill(key_states, KeyState{});
    std::ranges::fill(key_last_hit, 0.0f);
    last_key = InputKey::Unset;
    raw_mouse_state = {};
}

void Input::Update_State()
{
    float now = TimeManager::Get_Total_Seconds();

    key_states[(int)InputKey::Control_Key] =
        key_states[(int)InputKey::Left_Control_Key] |
        key_states[(int)InputKey::Right_Control_Key];
    key_states[(int)InputKey::Alt_Key] =
        key_states[(int)InputKey::Left_Alt_Key] |
        key_states[(int)InputKey::Right_Alt_Key];
    key_states[(int)InputKey::Shift_Key] =
        key_states[(int)InputKey::Left_Shift_Key] |
        key_states[(int)InputKey::Right_Shift_Key];

    if (raw_mouse_state.eat_mouse_held)
        key_states[(int)InputKey::Left_Mouse_Button] = {.released = true};

    for (int i = 0; i < (int)InputKey::Button_Count; i++)
    {
        auto& state = key_states[i];
        if (state.hit)
        {
            if ((now - key_last_hit[i]) < 0.25f)
            {
                state.double_hit = true;
            }
            key_last_hit[i] = now;
        }
    }

    float wheel_scale = -0.05f;

    float mouse_x = MouseScale * (float)raw_mouse_state.relative.x;
    float mouse_y = MouseScale * (float)raw_mouse_state.relative.y;
    float mouse_z = wheel_scale * (float)raw_mouse_state.wheel_delta;
    raw_mouse_state = {};

    if (TimeManager::Get_Frame_Real_Seconds() > 0.0f)
    {
        mouse_x /= TimeManager::Get_Frame_Real_Seconds();
        mouse_y /= TimeManager::Get_Frame_Real_Seconds();
    }
    else
    {
        mouse_x = 0.0f;
        mouse_y = 0.0f;
        mouse_z = 0.0f;
    }

    WWASSERT(WWMath::Is_Valid_Float(mouse_x));
    WWASSERT(WWMath::Is_Valid_Float(mouse_y));

    // it comes in as inverted
    if (!MouseInvert)
    {
        mouse_y = -mouse_y;
    }

    //	Debug_Say(( "Mouse %f %f\n", mouse_x, mouse_y ));

    Sliders[SLIDER_MOUSE_LEFT - FIRST_SLIDER] = MAX(-mouse_x, 0.0f);
    Sliders[SLIDER_MOUSE_RIGHT - FIRST_SLIDER] = MAX(mouse_x, 0.0f);
    Sliders[SLIDER_MOUSE_UP - FIRST_SLIDER] = MAX(-mouse_y, 0.0f);
    Sliders[SLIDER_MOUSE_DOWN - FIRST_SLIDER] = MAX(mouse_y, 0.0f);
    Sliders[SLIDER_MOUSE_WHEEL_FORWARD - FIRST_SLIDER] = MAX(-mouse_z, 0.0f);
    Sliders[SLIDER_MOUSE_WHEEL_BACKWARD - FIRST_SLIDER] = MAX(mouse_z, 0.0f);

#ifdef INPUT_JOYSTICK
    //
    // Update joystick sliders
    //
    float joystick_x = (float)DirectInput::Get_Joystick_Axis_State(DirectInput::JOYSTICK_X_AXIS) / 1000.0f;
    float joystick_y = (float)DirectInput::Get_Joystick_Axis_State(DirectInput::JOYSTICK_Y_AXIS) / 1000.0f;

    const float DEAD_ZONE = 0.15f;
    if (WWMath::Fabs(joystick_x) < DEAD_ZONE)
    {
        joystick_x = 0.0f;
    }
    else
    {
        joystick_x = WWMath::Sign(joystick_x) * (WWMath::Fabs(joystick_x) - DEAD_ZONE) * 1.5f / (1.0f - DEAD_ZONE);
    }

    Sliders[SLIDER_JOYSTICK_LEFT - FIRST_SLIDER] = MAX(-joystick_x, 0.0f);
    Sliders[SLIDER_JOYSTICK_RIGHT - FIRST_SLIDER] = MAX(joystick_x, 0.0f);
    Sliders[SLIDER_JOYSTICK_UP - FIRST_SLIDER] = MAX(-joystick_y, 0.0f);
    Sliders[SLIDER_JOYSTICK_DOWN - FIRST_SLIDER] = MAX(joystick_y, 0.0f);
#endif
}

void Input::Update(void)
{
    if (!GameInFocus)
    {
        Flush();
        return;
    }

    Update_State();

    //
    // zero all values
    //
    std::ranges::fill(FunctionValue, 0.0f);

    // No ESC, O, M, F1, etc in cinematics!
    if (COMBAT_CAMERA && COMBAT_CAMERA->Is_In_Cinematic() && !DebugManager::Allow_Cinematic_Keys())
    {
        return;
    }

    //
    //	Don't process function inputs if we're in the menu
    //
    if (MenuMode == false || ConsoleMode)
    {
        //
        //	Update the value of each function based on what keys are down
        //
        for (int index = 0; index < INPUT_FUNCTION_COUNT; index++)
        {
            float value1 = Get_Value(index, FunctionPrimaryKeys[index], 1.0F);
            float value2 = Get_Value(index, FunctionSecondaryKeys[index], 1.0F);
            FunctionValue[index] = max(value1, value2);
        }

        _StatsHelpScreen += Get_State(INPUT_FUNCTION_HELP_SCREEN) ? 1 : 0;
        _StatsObjectives += Get_State(INPUT_FUNCTION_EVA_OBJECTIVES_SCREEN) ? 1 : 0;
        _StatsMap += Get_State(INPUT_FUNCTION_EVA_MAP_SCREEN) ? 1 : 0;
        _StatsMenu += Get_State(INPUT_FUNCTION_MENU_TOGGLE) ? 1 : 0;

        //
        //	Special case sniping mode.  In sniping mode we make the primary zoom-in and zoom-out
        // keys exclusive.
        //
        if (COMBAT_CAMERA->Is_Star_Sniping())
        {
            auto key1 = FunctionPrimaryKeys[INPUT_FUNCTION_ZOOM_IN];
            auto key2 = FunctionPrimaryKeys[INPUT_FUNCTION_ZOOM_OUT];

            //
            //	Loop over and zero out any function that uses either of these primary keys
            //
            for (int index = 0; index < INPUT_FUNCTION_COUNT; index++)
            {
                //
                //	Does this function use one of the keys that is mapped to the zoom functions?
                //
                if (FunctionPrimaryKeys[index] == key1 || FunctionSecondaryKeys[index] == key1 ||
                    FunctionPrimaryKeys[index] == key2 || FunctionSecondaryKeys[index] == key2)
                {
                    //
                    //	Zero out this function as necessary
                    //
                    if (index != INPUT_FUNCTION_ZOOM_IN && index != INPUT_FUNCTION_ZOOM_OUT)
                    {
                        FunctionValue[index] = 0;
                    }
                }
            }
        }

        //
        //	Special case the vehicle turning so it inherits the values
        // of the strafe command
        //
        FunctionValue[INPUT_FUNCTION_VEHICLE_TURN_LEFT] = max(FunctionValue[INPUT_FUNCTION_MOVE_LEFT],
                                                              FunctionValue[INPUT_FUNCTION_TURN_LEFT]);
        FunctionValue[INPUT_FUNCTION_VEHICLE_TURN_RIGHT] = max(FunctionValue[INPUT_FUNCTION_MOVE_RIGHT],
                                                               FunctionValue[INPUT_FUNCTION_TURN_RIGHT]);

        //
        // Apply accelerated keys
        //
        for (int index = 0; index < AcceleratedKeyList.Count(); index++)
        {
            AcceleratedKeyDef& def = AcceleratedKeyList[index];

            //
            //	Is this fuction activated?
            //
            if (Get_Value(def.Function, FunctionPrimaryKeys[def.Function], 1.0F) ||
                Get_Value(def.Function, FunctionSecondaryKeys[def.Function], 1.0F))
            {
                //
                //	Apply the accelerated value
                //
                def.Value += def.Acceleration * TimeManager::Get_Frame_Real_Seconds();
                def.Value = WWMath::Clamp(def.Value, def.Min, def.Max);

                //
                //	Store the function's value (if necessary)
                //
                if (def.Value != 0.0F)
                {
                    FunctionValue[def.Function] = def.Value;
                }
            }
            else
            {
                //
                //	Reset the function's accelerated value
                //
                def.Value = 0.0F;
            }
        }
    }
    else
    {
        int index = INPUT_FUNCTION_BEGIN_CONSOLE;
        float value1 = Get_Value(index, FunctionPrimaryKeys[index], 1.0F);
        float value2 = Get_Value(index, FunctionSecondaryKeys[index], 1.0F);

        FunctionValue[index] = max(value1, value2);
    }

    for (auto& state : key_states)
    {
        state.hit = false;
        state.released = false;
    }
}

int Input::Console_Get_Key()
{
    if (QueueSize > 0)
    {
        int key = Queue[QueueTail];
        if (++QueueTail == QUEUE_MAX)
        {
            QueueTail = 0;
        }
        QueueSize--;
        return key;
    }
    return 0;
}

void Input::Console_Add_Key(int key)
{
    if (QueueSize < QUEUE_MAX)
    {
        Queue[QueueHead] = key;
        if (++QueueHead == QUEUE_MAX)
        {
            QueueHead = 0;
        }
        QueueSize++;
    }
}


/*
**
*/
bool Input::Is_Button_Down(InputKey button_id)
{
    if (button_id > InputKey::Unset && button_id < InputKey::Button_Count)
        return key_states[(int) button_id].held;
    return false;
}


/*
**
*/
float Input::Get_Value(int function_index, InputKey input, float clamp)
{
    if (input >= InputKey::Slider_First)
    {
        // return slider state

        float slider = Sliders[(int)input - (int)InputKey::Slider_First];

        //
        // ignore the dead zone
#if 0
		if ( slider < SLIDER_DEAD_ZONE ) {
			slider = 0;
		}

// Ignore the clamp
		if ( slider > clamp ) {
			slider = clamp;
		}
#endif

        //		if ( input == SLIDER_MOUSE_LEFT ) 	Debug_Say(( "Slider %f\n", slider ));

        return slider;
    }
    else if (input != InputKey::Unset)
    {
#if(0)
		//
		//	Bail if this is a debug key and the debug button isn't down
		//
		if ((FunctionKeyStates[function_index] & 0xF000) & BUTTON_DEBUG) {
			if ((DirectInput::Get_Button_Value(DIK_F9) & BUTTON_BIT_HELD) == 0) {
				return 0.0F;
			}
		} else if ((DirectInput::Get_Button_Value(DIK_F9) & BUTTON_BIT_HELD)) {
			return 0.0F;
		}
#endif

        int modifier = 0;

        // Special case ctrl and alt for the 0 - 9 keys
        if (input >= InputKey::_0_Key && input <= InputKey::_9_Key)
        {
            // Shift is a key, used for walk!
            //			modifier |= ((DirectInput::Get_Button_Value(DIK_SHIFT) & BUTTON_BIT_HELD) ? BUTTON_SHIFT : 0);
            modifier |= key_states[(int)InputKey::Control_Key].held ? BUTTON_CTRL : 0;
            modifier |= key_states[(int)InputKey::Alt_Key].held ? BUTTON_ALT : 0;
        }

        modifier |= key_states[(int)InputKey::F9_Key].held ? BUTTON_DEBUG : 0;

        int funcModifier = (FunctionKeyStates[function_index] & 0xF000);

        if (funcModifier != modifier)
        {
            return 0.0f;
        }

        auto state = key_states[((int)input)];
        switch (FunctionKeyStates[function_index] & 0x0F)
        {
        case BUTTON_UP:
            return state.held ? 0.0 : 1.0;

        case BUTTON_HIT:
            return state.hit ? 1.0 : 0.0;

        case BUTTON_RELEASE:
            return state.released ? 1.0 : 0.0;

        case BUTTON_HELD:
            return state.held ? 1.0 : 0.0;

        case BUTTON_DOUBLE:
            return state.double_hit ? 1.0 : 0.0;
        }
    }

    return 0.0f;
}


/*
**
*/
short Input::Get_Function(const char* name)
{
    if (name && name[0])
    {
        // find function
        for (int function = 0; function < NUM_FUNCTIONS; function++)
        {
            if (!stricmp(name, Functions[function].Name))
            {
                return Functions[function].ID;
            }
        }
        Debug_Say(( "Unknown function name %s\n", name ));
    }
    return 0;
}


/*
**
*/
const char* Input::Get_Key_Name(InputKey key_id)
{
    //
    // Check each button name
    //
    for (auto item : KeyNames)
    {
        if (item.ID == key_id)
        {
            return item.Name;
        }
    }

    Debug_Say(( "Could not find a name for key %d\n", key_id ));
    return NULL;
}

/*
**
*/
InputKey Input::Get_Key(const char* name)
{
    if (name != nullptr && name[0] != 0)
    {
        for (auto item : KeyNames)
        {
            if (::stricmp(name, item.Name) == 0)
            {
                return item.ID;
            }
        }

        Debug_Say(( "Could not find key name %s\n", name ));
    }

    return InputKey::Unset;
}


/*
**
*/
InputKey
Input::Get_Primary_Key_For_Function(int function_id)
{
    return FunctionPrimaryKeys[function_id];
}


/*
**
*/
InputKey
Input::Get_Secondary_Key_For_Function(int function_id)
{
    return FunctionSecondaryKeys[function_id];
}


/*
**
*/
void
Input::Set_Primary_Key_For_Function(int function_id, InputKey key_id)
{
    FunctionPrimaryKeys[function_id] = key_id;

    // Use weapon key is the same as secondary fire, just hit rather than held
    if (function_id == INPUT_FUNCTION_FIRE_WEAPON_SECONDARY)
    {
        Set_Primary_Key_For_Function(INPUT_FUNCTION_USE_WEAPON, key_id);
    }

    // Copy jump and crouch to move up and down
    if (function_id == INPUT_FUNCTION_CROUCH)
    {
        Set_Primary_Key_For_Function(INPUT_FUNCTION_MOVE_DOWN, key_id);
    }
    if (function_id == INPUT_FUNCTION_JUMP)
    {
        Set_Primary_Key_For_Function(INPUT_FUNCTION_MOVE_UP, key_id);
    }


    return;
}


/*
**
*/
void
Input::Set_Secondary_Key_For_Function(int function_id, InputKey key_id)
{
    FunctionSecondaryKeys[function_id] = key_id;

    // Use weapon key is the same as secondary fire, just hit rather than held
    if (function_id == INPUT_FUNCTION_FIRE_WEAPON_SECONDARY)
    {
        Set_Secondary_Key_For_Function(INPUT_FUNCTION_USE_WEAPON, key_id);
    }

    // Copy jump and crouch to move up and down
    if (function_id == INPUT_FUNCTION_CROUCH)
    {
        Set_Secondary_Key_For_Function(INPUT_FUNCTION_MOVE_DOWN, key_id);
    }
    if (function_id == INPUT_FUNCTION_JUMP)
    {
        Set_Secondary_Key_For_Function(INPUT_FUNCTION_MOVE_UP, key_id);
    }

    return;
}


/*
**
*/
void
Input::Save_Configuration(const char* filename)
{
    INIClass input_ini;

    //
    //	Loop over each function and save its key mappings to the INI file
    //
    for (int index = 0; index < NUM_FUNCTIONS; index++)
    {
        StringClass pri_name;
        StringClass sec_name;
        pri_name.Format("%s_Primary", Functions[index].Name);
        sec_name.Format("%s_Secondary", Functions[index].Name);

        StringClass pri_key;
        StringClass sec_key;

        //
        //	Get the name of the primary key that's mapped to this function
        //
        if (FunctionPrimaryKeys[index] != InputKey::Unset)
        {
            pri_key = Get_Key_Name(FunctionPrimaryKeys[index]);
        }

        //
        //	Get the name of the secondary  key that's mapped to this function
        //
        if (FunctionSecondaryKeys[index] != InputKey::Unset)
        {
            sec_key = Get_Key_Name(FunctionSecondaryKeys[index]);
        }

        //
        //	Read the primary and secondary keys for this function
        //
        input_ini.Put_String("Generic Key Mappings", pri_name, pri_key);
        input_ini.Put_String("Generic Key Mappings", sec_name, sec_key);
    }

    //
    //	Save the accelerated functions
    //
    Save_Accelerated_Keys(&input_ini);
    Save_Misc_Settings(&input_ini);

    //
    //	Save the data to a file
    //
    StringClass config_filename;
    config_filename.Format("config\\%s", filename);
    FileClass* ini_file = _TheWritingFileFactory->Get_File(config_filename);
    if (ini_file != NULL)
    {
        ini_file->Open(FileClass::WRITE);
        input_ini.Save(*ini_file);
        _TheWritingFileFactory->Return_File(ini_file);
    }

    return;
}

/*
**
*/
struct KeyTranslation
{
    int text_id;
    InputKey dik_id;
};


const KeyTranslation TranslationsList[] =
{
    {IDS_KEYNAME_DIK_F1, InputKey::F1_Key},
    {IDS_KEYNAME_DIK_F2, InputKey::F2_Key},
    {IDS_KEYNAME_DIK_F3, InputKey::F3_Key},
    {IDS_KEYNAME_DIK_F4, InputKey::F4_Key},
    {IDS_KEYNAME_DIK_F5, InputKey::F5_Key},
    {IDS_KEYNAME_DIK_F6, InputKey::F6_Key},
    {IDS_KEYNAME_DIK_F7, InputKey::F7_Key},
    {IDS_KEYNAME_DIK_F8, InputKey::F8_Key},
    {IDS_KEYNAME_DIK_F9, InputKey::F9_Key},
    {IDS_KEYNAME_DIK_F10, InputKey::F10_Key},
    {IDS_KEYNAME_DIK_F11, InputKey::F11_Key},
    {IDS_KEYNAME_DIK_F12, InputKey::F12_Key},
    {IDS_KEYNAME_DIK_0, InputKey::_0_Key},
    {IDS_KEYNAME_DIK_1, InputKey::_1_Key},
    {IDS_KEYNAME_DIK_2, InputKey::_2_Key},
    {IDS_KEYNAME_DIK_3, InputKey::_3_Key},
    {IDS_KEYNAME_DIK_4, InputKey::_4_Key},
    {IDS_KEYNAME_DIK_5, InputKey::_5_Key},
    {IDS_KEYNAME_DIK_6, InputKey::_6_Key},
    {IDS_KEYNAME_DIK_7, InputKey::_7_Key},
    {IDS_KEYNAME_DIK_8, InputKey::_8_Key},
    {IDS_KEYNAME_DIK_9, InputKey::_9_Key},
    {IDS_KEYNAME_DIK_A, InputKey::A_Key},
    {IDS_KEYNAME_DIK_B, InputKey::B_Key},
    {IDS_KEYNAME_DIK_C, InputKey::C_Key},
    {IDS_KEYNAME_DIK_D, InputKey::D_Key},
    {IDS_KEYNAME_DIK_E, InputKey::E_Key},
    {IDS_KEYNAME_DIK_F, InputKey::F_Key},
    {IDS_KEYNAME_DIK_G, InputKey::G_Key},
    {IDS_KEYNAME_DIK_H, InputKey::H_Key},
    {IDS_KEYNAME_DIK_I, InputKey::I_Key},
    {IDS_KEYNAME_DIK_J, InputKey::J_Key},
    {IDS_KEYNAME_DIK_K, InputKey::K_Key},
    {IDS_KEYNAME_DIK_L, InputKey::L_Key},
    {IDS_KEYNAME_DIK_M, InputKey::M_Key},
    {IDS_KEYNAME_DIK_N, InputKey::N_Key},
    {IDS_KEYNAME_DIK_O, InputKey::O_Key},
    {IDS_KEYNAME_DIK_P, InputKey::P_Key},
    {IDS_KEYNAME_DIK_Q, InputKey::Q_Key},
    {IDS_KEYNAME_DIK_R, InputKey::R_Key},
    {IDS_KEYNAME_DIK_S, InputKey::S_Key},
    {IDS_KEYNAME_DIK_T, InputKey::T_Key},
    {IDS_KEYNAME_DIK_U, InputKey::U_Key},
    {IDS_KEYNAME_DIK_V, InputKey::V_Key},
    {IDS_KEYNAME_DIK_W, InputKey::W_Key},
    {IDS_KEYNAME_DIK_X, InputKey::X_Key},
    {IDS_KEYNAME_DIK_Y, InputKey::Y_Key},
    {IDS_KEYNAME_DIK_Z, InputKey::Z_Key},
    {IDS_KEYNAME_DIK_MINUS, InputKey::Minus_Key},
    {IDS_KEYNAME_DIK_EQUALS, InputKey::Equals_Key},
    {IDS_KEYNAME_DIK_BACK, InputKey::Backspace_Key},
    {IDS_KEYNAME_DIK_TAB, InputKey::Tab_Key},
    {IDS_KEYNAME_DIK_LBRACKET, InputKey::Left_Bracket_Key},
    {IDS_KEYNAME_DIK_RBRACKET, InputKey::Right_Bracket_Key},
    {IDS_KEYNAME_DIK_RETURN, InputKey::Enter_Key},
    {IDS_KEYNAME_DIK_SEMICOLON, InputKey::Semicolon_Key},
    {IDS_KEYNAME_DIK_APOSTROPHE, InputKey::Apostrophe_Key},
    {IDS_KEYNAME_DIK_GRAVE, InputKey::Grave_Key},
    {IDS_KEYNAME_DIK_BACKSLASH, InputKey::Backslash_Key},
    {IDS_KEYNAME_DIK_COMMA, InputKey::Comma_Key},
    {IDS_KEYNAME_DIK_PERIOD, InputKey::Period_Key},
    {IDS_KEYNAME_DIK_SLASH, InputKey::Slash_Key},
    {IDS_KEYNAME_DIK_SPACE, InputKey::Space_Bar_Key},
    {IDS_KEYNAME_DIK_CAPITAL, InputKey::Caps_Lock_Key},
    {IDS_KEYNAME_DIK_NUMLOCK, InputKey::Num_Lock_Key},
    {IDS_KEYNAME_DIK_SCROLL, InputKey::Scroll_Lock_Key},
    {IDS_KEYNAME_DIK_ESCAPE, InputKey::Escape_Key},
    {IDS_KEYNAME_DIK_NUMPAD0, InputKey::Keypad_0_Key},
    {IDS_KEYNAME_DIK_NUMPAD1, InputKey::Keypad_1_Key},
    {IDS_KEYNAME_DIK_NUMPAD2, InputKey::Keypad_2_Key},
    {IDS_KEYNAME_DIK_NUMPAD3, InputKey::Keypad_3_Key},
    {IDS_KEYNAME_DIK_NUMPAD4, InputKey::Keypad_4_Key},
    {IDS_KEYNAME_DIK_NUMPAD5, InputKey::Keypad_5_Key},
    {IDS_KEYNAME_DIK_NUMPAD6, InputKey::Keypad_6_Key},
    {IDS_KEYNAME_DIK_NUMPAD7, InputKey::Keypad_7_Key},
    {IDS_KEYNAME_DIK_NUMPAD8, InputKey::Keypad_8_Key},
    {IDS_KEYNAME_DIK_NUMPAD9, InputKey::Keypad_9_Key},
    {IDS_KEYNAME_DIK_SUBTRACT, InputKey::Minus_Key},
    {IDS_KEYNAME_DIK_MULTIPLY, InputKey::Keypad_Star_Key},
    {IDS_KEYNAME_DIK_ADD, InputKey::Keypad_Plus_Key},
    {IDS_KEYNAME_DIK_DECIMAL, InputKey::Keypad_Period_Key},
    {IDS_KEYNAME_DIK_NUMPADENTER, InputKey::Keypad_Enter_Key},
    {IDS_KEYNAME_DIK_DIVIDE, InputKey::Keypad_Slash_Key},
    {IDS_KEYNAME_DIK_HOME, InputKey::Home_Key},
    {IDS_KEYNAME_DIK_PRIOR, InputKey::Page_Up_Key},
    {IDS_KEYNAME_DIK_END, InputKey::End_Key},
    {IDS_KEYNAME_DIK_NEXT, InputKey::Page_Down_Key},
    {IDS_KEYNAME_DIK_INSERT, InputKey::Insert_Key},
    {IDS_KEYNAME_DIK_DELETE, InputKey::Delete_Key},
    {IDS_KEYNAME_DIK_UP, InputKey::Up_Key},
    {IDS_KEYNAME_DIK_DOWN, InputKey::Down_Key},
    {IDS_KEYNAME_DIK_LEFT, InputKey::Left_Key},
    {IDS_KEYNAME_DIK_RIGHT, InputKey::Right_Key},
    {IDS_KEYNAME_DIK_SYSRQ, InputKey::Sys_Req_Key},
    {IDS_KEYNAME_DIK_CONTROL, InputKey::Control_Key},
    {IDS_KEYNAME_DIK_LCONTROL, InputKey::Left_Control_Key},
    {IDS_KEYNAME_DIK_RCONTROL, InputKey::Right_Control_Key},
    {IDS_KEYNAME_DIK_SHIFT, InputKey::Shift_Key},
    {IDS_KEYNAME_DIK_LSHIFT, InputKey::Left_Shift_Key},
    {IDS_KEYNAME_DIK_RSHIFT, InputKey::Right_Shift_Key},
    {IDS_KEYNAME_DIK_ALT, InputKey::Alt_Key},
    {IDS_KEYNAME_DIK_LALT, InputKey::Left_Alt_Key},
    {IDS_KEYNAME_DIK_RALT, InputKey::Right_Alt_Key},
    {IDS_KEYNAME_DIK_WIN, InputKey::Windows_Key},
    {IDS_KEYNAME_DIK_LWIN, InputKey::Left_Windows_Key},
    {IDS_KEYNAME_DIK_RWIN, InputKey::Right_Windows_Key},
    {IDS_KEYNAME_DIK_APPS, InputKey::App_Menu_Key},
    {IDS_INPUT_LBUTTON, InputKey::Left_Mouse_Button},
    {IDS_INPUT_RBUTTON, InputKey::Right_Mouse_Button},
    {IDS_INPUT_MBUTTON, InputKey::Center_Mouse_Button},
    {IDS_INPUT_MW_UP, InputKey::Mouse_Wheel_Forward},
    {IDS_INPUT_MW_DN, InputKey::Mouse_Wheel_Backward},
    {0, InputKey::Joystick_Button_A},
    {0, InputKey::Joystick_Button_B},
    {0, InputKey::Mouse_Left},
    {0, InputKey::Mouse_Right},
    {0, InputKey::Mouse_Up},
    {0, InputKey::Mouse_Down},
    {0, InputKey::Joystick_Left},
    {0, InputKey::Joystick_Right},
    {0, InputKey::Joystick_Up},
    {0, InputKey::Joystick_Down},
};

/*
**
*/
void
Input::Load_Configuration(const char* filename)
{
    Free_Mappings();

    //
    //	Try to load the INI file
    //
    INIClass* input_ini = Get_INI(filename);
    if (input_ini == NULL)
    {
        Debug_Say(("Input::Load_Configuration - Unable to load %s\n", filename));
        return;
    }

    //
    //	Loop over each function and try to read data about it from the INI
    //
    for (int index = 0; index < NUM_FUNCTIONS; index++)
    {
        StringClass pri_name;
        StringClass sec_name;
        pri_name.Format("%s_Primary", Functions[index].Name);
        sec_name.Format("%s_Secondary", Functions[index].Name);

        //
        //	Read the primary and secondary keys for this function
        //
        StringClass pri_key(0, true);
        input_ini->Get_String(pri_key, "Generic Key Mappings", pri_name);
        StringClass sec_key(0, true);
        input_ini->Get_String(sec_key, "Generic Key Mappings", sec_name);

        //
        //	Set the primary key for this function
        //
        if (pri_key.Is_Empty() == false)
        {
            FunctionPrimaryKeys[index] = Get_Key(pri_key);
        }

        //
        //	Set the secondary key for this function
        //
        if (pri_key.Is_Empty() == false)
        {
            FunctionSecondaryKeys[index] = Get_Key(sec_key);
        }
    }

    //
    //	Load the accelerated keys from the ini
    //
    Load_Accelerated_Keys(input_ini);
    Load_Misc_Settings(input_ini);

    //
    //	Free the INI
    //
    Release_INI(input_ini);

    //
    //	Reset the mouse sensitivity to ensure its clamped properly
    //
    Input::Set_Mouse_Sensitivity(MouseSensitivity);
    return;
}


/*
**
*/
void
Input::Load_Accelerated_Keys(INIClass* input_ini)
{
    int count = input_ini->Entry_Count("Accelerated Keys");

    //
    //	Load information about each accelerated key
    //
    for (int index = 0; index < count; index++)
    {
        StringClass entry_name(input_ini->Get_Entry("Accelerated Keys", index), true);
        StringClass section_name(0, true);
        input_ini->Get_String(section_name, "Accelerated Keys", entry_name);
        Load_Accelerated_Key(input_ini, section_name);
    }

    return;
}


/*
**
*/
void
Input::Load_Accelerated_Key(INIClass* input_ini, const char* section_name)
{
    StringClass function_name(0, true);
    input_ini->Get_String(function_name, section_name, ENTRY_FUNCTION);

    //
    //	Fill in the structure we use to describe the accelerated key from the INI section.
    //
    AcceleratedKeyDef def;
    def.Function = Get_Function(function_name);
    def.Min = input_ini->Get_Float(section_name, ENTRY_MIN, 0);
    def.Max = input_ini->Get_Float(section_name, ENTRY_MAX, 0);
    def.Acceleration = input_ini->Get_Float(section_name, ENTRY_ACCELERATION, 1);

    //
    //	Add this definition to the list
    //
    AcceleratedKeyList.Add(def);
    return;
}


/*
**
*/
void
Input::Save_Accelerated_Keys(INIClass* input_ini)
{
    for (int index = 0; index < AcceleratedKeyList.Count(); index++)
    {
        AcceleratedKeyDef& def = AcceleratedKeyList[index];

        //
        //	Add an entry for this key to the section
        //
        StringClass entry_name;
        StringClass section_name;
        entry_name.Format("%d", index + 1);
        section_name.Format("AcceleratedKey%d", index + 1);
        input_ini->Put_String("Accelerated Keys", entry_name, section_name);

        //
        //	Save this accelerated key's definition  to its own section
        //
        input_ini->Put_String(section_name, ENTRY_FUNCTION, Functions[def.Function].Name);
        input_ini->Put_Float(section_name, ENTRY_MIN, def.Min);
        input_ini->Put_Float(section_name, ENTRY_MAX, def.Max);
        input_ini->Put_Float(section_name, ENTRY_ACCELERATION, def.Acceleration);
    }

    return;
}


void Input::Eat_Mouse_Held_States()
{
    auto left_button = key_states[(int)InputKey::Left_Mouse_Button];
    if (left_button.held || left_button.hit)
    {
        raw_mouse_state.eat_mouse_held = true;
    }
}

InputKey Input::Get_Last_Key_Pressed()
{
    return last_key;
}

std::optional<LRESULT> Input::Process_Message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    default:
        return {};

    case WM_MOUSEMOVE:
        if (MenuMode)
        {
            CursorPos = { (float)LOWORD(lparam), (float)HIWORD(lparam) };
        }
        else
        {
            RECT rect = {};
            GetWindowRect(hwnd, &rect);
            POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
            ClientToScreen(hwnd, &center);
            SetCursorPos(center.x, center.y);
        }
        return {};

    case WM_INPUT:
        {
            auto code = GET_RAWINPUT_CODE_WPARAM(wparam);
            // ignore background messages
            if (code != RIM_INPUT)
                return {};

            RAWINPUT input;
            RAWINPUT* pinput = &input;

            std::vector<BYTE> data;
            UINT size = sizeof(RAWINPUT);
            if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, &input, &size, sizeof(RAWINPUTHEADER)) == -1)
            {
                data.resize(size);
                if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, data.data(), &size, sizeof(RAWINPUTHEADER)) == -1)
                {
                    Debug_Say(("GetRawInputData failed"));
                    return {};
                }
                pinput = reinterpret_cast<RAWINPUT*>(data.data());
            }

            switch (pinput->header.dwType)
            {
            case RIM_TYPEKEYBOARD:
                raw_keyboard_input(pinput->data.keyboard);
                break;
            case RIM_TYPEMOUSE:
                raw_mouse_input(pinput->data.mouse);
                break;
            }

            return {};
        }

    }
}

////////////////////////////////////////////////////////////////
//
//	Get_Translated_Key_Name
//
////////////////////////////////////////////////////////////////
void
Input::Get_Translated_Key_Name(InputKey id, WideStringClass& name)
{
    for (auto item : TranslationsList)
    {
        if (item.dik_id == id)
        {
            name = TRANSLATE(item.text_id);
            break;
        }
    }
}


/*
**
*/
void
Input::Load_Misc_Settings(INIClass* input_ini)
{
    DamageIndicatorsEnabled = input_ini->Get_Bool(SECTION_MISC_SETTINGS, ENTRY_DAMAGE_INDICATORS, true);
    MouseSensitivity = input_ini->Get_Float(SECTION_MISC_SETTINGS, ENTRY_MOUSE_SENSITIVITY, 0.5F);
    MouseScale = input_ini->Get_Float(SECTION_MISC_SETTINGS, ENTRY_MOUSE_SCALE, 1.0F);
    MouseInvert = input_ini->Get_Bool(SECTION_MISC_SETTINGS, ENTRY_MOUSE_INVERT, true);
    Mouse2DInvert = input_ini->Get_Bool(SECTION_MISC_SETTINGS, ENTRY_MOUSE_2D_INVERT, false);

    bool is_target_steering = input_ini->Get_Bool(SECTION_MISC_SETTINGS, ENTRY_TARGET_STEERING, false);
    VehicleGameObj::Set_Target_Steering(is_target_steering);

    //
    //	Apply the settings
    //
    Input::Set_Mouse_Sensitivity(MouseSensitivity);
    return;
}


/*
**
*/
void
Input::Save_Misc_Settings(INIClass* input_ini)
{
    input_ini->Put_Bool(SECTION_MISC_SETTINGS, ENTRY_DAMAGE_INDICATORS, DamageIndicatorsEnabled);
    input_ini->Put_Float(SECTION_MISC_SETTINGS, ENTRY_MOUSE_SENSITIVITY, MouseSensitivity);
    input_ini->Put_Float(SECTION_MISC_SETTINGS, ENTRY_MOUSE_SCALE, MouseScale);
    input_ini->Put_Bool(SECTION_MISC_SETTINGS, ENTRY_MOUSE_INVERT, MouseInvert);
    input_ini->Put_Bool(SECTION_MISC_SETTINGS, ENTRY_MOUSE_2D_INVERT, Mouse2DInvert);
    input_ini->Put_Bool(SECTION_MISC_SETTINGS, ENTRY_TARGET_STEERING, VehicleGameObj::Is_Target_Steering());
    return;
}


/*
**
*/
int
Input::Find_First_Function_By_Primary_Key(InputKey key_id)
{
    return Find_Next_Function_By_Primary_Key(-1, key_id);
}


/*
**
*/
int
Input::Find_Next_Function_By_Primary_Key(int function_id, InputKey key_id)
{
    int retval = -1;

    for (int index = (function_id + 1); index < INPUT_FUNCTION_COUNT; index++)
    {
        if (FunctionPrimaryKeys[index] == key_id)
        {
            retval = index;
            break;
        }
    }

    return retval;
}


/*
**
*/
int
Input::Find_First_Function_By_Secondary_Key(InputKey key_id)
{
    return Find_Next_Function_By_Secondary_Key(-1, key_id);
}


/*
**
*/
int
Input::Find_Next_Function_By_Secondary_Key(int function_id, InputKey key_id)
{
    int retval = -1;

    for (int index = (function_id + 1); index < INPUT_FUNCTION_COUNT; index++)
    {
        if (FunctionSecondaryKeys[index] == key_id)
        {
            retval = index;
            break;
        }
    }

    return retval;
}
