// **************************************************************************
// **
// ** sounds.c : Heretic 2 : Raven Software, Corp.
// **
// **************************************************************************

#include "h2def.h"
#include "sounds.h"

// Music info

/*
musicinfo_t S_music[] =
{
	{ "MUS_E1M1", 0 }, // 1-1
	{ "MUS_E1M2", 0 },
	{ "MUS_E1M3", 0 },
	{ "MUS_E1M4", 0 },
	{ "MUS_E1M5", 0 },
	{ "MUS_E1M6", 0 },
	{ "MUS_E1M7", 0 },
	{ "MUS_E1M8", 0 },
	{ "MUS_E1M9", 0 },
	{ "MUS_E2M1", 0 }, // 2-1
	{ "MUS_E2M2", 0 },
	{ "MUS_E2M3", 0 },
	{ "MUS_E2M4", 0 },
	{ "MUS_E1M4", 0 },
	{ "MUS_E2M6", 0 },
	{ "MUS_E2M7", 0 },
	{ "MUS_E2M8", 0 },
	{ "MUS_E2M9", 0 },
	{ "MUS_E1M1", 0 }, // 3-1
	{ "MUS_E3M2", 0 },
	{ "MUS_E3M3", 0 },
	{ "MUS_E1M6", 0 },
	{ "MUS_E1M3", 0 },
	{ "MUS_E1M2", 0 },
	{ "MUS_E1M5", 0 },
	{ "MUS_E1M9", 0 },
	{ "MUS_E2M6", 0 },
	{ "MUS_E1M6", 0 }, // 4-1
	{ "MUS_TITL", 0 },
	{ "MUS_INTR", 0 },
	{ "MUS_CPTD", 0 }
};
*/

// Sound info

#define SFX_NP(name, pitch)	\
	{ name, "", 256, -1, NULL, 0, 2, pitch, 0 }

sfxinfo_t S_sfx[] =
{
	// tagname, lumpname, priority, usefulness, snd_ptr, lumpnum, numchannels,
	//		pitchshift, length
	{ "", "", 0, -1, NULL, 0, 0, 0, 0 },
	SFX_NP("PlayerFighterNormalDeath", 1),
	SFX_NP("PlayerFighterCrazyDeath", 1),
	SFX_NP("PlayerFighterExtreme1Death", 1),
	SFX_NP("PlayerFighterExtreme2Death", 1),
	SFX_NP("PlayerFighterExtreme3Death", 1),
	SFX_NP("PlayerFighterBurnDeath", 1),
	SFX_NP("PlayerClericNormalDeath", 1),
	SFX_NP("PlayerClericCrazyDeath", 1),
	SFX_NP("PlayerClericExtreme1Death", 1),
	SFX_NP("PlayerClericExtreme2Death", 1),
	SFX_NP("PlayerClericExtreme3Death", 1),
	SFX_NP("PlayerClericBurnDeath", 1),
	SFX_NP("PlayerMageNormalDeath", 0),
	SFX_NP("PlayerMageCrazyDeath", 0),
	SFX_NP("PlayerMageExtreme1Death", 0),
	SFX_NP("PlayerMageExtreme2Death", 0),
	SFX_NP("PlayerMageExtreme3Death", 0),
	SFX_NP("PlayerMageBurnDeath", 0),
	SFX_NP("PlayerFighterPain", 1),
	SFX_NP("PlayerClericPain", 1),
	SFX_NP("PlayerMagePain", 0),
	SFX_NP("PlayerFighterGrunt", 1),
	SFX_NP("PlayerClericGrunt", 1),
	SFX_NP("PlayerMageGrunt", 0),
	{"PlayerLand", "", 32, -1, NULL, 0, 2, 1, 0 },
	SFX_NP("PlayerPoisonCough", 1),
	SFX_NP("PlayerFighterFallingScream", 1),
	SFX_NP("PlayerClericFallingScream", 1),
	SFX_NP("PlayerMageFallingScream", 0),
	SFX_NP("PlayerFallingSplat", 1),
	{ "PlayerFighterFailedUse", "", 256, -1, NULL, 0, 1, 1, 0 },
	{ "PlayerClericFailedUse", "", 256, -1, NULL, 0, 1, 1, 0 },
	{ "PlayerMageFailedUse", "", 256, -1, NULL, 0, 1, 0, 0 },
	{ "PlatformStart", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "PlatformStartMetal", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "PlatformStop", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "StoneMove", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MetalMove", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "DoorOpen", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "DoorLocked", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "DoorOpenMetal", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "DoorCloseMetal", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "DoorCloseLight", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "DoorCloseHeavy", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "DoorCreak", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "PickupWeapon", "", 36, -1, NULL, 0, 2, 0, 0 },
	{ "PickupArtifact", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "PickupKey", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "PickupItem", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "PickupPiece", "", 36, -1, NULL, 0, 2, 0, 0 },
	{ "WeaponBuild", "", 36, -1, NULL, 0, 2, 0, 0 },
	{ "UseArtifact", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "BlastRadius", "", 36, -1, NULL, 0, 2, 1, 0 },
	SFX_NP("Teleport", 1),
	{ "ThunderCrash", "", 30, -1, NULL, 0, 2, 1, 0 },
	{ "FighterPunchMiss", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterPunchHitThing", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterPunchHitWall", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterGrunt", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterAxeHitThing", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterHammerMiss", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterHammerHitThing", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterHammerHitWall", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterHammerContinuous", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "FighterHammerExplode", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterSwordFire", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "FighterSwordExplode", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "ClericCStaffFire", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "ClericCStaffExplode", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "ClericCStaffHitThing", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "ClericFlameFire", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "ClericFlameExplode", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "ClericFlameCircle", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "MageWandFire", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "MageLightningFire", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "MageLightningZap", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MageLightningContinuous", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MageLightningReady", "", 30, -1, NULL, 0, 2, 1, 0 },
	{ "MageShardsFire","", 80, -1, NULL, 0, 2, 1, 0 },
	{ "MageShardsExplode","", 36, -1, NULL, 0, 2, 1, 0 },
	{ "MageStaffFire","", 80, -1, NULL, 0, 2, 1, 0 },
	{ "MageStaffExplode","", 40, -1, NULL, 0, 2, 1, 0 },
	{ "Switch1", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "Switch2", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentSight", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentMeleeHit", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentBirth", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentFXContinuous", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SerpentFXHit", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "PotteryExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "Drip", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "CentaurSight", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "CentaurActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "CentaurPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "CentaurAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "CentaurDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "CentaurLeaderAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "CentaurMissileExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "Wind", "", 1, -1, NULL, 0, 2, 1, 0 },
	{ "BishopSight", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "BishopActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "BishopPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "BishopAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "BishopDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "BishopMissileExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "BishopBlur", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "DemonSight", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "DemonActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "DemonPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "DemonAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "DemonMissileFire", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "DemonMissileExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "DemonDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "WraithSight", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "WraithActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "WraithPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "WraithAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "WraithMissileFire", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "WraithMissileExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "WraithDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "PigActive1", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "PigActive2", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "PigPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "PigAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "PigDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "MaulatorSight", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MaulatorActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MaulatorPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MaulatorHamSwing", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MaulatorHamHit", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MaulatorMissileHit", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "MaulatorDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "FreezeDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "FreezeShatter", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "EttinSight", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "EttinActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "EttinPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "EttinAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "EttinDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "FireDemonSpawn", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "FireDemonActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "FireDemonPain", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "FireDemonAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "FireDemonMissileHit", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "FireDemonDeath", "", 40, -1, NULL, 0, 2, 1, 0 },
	{ "IceGuySight", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "IceGuyActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "IceGuyAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "IceGuyMissileExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	SFX_NP("SorcererSight", 1),
	SFX_NP("SorcererActive", 1),
	SFX_NP("SorcererPain", 1),
	SFX_NP("SorcererSpellCast", 1),
	{ "SorcererBallWoosh", "", 256, -1, NULL, 0, 4, 1, 0 },
	SFX_NP("SorcererDeathScream", 1),
	{ "SorcererBishopSpawn", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "SorcererBallPop", "", 80, -1, NULL, 0, 2, 1, 0 },
	{ "SorcererBallBounce", "", 80, -1, NULL, 0, 3, 1, 0 },
	{ "SorcererBallExplode", "", 80, -1, NULL, 0, 3, 1, 0 },
	{ "SorcererBigBallExplode", "", 80, -1, NULL, 0, 3, 1, 0 },
	SFX_NP("SorcererHeadScream", 1),
	{ "DragonSight", "", 64, -1, NULL, 0, 2, 1, 0 },
	{ "DragonActive", "", 64, -1, NULL, 0, 2, 1, 0 },
	{ "DragonWingflap", "", 64, -1, NULL, 0, 2, 1, 0 },
	{ "DragonAttack", "", 64, -1, NULL, 0, 2, 1, 0 },
	{ "DragonPain", "", 64, -1, NULL, 0, 2, 1, 0 },
	{ "DragonDeath", "", 64, -1, NULL, 0, 2, 1, 0 },
	{ "DragonFireballExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	SFX_NP("KoraxSight", 1),
	SFX_NP("KoraxActive", 1),
	SFX_NP("KoraxPain", 1),
	SFX_NP("KoraxAttack", 1),
	SFX_NP("KoraxCommand", 1),
	SFX_NP("KoraxDeath", 1),
	{ "KoraxStep", "", 128, -1, NULL, 0, 2, 1, 0 },
	{ "ThrustSpikeRaise", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "ThrustSpikeLower", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "GlassShatter", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "FlechetteBounce", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "FlechetteExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "LavaMove", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "WaterMove", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "IceStartMove", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "EarthStartMove", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "WaterSplash", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "LavaSizzle", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SludgeGloop", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "HolySymbolFire", "", 64, -1, NULL, 0, 2, 1, 0 },
	{ "SpiritActive", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SpiritAttack", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SpiritDie", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "ValveTurn", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "RopePull", "", 36, -1, NULL, 0, 2, 1, 0 },
	{ "FlyBuzz", "", 20, -1, NULL, 0, 2, 1, 0 },
	{ "Ignite", "", 32, -1, NULL, 0, 2, 1, 0 },
	SFX_NP("PuzzleSuccess", 1),
	SFX_NP("PuzzleFailFighter", 1),
	SFX_NP("PuzzleFailCleric", 1),
	SFX_NP("PuzzleFailMage", 1),
	{ "Earthquake", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "BellRing", "", 32, -1, NULL, 0, 2, 0, 0 },
	{ "TreeBreak", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "TreeExplode", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SuitofArmorBreak", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "PoisonShroomPain", "", 20, -1, NULL, 0, 2, 1, 0 },
	{ "PoisonShroomDeath", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "Ambient1", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient2", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient3", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient4", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient5", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient6", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient7", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient8", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient9", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient10", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient11", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient12", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient13", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient14", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "Ambient15", "", 1, -1, NULL, 0, 1, 1, 0 },
	{ "StartupTick", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "SwitchOtherLevel", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "Respawn", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceGreetings", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceReady", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceBlood", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceGame", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceBoard", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceWorship", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceMaybe", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceStrong", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "KoraxVoiceFace", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "BatScream", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "Chat", "", 512, -1, NULL, 0, 2, 1, 0 },
	{ "MenuMove", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "ClockTick", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "Fireball", "", 32, -1, NULL, 0, 2, 1, 0 },
	{ "PuppyBeat", "", 30, -1, NULL, 0, 2, 1, 0 },
	{ "MysticIncant", "", 32, -1, NULL, 0, 4, 1, 0 }
};
