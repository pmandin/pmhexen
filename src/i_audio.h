// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	System interface, sound.
//
//-----------------------------------------------------------------------------

#ifndef __I_AUDIO__
#define __I_AUDIO__

// Init at program start...
void I_InitAudio();

// ... update sound buffer and audio device at runtime...
void I_UpdateAudio(void *unused, Uint8 *stream, int len);

// ... shut down and relase at program termination.
void I_ShutdownAudio(void);

typedef struct {
	boolean enabled;
	boolean convert;
	int	music;
	SDL_AudioSpec	desired;
	SDL_AudioSpec	obtained;
	SDL_AudioCVT	audioCvt;
} sysaudio_t;

extern sysaudio_t sysaudio;

enum {
	MUSIC_MIDI=0,
	MUSIC_CD
};

#endif
