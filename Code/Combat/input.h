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
 *                     $Archive:: /Commando/Code/Combat/input.h                             $*
 *                                                                                             *
 *                      $Author:: Byon_g                                                      $*
 *                                                                                             *
 *                     $Modtime:: 2/13/02 2:49p                                               $*
 *                                                                                             *
 *                    $Revision:: 87                                                         $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef	INPUT_H
#define	INPUT_H

#ifndef	ALWAYS_H
	#include "always.h"
#endif

#ifndef	WWMATH_H
	#include "wwmath.h"
#endif

#ifndef	WWDEBUG_H
	#include "wwdebug.h"
#endif

#include <optional>

#include "vector2.h"
#include "widestring.h"


/*
**	Forward declarations
*/
class	INIClass;

/*
**	Constants
*/
extern const char *DEFAULT_INPUT_FILENAME;

// All inputs. Named to match whats saved in Data/config/input01.cfg
// (except for "0_Key".."9_Key" which need a _ prefix)
enum class InputKey
{
	Unset = -1, // not bound / saved

	F1_Key,
	F2_Key,
	F3_Key,
	F4_Key,
    F5_Key,
    F6_Key,
    F7_Key,
    F8_Key,
    F9_Key,
    F10_Key,
    F11_Key,
    F12_Key,
    _0_Key,
    _1_Key,
    _2_Key,
    _3_Key,
    _4_Key,
    _5_Key,
    _6_Key,
    _7_Key,
    _8_Key,
    _9_Key,
    A_Key,
    B_Key,
    C_Key,
    D_Key,
    E_Key,
    F_Key,
    G_Key,
    H_Key,
    I_Key,
    J_Key,
    K_Key,
    L_Key,
    M_Key,
    N_Key,
    O_Key,
    P_Key,
    Q_Key,
    R_Key,
    S_Key,
    T_Key,
    U_Key,
    V_Key,
    W_Key,
    X_Key,
    Y_Key,
    Z_Key,
    Minus_Key,
    Equals_Key,
    Backspace_Key,
    Tab_Key,
    Left_Bracket_Key,
    Right_Bracket_Key,
    Enter_Key,
    Semicolon_Key,
    Apostrophe_Key,
    Grave_Key,
    Backslash_Key,
    Comma_Key,
    Period_Key,
    Slash_Key,
    Space_Bar_Key,
    Caps_Lock_Key,
    Num_Lock_Key,
    Scroll_Lock_Key,
    Escape_Key,
    Keypad_0_Key,
    Keypad_1_Key,
    Keypad_2_Key,
    Keypad_3_Key,
    Keypad_4_Key,
    Keypad_5_Key,
    Keypad_6_Key,
    Keypad_7_Key,
    Keypad_8_Key,
    Keypad_9_Key,
    Keypad_Minus_Key,
    Keypad_Star_Key,
    Keypad_Plus_Key,
    Keypad_Period_Key,
    Keypad_Enter_Key,
    Keypad_Slash_Key,
    Home_Key,
    Page_Up_Key,
    End_Key,
    Page_Down_Key,
    Insert_Key,
    Delete_Key,
    Up_Key,
    Down_Key,
    Left_Key,
    Right_Key,
    Sys_Req_Key,
    Control_Key,
    Left_Control_Key,
    Right_Control_Key,
    Shift_Key,
    Left_Shift_Key,
    Right_Shift_Key,
    Alt_Key,
    Left_Alt_Key,
    Right_Alt_Key,
    Windows_Key,
    Left_Windows_Key,
    Right_Windows_Key,
    App_Menu_Key,

	Left_Mouse_Button,
	Right_Mouse_Button,
	Center_Mouse_Button,

	Joystick_Button_A,
	Joystick_Button_B,

	// "Sliders", splits input axes so it doesn't use negatives
	Mouse_Left, // Previously = 0x7000?
	Mouse_Right,
	Mouse_Up,
	Mouse_Down,
	Mouse_Wheel_Forward,
	Mouse_Wheel_Backward,
	Joystick_Left,
	Joystick_Right,
	Joystick_Up,
	Joystick_Down,

	Total_Count,
	Button_Count = Mouse_Left,
	Slider_First = Mouse_Left,
	Slider_Count = Total_Count - Mouse_Left,
};


/*
** Input Functions
*/
typedef enum {

	INPUT_FUNCTION_MOVE_FORWARD,
	INPUT_FUNCTION_MOVE_BACKWARD,
	INPUT_FUNCTION_MOVE_LEFT,
	INPUT_FUNCTION_MOVE_RIGHT,
	INPUT_FUNCTION_MOVE_UP,				// helicopters and orcas
	INPUT_FUNCTION_MOVE_DOWN,

	INPUT_FUNCTION_WALK_MODE,

	INPUT_FUNCTION_TURN_LEFT,
	INPUT_FUNCTION_TURN_RIGHT,

	INPUT_FUNCTION_VEHICLE_TURN_LEFT,
	INPUT_FUNCTION_VEHICLE_TURN_RIGHT,
	INPUT_FUNCTION_VEHICLE_TOGGLE_GUNNER,

	INPUT_FUNCTION_WEAPON_UP,
	INPUT_FUNCTION_WEAPON_DOWN,
	INPUT_FUNCTION_WEAPON_LEFT,
	INPUT_FUNCTION_WEAPON_RIGHT,
	INPUT_FUNCTION_WEAPON_RESET,

	INPUT_FUNCTION_ZOOM_IN,
	INPUT_FUNCTION_ZOOM_OUT,

	INPUT_FUNCTION_ACTION,				// Was LADDER
	INPUT_FUNCTION_JUMP,
	INPUT_FUNCTION_CROUCH,

	INPUT_FUNCTION_DIVE_FORWARD,
	INPUT_FUNCTION_DIVE_BACKWARD,
	INPUT_FUNCTION_DIVE_LEFT,
	INPUT_FUNCTION_DIVE_RIGHT,

	INPUT_FUNCTION_TURN_AROUND,
	INPUT_FUNCTION_DROP_FLAG,

	INPUT_FUNCTION_NEXT_WEAPON,
	INPUT_FUNCTION_PREV_WEAPON,
	INPUT_FUNCTION_FIRE_WEAPON_PRIMARY,
	INPUT_FUNCTION_FIRE_WEAPON_SECONDARY,
	INPUT_FUNCTION_USE_WEAPON,
	INPUT_FUNCTION_RELOAD_WEAPON,

	INPUT_FUNCTION_SELECT_NO_WEAPON,
	INPUT_FUNCTION_SELECT_WEAPON_0,
	INPUT_FUNCTION_SELECT_WEAPON_1,
	INPUT_FUNCTION_SELECT_WEAPON_2,
	INPUT_FUNCTION_SELECT_WEAPON_3,
	INPUT_FUNCTION_SELECT_WEAPON_4,
	INPUT_FUNCTION_SELECT_WEAPON_5,
	INPUT_FUNCTION_SELECT_WEAPON_6,
	INPUT_FUNCTION_SELECT_WEAPON_7,
	INPUT_FUNCTION_SELECT_WEAPON_8,
	INPUT_FUNCTION_SELECT_WEAPON_9,

	INPUT_FUNCTION_CYCLE_POG,

//	INPUT_FUNCTION_HUD_ZOOM_RADAR_IN,
//	INPUT_FUNCTION_HUD_ZOOM_RADAR_OUT,

	INPUT_FUNCTION_PANIC,
	INPUT_FUNCTION_CURSOR_TARGETING,
	INPUT_FUNCTION_FIRST_PERSON_TOGGLE,
	INPUT_FUNCTION_SUICIDE,

	INPUT_FUNCTION_VERBOSE_HELP,

	INPUT_FUNCTION_BEGIN_PUBLIC_MESSAGE,
	INPUT_FUNCTION_BEGIN_TEAM_MESSAGE,
	INPUT_FUNCTION_BEGIN_PRIVATE_MESSAGE,
	INPUT_FUNCTION_BEGIN_CONSOLE,

	INPUT_FUNCTION_HELP_SCREEN,

	INPUT_FUNCTION_EVA_OBJECTIVES_SCREEN,
	INPUT_FUNCTION_EVA_MAP_SCREEN,

	// DEBUGGING
	INPUT_FUNCTION_CAMERA_HEADING_LEFT,
	INPUT_FUNCTION_CAMERA_HEADING_RIGHT,

	INPUT_FUNCTION_CAMERA_TRANSTILT_INC,
	INPUT_FUNCTION_CAMERA_TRANSTILT_DEC,
	INPUT_FUNCTION_CAMERA_VIEWTILT_INC,
	INPUT_FUNCTION_CAMERA_VIEWTILT_DEC,
	INPUT_FUNCTION_CAMERA_DIST_INC,
	INPUT_FUNCTION_CAMERA_DIST_DEC,
	INPUT_FUNCTION_CAMERA_FOV_INC,
	INPUT_FUNCTION_CAMERA_FOV_DEC,
	INPUT_FUNCTION_CAMERA_HEIGHT_INC,
	INPUT_FUNCTION_CAMERA_HEIGHT_DEC,

	INPUT_FUNCTION_MENU_SERVERQUICKSTART,
   INPUT_FUNCTION_MENU_CLIENTQUICKSTART,
	INPUT_FUNCTION_MAKE_SCREEN_SHOT,
	INPUT_FUNCTION_TOGGLE_MOVIE_CAPTURE,
	//INPUT_FUNCTION_INCREMENT_ACTIVE_MESSAGE,
	//INPUT_FUNCTION_DECREMENT_ACTIVE_MESSAGE,
	//INPUT_FUNCTION_INCREMENT_DEBUG_SWITCH_1,
	//INPUT_FUNCTION_INCREMENT_DEBUG_SWITCH_2,
	INPUT_FUNCTION_DEBUG_SINGLE_STEP,
	INPUT_FUNCTION_DEBUG_SINGLE_STEP_STEP,
	INPUT_FUNCTION_DEBUG_RAPID_MOVE,
	INPUT_FUNCTION_DEBUG_GENERIC0,
	INPUT_FUNCTION_DEBUG_GENERIC1,
	INPUT_FUNCTION_DEBUG_FAR_CLIP_IN,
	INPUT_FUNCTION_DEBUG_FAR_CLIP_OUT,
   INPUT_FUNCTION_QUICK_FULL_EXIT,
	INPUT_FUNCTION_VIS_UPDATE,
//	INPUT_FUNCTION_TOGGLE_PERFORMANCE_SAMPLING,
	INPUT_FUNCTION_TOGGLE_SNAP_SHOT_MODE,
	INPUT_FUNCTION_SNAP_SHOT_ADVANCE,
	//INPUT_FUNCTION_DEBUG_OPTIONS_DIALOG,
	INPUT_FUNCTION_CNC,
	INPUT_FUNCTION_QUICKSAVE,

	// Menu
	INPUT_FUNCTION_MENU_TOGGLE,

	// In game EVA
	INPUT_FUNCTION_EVA_MISSION_OBJECTIVES_TOGGLE,

	INPUT_FUNCTION_RADIO_CMD_01,
	INPUT_FUNCTION_RADIO_CMD_02,
	INPUT_FUNCTION_RADIO_CMD_03,
	INPUT_FUNCTION_RADIO_CMD_04,
	INPUT_FUNCTION_RADIO_CMD_05,
	INPUT_FUNCTION_RADIO_CMD_06,
	INPUT_FUNCTION_RADIO_CMD_07,
	INPUT_FUNCTION_RADIO_CMD_08,
	INPUT_FUNCTION_RADIO_CMD_09,
	INPUT_FUNCTION_RADIO_CMD_10,

	INPUT_FUNCTION_RADIO_CMD_11,
	INPUT_FUNCTION_RADIO_CMD_12,
	INPUT_FUNCTION_RADIO_CMD_13,
	INPUT_FUNCTION_RADIO_CMD_14,
	INPUT_FUNCTION_RADIO_CMD_15,
	INPUT_FUNCTION_RADIO_CMD_16,
	INPUT_FUNCTION_RADIO_CMD_17,
	INPUT_FUNCTION_RADIO_CMD_18,
	INPUT_FUNCTION_RADIO_CMD_19,
	INPUT_FUNCTION_RADIO_CMD_20,

	INPUT_FUNCTION_RADIO_CMD_21,
	INPUT_FUNCTION_RADIO_CMD_22,
	INPUT_FUNCTION_RADIO_CMD_23,
	INPUT_FUNCTION_RADIO_CMD_24,
	INPUT_FUNCTION_RADIO_CMD_25,
	INPUT_FUNCTION_RADIO_CMD_26,
	INPUT_FUNCTION_RADIO_CMD_27,
	INPUT_FUNCTION_RADIO_CMD_28,
	INPUT_FUNCTION_RADIO_CMD_29,
	INPUT_FUNCTION_RADIO_CMD_30,

	INPUT_FUNCTION_TEAM_INFO_TOGGLE,
	INPUT_FUNCTION_BATTLE_INFO_TOGGLE,
	INPUT_FUNCTION_SERVER_INFO_TOGGLE,

	//INPUT_FUNCTION_VERBOSE_HELP_PAGE_UP,
	//INPUT_FUNCTION_VERBOSE_HELP_PAGE_DOWN,

	INPUT_FUNCTION_PROFILE_ENTER_CHILD0,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD1,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD2,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD3,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD4,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD5,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD6,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD7,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD8,
	INPUT_FUNCTION_PROFILE_ENTER_CHILD9,
	INPUT_FUNCTION_PROFILE_ENTER_PARENT,
	INPUT_FUNCTION_PROFILE_RESET,

	INPUT_FUNCTION_COUNT

} InputFunction;


class	InputFunctionMode;
class	InputFunctionModeState;

/*
**
*/
class	Input {

public:
	static	void	Init();
	static	void	Shutdown( void );
	static	void	Update( void );
	static	void	Flush( void );

	// Mouse
	static	float	Get_Mouse_Sensitivity( void );
	static	void	Set_Mouse_Sensitivity( float mouse_sensitivity );
	static	bool	Get_Mouse_Invert( void );
	static	void	Set_Mouse_Invert( bool invert );
	static	bool	Get_Mouse_2D_Invert( void );
	static	void	Set_Mouse_2D_Invert( bool invert );

	// Menu Mode
	static	void	Menu_Enable( bool onoff )	{ MenuMode = onoff; }

	// Console Mode
	static	void	Console_Enable()	{ ConsoleMode = true; Flush_Queue(); }
	static	void	Console_Disable()	{ ConsoleMode = false; }
	static	int	Console_Get_Key();
	static	void	Console_Add_Key( int key );
	static	bool	Is_Console_Enabled( void )	{ return ConsoleMode; }
	static	void	Flush_Queue()		{ QueueHead = QueueTail = QueueSize = 0; }

	// Get States
	static	float	Get_Amount( InputFunction function );
	static	bool	Get_State( InputFunction function )		{ return (MenuMode || ConsoleMode) ? false : Peek_State (function); }
	static	bool	Peek_State( InputFunction function )	{ return (FunctionValue[ function ] > 0.5f); }

	// Direct access
	static	bool	Is_Button_Down (InputKey button_id);

	// Moved from DirectInput
	static Vector2	Get_Cursor_Pos ()				{ return CursorPos; }
	static	void	Eat_Mouse_Held_States ();
	static InputKey Get_Last_Key_Pressed ();

	// Handle window messages
	static std::optional<LRESULT> Process_Message (UINT msg, WPARAM wparam, LPARAM lparam);

	// Key name access
	static	void	Get_Translated_Key_Name (InputKey key, WideStringClass &name);

	enum {
		QUEUE_MAX		= 20,

		/*
		** Sliders
		*/
		FIRST_SLIDER			= 0x7000,
		SLIDER_MOUSE_LEFT	= FIRST_SLIDER,
		SLIDER_MOUSE_RIGHT,
		SLIDER_MOUSE_UP,
		SLIDER_MOUSE_DOWN,
		SLIDER_MOUSE_WHEEL_FORWARD,
		SLIDER_MOUSE_WHEEL_BACKWARD,
		SLIDER_JOYSTICK_LEFT,
		SLIDER_JOYSTICK_RIGHT,
		SLIDER_JOYSTICK_UP,
		SLIDER_JOYSTICK_DOWN,
		LAST_SLIDER_PLUS_ONE,
		NUM_SLIDERS				= LAST_SLIDER_PLUS_ONE - FIRST_SLIDER

	};

	//
	// Configuration editing support
	//
	static InputKey Get_Primary_Key_For_Function(int function_id);
	static InputKey Get_Secondary_Key_For_Function(int function_id);
	static	void	Set_Primary_Key_For_Function (int function_id, InputKey key_id);
	static	void	Set_Secondary_Key_For_Function (int function_id, InputKey key_id);

	static	int	Find_First_Function_By_Primary_Key (InputKey key_id);
	static	int	Find_Next_Function_By_Primary_Key (int function_id, InputKey key_id);

	static	int	Find_First_Function_By_Secondary_Key (InputKey key_id);
	static	int	Find_Next_Function_By_Secondary_Key (int function_id, InputKey key_id);

	static	void	Load_Configuration (const char *filename);
	static	void	Save_Configuration (const char *filename);

	//
	//	Misc key settings
	//
	static	void	Enable_Damage_Indicators (bool onoff)		{ DamageIndicatorsEnabled = onoff; }
	static	bool	Are_Damage_Indicators_Enabled (void)		{ return DamageIndicatorsEnabled; }

private:

	//
	//	Internal methods
	//
	static	void	Update_State();

	//
	//	Private member data
	//
	static	float	Sliders[ NUM_SLIDERS ];

	static	bool	ConsoleMode;
	static	bool	MenuMode;
	static	int	Queue[ QUEUE_MAX ];
	static	int	QueueHead;
	static	int	QueueTail;
	static	int	QueueSize;
	static	bool	DamageIndicatorsEnabled;

	static Vector2  CursorPos;

	static	float	FunctionValue[ INPUT_FUNCTION_COUNT ];
	static	float	FunctionClamp[ INPUT_FUNCTION_COUNT ];

	static	InputKey	FunctionPrimaryKeys[ INPUT_FUNCTION_COUNT ];
	static	InputKey	FunctionSecondaryKeys[ INPUT_FUNCTION_COUNT ];

	static	void	Save_Accelerated_Keys (INIClass *input_ini);
	static	void	Load_Accelerated_Keys (INIClass *input_ini);
	static	void	Load_Accelerated_Key (INIClass *input_ini, const char *section_name);

	static	void	Load_Misc_Settings (INIClass	*input_ini);
	static	void	Save_Misc_Settings (INIClass	*input_ini);

	static	void	Free_Mappings( void );
	static	float	Get_Value(int function_index, InputKey input, float clamp);
	static	short	Get_Function( const char *name );
	static InputKey Get_Key(const char* name);
	static	const char * Get_Key_Name (InputKey key_id);
};


inline float Input::Get_Amount( InputFunction function )
{
	WWASSERT(WWMath::Is_Valid_Float(FunctionValue[ function ]));
	return ((MenuMode || ConsoleMode) ? 0 : FunctionValue[ function ]);
}

#endif
