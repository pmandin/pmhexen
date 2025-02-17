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
//	System specific network interface stuff.
//
//-----------------------------------------------------------------------------

#ifndef __I_NET__
#define __I_NET__

// Called by D_DoomMain.

void I_NetCmd (void);

extern void (*I_InitNetwork)(void);	/* Init network */
extern void (*I_ShutdownNetwork)(void);	/* Shutdown network */

void	(*netget) (void);
void	(*netsend) (void);

void I_InitNetwork_unix(void);
void I_ShutdownNetwork_unix(void);

void I_InitNetwork_sting(void);
void I_ShutdownNetwork_sting(void);

typedef struct {
	int layer;
} sysnetwork_t;

enum {
	NETWORK_UNIX=0,
	NETWORK_STING
};

extern sysnetwork_t sysnetwork;

#endif
