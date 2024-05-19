//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: Here are tags to enable/disable things throughout the source code.
//
//=============================================================================//

#ifndef VECTRONIC_SHAREDDEFS_H
#define VECTRONIC_SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

#ifdef SWARM_DLL
// Quick swap! NUM_AI_CLASSES = LAST_SHARED_ENTITY_CLASS in ASW!
#define NUM_AI_CLASSES LAST_SHARED_ENTITY_CLASS
#endif

// The default glow color.
#define GLOW_COLOR 0,128,255

// Gravity defined in movevars_shared.cpp
#define GAME_GRAVITY "600"

// Ball Colors (r,g,b,a)
/*
#define BALL0_COLOR 134,199,197,255				// Blue
#define BALL1_COLOR 112,176,109,255				// Green
#define BALL2_COLOR 136,123,193,255				// Purple
*/

#define BALL0_COLOR_R 134
#define BALL0_COLOR_G 199
#define BALL0_COLOR_B 197

#define BALL1_COLOR_R 112
#define BALL1_COLOR_G 176
#define BALL1_COLOR_B 109

#define BALL2_COLOR_R 136
#define BALL2_COLOR_G 123
#define BALL2_COLOR_B 193

/*
#define BALL3_COLOR_R vectronic_vecball3_color_r.GetInt()
#define BALL3_COLOR_G vectronic_vecball3_color_g.GetInt()
#define BALL3_COLOR_B vectronic_vecball3_color_b.GetInt()

#define BALL4_COLOR_R vectronic_vecball4_color_r.GetInt()
#define BALL4_COLOR_G vectronic_vecball4_color_g.GetInt()
#define BALL4_COLOR_B vectronic_vecball4_color_b.GetInt()

#define BALL5_COLOR_R vectronic_vecball5_color_r.GetInt()
#define BALL5_COLOR_G vectronic_vecball5_color_g.GetInt()
#define BALL5_COLOR_B vectronic_vecball5_color_b.GetInt()
*/

#define FLOAT_TO_VECTOR_MULTIPLER 0.00392156862745098f

  //-------------------
 // Ball Textures
//-------------------
#define BALL0_SPRITE "Effects/ball_blue"
#define BALL1_SPRITE "Effects/ball_green"
#define BALL2_SPRITE "Effects/ball_purple"
#define BALL_SPRITE_DEFAULT "vecball/ball_white" // DEFAULT BALL FOR 4 - 5

// Impact (i.e When we hit something)
#define BALL_SPRITE_IMPACT "vecball/ball_white_impact"

// FLASH
#define BALL_SPRITE_FLASH "vecball/ball_white_flash"

// TAPE
#define BALL_SPRITE_TRAIL	"vecball/ball_white_tape.vmt"

#define BALL0_SPRITE_TAPE "Effects/ball_blue_tape.vmt"
#define BALL1_SPRITE_TAPE "Effects/ball_green_tape.vmt"
#define BALL2_SPRITE_TAPE "Effects/ball_purple_tape.vmt"
#define BALL3_SPRITE_TAPE "vecball/ball_red_tape.vmt"
#define BALL4_SPRITE_TAPE "vecball/ball_yellow_tape.vmt"
#define BALL5_SPRITE_TAPE "vecball/ball_orange_tape.vmt"

  //-------------------
 // Ball Particles
//-------------------

// 3rd Person
#define BALL_THIRDPERSON			"projectile_ball_shared_3rdperson"

// GLOW
#define BALL_GLOW					"vecgun_shared_charge_glow"

// Projectile
#define BALL_PARTICLE_TRIAL			"projectile_ball_shared"
#define BALL_PARTICLE_NODES			"projectile_ball_shared_nodes"

// Dispenser
#define DISPENSER_CORE				"dispenser_core_shared"
#define DISPENSER_BASE				"dispenser_base"

// Cleanser
#define BALL_CLEANSER				"cleanser"
#define BALL_CLEANSER_MED			"cleanser_med"

  //-------------------
 // Vecbox Particles
//-------------------

#define VECBOX_CLEAR_PARTICLE		"ball_white_hit_edge" 
#define VECBOX_HIT_PARTICLE			"ball_hit"


//-----Optional Defines-----

// PLAYER_HEALTH_REGEN : Regen the player's health much like it does in Portal/COD
// PLAYER_WOOSH_SOUNDS : Play air simulation sounds when falling.
// PLAYER_MOUSEOVER_HINTS : When the player has their crosshair over whatever we put in UpdateMouseoverHints() it will display a hint
// PLAYER_IGNORE_FALLDAMAGE : Ingnore fall damage.
// PLAYER_DISABLE_THROWING : Disables throwing in the player pickup controller.

//------------------
// WEAPON_QUIET_PICKUP : Don't play a sound when picking up a weapon.
// WEAPON_NOHUDSHOW_PICKUP : Don't display pickup in the hud.
// WEAPON_DLIGHT_MUZZLEFLASH : Have a light flash when firing. (Source 2013 only!)
// WEAPON_ALLOW_VIEWMODEL_FLIP : Enables cl_righthand for viewmodel.
// WEAPON_DISABLE_DYNAIMIC_CROSSHAIR : Disables the CSS like crosshair.
// GLOWS_ENABLE : Uses the item glow system. (WIP)
// GE_GLOWS_ENABLE : Uses the GE item glow system.

//========================
// PLAYER RELATED OPTIONS
//========================
	#define PLAYER_HEALTH_REGEN
	#define PLAYER_WOOSH_SOUNDS

//========================
// WEAPON RELATED OPTIONS
//========================
	#define WEAPON_QUIET_PICKUP
	#define WEAPON_NOHUDSHOW_PICKUP
	#define WEAPON_DISABLE_DYNAIMIC_CROSSHAIR

//========================
// GLOWS
//========================
	//#define GE_GLOWS_ENABLE

//------------------

//========================
// NAV_MESH
//========================
#ifndef SWARM_DLL
	#define USE_NAV_MESH
#endif

#endif // SDK_SHAREDDEFS_H