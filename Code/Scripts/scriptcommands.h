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
 *                     $Archive:: /Commando/Code/Combat/scriptcommands.h                      $* 
 *                                                                                             * 
 *                      $Author:: Patrick                                                     $* 
 *                                                                                             * 
 *                     $Modtime:: 1/09/02 12:09p                                              $* 
 *                                                                                             * 
 *                    $Revision:: 211                                                         $* 
 *                                                                                             * 
 *---------------------------------------------------------------------------------------------* 
 * Functions:                                                                                  * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifndef	SCRIPTCOMMANDS_H
#define	SCRIPTCOMMANDS_H

#ifndef	VECTOR3_H
	#include "vector3.h"
#endif

#ifndef	COMBATSOUND_H
	#include "combatsound.h"
#endif

#ifndef	ACTIONPARAMS_H
	#include "actionparams.h"
#endif


/*
** DLL import/export macros
*/
#ifdef	BUILDING_DLL
	#define	SCRIPT_DLL_COMMAND	_declspec(dllimport)
#else
	#define	SCRIPT_DLL_COMMAND	extern "C" _declspec(dllexport)
#endif

/*
** Types
*/
class		ScriptableGameObj;
typedef	ScriptableGameObj	GameObject;
class		AudibleSoundClass;
typedef	AudibleSoundClass	Sound2D;
class		Sound3DClass;
typedef	Sound3DClass		Sound3D;
class		ScriptClass;
class		ScriptSaver;
class		ScriptLoader;


/*
** Script Commands
*/


/*
** MISC Script ENUMS
*/
enum {
		OBJECTIVE_TYPE_PRIMARY 				= 1,
		OBJECTIVE_TYPE_SECONDARY,
		OBJECTIVE_TYPE_TERTIARY,

		OBJECTIVE_STATUS_PENDING			= 0,
		OBJECTIVE_STATUS_ACCOMPLISHED,
		OBJECTIVE_STATUS_FAILED,
		OBJECTIVE_STATUS_HIDDEN,

		RADAR_BLIP_SHAPE_NONE				= 0,
		RADAR_BLIP_SHAPE_HUMAN,
		RADAR_BLIP_SHAPE_VEHICLE,
		RADAR_BLIP_SHAPE_STATIONARY,
		RADAR_BLIP_SHAPE_OBJECTIVE,

		RADAR_BLIP_COLOR_NOD					= 0,
		RADAR_BLIP_COLOR_GDI,
		RADAR_BLIP_COLOR_NEUTRAL,
		RADAR_BLIP_COLOR_MUTANT,
		RADAR_BLIP_COLOR_RENEGADE,
		RADAR_BLIP_COLOR_PRIMARY_OBJECTIVE,
		RADAR_BLIP_COLOR_SECONDARY_OBJECTIVE,
		RADAR_BLIP_COLOR_TERTIARY_OBJECTIVE,

		SCRIPT_PLAYERTYPE_SPECTATOR				= -4,		// -4
		SCRIPT_PLAYERTYPE_MUTANT,							// -3
		SCRIPT_PLAYERTYPE_NEUTRAL,							// -2
		SCRIPT_PLAYERTYPE_RENEGADE,							// -1
		SCRIPT_PLAYERTYPE_NOD,								//  0
		SCRIPT_PLAYERTYPE_GDI,								//  1
};

/*
** Script Commands List
*/

#define SCRIPT_COMMANDS_VERSION 174

// Made a virtual interface so default arguments work again.
// Should mostly work the same except the vtable is an additional indirection.
// Alternative is a bunch of wrapping methods.
struct ScriptCommands {
protected:
	~ScriptCommands() {}

public:
	unsigned int Version = SCRIPT_COMMANDS_VERSION;

	// Debug messages
	virtual void Debug_Message( const char *format, ... ) = 0;

	// Action Commands
	virtual void Action_Reset( GameObject * obj, float priority ) = 0;
	virtual void Action_Goto( GameObject * obj, const ActionParamsStruct & params ) = 0;
	virtual void Action_Attack( GameObject * obj, const ActionParamsStruct & params ) = 0;
	virtual void Action_Play_Animation( GameObject * obj, const ActionParamsStruct & params ) = 0;
	virtual void Action_Enter_Exit( GameObject * obj, const ActionParamsStruct & params ) = 0;
	virtual void Action_Face_Location( GameObject * obj, const ActionParamsStruct & params ) = 0;
	virtual void Action_Dock( GameObject * obj, const ActionParamsStruct & params ) = 0;
	virtual void Action_Follow_Input( GameObject * obj, const ActionParamsStruct & params ) = 0;

	virtual void Modify_Action( GameObject * obj, int action_id, const ActionParamsStruct & params, bool modify_move = true, bool modify_attack = true ) = 0;

	// Action information queries
	virtual int	Get_Action_ID( GameObject * obj ) = 0;
	virtual bool Get_Action_Params( GameObject * obj, ActionParamsStruct & params ) = 0;
	virtual bool Is_Performing_Pathfind_Action( GameObject * obj ) = 0;

	// Physical control
	virtual void Set_Position( GameObject * obj, const Vector3 & position ) = 0;
	virtual Vector3 Get_Position( GameObject * obj ) = 0;
	virtual Vector3 Get_Bone_Position( GameObject * obj, const char * bone_name ) = 0;
	virtual float Get_Facing( GameObject * obj ) = 0;
	virtual void Set_Facing( GameObject * obj, float degrees ) = 0;

	// Collision Control
	virtual void Disable_All_Collisions( GameObject * obj ) = 0;
	virtual void Disable_Physical_Collisions( GameObject * obj ) = 0;
	virtual void Enable_Collisions( GameObject * obj ) = 0;
	
	// Object Management
	virtual void Destroy_Object( GameObject * obj ) = 0;
	virtual GameObject * Find_Object( int obj_id ) = 0;
	virtual GameObject * Create_Object( const char * type_name, const Vector3 & position ) = 0;
	virtual GameObject * Create_Object_At_Bone( GameObject * host_obj, const char * new_obj_type_name, const char * bone_name ) = 0;
	virtual int	Get_ID( GameObject * obj ) = 0;
	virtual int	Get_Preset_ID( GameObject * obj ) = 0;
	virtual const char * Get_Preset_Name( GameObject * obj ) = 0;
	virtual void Attach_Script(GameObject* object, const char* scriptName, const char* scriptParams) = 0;
	virtual void Add_To_Dirty_Cull_List(GameObject* object) = 0;

	// Timers
	virtual void	Start_Timer( GameObject * obj, ScriptClass * script, float duration, int timer_id ) = 0;

	// Weapons
	virtual void	Trigger_Weapon( GameObject * obj, bool trigger, const Vector3 & target, bool primary = true ) = 0;
	virtual void	Select_Weapon( GameObject * obj, const char * weapon_name ) = 0;

	// Custom Script
	virtual void 	Send_Custom_Event( GameObject * from, GameObject * to, int type = 0, int param = 0, float delay = 0 ) = 0;
	virtual void 	Send_Damaged_Event( GameObject * object, GameObject * damager ) = 0;

	// Random Numbers
	virtual float Get_Random( float min, float max ) = 0;
	virtual int Get_Random_Int( int min, int max ) = 0;  // Get a random number between min and max-1, INCLUSIVE

	//	Random Selection
	virtual GameObject * Find_Random_Simple_Object( const char *preset_name ) = 0;

	// Object Display
	virtual void Set_Model( GameObject * obj, const char * model_name ) = 0;
	virtual void Set_Animation( GameObject * obj, const char * anim_name, bool looping, const char * sub_obj_name = NULL, float start_frame = 0.0F, float end_frame = -1.0F, bool is_blended = false ) = 0;
	virtual void Set_Animation_Frame( GameObject * obj, const char * anim_name, int frame ) = 0;

	// Sounds
	// Note: Each sound creation function returns the ID of the new sound (0 on error)
	virtual int	Create_Sound( const char * sound_preset_name, const Vector3 & position, GameObject * creator ) = 0;
	virtual int	Create_2D_Sound( const char * sound_preset_name ) = 0;
	virtual int	Create_2D_WAV_Sound( const char * wav_filename ) = 0;
	virtual int	Create_3D_WAV_Sound_At_Bone( const char * wav_filename, GameObject * obj, const char * bone_name ) = 0;
	virtual int	Create_3D_Sound_At_Bone( const char * sound_preset_name, GameObject * obj, const char * bone_name ) = 0;
	virtual int	Create_Logical_Sound( GameObject * creator, int type, const Vector3 & position, float radius ) = 0;
	virtual void Start_Sound( int sound_id ) = 0;
	virtual void Stop_Sound( int sound_id, bool destroy_sound = true ) = 0;
	virtual void Monitor_Sound( GameObject * game_obj, int sound_id ) = 0;
	virtual void Set_Background_Music( const char * wav_filename ) = 0;
	virtual void Fade_Background_Music( const char * wav_filename, int fade_out_time, int fade_in_time ) = 0;
	virtual void Stop_Background_Music( void ) = 0;

	// Object Properties
	virtual float	Get_Health( GameObject * obj ) = 0;
	virtual float	Get_Max_Health( GameObject * obj ) = 0;
	virtual void	Set_Health( GameObject * obj, float health ) = 0;
	virtual float	Get_Shield_Strength( GameObject * obj ) = 0;
	virtual float	Get_Max_Shield_Strength( GameObject * obj ) = 0;
	virtual void	Set_Shield_Strength( GameObject * obj, float strength ) = 0;
	virtual void	Set_Shield_Type( GameObject * obj, const char * name ) = 0;
	virtual int	Get_Player_Type( GameObject * obj ) = 0;
	virtual void	Set_Player_Type( GameObject * obj, int type ) = 0;

	// Math
	virtual float	Get_Distance( const Vector3 & p1, const Vector3 & p2 ) = 0;

	// Set Camera Host
	virtual void	Set_Camera_Host( GameObject * obj ) = 0;
	virtual void	Force_Camera_Look( const Vector3 & target ) = 0;

	// Get the Star
	virtual GameObject * Get_The_Star( void ) = 0;
	virtual GameObject * Get_A_Star( const Vector3 & pos ) = 0;
	virtual GameObject * Find_Closest_Soldier( const Vector3 & pos, float min_dist, float max_dist, bool only_human = true ) = 0;
	virtual bool		    Is_A_Star( GameObject * obj ) = 0;

	// Object Control
	virtual void	Control_Enable( GameObject * obj, bool enable ) = 0;

	// Hack
	virtual const char * Get_Damage_Bone_Name( void ) = 0;
	virtual bool			 Get_Damage_Bone_Direction( void ) = 0; // true means shot in the back

	// Visibility
	virtual bool	Is_Object_Visible( GameObject * looker, GameObject * obj ) = 0;
	virtual void	Enable_Enemy_Seen( GameObject * obj, bool enable = true ) = 0;

	// Display Text
	virtual void Set_Display_Color( unsigned char red = 255, unsigned char green = 255, unsigned char blue = 255 ) = 0;
	virtual void Display_Text( int string_id ) = 0;
	virtual void Display_Float( float value, const char * format = "%f" ) = 0;
	virtual void Display_Int( int value, const char * format = "%d" ) = 0;

	// SaveLoad
	virtual void 	Save_Data( ScriptSaver & saver, int id, int size, void * data ) = 0;
	virtual void 	Save_Pointer( ScriptSaver & saver, int id, void * pointer ) = 0;
	virtual bool 	Load_Begin( ScriptLoader & loader, int * id ) = 0;
	virtual void 	Load_Data( ScriptLoader & loader, int size, void * data ) = 0;
	virtual void 	Load_Pointer( ScriptLoader & loader, void ** pointer ) = 0;
	virtual void 	Load_End( ScriptLoader & loader ) = 0;

	virtual void Begin_Chunk(ScriptSaver& saver, unsigned int chunkID) = 0;
	virtual void End_Chunk(ScriptSaver& saver) = 0;
	virtual bool Open_Chunk(ScriptLoader& loader, unsigned int* chunkID) = 0;
	virtual void Close_Chunk(ScriptLoader& loader) = 0;

	// Radar Effects
	virtual void Clear_Radar_Markers( void ) = 0;
	virtual void Clear_Radar_Marker( int id ) = 0;
	virtual void Add_Radar_Marker( int id, const Vector3& position, int shape_type, int color_type ) = 0;
	virtual void Set_Obj_Radar_Blip_Shape( GameObject * obj, int shape_type ) = 0;	// Set to -1 to reset default
	virtual void Set_Obj_Radar_Blip_Color( GameObject * obj, int color_type ) = 0;	// Set to -1 to reset default
	virtual void Enable_Radar( bool enable ) = 0;

	//
	//	Map support
	//
	virtual void Clear_Map_Cell( int cell_x, int cell_y ) = 0;
	virtual void Clear_Map_Cell_By_Pos( const Vector3 &world_space_pos ) = 0;
	virtual void Clear_Map_Cell_By_Pixel_Pos( int pixel_pos_x, int pixel_pos_y ) = 0;
	virtual void Clear_Map_Region_By_Pos( const Vector3 &world_space_pos, int pixel_radius ) = 0;
	virtual void Reveal_Map( void ) = 0;
	virtual void Shroud_Map( void ) = 0;
	virtual void Show_Player_Map_Marker( bool onoff ) = 0;

	//
	//	Height DB access
	//
	virtual float	Get_Safe_Flight_Height( float x_pos, float y_pos ) = 0;

	// Explosions
	virtual void	Create_Explosion( const char * explosion_def_name, const Vector3 & pos, GameObject * creator = NULL ) = 0;
	virtual void	Create_Explosion_At_Bone( const char * explosion_def_name, GameObject * object, const char * bone_name, GameObject * creator = NULL ) = 0;

	// HUD
	virtual void Enable_HUD( bool enable ) = 0;
	virtual void Mission_Complete( bool success ) = 0;

	virtual void Give_PowerUp( GameObject * obj, const char * preset_name, bool display_on_hud = false ) = 0;

	// Administration
	virtual void Innate_Disable(GameObject* object) = 0;
	virtual void Innate_Enable(GameObject* object) = 0;

	// Innate Soldier AI Enable/Disable (returns old value)
	virtual bool Innate_Soldier_Enable_Enemy_Seen( GameObject * obj, bool state ) = 0;
	virtual bool Innate_Soldier_Enable_Gunshot_Heard( GameObject * obj, bool state ) = 0;
	virtual bool Innate_Soldier_Enable_Footsteps_Heard( GameObject * obj, bool state ) = 0;
	virtual bool Innate_Soldier_Enable_Bullet_Heard( GameObject * obj, bool state ) = 0;
	virtual bool Innate_Soldier_Enable_Actions( GameObject * obj, bool state ) = 0;
	virtual void Set_Innate_Soldier_Home_Location( GameObject * obj, const Vector3& home_pos, float home_radius = 999999 ) = 0;
	virtual void Set_Innate_Aggressiveness( GameObject * obj, float aggressiveness ) = 0;
	virtual void Set_Innate_Take_Cover_Probability( GameObject * obj, float probability ) = 0;
	virtual void Set_Innate_Is_Stationary( GameObject * obj, bool stationary ) = 0;

	virtual void Innate_Force_State_Bullet_Heard( GameObject * obj, const Vector3 & pos ) = 0;
	virtual void Innate_Force_State_Footsteps_Heard( GameObject * obj, const Vector3 & pos ) = 0;
	virtual void Innate_Force_State_Gunshots_Heard( GameObject * obj, const Vector3 & pos ) = 0;
	virtual void Innate_Force_State_Enemy_Seen( GameObject * obj, GameObject * enemy ) = 0;

	// Control of StaticAnimPhys
	virtual void Static_Anim_Phys_Goto_Frame( int obj_id, float frame, const char * anim_name = NULL ) = 0;
	virtual void Static_Anim_Phys_Goto_Last_Frame( int obj_id, const char * anim_name = NULL ) = 0;

	// Timing
	virtual unsigned int Get_Sync_Time( void ) = 0;

	// Objectives
	virtual void Add_Objective( int id, int type, int status, int short_description_id, char * description_sound_filename = NULL, int long_description_id = 0 ) = 0;
	virtual void Remove_Objective( int id ) = 0;
	virtual void Set_Objective_Status( int id, int status ) = 0;
	virtual void Change_Objective_Type( int id, int type ) = 0;
	virtual void Set_Objective_Radar_Blip( int id, const Vector3 & position ) = 0;
	virtual void Set_Objective_Radar_Blip_Object( int id, ScriptableGameObj * unit ) = 0;
	virtual void Set_Objective_HUD_Info( int id, float priority, const char * texture_name, int message_id ) = 0;
	virtual void Set_Objective_HUD_Info_Position( int id, float priority, const char * texture_name, int message_id, const Vector3 & position ) = 0;

	// Camaera Shakes
	virtual void Shake_Camera( const Vector3 & pos, float radius = 25, float intensity = 0.25f, float duration = 1.5f ) = 0;

	// Spawners
	virtual void	Enable_Spawner( int id, bool enable ) = 0;
	virtual GameObject * Trigger_Spawner( int id ) = 0;

	// Vehicles
	virtual void	Enable_Engine( GameObject* object, bool onoff ) = 0;

	// Difficulty Level
	virtual int	Get_Difficulty_Level( void ) = 0;

	// Keys
	virtual void	Grant_Key( GameObject* object, int key, bool grant = true ) = 0;
	virtual bool	Has_Key( GameObject* object, int key ) = 0;

	// Hibernation
	virtual void	Enable_Hibernation( GameObject * object, bool enable ) = 0;

	virtual void	Attach_To_Object_Bone( GameObject * object, GameObject * host_object, const char * bone_name ) = 0;

	// Conversation
	virtual int	Create_Conversation( const char *conversation_name, int priority = 0, float max_dist = 0, bool is_interruptable = true ) = 0;
	virtual void	Join_Conversation( GameObject * object, int active_conversation_id, bool allow_move = true, bool allow_head_turn = true, bool allow_face = true ) = 0;
	virtual void	Join_Conversation_Facing( GameObject * object, int active_conversation_id, int obj_id_to_face ) = 0;
	virtual void	Start_Conversation( int active_conversation_id, int action_id = 0 ) = 0;
	virtual void	Monitor_Conversation( GameObject * object, int active_conversation_id ) = 0;
	virtual void	Start_Random_Conversation( GameObject * object ) = 0;
	virtual void	Stop_Conversation( int active_conversation_id ) = 0;
	virtual void	Stop_All_Conversations( void ) = 0;

	// Locked facing support
	virtual void	Lock_Soldier_Facing( GameObject * object, GameObject * object_to_face, bool turn_body ) = 0;
	virtual void	Unlock_Soldier_Facing( GameObject * object ) = 0;

	// Apply Damage
	virtual void	Apply_Damage( GameObject * object, float amount, const char * warhead_name, GameObject * damager = NULL ) = 0;

	// Soldier
	virtual void	Set_Loiters_Allowed( GameObject * object, bool allowed ) = 0;

	virtual void	Set_Is_Visible( GameObject * object, bool visible ) = 0;
	virtual void	Set_Is_Rendered( GameObject * object, bool rendered ) = 0;

	// Points
	virtual float	Get_Points( GameObject * object ) = 0;
	virtual void	Give_Points( GameObject * object, float points, bool entire_team ) = 0;

	// Money (points and money were separated 09/06/01)
	virtual float	Get_Money( GameObject * object ) = 0;
	virtual void	Give_Money( GameObject * object, float money, bool entire_team ) = 0;

	// Buildings
	virtual bool	Get_Building_Power( GameObject * object ) = 0;
	virtual void	Set_Building_Power( GameObject * object, bool onoff ) = 0;
	virtual void	Play_Building_Announcement( GameObject * object, int text_id ) = 0;
	virtual GameObject * Find_Nearest_Building_To_Pos ( const Vector3 & position, const char * mesh_prefix ) = 0;
	virtual GameObject * Find_Nearest_Building ( GameObject * object, const char * mesh_prefix ) = 0;

	// Zones
	virtual int	Team_Members_In_Zone( GameObject * object, int player_type ) = 0;

	// Background
	virtual void  Set_Clouds (float cloudcover, float cloudgloominess, float ramptime) = 0;
	virtual void  Set_Lightning (float intensity, float startdistance, float enddistance, float heading, float distribution, float ramptime) = 0;
	virtual void  Set_War_Blitz (float intensity, float startdistance, float enddistance, float heading, float distribution, float ramptime) = 0;

	// Weather
	virtual void	Set_Wind			(float heading, float speed, float variability, float ramptime) = 0;
	virtual void	Set_Rain			(float density, float ramptime, bool prime) = 0;
	virtual void	Set_Snow			(float density, float ramptime, bool prime) = 0;
	virtual void	Set_Ash			(float density, float ramptime, bool prime) = 0;
	virtual void  Set_Fog_Enable (bool enabled) = 0;
	virtual void  Set_Fog_Range	(float startdistance, float enddistance, float ramptime) = 0;

	// Stealth control
	virtual void	Enable_Stealth	(GameObject * object, bool onoff) = 0;

	// Sniper control
	virtual void	Cinematic_Sniper_Control	(bool enabled, float zoom) = 0;

	// File Access
	virtual int	Text_File_Open			( const char * filename ) = 0;
	virtual bool	Text_File_Get_String	( int handle, char * buffer, int size ) = 0;
	virtual void	Text_File_Close		( int handle ) = 0;

	// Vehicle Transitions
	virtual void	Enable_Vehicle_Transitions	( GameObject * object, bool enable ) = 0;

	// Player terminal support
	virtual void	Display_GDI_Player_Terminal		() = 0;
	virtual void	Display_NOD_Player_Terminal		() = 0;
	virtual void	Display_Mutant_Player_Terminal	() = 0;

	// Encyclopedia support
	virtual bool	Reveal_Encyclopedia_Character	( int object_id ) = 0;
	virtual bool	Reveal_Encyclopedia_Weapon		( int object_id ) = 0;
	virtual bool	Reveal_Encyclopedia_Vehicle	( int object_id ) = 0;
	virtual bool	Reveal_Encyclopedia_Building	( int object_id ) = 0;
	virtual void	Display_Encyclopedia_Event_UI ( void ) = 0;

	virtual void	Scale_AI_Awareness( float sight_scale, float hearing_scale ) = 0;

	// Cinematic Freeze
	virtual void	Enable_Cinematic_Freeze( GameObject * object, bool enable ) = 0;

	virtual void	Expire_Powerup ( GameObject * object ) = 0;

	// Hud stuff
	virtual void	Set_HUD_Help_Text ( int string_id, const Vector3 &color ) = 0;
	virtual void	Enable_HUD_Pokable_Indicator ( GameObject * object, bool enable ) = 0;

	virtual void	Enable_Innate_Conversations ( GameObject * object, bool enable ) = 0;

	virtual void	Display_Health_Bar ( GameObject * object, bool display ) = 0;

	// Shadow control.  In certain cases we need to manually disable shadow casting
	// on an object.  Cinematics with too many characters are an example of this.
	virtual void	Enable_Shadow ( GameObject * object, bool enable ) = 0;

	virtual void	Clear_Weapons ( GameObject * object ) = 0;

	virtual void	Set_Num_Tertiary_Objectives ( int count ) = 0;

	// Letterbox and screen fading controls
	virtual void	Enable_Letterbox ( bool onoff, float seconds ) = 0;
	virtual void	Set_Screen_Fade_Color ( float r, float g, float b, float seconds ) = 0;
	virtual void	Set_Screen_Fade_Opacity ( float opacity, float seconds ) = 0;

};


/*
** Build a class to wrap the struct
*/
class ScriptCommandsClass {
public:
	ScriptCommands	*Commands;
};

/*
** Get Script Commands 
** This should only be called in the host application
** and not from the DLL
*/
ScriptCommands	*Get_Script_Commands( void );

#endif	// SCRIPTCOMMANDS_H
