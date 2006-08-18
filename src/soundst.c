/*
===============================================================================

		MUSIC & SFX API

===============================================================================
*/

#include <SDL.h>

#include "h2def.h"
#include "sounds.h"
#include "soundst.h"
#include "r_local.h"
#include "p_local.h"

#include "i_cdmus.h"
#include "i_audio.h"
#include "i_sound.h"
#include "i_music.h"

#define PRIORITY_MAX_ADJUST 10
#define DIST_ADJUST (MAX_SND_DIST/PRIORITY_MAX_ADJUST)
#define DEFAULT_ARCHIVEPATH     "archive/"

extern sfxinfo_t S_sfx[];
extern musicinfo_t S_music[];
extern void **lumpcache;

static channel_t Channel[MAX_CHANNELS];
static int RegisteredSong; //the current registered song.
static int NextCleanup;
static boolean MusicPaused;
static int Mus_Song = -1;
static int Mus_LumpNum;
static void *Mus_SndPtr;
static byte *SoundCurve;

static boolean UseSndScript;
static char ArchivePath[128];

extern int snd_MusicDevice;
extern int snd_SfxDevice;
extern int snd_MaxVolume;
extern int snd_MusicVolume;
extern int snd_Channels;

extern int startepisode;
extern int startmap;

// int AmbChan;

//==========================================================================
//
// S_Start
//
//==========================================================================

void S_Start(void)
{
	S_StopAllSound();
	S_StartSong(gamemap, true);
}

//==========================================================================
//
// S_StartSong
//
//==========================================================================

void S_StartSong(int song, boolean loop)
{
	char *songLump;
	int track;

	if(i_CDMusic)
	{ // Play a CD track, instead
		if(i_CDTrack)
		{ // Default to the player-chosen track
			track = i_CDTrack;
		}
		else
		{
			track = P_GetMapCDTrack(gamemap);
		}
		if(track == i_CDCurrentTrack && i_CDMusicLength > 0)
		{
			return;
		}
		if(!I_CDMusPlay(track))
		{
			if(loop)
			{
				i_CDMusicLength = 35*I_CDMusTrackLength(track);
				oldTic = gametic;
			}
			else
			{
				i_CDMusicLength = -1;
			}
			i_CDCurrentTrack = track;
		}
	}
	else
	{
		if(song == Mus_Song)
		{ // don't replay an old song
			return;
		}
		if(RegisteredSong)
		{
			I_StopSong(RegisteredSong);
			I_UnRegisterSong(RegisteredSong);
			if(UseSndScript)
			{
				Z_Free(Mus_SndPtr);
			}
			else
			{
				Z_ChangeTag(lumpcache[Mus_LumpNum], PU_CACHE);
			}
			RegisteredSong = 0;
		}
		songLump = P_GetMapSongLump(song);
		if(!songLump)
		{
			return;
		}
		if(UseSndScript)
		{
			char name[128];
			sprintf(name, "%s%s.lmp", ArchivePath, songLump);
			M_ReadFile(name, (byte **)&Mus_SndPtr);
		}
		else
		{
			Mus_LumpNum = W_GetNumForName(songLump);
			Mus_SndPtr = W_CacheLumpNum(Mus_LumpNum, PU_MUSIC);
		}
		RegisteredSong = I_RegisterSong(Mus_SndPtr, W_LumpLength(Mus_LumpNum));
		I_PlaySong(RegisteredSong, loop); // 'true' denotes endless looping.
		Mus_Song = song;
	}
}

//==========================================================================
//
// S_StartSongName
//
//==========================================================================

void S_StartSongName(char *songLump, boolean loop)
{
	int cdTrack;

	if(!songLump)
	{
		return;
	}
	if(i_CDMusic)
	{
		cdTrack = 0;

		if(!strcmp(songLump, "hexen"))
		{
			cdTrack = P_GetCDTitleTrack();
		}
		else if(!strcmp(songLump, "hub"))
		{
			cdTrack = P_GetCDIntermissionTrack();
		}
		else if(!strcmp(songLump, "hall"))
		{
			cdTrack = P_GetCDEnd1Track();
		}
		else if(!strcmp(songLump, "orb"))
		{
			cdTrack = P_GetCDEnd2Track();
		}
		else if(!strcmp(songLump, "chess") && !i_CDTrack)
		{
			cdTrack = P_GetCDEnd3Track();
		}
/*	Uncomment this, if Kevin writes a specific song for startup
		else if(!strcmp(songLump, "start"))
		{
			cdTrack = P_GetCDStartTrack();
		}
*/
		if(!cdTrack || (cdTrack == i_CDCurrentTrack && i_CDMusicLength > 0))
		{
			return;
		}
		if(!I_CDMusPlay(cdTrack))
		{
			if(loop)
			{
				i_CDMusicLength = 35*I_CDMusTrackLength(cdTrack);
				oldTic = gametic;
			}
			else
			{
				i_CDMusicLength = -1;
			}
			i_CDCurrentTrack = cdTrack;
			i_CDTrack = false;
		}
	}
	else
	{
		if(RegisteredSong)
		{
			I_StopSong(RegisteredSong);
			I_UnRegisterSong(RegisteredSong);
			if(UseSndScript)
			{
				Z_Free(Mus_SndPtr);
			}
			else
			{
				Z_ChangeTag(lumpcache[Mus_LumpNum], PU_CACHE);
			}
			RegisteredSong = 0;
		}
		if(UseSndScript)
		{
			char name[128];
			sprintf(name, "%s%s.lmp", ArchivePath, songLump);
			M_ReadFile(name, (byte **)&Mus_SndPtr);
		}
		else
		{
			Mus_LumpNum = W_GetNumForName(songLump);
			Mus_SndPtr = W_CacheLumpNum(Mus_LumpNum, PU_MUSIC);
		}
		RegisteredSong = I_RegisterSong(Mus_SndPtr, W_LumpLength(Mus_LumpNum));
		I_PlaySong(RegisteredSong, loop); // 'true' denotes endless looping.
		Mus_Song = -1;
	}
}

//==========================================================================
//
// S_GetSoundID
//
//==========================================================================

int S_GetSoundID(char *name)
{
	int i;

	if (name==NULL) {
		fprintf(stderr,"S_GetSoundID(): NULL sound name\n");
		return 0;
	}

	for(i = 0; i < NUMSFX; i++)
	{
		if(!strcmp(S_sfx[i].tagName, name))
		{
			return i;
		}
	}
	return 0;
}

//==========================================================================
//
// S_StartSound
//
//==========================================================================

void S_StartSound(mobj_t *origin, int sound_id)
{
	S_StartSoundAtVolume(origin, sound_id, 127);
}

//==========================================================================
//
// S_StartSoundAtVolume
//
//==========================================================================

void S_StartSoundAtVolume(mobj_t *origin, int sound_id, int volume)
{
	int dist, vol;
	int i;
	int priority;
	int sep;
	int angle;
	int absx;
	int absy;

	static int sndcount = 0;
	int chan;

	if (!sysaudio.enabled)
		return;

	if(sound_id == 0 || snd_MaxVolume == 0)
		return;
	if(origin == NULL)
	{
		origin = players[displayplayer].mo;
	}
	if(volume == 0)
	{
		return;
	}

	// calculate the distance before other stuff so that we can throw out
	// sounds that are beyond the hearing range.
	absx = absy = 0;
	if (origin) {
		absx = abs(origin->x-players[displayplayer].mo->x);
		absy = abs(origin->y-players[displayplayer].mo->y);
	}
	dist = absx+absy-(absx > absy ? absy>>1 : absx>>1);
	dist >>= FRACBITS;
	if(dist >= MAX_SND_DIST)
	{
	  return; // sound is beyond the hearing range...
	}
	if(dist < 0)
	{
		dist = 0;
	}
	priority = S_sfx[sound_id].priority;
	priority *= (PRIORITY_MAX_ADJUST-(dist/DIST_ADJUST));
	if(!S_StopSoundID(sound_id, priority))
	{
		return; // other sounds have greater priority
	}
	for(i=0; i<snd_Channels; i++)
	{
		if (origin) {
			if(origin->player)
			{
				i = snd_Channels;
				break; // let the player have more than one sound.
			}
			if(origin == Channel[i].mo)
			{ // only allow other mobjs one sound
				S_StopSound(Channel[i].mo);
				break;
			}
		}
	}
	if(i >= snd_Channels)
	{
		for(i = 0; i < snd_Channels; i++)
		{
			if(Channel[i].mo == NULL)
			{
				break;
			}
		}
		if(i >= snd_Channels)
		{
			// look for a lower priority sound to replace.
			sndcount++;
			if(sndcount >= snd_Channels)
			{
				sndcount = 0;
			}
			for(chan = 0; chan < snd_Channels; chan++)
			{
				i = (sndcount+chan)%snd_Channels;
				if(priority >= Channel[i].priority)
				{
					chan = -1; //denote that sound should be replaced.
					break;
				}
			}
			if(chan != -1)
			{
				return; //no free channels.
			}
			else //replace the lower priority sound.
			{
				if(Channel[i].handle)
				{
					if(I_SoundIsPlaying(Channel[i].handle))
					{
						I_StopSound(Channel[i].handle);
					}
					if(S_sfx[Channel[i].sound_id].usefulness > 0)
					{
						S_sfx[Channel[i].sound_id].usefulness--;
					}
				}
			}
		}
	}
	if(S_sfx[sound_id].lumpnum == 0)
	{
		S_sfx[sound_id].lumpnum = I_GetSfxLumpNum(&S_sfx[sound_id]);
	}
	if(S_sfx[sound_id].snd_ptr == NULL)
	{
		if(UseSndScript)
		{
			char name[128];
			sprintf(name, "%s%s.lmp", ArchivePath, S_sfx[sound_id].lumpname);
			M_ReadFile(name, (byte **)&S_sfx[sound_id].snd_ptr);
		}
		else
		{
			int length;

//			S_sfx[sound_id].snd_ptr = W_CacheLumpNum(S_sfx[sound_id].lumpnum,
//				PU_SOUND);

			S_sfx[sound_id].snd_ptr = I_LoadSfx(S_sfx[sound_id].lumpname, &length);
			S_sfx[sound_id].length = length;
		}
	}

	vol = (SoundCurve[dist]*(snd_MaxVolume*8)*volume)>>14;
	if(origin == players[displayplayer].mo)
	{
		sep = 128;
//              vol = (volume*(snd_MaxVolume+1)*8)>>7;
	}
	else
	{
		if (Channel[i].mo) {
			angle = R_PointToAngle2(players[displayplayer].mo->x,
				players[displayplayer].mo->y, Channel[i].mo->x, Channel[i].mo->y);
		} else {
			angle = 0;
		}
		angle = (angle-viewangle)>>24;
		sep = angle*2-128;
		if(sep < 64)
			sep = -sep;
		if(sep > 192)
			sep = 512-sep;
//              vol = SoundCurve[dist];
	}

	if(S_sfx[sound_id].changePitch)
	{
		Channel[i].pitch = (byte)(127+(M_Random()&7)-(M_Random()&7));
	}
	else
	{
		Channel[i].pitch = 127;
	}
	Channel[i].handle = I_StartSound(sound_id, /*S_sfx[sound_id].snd_ptr,*/ vol,
		sep, Channel[i].pitch, 0);
	Channel[i].mo = origin;
	Channel[i].sound_id = sound_id;
	Channel[i].priority = priority;
	Channel[i].volume = volume;
	if(S_sfx[sound_id].usefulness < 0)
	{
		S_sfx[sound_id].usefulness = 0;
	}
	S_sfx[sound_id].usefulness++;
}

//==========================================================================
//
// S_StopSoundID
//
//==========================================================================

boolean S_StopSoundID(int sound_id, int priority)
{
	int i;
	int lp; //least priority
	int found;

	if(S_sfx[sound_id].numchannels == -1)
	{
		return(true);
	}
	lp = -1; //denote the argument sound_id
	found = 0;
	for(i=0; i<snd_Channels; i++)
	{
		if(Channel[i].sound_id == sound_id && Channel[i].mo)
		{
			found++; //found one.  Now, should we replace it??
			if(priority >= Channel[i].priority)
			{ // if we're gonna kill one, then this'll be it
				lp = i;
				priority = Channel[i].priority;
			}
		}
	}
	if(found < S_sfx[sound_id].numchannels)
	{
		return(true);
	}
	else if(lp == -1)
	{
		return(false); // don't replace any sounds
	}
	if(Channel[lp].handle)
	{
		if(I_SoundIsPlaying(Channel[lp].handle))
		{
			I_StopSound(Channel[lp].handle);
		}
		if(S_sfx[Channel[lp].sound_id].usefulness > 0)
		{
			S_sfx[Channel[lp].sound_id].usefulness--;
		}
		Channel[lp].mo = NULL;
	}
	return(true);
}

//==========================================================================
//
// S_StopSound
//
//==========================================================================

void S_StopSound(mobj_t *origin)
{
	int i;

	for(i=0;i<snd_Channels;i++)
	{
		if(Channel[i].mo == origin)
		{
			I_StopSound(Channel[i].handle);
			if(S_sfx[Channel[i].sound_id].usefulness > 0)
			{
				S_sfx[Channel[i].sound_id].usefulness--;
			}
			Channel[i].handle = 0;
			Channel[i].mo = NULL;
		}
	}
}

//==========================================================================
//
// S_StopAllSound
//
//==========================================================================

void S_StopAllSound(void)
{
	int i;

	//stop all sounds
	for(i=0; i < snd_Channels; i++)
	{
		if(Channel[i].handle)
		{
			S_StopSound(Channel[i].mo);
		}
	}
	memset(Channel, 0, 8*sizeof(channel_t));
}

//==========================================================================
//
// S_SoundLink
//
//==========================================================================

void S_SoundLink(mobj_t *oldactor, mobj_t *newactor)
{
	int i;

	for(i=0;i<snd_Channels;i++)
	{
		if(Channel[i].mo == oldactor)
			Channel[i].mo = newactor;
	}
}

//==========================================================================
//
// S_PauseSound
//
//==========================================================================

void S_PauseSound(void)
{
	if(i_CDMusic)
	{
		I_CDMusStop();
	}
	else
	{
		I_PauseSong(RegisteredSong);
	}
}

//==========================================================================
//
// S_ResumeSound
//
//==========================================================================

void S_ResumeSound(void)
{
	if(i_CDMusic)
	{
		I_CDMusResume();
	}
	else
	{
		I_ResumeSong(RegisteredSong);
	}
}

//==========================================================================
//
// S_UpdateSounds
//
//==========================================================================

void S_UpdateSounds(mobj_t *listener)
{
	int i, dist, vol;
	int angle;
	int sep;
	int priority;
	int absx;
	int absy;

	if (!sysaudio.enabled)
		return;

	if(i_CDMusic)
	{
		I_UpdateCDMusic();
	}
	if(snd_MaxVolume == 0)
	{
		return;
	}

	// Update any Sequences
	SN_UpdateActiveSequences();

	if(NextCleanup < gametic)
	{
		if(UseSndScript)
		{
			for(i = 0; i < NUMSFX; i++)
			{
				if(S_sfx[i].usefulness == 0 && S_sfx[i].snd_ptr)
				{
					S_sfx[i].usefulness = -1;
				}
			}
		}
		else
		{
			for(i = 0; i < NUMSFX; i++)
			{
				if(S_sfx[i].usefulness == 0 && S_sfx[i].snd_ptr)
				{
					if(lumpcache[S_sfx[i].lumpnum])
					{
						if(((memblock_t *)((byte*)
							(lumpcache[S_sfx[i].lumpnum])-
							sizeof(memblock_t)))->id == 0x1d4a11)
						{ // taken directly from the Z_ChangeTag macro
							Z_ChangeTag2(lumpcache[S_sfx[i].lumpnum],
								PU_CACHE);
						}
					}
					S_sfx[i].usefulness = -1;
					S_sfx[i].snd_ptr = NULL;
				}
			}
		}
		NextCleanup = gametic+35*30; // every 30 seconds
	}
	for(i=0;i<snd_Channels;i++)
	{
		if(!Channel[i].handle || S_sfx[Channel[i].sound_id].usefulness == -1)
		{
			continue;
		}
		if(!I_SoundIsPlaying(Channel[i].handle))
		{
			if(S_sfx[Channel[i].sound_id].usefulness > 0)
			{
				S_sfx[Channel[i].sound_id].usefulness--;
			}
			Channel[i].handle = 0;
			Channel[i].mo = NULL;
			Channel[i].sound_id = 0;
		}
		if(Channel[i].mo == NULL || Channel[i].sound_id == 0
			|| Channel[i].mo == listener)
		{
			continue;
		}
		else
		{
			absx = abs(Channel[i].mo->x-listener->x);
			absy = abs(Channel[i].mo->y-listener->y);
			dist = absx+absy-(absx > absy ? absy>>1 : absx>>1);
			dist >>= FRACBITS;

			if(dist >= MAX_SND_DIST)
			{
				S_StopSound(Channel[i].mo);
				continue;
			}
			if(dist < 0)
			{
				dist = 0;
			}
			//vol = SoundCurve[dist];
			vol = (SoundCurve[dist]*(snd_MaxVolume*8)*Channel[i].volume)>>14;
			if(Channel[i].mo == listener)
			{
				sep = 128;
			}
			else
			{
				angle = R_PointToAngle2(listener->x, listener->y,
								Channel[i].mo->x, Channel[i].mo->y);
				angle = (angle-viewangle)>>24;
				sep = angle*2-128;
				if(sep < 64)
					sep = -sep;
				if(sep > 192)
					sep = 512-sep;
			}
			I_UpdateSoundParams(Channel[i].handle, vol, sep,
				Channel[i].pitch);
			priority = S_sfx[Channel[i].sound_id].priority;
			priority *= PRIORITY_MAX_ADJUST-(dist/DIST_ADJUST);
			Channel[i].priority = priority;
		}
	}
}

//==========================================================================
//
// S_Init
//
//==========================================================================

void S_Init(void)
{
	SoundCurve = W_CacheLumpName("SNDCURVE", PU_STATIC);
//      SoundCurve = Z_Malloc(MAX_SND_DIST, PU_STATIC, NULL);
	I_StartupSound();
	if(snd_Channels > 8)
	{
		snd_Channels = 8;
	}
	I_SetChannels(snd_Channels);
	I_SetMusicVolume(snd_MusicVolume);

	// Attempt to setup CD music
	if(sysaudio.music == MUSIC_CD)
	{
	   	ST_Message("    Attempting to initialize CD Music: ");
		if(!cdrom)
		{
			i_CDMusic = (I_CDMusInit() != -1);
		}
		else
		{ // The user is trying to use the cdrom for both game and music
			i_CDMusic = false;
		}
	   	if(i_CDMusic)
	   	{
	   		ST_Message("initialized.\n");
	   	}
	   	else
	   	{
	   		ST_Message("failed.\n");
	   	}
	}
}

//==========================================================================
//
// S_GetChannelInfo
//
//==========================================================================

void S_GetChannelInfo(SoundInfo_t *s)
{
	int i;
	ChanInfo_t *c;

	s->channelCount = snd_Channels;
	s->musicVolume = snd_MusicVolume;
	s->soundVolume = snd_MaxVolume;
	for(i = 0; i < snd_Channels; i++)
	{
		c = &s->chan[i];
		c->id = Channel[i].sound_id;
		c->priority = Channel[i].priority;
		c->name = S_sfx[c->id].lumpname;
		c->mo = Channel[i].mo;
		c->distance = P_AproxDistance(c->mo->x-viewx, c->mo->y-viewy)
			>>FRACBITS;
	}
}

//==========================================================================
//
// S_GetSoundPlayingInfo
//
//==========================================================================

boolean S_GetSoundPlayingInfo(mobj_t *mobj, int sound_id)
{
	int i;

	for(i = 0; i < snd_Channels; i++)
	{
		if(Channel[i].sound_id == sound_id && Channel[i].mo == mobj)
		{
			if(I_SoundIsPlaying(Channel[i].handle))
			{
				return true;
			}
		}
	}
	return false;
}

//==========================================================================
//
// S_SetMusicVolume
//
//==========================================================================

void S_SetMusicVolume(void)
{
	if(i_CDMusic)
	{
		I_CDMusSetVolume(snd_MusicVolume*16); // 0-255
	}
	else
	{
		I_SetMusicVolume(snd_MusicVolume);
	}
	if(snd_MusicVolume == 0)
	{
		if(!i_CDMusic)
		{
			I_PauseSong(RegisteredSong);
		}
		MusicPaused = true;
	}
	else if(MusicPaused)
	{
		if(!i_CDMusic)
		{
			I_ResumeSong(RegisteredSong);
		}
		MusicPaused = false;
	}
}

//==========================================================================
//
// S_ShutDown
//
//==========================================================================

void S_ShutDown(void)
{
	I_StopSong(RegisteredSong);
	I_UnRegisterSong(RegisteredSong);
	I_ShutdownSound();

	if(i_CDMusic)
	{
		I_CDMusStop();
	}
}

//==========================================================================
//
// S_InitScript
//
//==========================================================================

void S_InitScript(void)
{
	int p;
	int i;

	strcpy(ArchivePath, DEFAULT_ARCHIVEPATH);
	if(!(p = M_CheckParm("-devsnd")))
	{
		UseSndScript = false;
		SC_OpenLump("sndinfo");
	}
	else
	{
		UseSndScript = true;
		SC_OpenFile(myargv[p+1]);
	}
	while(SC_GetString())
	{
		if(*sc_String == '$')
		{
			if(!strcmp(sc_String, "$ARCHIVEPATH"))
			{
				SC_MustGetString();
				strcpy(ArchivePath, sc_String);
			}
			else if(!strcmp(sc_String, "$MAP"))
			{
				SC_MustGetNumber();
				SC_MustGetString();
				if(sc_Number)
				{
					P_PutMapSongLump(sc_Number, sc_String);
				}
			}
			continue;
		}
		else
		{
			for(i = 0; i < NUMSFX; i++)
			{
				if(!strcmp(S_sfx[i].tagName, sc_String))
				{
					SC_MustGetString();
					if(*sc_String != '?')
					{
						strcpy(S_sfx[i].lumpname, sc_String);
					}
					else
					{
						strcpy(S_sfx[i].lumpname, "default");
					}
					break;
				}
			}
			if(i == NUMSFX)
			{
				SC_MustGetString();
			}
		}
	}
	SC_Close();

	for(i = 0; i < NUMSFX; i++)
	{
		if(!strcmp(S_sfx[i].lumpname, ""))
		{
			strcpy(S_sfx[i].lumpname, "default");
		}
	}
}

//==========================================================================
//
// I_UpdateCDMusic
//
// Updates playing time for current track, and restarts the track, if
// needed
//
//==========================================================================

void I_UpdateCDMusic(void)
{
	extern boolean MenuActive;

	if(MusicPaused || i_CDMusicLength < 0
	|| (paused && !MenuActive))
	{ // Non-looping song/song paused
		return;
	}
	i_CDMusicLength -= gametic-oldTic;
	oldTic = gametic;
	if(i_CDMusicLength <= 0)
	{
		S_StartSong(gamemap, true);
	}
}
