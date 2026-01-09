#ifndef __GAME_DEF_H_
#define __GAME_DEF_H_

#include <string>
#include "DInput.h"
const int CURRENT_VERSION = 1068; // ÃƒÃ·â��Ã¾ â•£Ã·â��Â³

const float PACKET_INTERVAL_MOVE = 1.5f;				// â�´Ã±â–’Ã�â��Â¹â��Â©Ã€â•¬ â•‘Â©â�‚â•—â�¤â�¬ Ã£Ã°â�¼Ã‚ Â¢â�œâ–‘Ãº â–‘Ãºâ–‘Â¦..
const float PACKET_INTERVAL_ROTATE = 4.0f;
const float PACKET_INTERVAL_REQUEST_TARGET_HP = 2.0f;

// â�¤â–„â�œÃ“â�¼â–‘ â�´Ã·â�´Ã±ÃƒÃ â�‚Â§â��â•‘ â•‘â•¬â•‘Ã°..
enum eKeyMap {	KM_HOTKEY1 = DIK_1, 
				KM_HOTKEY2 = DIK_2, 
				KM_HOTKEY3 = DIK_3, 
				KM_HOTKEY4 = DIK_4, 
				KM_HOTKEY5 = DIK_5, 
				KM_HOTKEY6 = DIK_6, 
				KM_HOTKEY7 = DIK_7, 
				KM_HOTKEY8 = DIK_8, 
				KM_TOGGLE_RUN = DIK_T, 
				KM_TOGGLE_MOVE_CONTINOUS = DIK_E, 
				KM_TOGGLE_ATTACK = DIK_R, 
				KM_TOGGLE_SITDOWN = DIK_C, 
				KM_TOGGLE_INVENTORY = DIK_I, 
				KM_TOGGLE_SKILL = DIK_K, 
				KM_TOGGLE_STATE = DIK_U, 
				KM_TOGGLE_MINIMAP = DIK_M, 
				KM_TOGGLE_HELP = DIK_F1, 
				KM_CAMERA_CHANGE = DIK_F9, 
				KM_DROPPED_ITEM_OPEN = DIK_F, 
				KM_MOVE_FOWARD = DIK_W, 
				KM_MOVE_BACKWARD = DIK_S, 
				KM_ROTATE_LEFT = DIK_A, 
				KM_ROTATE_RIGHT = DIK_D, 
				KM_TARGET_NEARST_ENEMY = DIK_Z, 
				KM_TARGET_NEARST_PARTY = DIK_X, 
				KM_TARGET_NEARST_FRIEND = DIK_V, 
				KM_TARGET_PARTY1 = DIK_F1, 
				KM_TARGET_PARTY2 = DIK_F2, 
				KM_TARGET_PARTY3 = DIK_F3, 
				KM_TARGET_PARTY4 = DIK_F4, 
				KM_TARGET_PARTY5 = DIK_F5, 
				KM_TARGET_PARTY6 = DIK_F6, 
				KM_TARGET_PARTY7 = DIK_F7, 
				KM_TARGET_PARTY8 = DIK_F8 };

enum e_PlayerType { PLAYER_BASE = 0, PLAYER_NPC = 1, PLAYER_OTHER = 2, PLAYER_MYSELF = 3 };

enum e_Race {	RACE_ALL = 0,
				RACE_KA_ARKTUAREK = 1, RACE_KA_TUAREK = 2, RACE_KA_WRINKLETUAREK = 3, RACE_KA_PURITUAREK = 4, 
				RACE_EL_BABARIAN = 11, RACE_EL_MAN = 12, RACE_EL_WOMEN = 13,
				//RACE_KA_NORMAL = 11, RACE_KA_WARRIOR = 12, RACE_KA_ROGUE = 13, RACE_KA_MAGE = 14,
				RACE_NPC = 100,
				RACE_UNKNOWN = 0xffffffff };

enum e_Class {	CLASS_KINDOF_WARRIOR = 1, CLASS_KINDOF_ROGUE, CLASS_KINDOF_WIZARD, CLASS_KINDOF_PRIEST,
				CLASS_KINDOF_ATTACK_WARRIOR, CLASS_KINDOF_DEFEND_WARRIOR, CLASS_KINDOF_ARCHER, CLASS_KINDOF_ASSASSIN, 
				CLASS_KINDOF_ATTACK_WIZARD, CLASS_KINDOF_PET_WIZARD, CLASS_KINDOF_HEAL_PRIEST, CLASS_KINDOF_CURSE_PRIEST,

				CLASS_KA_WARRIOR = 101, CLASS_KA_ROGUE, CLASS_KA_WIZARD, CLASS_KA_PRIEST, // â�Â®â–’Ã�â–’Â¯â�´Ã· â–’Ã�â•‘â•— â�´Â¸Â¥Â¸
				CLASS_KA_BERSERKER = 105, CLASS_KA_GUARDIAN, CLASS_KA_HUNTER = 107, CLASS_KA_PENETRATOR, 
				CLASS_KA_SORCERER = 109, CLASS_KA_NECROMANCER, CLASS_KA_SHAMAN = 111, CLASS_KA_DARKPRIEST, 
				
				CLASS_EL_WARRIOR = 201, CLASS_EL_ROGUE, CLASS_EL_WIZARD, CLASS_EL_PRIEST, // â�Â®â–’Ã�â–’Â¯â�´Ã· â–’Ã�â•‘â•— â�´Â¸Â¥Â¸ 
				CLASS_EL_BLADE = 205, CLASS_EL_PROTECTOR, CLASS_EL_RANGER = 207, CLASS_EL_ASSASIN, 
				CLASS_EL_MAGE = 209, CLASS_EL_ENCHANTER, CLASS_EL_CLERIC = 211, CLASS_EL_DRUID,
				
				CLASS_UNKNOWN = 0xffffffff };

enum e_Class_Represent { CLASS_REPRESENT_WARRIOR = 0, CLASS_REPRESENT_ROGUE, CLASS_REPRESENT_WIZARD, CLASS_REPRESENT_PRIEST, CLASS_REPRESENT_UNKNOWN = 0xffffffff };

const float WEAPON_WEIGHT_STAND_SWORD = 5.0f; // â•£Â½â–’Ã�â��Ãƒ â•£Â½â–‘Ãˆ â–’Ã�â�´Ã...â�€Â«
const float WEAPON_WEIGHT_STAND_AXE = 5.0f; // â•£Â½â–’Ã�â��Ãƒ â•£Â½â–‘Ãˆ â–’Ã�â�´Ã...ÃÃâ�‚Ã³
const float WEAPON_WEIGHT_STAND_BLUNT = 8.0f; // â•£Â½â–’Ã�â��Ãƒ â•£Â½â–‘Ãˆ â–’Ã�â�´Ã...Â©Â¨ÃÄ±â��â•  â�´Â¥Ã€Â¨

enum e_Ani {	ANI_BREATH = 0, ANI_WALK, ANI_RUN, ANI_WALK_BACKWARD, ANI_STRUCK0, ANI_STRUCK1, ANI_STRUCK2, ANI_GUARD,
				ANI_DEAD_NEATLY = 8, ANI_DEAD_KNOCKDOWN, ANI_DEAD_ROLL, ANI_SITDOWN, ANI_SITDOWN_BREATH, ANI_STANDUP,
				ANI_ATTACK_WITH_WEAPON_WHEN_MOVE = 14, ANI_ATTACK_WITH_NAKED_WHEN_MOVE, 

				ANI_SPELLMAGIC0_A = 16, ANI_SPELLMAGIC0_B, 
				ANI_SPELLMAGIC1_A = 18, ANI_SPELLMAGIC1_B, 
				ANI_SPELLMAGIC2_A = 20, ANI_SPELLMAGIC2_B, 
				ANI_SPELLMAGIC3_A = 22, ANI_SPELLMAGIC3_B, 
				ANI_SPELLMAGIC4_A = 24, ANI_SPELLMAGIC4_B, 
				
				ANI_SHOOT_ARROW_A = 26, ANI_SHOOT_ARROW_B, 
				ANI_SHOOT_QUARREL_A = 28, ANI_SHOOT_QUARREL_B, 
				ANI_SHOOT_JAVELIN_A = 30, ANI_SHOOT_JAVELIN_B, 
				
				ANI_SWORD_BREATH_A = 32,	ANI_SWORD_ATTACK_A0, ANI_SWORD_ATTACK_A1,
				ANI_SWORD_BREATH_B,			ANI_SWORD_ATTACK_B0, ANI_SWORD_ATTACK_B1,		// ÃƒÃâ•Ä±â–‘â•¦
				
				ANI_DAGGER_BREATH_A = 38,	ANI_DAGGER_ATTACK_A0, ANI_DAGGER_ATTACK_A1,
				ANI_DAGGER_BREATH_B,		ANI_DAGGER_ATTACK_B0, ANI_DAGGER_ATTACK_B1,		// â�¤â–„ÃÃ
				
				ANI_DUAL_BREATH_A = 44,		ANI_DUAL_ATTACK_A0, ANI_DUAL_ATTACK_A1, 
				ANI_DUAL_BREATH_B,			ANI_DUAL_ATTACK_B0, ANI_DUAL_ATTACK_B1,			// â��â• ÃÃÃ€Â¨
				
				ANI_SWORD2H_BREATH_A = 50,	ANI_SWORD2H_ATTACK_A0, ANI_SWORD2H_ATTACK_A1, 
				ANI_SWORD2H_BREATH_B,		ANI_SWORD2H_ATTACK_B0, ANI_SWORD2H_ATTACK_B1,	// Â¥Ã¾â•Ä±â–‘â•¦
				
				ANI_BLUNT_BREATH_A = 56,	ANI_BLUNT_ATTACK_A0, ANI_BLUNT_ATTACK_A1, 
				ANI_BLUNT_BREATH_B,			ANI_BLUNT_ATTACK_B0, ANI_BLUNT_ATTACK_B1,		// ÃÃ°â–’Ã� - Â©Â¨ÃÄ±â��â•  ?
				
				ANI_BLUNT2H_BREATH_A = 62,	ANI_BLUNT2H_ATTACK_A0, ANI_BLUNT2H_ATTACK_A1, 
				ANI_BLUNT2H_BREATH_B,		ANI_BLUNT2H_ATTACK_B0, ANI_BLUNT2H_ATTACK_B1,	// Â¥Ã¾â•Ä±â��Â©Ã€â•¬ â��Ã�â�¤â�¬ ÃÃ°â–’Ã�. - Â¥Ã¾â•Ä± ÃÃâ�‚Ã³â�â• â–‘â–‘â�¤â�˜.
				
				ANI_AXE_BREATH_A = 68,		ANI_AXE_ATTACK_A0, ANI_AXE_ATTACK_A1, 
				ANI_AXE_BREATH_B,			ANI_AXE_ATTACK_B0, ANI_AXE_ATTACK_B1,			// ÃƒÃâ•Ä± ÃÃâ�‚Ã³ 
				
				ANI_SPEAR_BREATH_A = 74,	ANI_SPEAR_ATTACK_A0, ANI_SPEAR_ATTACK_A1, 
				ANI_SPEAR_BREATH_B,			ANI_SPEAR_ATTACK_B0, ANI_SPEAR_ATTACK_B1,		// â�œÃ³ - â•‘Ãºâ�¤â�¬ â�‚Â»â��â•  Â¥Â°â�¤â�¬ â–’ÃŽâ�‚â•� â�œÃ³â��â• â�¤â�˜.
				
				ANI_POLEARM_BREATH_A = 80,	ANI_POLEARM_ATTACK_A0, ANI_POLEARM_ATTACK_A1, 
				ANI_POLEARM_BREATH_B,		ANI_POLEARM_ATTACK_B0, ANI_POLEARM_ATTACK_B1,	// Â¥Ã¾â•Ä±â��Â©Ã€â•¬ â��Ã�â�¤â�¬ â�‚Â»â��Ãâ�¤â�¬ â�œÃ³ - â�œâ•—Ã€ÂµÃÃ ??
				
				ANI_NAKED_BREATH_A = 86,	ANI_NAKED_ATTACK_A0, ANI_NAKED_ATTACK_A1, 
				ANI_NAKED_BREATH_B,			ANI_NAKED_ATTACK_B0, ANI_NAKED_ATTACK_B1,		// Â©ÃƒÂ©Ã·â��Â©Ã€â•¬ ??
				
				ANI_BOW_BREATH = 92,		ANI_CROSS_BOW_BREATH, ANI_LAUNCHER_BREATH, 
				ANI_BOW_BREATH_B,			ANI_BOW_ATTACK_B0, ANI_BOW_ATTACK_B1,			// â•šâ–‘ â–‘Â°â–‘Â¦
				
				ANI_SHIELD_BREATH_A = 98,	ANI_SHIELD_ATTACK_A0, ANI_SHIELD_ATTACK_A1, 
				ANI_SHIELD_BREATH_B,		ANI_SHIELD_ATTACK_B0, ANI_SHIELD_ATTACK_B1,		// â•£ÂµÃ£Ã° â–‘Â°â–‘Â¦

				ANI_GREETING0 = 104, ANI_GREETING1, ANI_GREETING2, 
				ANI_WAR_CRY0 = 107, ANI_WAR_CRY1, ANI_WAR_CRY2, ANI_WAR_CRY3, ANI_WAR_CRY4, 

				ANI_SKILL_AXE0 = 112, ANI_SKILL_AXE1, ANI_SKILL_AXE2, ANI_SKILL_AXE3, 
				ANI_SKILL_DAGGER0 = 116, ANI_SKILL_DAGGER1,
				ANI_SKILL_DUAL0 = 118, ANI_SKILL_DUAL1,
				ANI_SKILL_BLUNT0 = 120, ANI_SKILL_BLUNT1, ANI_SKILL_BLUNT2, ANI_SKILL_BLUNT3, 
				ANI_SKILL_POLEARM0 = 124, ANI_SKILL_POLEARM1,
				ANI_SKILL_SPEAR0 = 126, ANI_SKILL_SPEAR1,
				ANI_SKILL_SWORD0 = 128, ANI_SKILL_SWORD1, ANI_SKILL_SWORD2, ANI_SKILL_SWORD3, 
				ANI_SKILL_AXE2H0 = 132, ANI_SKILL_AXE2H1,
				ANI_SKILL_SWORD2H0 = 134, ANI_SKILL_SWORD2H1,

				// â�Â®â–’Ã�â•‘â•¬â�¼â•â�¤â�¬ NPC Animation
				ANI_NPC_BREATH = 0, ANI_NPC_WALK, ANI_NPC_RUN, ANI_NPC_WALK_BACKWARD,
				ANI_NPC_ATTACK0 = 4, ANI_NPC_ATTACK1, ANI_NPC_STRUCK0, ANI_NPC_STRUCK1, ANI_NPC_STRUCK2, ANI_NPC_GUARD, 
				ANI_NPC_DEAD0 = 10, ANI_NPC_DEAD1, ANI_NPC_TALK0, ANI_NPC_TALK1, ANI_NPC_TALK2, ANI_NPC_TALK3, 
				ANI_NPC_SPELLMAGIC0 = 16, ANI_NPC_SPELLMAGIC1, 

				ANI_UNKNOWN = 0xffffffff };


//MAX_INCLINE_CLIMB = sqrt( 1 - sin(90-â�œÃâ�¤Ã™â–‘Âµâ•—Ã¾â–‘Ã³)^2 )
//const	float MAX_INCLINE_CLIMB = 0.5f;	   // â�â�œÃ‚Â¾â–‘Ã‘â•Ã· â��Ãâ�¤â�¬ â�œÃâ�¤Ã™ â–‘Âµâ•—Ã¾â–‘Â¬ = 30 ÃÃ
const	float MAX_INCLINE_CLIMB = 0.6430f; // â�â�œÃ‚Â¾â–‘Ã‘â•Ã· â��Ãâ�¤â�¬ â�œÃâ�¤Ã™ â–‘Âµâ•—Ã¾â–‘Â¬ = 40 ÃÃ
//const	float MAX_INCLINE_CLIMB = 0.7071f; // â�â�œÃ‚Â¾â–‘Ã‘â•Ã· â��Ãâ�¤â�¬ â�œÃâ�¤Ã™ â–‘Âµâ•—Ã¾â–‘Â¬ = 45 ÃÃ
//const	float MAX_INCLINE_CLIMB = 0.7660f; // â�â�œÃ‚Â¾â–‘Ã‘â•Ã· â��Ãâ�¤â�¬ â�œÃâ�¤Ã™ â–‘Âµâ•—Ã¾â–‘Â¬ = 50 ÃÃ
//const	float MAX_INCLINE_CLIMB = 0.8660f; // â�â�œÃ‚Â¾â–‘Ã‘â•Ã· â��Ãâ�¤â�¬ â�œÃâ�¤Ã™ â–‘Âµâ•—Ã¾â–‘Â¬ = 60 ÃÃ


enum e_MoveDirection { MD_STOP, MD_FOWARD, MD_BACKWARD, MD_UNKNOWN = 0xffffffff };

const float MOVE_DELTA_WHEN_RUNNING = 3.0f; // Ã‚â–ˆÃ‚Âº â–‘Ã·ÃƒÃâ�´Ã·â�¤â�¬ â•‘Â»â•Ã·..
const float MOVE_SPEED_WHEN_WALK = 1.5f; // Player ÃÃšâ��â•  â–‘â•�â��â•—Ã‚Âº ÃƒÃ‘â�´Ãâ•Ã‹ÃÃ

// ÃƒÃ·â��Ã¾ â•—Â¾â�¼â�¬...
enum e_StateMove {	PSM_STOP = 0,
					PSM_WALK,
					PSM_RUN,
					PSM_WALK_BACKWARD,
					PSM_COUNT };

enum e_StateAction {	PSA_BASIC = 0,		// Â¥Ã£â•£Â½â�´â– ÃÃ Â¥â•šÃƒÂ¤â–‘Ã â��Ãâ��Â¢...
						PSA_ATTACK,			// â–‘Â°â–‘Â¦ â•—Â¾â�¼â�¬.. 
						PSA_GUARD,			// â•£ÂµÂ¥Â¯â•â•‘â–‘Â° - Â©Ã€â��Â¢..
						PSA_STRUCK,			// Â¥â€—Â¥Â¯â�¼â•â�´Â³.
						PSA_DYING,			// â�´ÃŽâ–‘Ã â��Ãâ�¤â�¬â�´â–€(Â¥â–“Ã€Â»â�´Ã·â�¤â�¬â�´â–€)
						PSA_DEATH,			// â�´ÃŽÂ¥Â¯â•Â¡ â•—Â©â��Â¢..
						PSA_SPELLMAGIC,		// Â©Ã‚â•£Â² â�´Ãâ•£Â« â�â–„â�Ã½â�¤â�¬ â�´â–€..
						PSA_SITDOWN, 		// Â¥â•�Â¥Ã£ â��Ãâ�¤â�¬â�´â–€...
						PSA_COUNT }; 

enum e_StateDying {		PSD_DISJOINT = 0,	// â•‘Ã°ÃƒÃÃÃƒÂ¥Â¯ â�´ÃŽâ�¤â�¬â�¤â�˜.. 
						PSD_KNOCK_DOWN,		// â�‚Â»Â¥Ã£â–‘Ã­â–‘â�¼â�‚Â¬ Ãâ�ŒÃ€â•¬ â•£Ã°Â©Â«Â©Ãšâ•Â¡ â�´ÃŽâ�¤â�¬â�¤â�˜.
						PSD_KEEP_POSITION,	// â�´Âª â��â�ŒÂ©Â«â�Ã­â•Â¡ Ã£Â¹â��Ã�â–‘Ã â�´ÃŽâ�¤â�¬â�¤â�˜..
						PSD_COUNT,

						PSD_UNKNOWN = 0xffffffff };

enum e_StateParty {	PSP_NORMAL = 0,
					PSP_POISONING = 1,
					PSP_CURSED = 2,
					PSP_MAGIC_TAKEN = 4,
					PSP_BLESSED = 8,
					PSP_UNKNOWN = 0xffffffff };

enum e_PartPosition	{	PART_POS_UPPER = 0,
						PART_POS_LOWER,
						PART_POS_FACE,
						PART_POS_HANDS,
						PART_POS_FEET, 
						PART_POS_HAIR_HELMET,
						PART_POS_COUNT,
						PART_POS_UNKNOWN = 0xffffffff };

enum e_PlugPosition {	PLUG_POS_RIGHTHAND = 0,
						PLUG_POS_LEFTHAND, 
						PLUG_POS_BACK, 
						PLUG_POS_KNIGHTS_GRADE, 
						PLUG_POS_COUNT,
						PLUG_POS_UNKNOWN = 0xffffffff };

/*
enum e_ItemClass	{	ITEM_CLASS_DAGGER = 1, // â�¤â–„â–‘â•¦(dagger)
						ITEM_CLASS_SWORD, // 2 : ÃƒÃâ•Ä±â–‘â•¦(onehandsword)
						ITEM_CLASS_SWORD_2H, // 3 : Â¥Ã¾â•Ä±â–‘â•¦(twohandsword)
						ITEM_CLASS_AXE, // 4 : ÃƒÃâ•Ä±ÃÃâ�‚Ã³(onehandaxe)
						ITEM_CLASS_AXE_2H, // 5 : Ãâ•¬â•Ä±ÃÃâ�‚Ã³(twohandaxe)
						ITEM_CLASS_MACE, // 6 : ÃƒÃâ•Ä±â�¼Â©â–‘Â¦â•£Â½â–’Ã�(mace)
						ITEM_CLASS_MACE_2H, // 7 : Ãâ•¬â•Ä±â�¼Â©â–‘Â¦â•£Â½â–’Ã�(twohandmace)
						ITEM_CLASS_SPEAR, // 8 : â�œÃ³(spear)
						ITEM_CLASS_POLEARM, // 9 : Ã£Â·Â¥Â¤(polearm)
						
						ITEM_CLASS_SHIELD_SMALL = 11, // 11 : Â¢â•‘Â©Â¶Â¢Â»ÃÃ•(smallshield)
						ITEM_CLASS_SHIELD_LARGE, // 12 : Ã‚Â¾Â¥Ã£â�´Ã·Â¢Â»ÃÃ•(largeshield)
						ITEM_CLASS_SHIELD_KITE, // 13 : â�€Â½â��â• Ã£Â«Â¢Â»ÃÃ•(kiteshield)
						ITEM_CLASS_SHIELD_LARGETKITE, // 14 : Ã‚Â¾Â¥Ã£â�´Ã·â�€Â½â��â• Ã£Â«(largekite)
						ITEM_CLASS_SHIELD_PLATE, // 15 : Ãƒâ�œÃ€â•£â��â• Ã£Â«Â¢Â»ÃÃ•(plateshield)
						
						ITEM_CLASS_BOW_SHORT = 21, // 21 : â•Â¯Ã£Â«â•‘Â©â�Ã½(Shortbow)
						ITEM_CLASS_BOW_WRAPT, // 22 : Ã€ÂªÃ£Â«â•‘Â©â�Ã½(wraptbow)
						ITEM_CLASS_BOW_COMPOSITE, // 23 : â�€ÃŒÃ£Â¸â�´Ã·Ã£Â«â•‘Â©â�Ã½(compositebow)
						ITEM_CLASS_BOW_IRON, // 24 : Â¥Ã£â��â• Â¥Â­â•‘Â©â�Ã½(ironbow)
						ITEM_CLASS_BOW_LONG, // 25 : Ã€Ä±â•‘Â©â�Ã½(longbow)
						ITEM_CLASS_BOW_CROSS, // 28 : â�¼Â®Ã€â•¬Â¢â•‘â•‘Â©â�Ã½(crossbow)
						
						ITEM_CLASS_STAFF = 31, // 31 : â�´Ã·Ã£â•¬â��â• (staff)
						ITEM_CLASS_ETC, // 32 : â–’Ã�â�¼Â© Â©Ã‚â•£Â² â•£â–‘Ãƒâ–‘
						
						ITEM_CLASS_ARMOR_COTTON = 41, // 41 : â�œÃâ•£ÂµÂ¥Â¯â–’Â©(cotton)
						ITEM_CLASS_ARMOR_FUR, // 42 : â�¼Ã°â–‘Ã­â�´ÃŽ(Fur)
						ITEM_CLASS_ARMOR_LEATHER, // 43 : â–‘Ã­â�´ÃŽâ•£ÂµÂ¥Â¯â–’Â©(leather)
						ITEM_CLASS_ARMOR_HADLEATHER, // 44 : ÃƒÂ¤ÃÃ•Ã€â•£â�¤Â§â•£ÂµÂ¥Â¯â–’Â©(hardleather)
						ITEM_CLASS_ARMOR_RINGMAIL, // 45 : Â©Ãâ•£ÂµÂ¥Â¯â–’Â©(ringmail)
						ITEM_CLASS_ARMOR_SCALEMAIL, // 46 : â•‘Â±â�¤â�œâ•£ÂµÂ¥Â¯â–’Â©(scaledmail)
						ITEM_CLASS_ARMOR_HALFPLATE, // 47 : ÃƒÂ¤Ãƒâ�´ Ãƒâ�œÃ€â•£â��â• Ã£Â« â•£ÂµÂ¥Â¯â–’Â©
						ITEM_CLASS_ARMOR_FULLPLATE, // 48 : â�œÃ‚Ã£Ãƒâ•£ÂµÂ¥Â¯â–’Â©(platemail)
						ITEM_CLASS_ROBE, // 49 : Â©Ã‚â•£Â²â•—Ã¾Ã€â•¬â•‘Ã›(robe)
						
						ITEM_CLASS_ARROW = 101,
						
						ITEM_CLASS_UNKNOWN = 0xffffffff }; // 101: â•šÂ¡â•—Ã½(arrow) 
*/
enum e_ItemAttrib	{
						ITEM_ATTRIB_GENERAL = 0,
						ITEM_ATTRIB_MAGIC	= 1,
						ITEM_ATTRIB_LAIR	= 2,
						ITEM_ATTRIB_CRAFT	= 3,
						ITEM_ATTRIB_UNIQUE	= 4,
						ITEM_ATTRIB_UPGRADE	= 5,		
						ITEM_ATTRIB_UNKNOWN = 0xffffffff };	

enum e_ItemClass	{	ITEM_CLASS_DAGGER = 11, // â�¤â–„â–‘â•¦(dagger)
						ITEM_CLASS_SWORD = 21, // ÃƒÃâ•Ä±â–‘â•¦(onehandsword)
						ITEM_CLASS_SWORD_2H = 22, // 3 : Â¥Ã¾â•Ä±â–‘â•¦(twohandsword)
						ITEM_CLASS_AXE = 31, // ÃƒÃâ•Ä±ÃÃâ�‚Ã³(onehandaxe)
						ITEM_CLASS_AXE_2H = 32, // Ãâ•¬â•Ä±ÃÃâ�‚Ã³(twohandaxe)
						ITEM_CLASS_MACE = 41, // ÃƒÃâ•Ä±â�¼Â©â–‘Â¦â•£Â½â–’Ã�(mace)
						ITEM_CLASS_MACE_2H = 42, // Ãâ•¬â•Ä±â�¼Â©â–‘Â¦â•£Â½â–’Ã�(twohandmace)
						ITEM_CLASS_SPEAR = 51, // â�œÃ³(spear)
						ITEM_CLASS_POLEARM = 52, // Ã£Â·Â¥Â¤(polearm)
						
						ITEM_CLASS_SHIELD = 60, // Â¢Â»ÃÃ•(shield)

						ITEM_CLASS_BOW = 70, //  â•Â¯Ã£Â«â•‘Â©â�Ã½(Shortbow)
						ITEM_CLASS_BOW_CROSS = 71, // â�¼Â®Ã€â•¬Â¢â•‘â•‘Â©â�Ã½(crossbow)
						ITEM_CLASS_BOW_LONG = 80, // Ã€Ä±â•‘Â©â�Ã½(longbow)

						ITEM_CLASS_EARRING = 91, // â–’â•â–‘â•�â��â• 
						ITEM_CLASS_AMULET = 92, // Â©Â±â–‘â•�â��â• 
						ITEM_CLASS_RING = 93, // â•£Â¦â�´Ã·
						ITEM_CLASS_BELT = 94, // ÃƒÃ’Â©Â«Ã‚Ã½
						ITEM_CLASS_CHARM = 95, //â��â•¬â•‘Ã‘â�¼ÃµÂ©Â«â�Ã­ â�´Ã·â�¤Â¤â–‘Ã â��Ãâ�¤â�¬ Â¥Ã£â��â• â�¼â–ˆ
						ITEM_CLASS_JEWEL = 96, //â•‘Â©â•Â«â�´Â¥Ã€Â¨
						ITEM_CLASS_POTION = 97, // â•£â–‘Â¥Ã“
						ITEM_CLASS_SCROLL = 98, // Â¢â•‘â�¼Â®Ã€Ã

						ITEM_CLASS_LAUNCHER = 100, // â�œÃ³â�¤Â°â�´Â·Ã‚Âº Â¥â–“â�¤â�¬ Â¥Ã£â��â• â�¼â–ˆ..
						
						ITEM_CLASS_STAFF = 110, // â�´Ã·Ã£â•¬â��â• (staff)
						ITEM_CLASS_ARROW = 120, // â•šÂ¡â•—Ã½(Arrow)
						ITEM_CLASS_JAVELIN = 130, // â�¼Â§â�œÃ³
						
						ITEM_CLASS_ARMOR_WARRIOR = 210, // â��Â³â•—Ã¾ â•£ÂµÂ¥Â¯â–’Â©
						ITEM_CLASS_ARMOR_ROGUE = 220, // Ã€â•¬â–’ÃŽ â•£ÂµÂ¥Â¯â–’Â©
						ITEM_CLASS_ARMOR_MAGE = 230, // Â©Ã‚â•£Â²â•—Ã¾ â•£ÂµÂ¥Â¯â–’Â©
						ITEM_CLASS_ARMOR_PRIEST = 240, // â•—Ã¾â�´Âª â•£ÂµÂ¥Â¯â–’Â©

						ITEM_CLASS_ETC = 251, // â–’Ã�â�¼Â©

						ITEM_CLASS_UNKNOWN = 0xffffffff }; // 

enum e_Nation { NATION_NOTSELECTED = 0, NATION_KARUS, NATION_ELMORAD, NATION_UNKNOWN = 0xffffffff };

struct __TABLE_ITEM_BASIC;
struct __TABLE_ITEM_EXT;
struct __TABLE_PLAYER;

enum e_Authority { AUTHORITY_MANAGER = 0, AUTHORITY_USER, AUTHORITY_NOCHAT, AUTHORITY_NPC, AUTHORITY_BLOCK_USER = 0xff };

struct __InfoPlayerBase
{
	int			iID;			// â–‘Ãâ��Â» ID
	std::string szID;			// â��â• Â©Âº
	D3DCOLOR	crID;			// â��â• Â©Âº â•—Ã·â–’â€—..
	e_Race		eRace;			// â�€â�‚Â©Â»â�¼â• â–‘Â±â–‘Â¦â�Ã­ ÃÂ¹Â©Ã‘ â�´Â¥â�´Ã€
	e_Nation	eNation;		// â•ÃŠâ•Ã‹ â–’â•£â–‘Ã­..
	e_Class		eClass;			// â�´Â¸Â¥Â¸
	int			iLevel;			// Ã€â•£â•‘Âº
	int			iHPMax;	
	int			iHP; 
	int			iAuthority;		// â–’ÃƒÃƒÃ - 0 â–‘Â³Â©Â«â��â�Œ, 1 - â��Â¤â•£Â¦â��Â»â��Â·, 255 - â•‘ÃÃ€â–‘â�¤Ã¾ÃƒÃ â��Â»â��Â·...

	bool		bRenderID;		// â•šÂ¡Â©Ãšâ�Ã­ ID Â©Âª â�¬Â´â�¤â�¬â�´Ã·..

	void Init()
	{
		iID = 0;					// â–‘Ãâ��Â» ID
		szID = "";					// â��â• Â©Âº
		crID = 0;					// â��â• Â©Âº â•—Ã·â–’â€—..
		eRace = RACE_UNKNOWN;		// â�€â�‚Â©Â»â�¼â• â–‘Â±â–‘Â¦â�Ã­ ÃÂ¹Â©Ã‘ â�´Â¥â�´Ã€
		eNation = NATION_UNKNOWN;	// â•ÃŠâ•Ã‹ â–’â•£â–‘Ã­..
		eClass = CLASS_UNKNOWN;		// â�´Â¸Â¥Â¸
		iLevel = 0;					// Ã€â•£â•‘Âº
		iHPMax = 0;	
		iHP = 0; 
		iAuthority = 1;				// â–’ÃƒÃƒÃ - 0 â–‘Â³Â©Â«â��â�Œ, 1 - â��Â¤â•£Â¦â��Â»â��Â·, 255 - â•‘ÃÃ€â–‘â�¤Ã¾ÃƒÃ â��Â»â��Â·...
		bRenderID = true;
	}
};

struct __InfoPlayerOther
{
	int			iFace;			// Â¥Â¾â–’â• Â©Â­Â¥Ã¾..
	int			iHair;			// Â©Ã‹Â©Â«â�€Â½Ã‚Â¶ Â©Â­Â¥Ã¾..

	int			iCity;			// â•ÃŠâ•Ã‹ÃÃÂ¢â�œ
	int			iKnightsID;		// â•ÃŠâ•Ã‹ â–’Ã�â•—Ã¾â�¤â–„(â–’ÂµÃÃ•) ID
	std::string szKnights;		// â•ÃŠâ•Ã‹ â–’Ã�â•—Ã¾â�¤â–„(â–’ÂµÃÃ•) â��â• Â©Âº
	int			iKnightsGrade;	// â•ÃŠâ•Ã‹ â–’Ã�â•—Ã¾â�¤â–„(â–’ÂµÃÃ•) ÃÂ¯â–’ÃŒ
	int			iKnightsRank;	// â•ÃŠâ•Ã‹ â–’Ã�â•—Ã¾â�¤â–„(â–’ÂµÃÃ•) â•Â°â��Âº

	int			iRank;			// â��â–ˆâ��Âº - â•£Ãšâ��â–ˆ, â–‘Â°â��â–ˆ - â–’ÃƒÃƒÃâ�Ã­ ÃÂ¹Â©Ã‘ â•‘Ã°Ã€Â¨
	int			iTitle;			// â�´Â¸â��Âº - â�Ãâ�´Ã, â•â•‘â�´Ã - â�¤â–„â•Â°ÃƒÃ Â¢â�¼â•‘Ã° -> â–’ÂµÃÃ•â��Ã•...

	void Init()
	{
		iFace = 0;			// Â¥Â¾â–’â• Â©Â­Â¥Ã¾..
		iHair = 0;			// Â©Ã‹Â©Â«â�€Â½Ã‚Â¶ Â©Â­Â¥Ã¾..
		iCity;				// â•ÃŠâ•Ã‹ÃÃÂ¢â�œ
		iKnightsID = 0;		// â•ÃŠâ•Ã‹ â–’Ã�â•—Ã¾â�¤â–„(â–’ÂµÃÃ•)
		szKnights = "";		// â•ÃŠâ•Ã‹ â–’Ã�â•—Ã¾â�¤â–„(â–’ÂµÃÃ•)
		iKnightsGrade = 0;		// â•ÃŠâ•Ã‹ â–’Ã�â•—Ã¾â�¤â–„(â–’ÂµÃÃ•) â•—Ã·â–’â€—
		iKnightsRank = 0;			// â��â–ˆâ��Âº - â•£Ãšâ��â–ˆ, â–‘Â°â��â–ˆ - â–’ÃƒÃƒÃâ�Ã­ ÃÂ¹Â©Ã‘ â•‘Ã°Ã€Â¨
		iTitle = 0;			// â�´Â¸â��Âº - â�Ãâ�´Ã, â•â•‘â�´Ã - â�¤â–„â•Â°ÃƒÃ Â¢â�¼â•‘Ã° -> â–’ÂµÃÃ•â��Ã•...
	}
};

// â–’Ã�â•—Ã¾â�¤â–„ â�´Â¸â��Âº..
enum e_KnightsDuty {	KNIGHTS_DUTY_UNKNOWN = 0,		// ????? â�¬Ãâ–‘â–„â�‚â–“??
						KNIGHTS_DUTY_CHIEF = 1,			// â�¤â–„â��Ã•
						KNIGHTS_DUTY_VICECHIEF = 2,		// â•‘â•¬â�¤â–„â��Ã• 
						KNIGHTS_DUTY_PUNISH = 3,		// â�¬Ã­â–‘Ãžâ�´â–€.
						KNIGHTS_DUTY_TRAINEE = 4,		// â–‘â–€Â¢â��â–’Ã�â•—Ã¾
						KNIGHTS_DUTY_KNIGHT = 5,		// â��Â¤â•£Â¦â–’Ã�â•—Ã¾
						KNIGHTS_DUTY_OFFICER = 6		// â��Ã•â–’â�‚
					};

#define VICTORY_ABSENCE		0
#define VICTORY_KARUS		1
#define VICTORY_ELMORAD		2

struct __InfoPlayerMySelf : public __InfoPlayerOther
{
	int					iBonusPointRemain; // â�‚â–“â�¤â�¬ â•‘Â©â�‚â•©Â¢â•‘ Ã£Â¸â��â•¬Ã£Â«...
	int					iLevelPrev; // â�´Â¸â��Â³â��Ãƒ Ã€â•£â•‘Âº...

	int					iMSPMax; 
	int					iMSP; 
			
	int					iTargetHPPercent;
	int					iGold;
	int					iExpNext;
	int					iExp; 
	int					iRealmPoint;		// â–’â•£â–‘Ã­ â–’Ã�â�Â®ÃÃ
	e_KnightsDuty		eKnightsDuty;		// â–’Ã�â•—Ã¾â�¤â–„ â–’ÃƒÃƒÃ
	int					iWeightMax;			// Â¥Ã£â��â• â�¼â–ˆâ��Ãƒ â�œÃâ•£Â½â–‘Ãˆ
	int					iWeight;			// ÃÃšâ•Ã· â��Ãâ�¤â�¬ Â¥Ã£â��â• â�¼â–ˆ â•£Â½â–‘Ãˆ..
	int					iStrength;			// â•šÂ¹
	int					iStrength_Delta;		// Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
	int					iStamina;			// Â¢â•‘â�¼ÃŽâ•£â• â�‚â•©
	int					iStamina_Delta;		// Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
	int					iDexterity;			// â•£â•¬â�œÃ€â•â•‘..
	int					iDexterity_Delta;	// Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
	int					iIntelligence;		// â�´Ã·â�¤â•�
	int					iIntelligence_Delta; // Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
	int 				iMagicAttak;		// Â©Ã‚Ã€â�¬
	int 				iMagicAttak_Delta;	// Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
	
	int 				iAttack;		// â–‘Â°â–‘Â¦Ã€â�¬
	int 				iAttack_Delta;	// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•š â–‘Â¬..
	int 				iGuard;			// â•£ÂµÂ¥Â¯Ã€â�¬
	int 				iGuard_Delta;	// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•š â–‘Â¬..

	int 				iRegistFire;			// â��Â·ÃƒÃŽÃ€â�¬
	int 				iRegistFire_Delta;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
	int 				iRegistCold;			// â��Â·ÃƒÃŽÃ€â�¬
	int 				iRegistCold_Delta;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
	int 				iRegistLight;			// â��Â·ÃƒÃŽÃ€â�¬
	int 				iRegistLight_Delta;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
	int 				iRegistMagic;			// â��Â·ÃƒÃŽÃ€â�¬
	int 				iRegistMagic_Delta;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
	int 				iRegistCurse;			// â��Â·ÃƒÃŽÃ€â�¬
	int 				iRegistCurse_Delta;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
	int 				iRegistPoison;			// â��Â·ÃƒÃŽÃ€â�¬
	int 				iRegistPoison_Delta;	// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..

	int					iZoneInit;				// â•Â¡â•£Ã·ÃƒÃâ�¼ÃŽ â�œâ�‚â��Â¢â�Ã­ â•£ÃŒâ��â•‘ â�´Â©â•£Â°â•šÃº
	int					iZoneCur;				// ÃƒÃ·â��Ã¾ â�´Â©..
	int					iVictoryNation;			// 0: â•£Â½Â¢â�¬â•‘â•¬ 1:â�Ã±Â©Â­Ã‚Â¾ÃÃ• Â¢â�¬Â©Â« 2:â�€Â½Ã€Ã¾Â¢â•‘ Â¢â�¬Â©Â«

	void Init()
	{
		__InfoPlayerOther::Init();

		iBonusPointRemain = 0; // â�‚â–“â�¤â�¬ â•‘Â©â�‚â•©Â¢â•‘ Ã£Â¸â��â•¬Ã£Â«...
		iLevelPrev = 0; // â�´Â¸â��Â³â��Ãƒ Ã€â•£â•‘Âº...

		iMSPMax = 0; 
		iMSP = 0; 
		
		iTargetHPPercent = 0;
		iGold = 0;
		iExpNext = 0;
		iExp = 0; 
		iRealmPoint = 0;		// â–’â•£â–‘Ã­ â–’Ã�â�Â®ÃÃ
		eKnightsDuty = KNIGHTS_DUTY_UNKNOWN;		// â–’Ã�â•—Ã¾â�¤â–„ â–’ÃƒÃƒÃ
		iWeightMax = 0;			// Â¥Ã£â��â• â�¼â–ˆâ��Ãƒ â�œÃâ•£Â½â–‘Ãˆ
		iWeight = 0;			// ÃÃšâ•Ã· â��Ãâ�¤â�¬ Â¥Ã£â��â• â�¼â–ˆ â•£Â½â–‘Ãˆ..
		iStrength = 0;			// â•šÂ¹
		iStrength_Delta = 0;	// Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
		iStamina = 0;			// Â¢â•‘â�¼ÃŽâ•£â• â�‚â•©
		iStamina_Delta = 0;		// Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
		iDexterity = 0;			// â•£â•¬â�œÃ€â•â•‘..
		iDexterity_Delta = 0;	// Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
		iIntelligence = 0;		// â�´Ã·â�¤â•�
		iIntelligence_Delta = 0; // Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
		iMagicAttak = 0;		// Â©Ã‚Ã€â�¬
		iMagicAttak_Delta = 0;	// Â¥Ã£â��â• â�¼â–ˆâ��â• â�‚Â¬ Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•šâ–‘Â¬.
		
		iAttack = 0;		// â–‘Â°â–‘Â¦Ã€â�¬
		iAttack_Delta = 0;	// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•š â–‘Â¬..
		iGuard = 0;			// â•£ÂµÂ¥Â¯Ã€â�¬
		iGuard_Delta = 0;	// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â–‘Ã­â–‘Â¿Ãâ•š â–‘Â¬..

		iRegistFire = 0;			// â��Â·ÃƒÃŽÃ€â�¬
		iRegistFire_Delta = 0;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
		iRegistCold = 0;			// â��Â·ÃƒÃŽÃ€â�¬
		iRegistCold_Delta = 0;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
		iRegistLight = 0;			// â��Â·ÃƒÃŽÃ€â�¬
		iRegistLight_Delta = 0;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
		iRegistMagic = 0;			// â��Â·ÃƒÃŽÃ€â�¬
		iRegistMagic_Delta = 0;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
		iRegistCurse = 0;			// â��Â·ÃƒÃŽÃ€â�¬
		iRegistCurse_Delta = 0;		// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..
		iRegistPoison = 0;			// â��Â·ÃƒÃŽÃ€â�¬
		iRegistPoison_Delta = 0;	// Â©Ã‚â•£Â²â�Ã­ â��ÃƒÃƒÃ â��Â·ÃƒÃŽÃ€â�¬ â•‘Â»â•šÂ¡â–‘Â¬..

		iZoneInit = 0x01;			// â•Â¡â•£Ã·ÃƒÃâ�¼ÃŽ â�œâ�‚â��Â¢â�Ã­ â•£ÃŒâ��â•‘ â�´Â©â•£Â°â•šÃº
		iZoneCur = 0;				// ÃƒÃ·â��Ã¾ â�´Â©..
		iVictoryNation = -1;		// â��Â³â��Â´â�Ã­â•Â¡ â��â• â–’Ãµ â–’â•£â–‘Ã­
	}
};

const int MAX_PARTY_OR_FORCE = 8;

struct __InfoPartyOrForce
{
	int			iID;			// Ã£â�€Ã£â•â�Â° ID
	int			iLevel;			// Level
	e_Class		eClass;			// â�´Â¸Â¥Â¸
	int			iHP;			// Hit Point
	int			iHPMax;			// Hit Point Max
	bool		bSufferDown_HP;			// Status - HP Ã‚â�‚Â¥Â¯â�´Â³...
	bool		bSufferDown_Etc;		// Status - â��Â·â�´Ãâ–‘Ãžâ�Â¡ â�ÃÃƒÃ�â��â•— â•£ÃŒâ��â•—Ã‚Âº
	std::string szID;		// Ã£â�€Ã£â• â��â• Â©Âº â•£Â«â��â�Œâ�Â¡

	void Init()
	{
		iID = -1;
		iLevel = 0;
		eClass = CLASS_UNKNOWN;
		iHP = 0;
		iHPMax = 0;
		szID = "";

		bSufferDown_HP = false;			// Status - HP Ã‚â�‚Â¥Â¯â�´Â³...
		bSufferDown_Etc = false;		// Status - â��Â·â�´Ãâ–‘Ãžâ�Â¡ â�ÃÃƒÃ�â��â•— â•£ÃŒâ��â•—Ã‚Âº
	};

	__InfoPartyOrForce()
	{
		this->Init();
	}
};

enum e_PartyStatus { PARTY_STATUS_DOWN_HP = 1, PARTY_STATUS_DOWN_ETC = 2 };

struct __InfoPartyBBS // Ã£â�€Ã£â• â�´Ã·â�Â° â–‘ÃˆÂ¢â�œÃ£Ãƒ â–’Â©â�´Ã‚â�œâ•..
{
	std::string szID;			// Ã£â�€Ã£â• â��â• Â©Âº â•£Â«â��â�Œâ�Â¡
	int			iID;			// Ã£â�€Ã£â•â�Â° ID
	int			iLevel;			// Level
	e_Class		eClass;			// â�´Â¸Â¥Â¸
	int			iMemberCount;

	void Init()
	{
		szID = "";
		iID = -1;
		iLevel = 0;
		eClass = CLASS_UNKNOWN;
		iMemberCount = 0;
	};

	__InfoPartyBBS()
	{
		this->Init();
	}
};

enum {	RESRC_UI_LOGIN = 0, RESRC_UI_CHARACTERSELECT, RESRC_UI_CHARACTERCREATE, RESRC_UI_INVENTORY, RESRC_UI_CHAT,
		RESRC_UI_MYINFORMATION, RESRC_UI_TARGET, RESRC_UI_HOTKEY, RESRC_UI_FUNCTION };

typedef struct __TABLE_ZONE
{
	DWORD		dwID; // zone ID
	// CN data has additional columns; map sequentially to keep alignment.
	std::string	szTerrainFN; // Terrain FileName - Terrain, Tile Map, ColorMap....
	std::string	szColorMapFN;
	std::string	szLightMapFN;
	std::string	szObjectPostDataFN; // Object 위치 정보 - �안� 충� 체�� ��터� 들어 있다.
	std::string	szMiniMapFN;
	std::string szSkySetting; // 하늘과 분위기 조정한 세팅 정보 파�..
	std::string szUnknownStr1;
	std::string szUnknownStr2;
	BOOL		bIndicateEnemyPlayer;	// �국 플레�어를 표시하나?? (mapped to int column)
	int		iFixedSundDirection;	// 해� 방향� 고정시키는지..
	std::string szLightObjFN;		// 지형� 배치�어 있는 ��트정보파�..
	std::string szUnknownStr3;
	int		iUnknown1;
	std::string szUnknownStr4;
	float		fUnknown1;
	std::string szUnknownStr5;
	DWORD		dwUnknownD1;
	DWORD		dwUnknownD2;
	DWORD		dwUnknownD3;
	DWORD		dwUnknownD4;
	std::string szUnknownStr6;
	int		iUnknown2;
	std::string szUnknownStr7;
} TABLE_ZONE;

typedef struct __TABLE_UI_RESRC
{
	DWORD dwID;							// Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ â•ÃŠâ•Ã‹ â–’â•£â–‘Ã­â�Ã­ ÃÂ¹Â©Ã‘ â�¼â–‘Ã€â•¬ Â¥â�¤â�¤â�˜..

	std::string szLogIn;				// 0
	std::string szCmd;					// 1
	std::string szChat;					// 2
	std::string szMsgOutput;			// 3
	std::string szStateBar;				// 4
	std::string szVarious;				// 5 - â�€â�‚Â©Â»â�¼â• â•—Â¾â�¼â�¬, â–’Ã�â•—Ã¾â�¤â–„ â–‘Â³Â©Â«, â�€Â¨Â¢â•‘Ã£Â«, â�€Ãºâ–’Â©.. ÃÂ¯ÃÂ¯ â�‚ÃŽÃ£Ãµâ��â• â�´Ã·â–‘Ã­ ÃƒÃâ–‘â�‚â�Ã­ ÃÃšÂ¥Â¯â��Ãâ��Â¢.
	std::string szState;				// 6 - â�€â�‚Â©Â»â�¼â• â•—Â¾â�¼â�¬
	std::string szKnights;				// 7 - â–’Ã�â•—Ã¾â�¤â–„ â–‘Â³Â©Â«
	std::string szQuest;				// 8 - â�€Â¨Â¢â•‘Ã£Â«
	std::string szFriends;				// 9 - â�€Ãºâ–’Â© â–‘Â³Â©Â«

	std::string szInventory;			// 10
	std::string szTransaction;			// 11
	std::string szDroppedItem;			// 12

	std::string szTargetBar;			// 13
	std::string szTargetSymbolShape;	// 14
	std::string szSkillTree;			// 15
	std::string szHotKey;				// 16

	std::string szMiniMap;				// 17
	std::string szPartyOrForce;			// 18
	std::string szPartyBBS;				// 19

	std::string szHelp;					// 20
	std::string szNotice;				// 21

	std::string szCharacterCreate;		// 22
	std::string szCharacterSelect;		// 23

	std::string szToolTip;				// 24
	std::string szMessageBox;			// 25
	std::string szLoading;				// 26
	std::string szItemInfo;				// 27
	std::string szPersonalTrade;		// 28
	std::string szPersonalTradeEdit;	// 29

	std::string szNpcEvent;				// 30
	std::string szZoneChangeOrWarp;		// 31
	std::string szExchangeRepair;		// 32

	std::string szRepairTooltip;		// 33
	std::string szNpcTalk;				// 34
	std::string szNpcExchangeList;		// 35

	std::string szKnightsOperation;		// 36
	std::string szClassChange;			// 37

	std::string szEndingDisplay;		// 38
	std::string szWareHouse;			// 39

	std::string szChangeClassInit;		// 40
	std::string szChangeInitBill;		// 41

	std::string szInn;					// 42
	std::string szInputClanName;		// 43
	std::string szTradeBBS;				// 44
	std::string szTradeBBSSelector;		// 45
	std::string szTradeExplanation;		// 46
	std::string szTradeMemolist;		// 47
	std::string szQuestMenu;			// 48
	std::string szQuestTalk;			// 49
	std::string szDead;					// 50
	std::string szElLoading;			// 51
	std::string szKaLoading;			// 52
	std::string szNationSelect;			// 53

} TABLE_UI_RESRC;

typedef struct __TABLE_ITEM_BASIC // â��Ã•â�¬Â° Â¥Ã£â��â• â�¼â–ˆâ�Ã­ â–‘Â³ÃƒÃ Â©Â«â•ÃŠÂ¢â•‘ Ã€â•£â�€â�ŒÃÃ•...
{
	DWORD		dwID;				// 00 â�€â�ŒÃÃ•â•šÂ¡Ãâ•š Â¥Ã£â��â• â�¼â–ˆâ•£Â°â•šÃº d - // 00 - Item â�´Â¥Ã€Â¨, 00 - Item â��Ã•â�¬Â° â��Âºâ�€Ã­(â��Ã•â�¬Â°â��Âºâ�€Ã­Ã€â•¬ Plug â��â•¬â�´Ã· Part â��â•¬â�´Ã· Ã£Ãƒâ�¤â–„â��â•  â–‘Ã­â�¤â•�ÃƒÂ¤â�¤â�˜.) - 0000 - ItemIndex
	BYTE 		byExtIndex;			// 01 â•šÂ«â��Ã• â��â•¬ÃÂªÂ¢â•‘
	std::string	szName;				// 02 â��â• Â©Âº	
	std::string	szRemark;			// 03 Â¥Ã£â��â• â�¼â–ˆ â•â�‚Â©Ã	
	DWORD		dwIDResrc;			// 04 â�€â�ŒÃÃ•â•šÂ¡Ãâ•š Â¥Ã£â��â• â�¼â–ˆ Â©Â«â•ÃŠÂ¢â•‘	d
	DWORD		dwIDIcon;			// 05 â�€â�ŒÃÃ•â•šÂ¡Ãâ•š Â¥Ã£â��â• â�¼â–ˆ Â¥Ã£â��â• â�€â–„ Â©Â«â•ÃŠÂ¢â•‘	d
	DWORD		dwSoundID0;			// 06 Sound ID - 0 â��â• Â©Ãš â•—Ã¾â�Â¯ÃÃ• Â¥Â°â�¤â�˜~..
	DWORD		dwSoundID1;			// 07 Sound ID - 0 â��â• Â©Ãš â•—Ã¾â�Â¯ÃÃ• Â¥Â°â�¤â�˜~..

	BYTE	byClass;			// 08 Â¥Ã£â��â• â�¼â–ˆ ÃƒÂ³â�¼â�¬	b enum e_ItemClass â�¬Â³â�´Ã‚....
	BYTE	byIsRobeType;		// 09 â��ÂºÂ¥Ã£Ã€Ã­â–‘Ã­ â�¼Ã™â�¬â–‘Ã€â•¬ Ãâ•š Ã€â•¬â•‘Ã›â�¼Â©â��Ãˆâ��Ãƒ Â¥Ã£â��â• â�¼â–ˆ.....
	BYTE	byAttachPoint;		// 10 â��Ã•â�¬Â° â��Âºâ�€Ã­	b
	BYTE	byNeedRace;			// 11 â�´Â¥â�´Ã€	b
	BYTE	byNeedClass;		// 12 â�´Â¸Â¥Â¸	b

	short	siDamage;			// 13 â•£Â½â–’Ã�â�¼Â©â–‘Â¦	b
	short	siAttackInterval;	// 14 â–‘Â°â–‘Â¦ Â¢â�œâ–‘Ãº 100 â��â•‘ 1 â�œâ•©
	short	siAttackRange;		// 15 â��Â»â•šâ� â–‘â�¼Â©Â«	.1 meter â�¤â–„â��Âº
	short	siWeight;			// 16 â•£Â½â–‘Ãˆ .1 â�´â–€Ã€Â«â�¤â–„â��Âº
	short	siMaxDurability;	// 17 â�‚â•—â–’Â©Ã€â�¬
	int		iPrice;				// 18 Â©â�¼â•Ã·â–‘Ã­
	int		iPriceSale;			// 19 Â©â�¼ÃÃâ–‘Ã­
	short	siDefense;			// 20 â•£ÂµÂ¥Â¯Ã€â�¬
	BYTE	byContable;			// 21 â•Â²â��â�Œâ–‘Ã­ â��Ãâ�¤â�¬ Â¥Ã£â��â• â�¼â–ˆâ��â•¬â–‘Ã­??

	DWORD	dwEffectID1;		// 22 Â©Ã‚â•£Â² â•šâ�â–‘Â· ID1
	DWORD	dwEffectID2;		// 23 Â©Ã‚â•£Â² â•šâ�â–‘Â· ID1

	char	cNeedLevel;		// 24 â�Ãµâ–’Â© Ã€â•£â•‘Âº		Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ iLeve.. - â��Â¢â•Ã·â–‘Â¬â��â•  â��Ãâ��â•—â•Ã·ÃÃ â��Ãâ�¤â�˜..
	BYTE	byNeedRank;			// 25 â�Ãµâ–’Â© â��â–ˆâ��Âº		Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ iRank..
	BYTE	byNeedTitle;		// 26 â�Ãµâ–’Â© â�¼Â©â��â• Ã£â–“ 	Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ iTitle..
	BYTE	byNeedStrength;		// 27 â�Ãµâ–’Â© â•šÂ¹ 		Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ iStrength..
	BYTE	byNeedStamina;		// 28 â�Ãµâ–’Â© â�´Ã±Ã€â�¬ 	Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ iStamina..
	BYTE	byNeedDexterity;	// 29 â�Ãµâ–’Â© â•£â•¬â�œÂ©		Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ iDexterity..
	BYTE	byNeedInteli;		// 30 â�Ãµâ–’Â© â�´Ã·â�¤â•�		Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ iIntelligence..
	BYTE	byNeedMagicAttack;	// 31 â�Ãµâ–’Â© Â©Ã‚Ã€â�¬		Ãƒâ�œÃ€â•£â��â• Â¥Â¯â��Ãƒ iMagicAttak..

	BYTE	bySellGroup;		// 32 â•—Â¾â��â•¬â��â•  Ã£â�€â�¤â�¬ÃÃ‘â�Ã­ â�¤Ã™ÃƒÃ â–’ÃŽÃ€Ã½..
} TABLE_ITEM_BASIC;

const int MAX_ITEM_EXTENSION = 22; // â•šÂ«â��Ã• Â¥Ã£â��â• â�¼â–ˆ â�¼ÃŽâ��â• â•‘Ã â–‘â•£â•Ã·.
const int LIMIT_FX_DAMAGE = 64;
const int ITEM_UNIQUE = 4;
const int ITEM_LIMITED_EXHAUST = 17;

typedef struct __TABLE_ITEM_EXT // CN item extension layout (1097 client)
{
    DWORD       dwID;               // 00 - Item index
    std::string szHeader;           // 01
    DWORD       dwExtCode;          // 02 extra DWORD (unknown)
    std::string szRemark;           // 03
    DWORD       dwUnk1;             // 04 extra DWORD (unknown)
    DWORD       dwUnk2;             // 05 extra DWORD (unknown)
    DWORD       dwUnk3;             // 06 extra DWORD (unknown)
    BYTE        byMagicOrRare;      // 07

    short       siDamage;               // 08
    short       siAttackIntervalPercentage;        // 09
    short       siHitRate;              // 10
    short       siEvationRate;          // 11

    short       siMaxDurability;        // 12
    short       siPriceMultiply;        // 13
    short       siDefense;              // 14
    
    short       siDefenseRateDagger;    // 15
    short       siDefenseRateSword;     // 16
    short       siDefenseRateBlow;      // 17
    short       siDefenseRateAxe;       // 18
    short       siDefenseRateSpear;     // 19
    short       siDefenseRateArrow;     // 20
    
    BYTE        byDamageFire;           // 21
    BYTE        byDamageIce;            // 22
    BYTE        byDamageThuner;         // 23
    BYTE        byDamagePoison;         // 24

    BYTE        byStillHP;              // 25
    BYTE        byDamageMP;             // 26
    BYTE        byStillMP;              // 27
    BYTE        byReturnPhysicalDamage; // 28

    BYTE        bySoulBind;             // 29
    
    short       siBonusStr;             // 30
    short       siBonusSta;             // 31
    short       siBonusDex;             // 32
    short       siBonusInt;             // 33
    short       siBonusMagicAttak;      // 34
    short       siBonusHP;              // 35
    short       siBonusMSP;             // 36

    short       siRegistFire;           // 37
    short       siRegistIce;            // 38
    short       siRegistElec;           // 39
    short       siRegistMagic;          // 40
    short       siRegistPoison;         // 41
    short       siRegistCurse;          // 42
    
    DWORD       dwEffectID1;        // 43
    DWORD       dwEffectID2;        // 44

    short       siNeedLevel;            // 45
    short       siNeedRank;             // 46
    short       siNeedTitle;            // 47
    short       siNeedStrength;         // 48
    short       siNeedStamina;          // 49
    short       siNeedDexterity;        // 50
    short       siNeedInteli;           // 51
    short       siNeedMagicAttack;      // 52
} TABLE_ITEM_EXT;

const int MAX_NPC_SHOP_ITEM = 30;
typedef struct __TABLE_NPC_SHOP
{
	DWORD		dwNPCID;
	std::string	szName;
	DWORD		dwItems[MAX_NPC_SHOP_ITEM];
} TABLE_NPC_SHOP;

enum e_ItemType { ITEM_TYPE_PLUG = 1, ITEM_TYPE_PART, ITEM_TYPE_ICONONLY, ITEM_TYPE_GOLD = 9, ITEM_TYPE_SONGPYUN = 10, ITEM_TYPE_UNKNOWN = 0xffffffff };

enum e_ItemPosition {	ITEM_POS_DUAL = 0,	ITEM_POS_RIGHTHAND, ITEM_POS_LEFTHAND,	ITEM_POS_TWOHANDRIGHT,	ITEM_POS_TWOHANDLEFT,
						ITEM_POS_UPPER = 5, ITEM_POS_LOWER,		ITEM_POS_HEAD,		ITEM_POS_GLOVES,		ITEM_POS_SHOES,
						ITEM_POS_EAR = 10,	ITEM_POS_NECK,		ITEM_POS_FINGER,	ITEM_POS_SHOULDER,		ITEM_POS_BELT,
						ITEM_POS_INVENTORY = 15, ITEM_POS_GOLD = 16, ITEM_POS_SONGPYUN = 17,
						ITEM_POS_UNKNOWN = 0xffffffff };
					
enum e_ItemSlot {	ITEM_SLOT_EAR_RIGHT = 0,	ITEM_SLOT_HEAD	= 1,	ITEM_SLOT_EAR_LEFT	= 2,
					ITEM_SLOT_NECK = 3,			ITEM_SLOT_UPPER	= 4,	ITEM_SLOT_SHOULDER	= 5,
					ITEM_SLOT_HAND_RIGHT = 6,	ITEM_SLOT_BELT	= 7,	ITEM_SLOT_HAND_LEFT = 8,
					ITEM_SLOT_RING_RIGHT = 9,	ITEM_SLOT_LOWER = 10,	ITEM_SLOT_RING_LEFT = 11,
					ITEM_SLOT_GLOVES = 12,		ITEM_SLOT_SHOES = 13, 
					ITEM_SLOT_COUNT = 14, ITEM_SLOT_UNKNOWN = 0xffffffff };


typedef struct __TABLE_PLAYER_LOOKS // NPC, Mob Â©Â­Â¢â�� â–‘Â³ÃƒÃ Â©Â«â•ÃŠÂ¢â•‘ Ã€â•£â�€â�ŒÃÃ•...
{
	DWORD		dwID; // NPC â–‘Ãâ��Â» ID
	std::string	szName; // â�€â�‚Â©Â»â�¼â• â��â• Â©Âº
	std::string	szJointFN; // â–‘Â³â��Â² Ã£â�€â��Â¤ â��â• Â©Âº
	std::string	szAniFN; // â�Ã­â�¤Â¤Â©ÃŒâ��â• â•Ãƒ Ã£â�€â��Â¤ â��â• Â©Âº
	std::string	szPartFNs[7]; // â–‘Ã³ Character Part - â•—Â¾â�œâ•, ÃƒÂ¤â�œâ•, Â©Ã‹Â©Â«, Ã£â•š, â�¤â�˜Â©Â«, Â©Ã‹Â©Â«â�€Â½Ã‚Â¶, Â©â�´â�¼Ãµ

	int			iJointRH;			// â�â��Â©Ã‘â•Ä± â�‚Ã­ â–‘Â³â��Â²â•£Â°â•šÃº
	int			iJointLH;			// â�ÃŒâ•Ä± â�‚Ã­ â–‘Â³â��Â²â•£Â°â•šÃº
	int			iJointLH2;			// â�ÃŒâ•Ä± Ã£â•šÃ‚ÃŠ â–‘Â³â��Â²â•£Â°â•šÃº
	int			iJointCloak;		// Â©â�´â�¼Ãµ â•‘â�˜â��â•— â–‘Â³â��Â² â•£Â°â•šÃº
	
	int			iSndID_Move;
	int			iSndID_Attack0;
	int			iSndID_Attack1;
	int			iSndID_Struck0;
	int			iSndID_Struck1;
	int			iSndID_Dead0;
	int			iSndID_Dead1;
	int			iSndID_Breathe0;
	int			iSndID_Breathe1;
	int			iSndID_Reserved0;
	int			iSndID_Reserved1;
} TABLE_PLAYER;

typedef struct __TABLE_EXCHANGE_QUEST
{
	DWORD		dwID;					// â�€Â¨Â¢â•‘Ã£Â« â•£Â°â•šÃº..	0
	DWORD		dwNpcNum;				// npc â•£Â°â•šÃº..	1
	std::string szDesc;					// â•â�‚Â©Ã..		2
	int			iCondition0;			// â�´Ã‚â–‘Ãƒ 1..		3
	int			iCondition1;			// â�´Ã‚â–‘Ãƒ 2..		4
	int			iCondition2;			// â�´Ã‚â–‘Ãƒ 3..		5
	int			iCondition3;			// â�´Ã‚â–‘Ãƒ 4..		6
	int			iNeedGold;				// â�Ãµâ–’Â© â�‚Ã™Â¥Ã£..	7	
	BYTE		bNeedLevel;				// â�Ãµâ–’Â© Ã€â•£â•‘Âº	8
	BYTE		bNeedClass;				// â�Ãµâ–’Â© â�´Â¸Â¥Â¸	9
	BYTE		bNeedRank;				// â�Ãµâ–’Â© â��â–ˆâ��Âº	10	
	BYTE		bNeedExtra1;			// â�Ãµâ–’Â© â�Â®â•‘Ã°1	11	
	BYTE		bNeedExtra2;			// â�Ãµâ–’Â© â�Â®â•‘Ã°2	12
	BYTE		bCreatePercentage;		// â•—Â²â•â•‘ â•šÂ«Ã€Â³	13
	int			iArkTuarek;				// Â¥Ã£â�¼Â® â�¼Â§Â¥Ã£Ã€â•‘..	14	
	int			iTuarek;				// â�¼Â§Â¥Ã£Ã€â•‘..			15
	int			iRinkleTuarek;			// Â©Ãâ�¼Â¼ â�¼Â§Â¥Ã£Ã€â•‘..	16
	int			iBabarian;				// â•£â�˜â•£â�˜Â©Â«Â¥â•š..		17
	int			iMan;					// â�‚â–“â��â�Œ..			18
	int			iWoman;					// â�Â®â��â�Œ..			19
} TABLE_EXCHANGE_QUEST;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Â©Ã‚â•£Â²â�¼ÃŽâ��â• â•‘Ã...

typedef struct __TABLE_UPC_SKILL
{
	DWORD		dwID;				// SKILL â–‘Ãâ��Â» ID
	std::string	szEngName;			// Â¢â•‘â�¼â�‚ â�ÃÂ¥Â¯ â��â• Â©Âº
	std::string	szName;				// Â¢â•‘â�¼â�‚ ÃƒÃâ–’â–ˆ â��â• Â©Âº
	std::string	szDesc;				// Â¢â•‘â�¼â�‚ â��â• Â©Âº
	int			iSelfAnimID1;		// Â¢â�œâ��Â³â��â�Œ Ãâ�â��â–ˆÂ¢â�œâ��â–ˆ
	int			iSelfAnimID2;		// Â¢â�œâ��Â³â��â�Œ Ãâ�â��â–ˆâ�‚Ã­

	int			idwTargetAnimID;	// â�¼Â©â–‘â�˜ Ãâ�â��â–ˆ
	int			iSelfFX1;			// Â¢â�œâ��Â³â��â�Œ â•šâ�â–‘Â· 1
	int			iSelfPart1;			// Â¢â�œâ��Â³â��â�Œ â•šâ�â–‘Â· â��Â¤Â¥Â¯â�‚Â¬â�¤â�¬ â��Âºâ�€Ã­ 1
	int			iSelfFX2;			// Â¢â�œâ��Â³â��â�Œ â•šâ�â–‘Â· 2
	int			iSelfPart2;			// Â¢â�œâ��Â³â��â�Œ â•šâ�â–‘Â· â��Â¤Â¥Â¯â�‚Â¬â�¤â�¬ â��Âºâ�€Ã­ 2	
	int			iFlyingFX;			// â•‘Â±ÃƒÃ“ â•šâ�â–‘Â·
	int			iTargetFX;			// â�¼Â©â–‘â�˜ â•šâ�â–‘Â·

	int			iTargetPart;		// â•šâ�â–‘Â·â–‘Ã­ â��Â¤Â¥Â¯â�‚Â» â��Âºâ�€Ã­.
	int			iTarget;			// â�¼Â©â–‘â�˜	
	int			iNeedLevel;			// â�Ãµâ–’Â© Ã€â•£â•‘Âº
	int			iNeedSkill;			// â�Ãµâ–’Â© Â¢â•‘â�¼â�‚
	int			iExhaustMSP;		// â•ÃŠÂ©Â­ MSP

	int			iExhaustHP;			// â•ÃŠÂ©Â­ HP
	DWORD		dwNeedItem;			// â�Ãµâ–’Â© Â¥Ã£â��â• â�¼â–ˆ e_ItemClass â�¬Â³â�´Ã‚.. 10â��â•— â�‚Â¬â�¤Â½â–‘Â¬â��â• â�¤â�˜..
	DWORD		dwExhaustItem;
	int			iCastTime;			// â�€â�‚Â¢â•‘Ã£â�œ Â¢â�œâ–‘Ãº
	int			iReCastTime;		// â�¤â�˜Â¢â�œ â�€â�‚Â¢â•‘Ã£â�œÃƒÃŠÃ‚Âºâ–’Â¯â�´Ã· â–‘â•�Â©Â«â�¤â�¬ Â¢â�œâ–‘Ãº.
	
	int			iPercentSuccess;	// â•â•‘â–‘Â°Ã€Â³
	DWORD		dw1stTableType;		// â�œâ•£â•£Â°â�¬â–‘ â�¼Â©â��Ãˆ.
	DWORD		dw2ndTableType;		// Ãâ•¬â•£Â°â�¬â–‘ â�¼Â©â��Ãˆ.
	int			iValidDist;			// â��Â»â•šâ�â–‘â�¼Â©Â«
	
} TABLE_UPC_ATTACK_B;

typedef struct __TABLE_UPC_SKILL_TYPE_1
{
	DWORD		dwID;			// SKILL â–‘Ãâ��Â» ID
	int			iSuccessType;	// â•â•‘â–‘Â°â�¼Â©â��Ãˆ.
	int			iSuccessRatio;	// â•â•‘â–‘Â°Ã€Â³
	int			iPower;			// â–‘Â°â–‘Â¦Ã€â�¬
	int			iDelay;			// ÃÂ¶Ã€â•£â��â• 
	int			iComboType;		// â�€ÃŒâ•‘Â©â�¼Â©â��Ãˆ
	int			iNumCombo;		// â�€ÃŒâ•‘Â©â•šÂ©â•Ã·
	int			iComboDamage;	// â�€ÃŒâ•‘Â©â�¤Ã™â•£â• â�´Ã·			
	int			iValidAngle;	// â–‘Â°â–‘Â¦â•£Â¦â–‘Âµ
	int			iAct[3];
} TABLE_UPC_SKILL_TYPE_1;

typedef struct __TABLE_UPC_SKILL_TYPE_2
{
	DWORD		dwID;			// SKILL â–‘Ãâ��Â» ID
	int			iSuccessType;	// â•â•‘â–‘Â°â�¼Â©â��Ãˆ.
	int			iPower;			// â–‘Â°â–‘Â¦Ã€â�¬
	int			iAddDist;		// â–‘â�¼Â©Â«â�´Â§â–‘Ã­
	int			iNumArrow;		// â•šÂ¡â•—Ã½â�Ãµâ–’Â©â•Ã·
} TABLE_UPC_SKILL_TYPE_2;

typedef struct __TABLE_UPC_SKILL_TYPE_3
{
	DWORD		dwID;			// SKILL â–‘Ãâ��Â» ID
	int			iDDType;
	int			iStartDamage;
	int			iDuraDamage;
	int			iDurationTime;	// â�´Ã·â•Ã‹Â¢â�œâ–‘Ãº
	int			iAttribute;
} TABLE_UPC_SKILL_TYPE_3;

typedef struct __TABLE_UPC_SKILL_TYPE_4
{
	DWORD		dwID;			// â��Â¤Ã€â�œâ•£Â°â•šÃº
	int			iBuffType;		// â•£Ã·Ãƒâ�´â�¼Â©â��Ãˆ
	int			iDuration;
	int			iAttackSpeed;	// â–‘Â°â–‘Â¦â•Ã‹ÃÃ
	int			iMoveSpeed;		// â��â• Ãâ�â•Ã‹ÃÃ
	int			iAC;			// â•£ÂµÂ¥Â¯Ã€â�¬
	int			iAttack;		// â–‘Â°â–‘Â¦Ã€â�¬
	int			iMaxHP;			// MAXHP
	int			iStr;			// â•šÂ¹
	int			iSta;			// â�œâ•Ã€â�¬
	int			iDex;			// â•£â•¬â�œÂ©
	int			iInt;			// â�´Ã·â�¤â•�
	int			iMAP;			// Â©Ã‚Ã€â�¬
	int			iFireResist;	// â•‘ÃŠâ��Â·ÃƒÃŽ
	int			iColdResist;	// â�‚â�œâ–’Ã�â��Â·ÃƒÃŽ
	int			iLightningResist;// â��Â³â–’Ã�â��Â·ÃƒÃŽ
	int			iMagicResist;	// Â©Ã‚â•£Â²â��Â·ÃƒÃŽ
	int			iDeseaseResist;	// â��Â·â�´Ãâ��Â·ÃƒÃŽ
	int			iPoisonResist;	// ÃÃ‚â��Â·ÃƒÃŽ
} TABLE_UPC_SKILL_TYPE_4;

typedef struct __TABLE_UPC_SKILL_TYPE_5
{
	DWORD		dwID;			// â��Â¤Ã€â�œâ•£Â°â•šÃº
	DWORD		dwTarget;		// â�¼Â©â–‘â�˜
	int			iSuccessRatio;	// â•â•‘â–‘Â°Ã€Â³
	int			iValidDist;		// â��Â»â•šâ�â–‘â�¼Â©Â«
	int			iRadius;		// â•£Â¦â–‘Âµ
	float		fCastTime;		// â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	float		fRecastTime;	// Â©Â«â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	int			iDurationTime;	// â�´Ã·â•Ã‹Â¢â�œâ–‘Ãº
	DWORD		dwExhaustItem;	// â•ÃŠÂ©Â­Â¥Ã£â��â• â�¼â–ˆ
	DWORD		dwFX;			// Â©Ã‚â•£Â²â•šâ�â–‘Â·
} TABLE_UPC_SKILL_TYPE_5;

typedef struct __TABLE_UPC_SKILL_TYPE_6
{
	DWORD		dwID;			// â��Â¤Ã€â�œâ•£Â°â•šÃº
	DWORD		dwTarget;		// â�¼Â©â–‘â�˜
	int			iSuccessRatio;	// â•â•‘â–‘Â°Ã€Â³
	int			iValidDist;		// â��Â»â•šâ�â–‘â�¼Â©Â«
	int			iRadius;		// â•£Â¦â–‘Âµ
	float		fCastTime;		// â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	float		fRecastTime;	// Â©Â«â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	int			iDurationTime;	// â�´Ã·â•Ã‹Â¢â�œâ–‘Ãº
	DWORD		dwExhaustItem;	// â•ÃŠÂ©Â­Â¥Ã£â��â• â�¼â–ˆ
	DWORD		dwFX;			// Â©Ã‚â•£Â²â•šâ�â–‘Â·
	DWORD		dwTranform;		// â•‘Â»Â¢â�¼
} TABLE_UPC_SKILL_TYPE_6;

typedef struct __TABLE_UPC_SKILL_TYPE_7
{
	DWORD		dwID;			// â��Â¤Ã€â�œâ•£Â°â•šÃº
	DWORD		dwTarget;		// â�¼Â©â–‘â�˜
	DWORD		dwValidGroup;	// â��Â»â•šâ�â–’ÃŽÃ€Ã½
	int			iSuccessRatio;	// â•â•‘â–‘Â°Ã€Â³
	int			iValidDist;		// â��Â»â•šâ�â–‘â�¼Â©Â«
	int			iRadius;		// â•£Â¦â–‘Âµ
	float		fCastTime;		// â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	float		fRecastTime;	// Â©Â«â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	int			iDurationTime;	// â�´Ã·â•Ã‹Â¢â�œâ–‘Ãº
	DWORD		dwExhaustItem;	// â•ÃŠÂ©Â­Â¥Ã£â��â• â�¼â–ˆ
	DWORD		dwFX;			// Â©Ã‚â•£Â²â•šâ�â–‘Â·	
} TABLE_UPC_SKILL_TYPE_7;

typedef struct __TABLE_UPC_SKILL_TYPE_8
{
	DWORD		dwID;			// â��Â¤Ã€â�œâ•£Â°â•šÃº
	DWORD		dwTarget;		// â�¼Â©â–‘â�˜
	int			iRadius;		// â•£Â¦â–‘Âµ
	DWORD		dwWarpType;		// â•Â°â–‘Ãºâ��â• Ãâ�â�¼Â©â��Ã‹
	float		fRefillEXP;		// â–‘ÂµÃƒÃžâ�€Ã­â•šÂ©â•‘â•£
	DWORD		dwZone1;		// â�´Â©â�‚Ãâ•£Ã·1
	DWORD		dwZone2;		// â�´Â©â�‚Ãâ•£Ã·2
	DWORD		dwZone3;		// â�´Â©â�‚Ãâ•£Ã·3
	DWORD		dwZone4;		// â�´Â©â�‚Ãâ•£Ã·4
	DWORD		dwZone5;		// â�´Â©â�‚Ãâ•£Ã·5
} TABLE_UPC_SKILL_TYPE_8;

typedef struct __TABLE_UPC_SKILL_TYPE_9
{
	DWORD		dwID;			// â��Â¤Ã€â�œâ•£Â°â•šÃº
	DWORD		dwTarget;		// â�¼Â©â–‘â�˜
	int			iSuccessRatio;	// â•â•‘â–‘Â°Ã€Â³
	int			iValidDist;		// â��Â»â•šâ�â–‘â�¼Â©Â«
	int			iRadius;		// â•£Â¦â–‘Âµ
	float		fCastTime;		// â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	float		fRecastTime;	// Â©Â«â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	int			iDurationTime;	// â�´Ã·â•Ã‹Â¢â�œâ–‘Ãº
	DWORD		dwExhaustItem;	// â•ÃŠÂ©Â­Â¥Ã£â��â• â�¼â–ˆ
	DWORD		dwAttr;			// â•Ã‹â•â•‘
	int			iDamage;		// â�¤Ã™â•£â• â�´Ã·	
} TABLE_UPC_SKILL_TYPE_9;

typedef struct __TABLE_UPC_SKILL_TYPE_10
{
	DWORD		dwID;			// â��Â¤Ã€â�œâ•£Â°â•šÃº
	DWORD		dwTarget;		// â�¼Â©â–‘â�˜
	int			iSuccessRatio;	// â•â•‘â–‘Â°Ã€Â³
	int			iValidDist;		// â��Â»â•šâ�â–‘â�¼Â©Â«
	int			iRadius;		// â•£Â¦â–‘Âµ
	float		fCastTime;		// â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	float		fRecastTime;	// Â©Â«â�€â�‚Â¢â•‘Ã£â�œâ�¼Â©â��Ã‹
	DWORD		dwExhaustItem;	// â•ÃŠÂ©Â­Â¥Ã£â��â• â�¼â–ˆ
	DWORD		dwRemoveAttr;	// â�´Âªâ–‘â�¼â•Ã‹â•â•‘	
} TABLE_UPC_SKILL_TYPE_10;

//Â©Ã‚â•£Â²â�¼ÃŽâ��â• â•‘Ã...
///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct __TABLE_QUEST_MENU
{
	DWORD		dwID;		// â��Â¤Ã€â�œâ•£Â°â•šÃº
	std::string szMenu;		// â•â–’â�¼â�œ Â©ÃŒâ�¤â•‘
} TABLE_QUEST_MENU;

typedef struct __TABLE_QUEST_TALK
{
	DWORD		dwID;		// â��Â¤Ã€â�œâ•£Â°â•šÃº
	std::string szTalk;		// â�€Â¨Â¢â•‘Ã£Â« â�´Ã·â•£Â«
} TABLE_QUEST_TALK;

const int MAX_ITEM_SLOT_OPC = 8; // â�¬Â°â�Ã™ Â¥Ã£â��â• â�¼â–ˆ - â�¤â�˜Â©Ã‘ Ãƒâ�œÃ€â•£â��â• Â¥Â¯(NPC Ã£Â¸ÃƒÃˆ) 0 ~ 4 â•—Â¾â�œâ•,ÃƒÂ¤â�œâ•,ÃƒÂ´Â©Ãµ,Ã£â•š,â•£â–€ 5 Â©â�´â�¼Ãµ 6 â�â��Â©Ã‘â•Ä± 7 â�ÃŒâ•Ä±
const int MAX_ITEM_INVENTORY = 28;		// â•ÃŠâ��Â» Â¥Ã£â�¼â–ˆ MAX (â��â•¬â•‘Ã‘â�¼ÃµÂ©Â«â�œÃ³)
const int MAX_ITEM_TRADE = 24;	// â•—Â¾â��â•¬â–‘Â· â–‘â�¼Ã€Ã­..
const int MAX_ITEM_TRADE_PAGE = 12;
const int MAX_ITEM_WARE_PAGE = 8;
const int MAX_ITEM_PER_TRADE = 12;	// â–‘â�‚â��â•¬â–‘Â· â–‘â�¼Ã€Ã­..	
const int MAX_ITEM_BUNDLE_DROP_PIECE = 6;
const int MAX_ITEM_EX_RE_NPC = 4; // â–’â�‚â•šÂ», â•Ã·Â©Â«â�œÃ³ NPC â�Ãâ�Â¬..

const int MAX_SKILL_FROM_SERVER = 9;		// â•Â¡â•£Ã·â�Ã­â–‘Ãˆâ•Â¡ â•£ÃŒâ�¤â�¬ â�´Ã±â•‘Â© Â¢Â¢Ã€Ãˆ â–‘â•£â•Ã·..			

const int MAX_SKILL_KIND_OF = 5;			// Base Skill 1â–‘â�‚, â��Â³â•£Â« Â¢â•‘â�¼â�‚ 4â–‘â�‚..			
const int MAX_SKILL_IN_PAGE = 8;			// ÃƒÃ Ã£Ãµâ��â• â�´Ã· â�‚â•—â��Ãƒ Â¥Ã£â��â• â�€â–„ â–‘â•£â•Ã·..				
const int MAX_SKILL_PAGE_NUM = 3;			// Â¢â•‘â�¼â�‚ ÃƒÃâ–‘â�‚â–‘Ã­ â–‘Ã­â�´Ã·â�¤â�¬ â�œÃâ�¤Ã™ Ã£Ãµâ��â• â�´Ã·â•Ã·..		


const int MAX_SKILL_HOTKEY_PAGE = 8;		// Hot Keyâ��Ãƒ â�œÃ Ã£Ãµâ��â• â�´Ã· â•Ã·.. 				
const int MAX_SKILL_IN_HOTKEY = 8;			// Hot Keyâ��Ãƒ ÃƒÃ· Ã£Ãµâ��â• â�´Ã·â�‚â•—â��Ãƒ â–‘â•£â•Ã·..			
		
const int MAX_AVAILABLE_CHARACTER = 3;		// ÃƒÃ â•Â¡â•£Ã·â�¤Ã¾ â•â–’â�¼â�œÃƒÃŠâ•Ã· â��Ãâ�¤â�¬ â�œÃâ�¤Ã™ â�€â�‚Â©Â»â�¼â• â•Ã·..	

// Â¢â•¬â�Â¯ÃÃ.... By ecli666
const int ID_SOUND_ITEM_ETC_IN_INVENTORY	= 2000;
const int ID_SOUND_ITEM_IN_REPAIR			= 2001;
const int ID_SOUND_ITEM_WEAPON_IN_INVENTORY = 2002;
const int ID_SOUND_ITEM_ARMOR_IN_INVENTORY	= 2003;
const int ID_SOUND_GOLD_IN_INVENTORY		= 3000;
const int ID_SOUND_SKILL_THROW_ARROW		= 5500;
const int ID_SOUND_BGM_TOWN					= 20000;
const int ID_SOUND_BGM_KA_BATTLE			= 20002;
const int ID_SOUND_BGM_EL_BATTLE			= 20003;
const int ID_SOUND_CHR_SELECT_ROTATE		= 2501;

const float SOUND_RANGE_TO_SET = 10.0f;
const float SOUND_RANGE_TO_RELEASE = 20.0f;

const float STUN_TIME = 3.0f;

enum e_Behavior {	BEHAVIOR_NOTHING = 0,
					BEHAVIOR_EXIT,			// Ãƒâ�´Ã€â•¬â–’ÃŽÃ€Ã‘ â�‚Ã­â�‚â•—â–’Ã�..
					BEHAVIOR_RESTART_GAME,	// â–‘Ãˆâ��Ã‹ â�¤â�˜Â¢â�œ Â¢â�œâ��â–ˆÃƒÂ¤â–’Ã�(â�€â�‚Â©Â»â�¼â• â•â–’â�¼â�œ)
					BEHAVIOR_REGENERATION,	// â•‘â•¬â•šâ–‘
					BEHAVIOR_CANCEL,		// â�œÃ™â•ÃŠ.. ÃƒÃ·â��Ã¾â�¤â�¬ Â¥Ã£â��â• â�¼â–ˆ â–‘â�¼Ã€Ã­ Â¢â�¼â�œâ•—Â¢â�œ â��Â»â��Â·â–‘Ã­ â�œÃ™â•ÃŠÂ©Âª â�¤Â¡Ã€Ã‚â��â•—Ã‚Âº..

					BEHAVIOR_PARTY_PERMIT,	// â•—Â¾â�¤Ã™â•£Âµâ��â•  Ã£â�€Ã£â•â–‘Ã­â��Ãˆ â�Ãµâ�œâ•—Â¢â�œ ÃÃšÂ¥Â¯â–‘Ã­â�¤â�¬â–‘â•â��â•— ÃƒÃ’Ã‚Â¶.
					BEHAVIOR_PARTY_DISBAND, // Ã£â�€Ã£â• â�¼â•—â�¼Â­..
					BEHAVIOR_FORCE_PERMIT,	// â•—Â¾â�¤Ã™â•£Âµâ��â•  â•‘â•¬â�¤Ã™â–‘Ã­â��Ãˆ â�Ãµâ�œâ•—Â¢â�œ ÃÃšÂ¥Â¯â–‘Ã­â�¤â�¬â–‘â•â��â•— ÃƒÃ’Ã‚Â¶.
					BEHAVIOR_FORCE_DISBAND, // â•‘â•¬â�¤Ã™ â�¼â•—â�¼Â­..

					BEHAVIOR_REQUEST_BINDPOINT, // â•£â�˜â��â•¬ÃÂ¨ Ã£Â¸â��â•¬Ã£Â«Ã€â•¬ ... 

					BEHAVIOR_DELETE_CHR,

					BEHAVIOR_KNIGHTS_CREATE,
					BEHAVIOR_KNIGHTS_DESTROY, // â–’Ã�â•—Ã¾â�¤â–„ ÃƒÃâ�œâ•..
					BEHAVIOR_KNIGHTS_WITHDRAW, // â–’Ã�â•—Ã¾â�¤â–„ ÃƒÃâ�œâ•..

					BEHAVIOR_PERSONAL_TRADE_FMT_WAIT,	// â–‘â�‚â��â•¬â–‘â�¼Ã€Ã­.. â�‚â•—â–‘Ã­ Â¢â�¼â�œâ•—ÃƒÃ â–‘Âµâ�Ã½..
					BEHAVIOR_PERSONAL_TRADE_PERMIT,		// â–‘â�‚â��â•¬â–‘â�¼Ã€Ã­.. â�‚â•—â–‘Ã­ Â¢â�¼â�œâ•—â•£ÃŒâ��â•‘ â–‘Âµâ�Ã½..

					BEHAVIOR_MGAME_LOGIN,
					
					BEHAVIOR_CLAN_JOIN,
					BEHAVIOR_PARTY_BBS_REGISTER,		// Ã£â�€Ã£â• â–‘ÃˆÂ¢â�œÃ£Ãƒâ�Ã­ ÃÂ¯Ã€Â¤ 
					BEHAVIOR_PARTY_BBS_REGISTER_CANCEL, // Ã£â�€Ã£â• â–‘ÃˆÂ¢â�œÃ£Ãƒâ�Ã­ ÃÂ¯Ã€Â¤ ÃƒÃâ�´Âª

					BEHAVIOR_EXECUTE_OPTION,			// â–‘Ãˆâ��Ã‹ â�´Â¥Ã€ÃŸÃƒÂ¤â–‘Ã â�â•�â•Ãƒ Â¢ÃƒÃƒÃ“..
				
					BEHAVIOR_UNKNOWN = 0xffffffff
				};

enum e_SkillMagicTaget	{	SKILLMAGIC_TARGET_SELF = 1,				// â�‚Â¬ â��â�ŒÂ¢â�¼..
							SKILLMAGIC_TARGET_FRIEND_WITHME = 2,	// â�‚Â¬Â©Âª Ã£Â¸ÃƒÃˆÃƒÃ â�Ã½Â©Â«Ã£Ã(â–’â•£â–‘Ã­) â�´â–€ ÃƒÂ¤â�‚Â¬ ..
							SKILLMAGIC_TARGET_FRIEND_ONLY = 3,		// â�‚Â¬Â©Âª â•—Â½ â�Ã½Â©Â«Ã£Ã â�´â–€ ÃƒÂ¤â�‚Â¬ 
							SKILLMAGIC_TARGET_PARTY = 4,			// â�‚Â¬Â©Âª Ã£Â¸ÃƒÃˆÃƒÃ â�Ã½Â©Â«Ã£â�€Ã£â• â�´â–€ ÃƒÂ¤â�‚Â¬..
							SKILLMAGIC_TARGET_NPC_ONLY = 5,			// NPCâ�´â–€ ÃƒÂ¤â�‚Â¬.
							SKILLMAGIC_TARGET_PARTY_ALL = 6,		// â�‚Â¬Â©Âª â•šÃºÃƒÃˆÃƒÃ â�Ã½Â©Â«Ã£â�€Ã£â• Â©Â­Ãâ•¬..
							SKILLMAGIC_TARGET_ENEMY_ONLY = 7,		// â�Â´Ã£Ãâ��â•— â�´Âªâ�â–„ÃƒÃ Â©Â­ÃÃ¾ â��Â¹â�´â–€ ÃƒÂ¤â�‚Â¬(NPCÃ£Â¸ÃƒÃˆ)
							SKILLMAGIC_TARGET_ALL = 8,				// â–‘ÃŽâ•—Â¾â�Ã­ â�´Â©â��Ã¾ÃƒÂ¤â�¤â�¬ Â©Â­ÃÃ¾ â–‘â•â�´â–€ ÃƒÂ¤â�‚Â¬.
							
							SKILLMAGIC_TARGET_AREA_ENEMY = 10,		// â–’ÃŽ â�´Ã·â�Â¬â�Ã­ â��Ãâ�¤â�¬ â��Â¹ÃÃš...
							SKILLMAGIC_TARGET_AREA_FRIEND = 11,		// â–’ÃŽ â�´Ã·â�Â¬â�Ã­ â��Ãâ�¤â�¬ â�Â´Ã£Ã.
							SKILLMAGIC_TARGET_AREA_ALL = 12,		// â–’ÃŽ â�´Ã·â�Â¬â�Ã­ â��Ãâ�¤â�¬ Â©Â­Ãâ•¬ÃÃš.
							SKILLMAGIC_TARGET_AREA = 13,			// â�‚Â¬Â©Âª â�´â–€Â¢â•�â��Â©Ã€â•¬ ÃƒÃ â�´Ã·â�Â¬
							SKILLMAGIC_TARGET_DEAD_FRIEND_ONLY = 25,	//â�´ÃŽâ��â•‘ â•—Ã¾Ã‚Â¸â�´â–€ â�‚Â¬Â©Âª â•—Â½ â�Ã½Â©Â«Ã£Ã â�´â–€ ÃƒÂ¤â�‚Â¬.
							
							SKILLMAGIC_TARGET_UNKNOWN = 0xffffffff
						};


//define fx...
typedef struct __TABLE_FX	// FX Â©Â«â•ÃŠÂ¢â•‘ Ã€â•£â�€â�ŒÃÃ•...
{
	DWORD		dwID;		// â–‘Ãâ��Â» ID
	std::string	szFN;		// file name
	DWORD		dwSoundID;	// â•šâ�â–‘Â·â�Ã­ Â¥â–“â�¤â�¬ â•—Ã¾â�Â¯ÃÃ• Â¥Ã£ÃÂ­.
} TABLE_FX;

const int	MAX_COMBO = 3;

const int	FXID_BLOOD = 10002;
const int	FXID_LEVELUP_KARUS			= 10012;
const int	FXID_LEVELUP_ELMORAD		= 10018;
const int	FXID_REGEN_ELMORAD			= 10019;
const int	FXID_REGEN_KARUS			= 10020;
const int	FXID_SWORD_FIRE_MAIN		= 10021;
const int	FXID_SWORD_FIRE_TAIL		= 10022;
const int	FXID_SWORD_FIRE_TARGET		= 10031;
const int	FXID_SWORD_ICE_MAIN			= 10023;
const int	FXID_SWORD_ICE_TAIL			= 10024;
const int	FXID_SWORD_ICE_TARGET		= 10032;
const int	FXID_SWORD_LIGHTNING_MAIN	= 10025;
const int	FXID_SWORD_LIGHTNING_TAIL	= 10026;
const int	FXID_SWORD_LIGHTNING_TARGET = 10033;
const int	FXID_SWORD_POISON_MAIN		= 10027;
const int	FXID_SWORD_POISON_TAIL		= 10028;
const int	FXID_SWORD_POISON_TARGET	= 10034;
//const int	FXID_GROUND_TARGET = 10035;
const int	FXID_REGION_TARGET_EL_ROGUE		= 10035;
const int	FXID_REGION_TARGET_EL_WIZARD	= 10036;
const int	FXID_REGION_TARGET_EL_PRIEST	= 10037;
const int	FXID_REGION_TARGET_KA_ROGUE		= 10038;
const int	FXID_REGION_TARGET_KA_WIZARD	= 10039;
const int	FXID_REGION_TARGET_KA_PRIEST	= 10040;
const int	FXID_CLAN_RANK_1				= 10041;
const int	FXID_REGION_POISON				= 10100;

//define skillmagic_type4_bufftype
enum e_SkillMagicType4	{	BUFFTYPE_MAXHP = 1,				//MaxHPâ•‘Â»â•šÂ¡..
							BUFFTYPE_AC = 2,				//ACâ•‘Â»â•šÂ¡..
							BUFFTYPE_RESIZE = 3,			//â�€â�‚Â©Â»â�¼â• â�¼Â®â–’Ã� â�´Ã‚â�´Ã±..
							BUFFTYPE_ATTACK = 4,			//â–‘Â°â–‘Â¦Ã€â�¬..
							BUFFTYPE_ATTACKSPEED = 5,		//â–‘Â°â–‘Â¦Â¢â•‘ÃƒÃƒÃÃ•..
							BUFFTYPE_SPEED = 6,				//â��â• Ãâ�â•Ã‹ÃÃ..
							BUFFTYPE_ABILITY = 7,			//â�¤â�˜â•Â©â–‘Ã­â�´Ã· â�¤â•�Ã€â�¬(str, sta, cha, dex int)
							BUFFTYPE_RESIST = 8,			//â�¤â�˜â•Â©â–‘Ã­â�´Ã· â��Â·ÃƒÃŽÃ€â�¬..
							BUFFTYPE_HITRATE_AVOIDRATE = 9,	//hitrate n avoidrate
							BUFFTYPE_TRANS = 10,			//â•‘Â»Â¢â�¼, â�¼Â§Â©Ã..
							BUFFTYPE_SLEEP = 11,			//â��ÃŸâ��Ã¾â�Ã½â–’Ã�..
							BUFFTYPE_EYE = 12				//Â¢â�œÃ€â�¬â–‘Â³Ã€â�œ...							
};

enum e_SkillMagicType3	{	DDTYPE_TYPE3_DUR_OUR = 100,
							DDTYPE_TYPE3_DUR_ENEMY = 200
};



enum e_SkillMagicFailMsg {	SKILLMAGIC_FAIL_CASTING	= -100,		//â�€â�‚Â¢â•‘Ã£â�œ Â¢ÃƒÃ£Ã°..
							SKILLMAGIC_FAIL_KILLFLYING = -101,	//Flyingâ–‘â�¤â�œâ• â�´ÃŽâ�Â®..
							SKILLMAGIC_FAIL_ENDCOMBO = -102,	//combo Ãâ�â��â–ˆ â�‚Ã­â�‚Ãâ�¤â�˜.
							SKILLMAGIC_FAIL_NOEFFECT = -103,	//Effecting â•£Â½â•—Ã›...
							SKILLMAGIC_FAIL_ATTACKZERO = -104,	//â�¼Â©â–‘Â¦â�€Ã­ 0...(mpâ�¤â�¬ â•ÃŠÂ©Â­ÃƒÂ¤â–‘Ã â�¼Â©â–‘Â¦Â¢ÃƒÃ£Ã°..)

							SKILLMAGIC_FAIL_UNKNOWN = 0xffffffff
};

enum e_ObjectType	{	OBJECT_TYPE_BINDPOINT,
						OBJECT_TYPE_DOOR_LEFTRIGHT,
						OBJECT_TYPE_DOOR_TOPDOWN,
						OBJECT_TYPE_LEVER_TOPDOWN,
						OBJECT_TYPE_FLAG,
						OBJECT_TYPE_WARP_POINT,
						OBJECT_TYPE_UNKNOWN = 0xffffffff
					};

//definitions related clan....
const int	CLAN_LEVEL_LIMIT	= 20;
const int	CLAN_COST			= 500000;
const DWORD KNIGHTS_FONT_COLOR	= 0xffff0000; // â–’Ã�â•—Ã¾â�¤â–„(â�¼Â¼Ã€Ãº)â��â• Â©Âº Ã£Â¨Ã£Â« â�€â�œÃ€Â»..

enum e_Cursor		{	CURSOR_ATTACK,
						CURSOR_EL_NORMAL,
						CURSOR_EL_CLICK,
						CURSOR_KA_NORMAL,
						CURSOR_KA_CLICK,
						CURSOR_PRE_REPAIR,
						CURSOR_NOW_REPAIR,
						CURSOR_COUNT,
						CURSOR_UNKNOWN = 0xffffffff
					};

#endif // end of #define __GAME_DEF_H_

