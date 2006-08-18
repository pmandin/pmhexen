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

typedef struct {
	short lengthMin;
	short lengthSec;
	int sectorStart;
	int sectorLength;
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
				cd_FirstTrack = sdlcd->track[i].id;
			cd_LastTrack = sdlcd->track[i].id;
			cd_TrackCount++;
		}
	}

	return 0;

#if 0

	// Set track variables
	InputIOCTL(AUDIODISKINFO, cd_DiskInfoSeg);
	cd_FirstTrack = cd_DiskInfo->lowTrack;
	cd_LastTrack = cd_DiskInfo->highTrack;
	if(cd_FirstTrack == 0 && cd_FirstTrack == cd_LastTrack)
	{
		cd_Error = CDERR_NOAUDIOTRACKS;
		return -1;
	}
	cd_TrackCount = cd_LastTrack-cd_FirstTrack+1;
	cd_LeadOutMin = cd_DiskInfo->startLeadOut>>16 & 0xFF;
	cd_LeadOutSec = cd_DiskInfo->startLeadOut>>8 & 0xFF;
	cd_LeadOutRed = cd_DiskInfo->startLeadOut;
	cd_LeadOutSector = RedToSectors(cd_DiskInfo->startLeadOut);

	// Create Red Book start, sector start, and sector length
	// for all tracks
	sect = cd_LeadOutSector;
	for(i = cd_LastTrack; i >= cd_FirstTrack; i--)
	{
		cd_TrackInfo->track = i;
		InputIOCTL(AUDIOTRACKINFO, cd_TrackInfoSeg);
		if(i < MAX_AUDIO_TRACKS)
		{
			cd_AudioTracks[i].redStart = cd_TrackInfo->start;
			cd_AudioTracks[i].sectorStart =
				RedToSectors(cd_TrackInfo->start);
			cd_AudioTracks[i].sectorLength =
				sect-RedToSectors(cd_TrackInfo->start);
		}
		sect = RedToSectors(cd_TrackInfo->start);
	}

	// Create track lengths in minutes and seconds
	if(cd_LastTrack >= MAX_AUDIO_TRACKS)
	{
		maxTrack = MAX_AUDIO_TRACKS-1;
	}
	else
	{
		maxTrack = cd_LastTrack;
	}
	cd_TrackInfo->track = cd_FirstTrack;
	InputIOCTL(AUDIOTRACKINFO, cd_TrackInfoSeg);
	startMin1 = (cd_TrackInfo->start >> 16);
	startSec1 = (cd_TrackInfo->start >> 8);
	for(i = cd_FirstTrack; i <= maxTrack; i++)
	{
		cd_TrackInfo->track = i+1;
		if(i < cd_LastTrack)
		{
			InputIOCTL(AUDIOTRACKINFO, cd_TrackInfoSeg);
			startMin2 = (cd_TrackInfo->start >> 16);
			startSec2 = (cd_TrackInfo->start >> 8);
		}
		else
		{
			startMin2 = cd_LeadOutRed>>16;
			startSec2 = cd_LeadOutRed>>8;
		}
		lengthSec = startSec2 - startSec1;
		lengthMin = startMin2 - startMin1;
		if(lengthSec < 0)
		{
			lengthSec += 60;
			lengthMin--;
		}
		cd_AudioTracks[i].lengthMin = lengthMin;
		cd_AudioTracks[i].lengthSec = lengthSec;
		startMin1 = startMin2;
		startSec1 = startSec2;
	}

	// Clip high tracks
	cd_LastTrack = maxTrack;

	OkInit = 1;
	return 0;
#endif
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
	return -1;

#if 0
	int start;
	int len;

	if(track < cd_FirstTrack || track > cd_LastTrack)
	{
		cd_Error = CDERR_BADTRACK;
		return(-1);
	}
	I_CDMusStop();
	start = cd_AudioTracks[track].redStart;
	len = cd_AudioTracks[track].sectorLength;
	cd_PlayReq->addressMode = RED_MODE;
	cd_PlayReq->startSector = start;
	cd_PlayReq->numberToRead = len;
	memset(&RegBlock, 0, sizeof(RegBlock));
	RegBlock.eax = CDROM_SENDDEVICEREQ;
	RegBlock.ecx = cd_CurDrive;
	RegBlock.ebx = 0;
	RegBlock.es = cd_PlayReqSeg;
	DPMI_SimRealInt(MULTIPLEX_INT, &RegBlock);
	if(cd_PlayReq->status&0x8000)
	{
		cd_Error = CDERR_DEVREQBASE+(cd_PlayReq->status)&0x00ff;
		return(-1);
	}
	return(0);
#endif
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
	return cd_LastTrack;
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
	return -1;
#if 0
	if(track < cd_FirstTrack || track > cd_LastTrack)
	{
		cd_Error = CDERR_BADTRACK;
		return -1;
	}
	return cd_AudioTracks[track].lengthMin*60
		+cd_AudioTracks[track].lengthSec;
#endif
}
