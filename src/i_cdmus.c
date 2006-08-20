// **************************************************************************
// **
// ** i_cdmus.c
// **
// **************************************************************************

// HEADER FILES ------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <SDL.h>

#include "h2def.h"
#include "i_sound.h"

// MACROS ------------------------------------------------------------------

#define MAX_AUDIO_TRACKS 100

// TYPES -------------------------------------------------------------------

#if 0
typedef struct {
	short lengthMin;
	short lengthSec;
	int sectorStart;
	int sectorLength;
} AudioTrack_t;
#endif

typedef struct {
	int audio_track;
	int length;
} AudioTrack_t;

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

int cd_Error;

boolean i_CDMusic;
int i_CDTrack;
int i_CDCurrentTrack;
int i_CDMusicLength;
int oldTic;

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static SDL_CD	*sdlcd;

static int cd_FirstTrack;
static int cd_LastTrack;
static int cd_TrackCount;

static AudioTrack_t cd_AudioTracks[MAX_AUDIO_TRACKS];

// CODE --------------------------------------------------------------------

//==========================================================================
//
// I_CDMusInit
//
// Initializes the CD audio system.  Must be called before using any
// other I_CDMus functions.
//
// Returns: 0 (ok) or -1 (error, in cd_Error).
//
//==========================================================================

int I_CDMusInit(void)
{
	int i;

	memset(cd_AudioTracks, 0, sizeof(cd_AudioTracks));

	if (SDL_CDNumDrives()<1)
		return -1;

	sdlcd = SDL_CDOpen(0);
	if (sdlcd==NULL)
		return -1;

	SDL_CDStop(sdlcd);

	SDL_CDStatus(sdlcd);

	cd_FirstTrack = cd_LastTrack = -1;
	cd_TrackCount = 0;
	for (i=0;i<sdlcd->numtracks;i++) {
		if (sdlcd->track[i].type == SDL_AUDIO_TRACK) {
			if (cd_FirstTrack==-1)
				cd_FirstTrack = cd_TrackCount;
			cd_LastTrack = cd_TrackCount;
			cd_AudioTracks[cd_TrackCount].audio_track = i /*sdlcd->track[i].id*/;
			cd_AudioTracks[cd_TrackCount].length = sdlcd->track[i].length * CD_FPS;
			cd_TrackCount++;
		}
	}

	return 0;
}

//==========================================================================
//
// I_CDMusPlay
//
// Play an audio CD track.
//
// Returns: 0 (ok) or -1 (error, in cd_Error).
//
//==========================================================================

int I_CDMusPlay(int track)
{
	if(track < cd_FirstTrack || track > cd_LastTrack)
		return -1;

	if (sdlcd==NULL)
		return -1;

	return SDL_CDPlayTracks(sdlcd, track, 0, 1, 0);
}

//==========================================================================
//
// I_CDMusStop
//
// Stops the playing of an audio CD.
//
// Returns: 0 (ok) or -1 (error, in cd_Error).
//
//==========================================================================

int I_CDMusStop(void)
{
	return SDL_CDStop(sdlcd);
}

//==========================================================================
//
// I_CDMusResume
//
// Resumes the playing of an audio CD.
//
// Returns: 0 (ok) or -1 (error, in cd_Error).
//
//==========================================================================

int I_CDMusResume(void)
{
	return SDL_CDResume(sdlcd);
}

//==========================================================================
//
// I_CDMusSetVolume
//
// Sets the CD audio volume (0 - 255).
//
// Returns: 0 (ok) or -1 (error, in cd_Error).
//
//==========================================================================

int I_CDMusSetVolume(int volume)
{
	return -1;
}

//==========================================================================
//
// I_CDMusFirstTrack
//
// Returns: the number of the first track.
//
//==========================================================================

int I_CDMusFirstTrack(void)
{
	return cd_FirstTrack;
}

//==========================================================================
//
// I_CDMusLastTrack
//
// Returns: the number of the last track.
//
//==========================================================================

int I_CDMusLastTrack(void)
{
	return cd_LastTrack-1;
}

//==========================================================================
//
// I_CDMusTrackLength
//
// Returns: Length of the given track in seconds, or -1 (error, in
// cd_Error).
//
//==========================================================================

int I_CDMusTrackLength(int track)
{
	if(track < cd_FirstTrack || track > cd_LastTrack)
		return -1;

	return cd_AudioTracks[track].length;
}
