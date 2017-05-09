// I_IBM.C

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdarg.h>
#ifdef __MINT__
#include <mint/cookie.h>
#include <mint/osbind.h>
#endif

#include <SDL.h>

#include "h2def.h"
#include "r_local.h"
#include "sounds.h"
#include "soundst.h"
#include "i_audio.h"
#include "i_video.h"
#include "i_system.h"
#include "i_net.h"
#include "i_cdmus.h"

// Public Data

int DisplayTicker = 0;
int UpdateState;

sysgame_t	sysgame={DEFAULT_HEAP_SIZE,NULL,false};

// Code

static void I_InitFpu(void);

int main(int argc, char **argv)
{
	myargc = argc;
	myargv = argv;

	printf(PACKAGE_STRING " compiled on " __DATE__ "\n");

	H2_Main();
	return 0;
}

void
I_Tactile
( int	on,
  int	off,
  int	total )
{
  // UNUSED.
  on = off = total = 0;
}

//--------------------------------------------------------------------------
//
// FUNC I_GetTime
//
// Returns time in 1/35th second tics.
//
//--------------------------------------------------------------------------

int I_GetTime (void)
{
    int			newtics;
    static int		basetime=0;

    if (!basetime)
	basetime = SDL_GetTicks();
    newtics = ((SDL_GetTicks()-basetime)*TICRATE)/1000;
    return newtics;
}

//--------------------------------------------------------------------------
//
// PROC I_WaitVBL
//
//--------------------------------------------------------------------------

void I_WaitVBL(int vbls)
{
	SDL_Delay((vbls*1000)/(TICRATE<<1));
}

/*
==============
=
= I_Update
=
==============
*/

void I_Update (void)
{
}

/*
===============
=
= I_Init
=
= hook interrupts and set graphics mode
=
===============
*/

void I_Init (void)
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0) {
		fprintf(stderr, "Can not initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	if (sysaudio.enabled) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO)<0) {
			sysaudio.enabled = false;
		}
	}

#ifdef __MINT__
	if (sysnetwork.layer==NETWORK_STING) {
		I_InitNetwork = I_InitNetwork_sting;
		I_ShutdownNetwork = I_ShutdownNetwork_sting;
	}
#endif

	I_InitFpu();
	I_InitAudio();
	if (i_CDMusic) {
		if (I_CDMusInit() == -1) {
			fprintf(stderr, "Can not use CD for music replay\n");
			i_CDMusic = false;
		}
	}
	S_Init();
	S_Start();
}

static void I_InitFpu(void)
{
#if defined(__MINT__) && !defined(__mcoldfire__)
	long cpu_cookie, fpu_cookie;

	if (Getcookie(C__CPU, &cpu_cookie) != C_FOUND) {
		return;
	}
	if (Getcookie(C__FPU, &fpu_cookie) != C_FOUND) {
		fpu_cookie = 0;
	}

	cpu_cookie &= 0xffff;
	if ((cpu_cookie<20) || (cpu_cookie>60)) {
		return;
	}

	FixedMul = FixedMul020;
	FixedDiv2 = FixedDiv2020;

	if (cpu_cookie==60) {
		if ((fpu_cookie>>16) & 0xfffe) == 16) {
			__asm__ __volatile__ (
					".chip	68060\n"
				"	fmove%.l	fpcr,d0\n"
				"	andl	#~0x30,d0\n"
				"	orb		#0x20,d0\n"
				"	fmove%.l	d0,fpcr\n"
#if defined(__mc68020__)
				"	.chip	68020"
#else
				"	.chip	68000"
#endif
				: /* no return value */
				: /* no input */
				: /* clobbered registers */
					"d0", "cc"
			);
		}

		FixedMul = FixedMul060;
		FixedDiv2 = FixedDiv2060;
		sysgame.cpu060 = true;
	}
#endif
}

/*
===============
=
= I_Shutdown
=
= return to default system state
=
===============
*/

void I_Shutdown (void)
{
	S_ShutDown ();
	if (i_CDMusic) {
		I_CDMusShutdown();
	}
	I_ShutdownNetwork();
	I_ShutdownAudio();
	I_ShutdownGraphics();
	if (sysgame.zone) {
#ifdef __MINT__
		Mfree(sysgame.zone);
#else
		free(sysgame.zone);
#endif
		sysgame.zone=NULL;
	}
	SDL_Quit();
}


/*
================
=
= I_Error
=
================
*/

void I_Error (char *error, ...)
{
	static int firsttime = 1;
	if (!firsttime) {	/* Avoid infinite error loop */
		SDL_Quit();
		exit(1);
	}

	va_list	argptr;

	// Message first.
	va_start (argptr,error);
	fprintf (stderr, "Error: ");
	vfprintf (stderr,error,argptr);
	fprintf (stderr, "\n");
	va_end (argptr);

	fflush( stderr );

	firsttime=0;

	D_QuitNetGame ();
	I_Shutdown ();
#ifdef __MINT__
	Cconws("- Press a key to exit -");
	while (Cconis()==0) {
	}
#endif
	exit (1);
}

//--------------------------------------------------------------------------
//
// I_Quit
//
// Shuts down net game, saves defaults, prints the exit text message,
// goes to text mode, and exits.
//
//--------------------------------------------------------------------------

void I_Quit(void)
{
	D_QuitNetGame();
	M_SaveDefaults();
	I_Shutdown();
	exit(0);
}

/*
===============
=
= I_ZoneBase
=
===============
*/

byte *I_ZoneBase (int *size)
{
#ifdef __MINT__
	int mxalloc_present;
	long maximal_heap_size=0;

	mxalloc_present = ((Sversion()&0xFF)>=0x01)|(Sversion()>=0x1900);
	if (mxalloc_present) {
		maximal_heap_size = Mxalloc(-1,MX_PREFTTRAM);
	} else {
		maximal_heap_size = Malloc(-1);
	}
	maximal_heap_size >>= 10;
	maximal_heap_size -= 256;	/* Keep 128 KB */
	if (sysgame.kb_used>maximal_heap_size)
		sysgame.kb_used=maximal_heap_size;
#endif

    *size = sysgame.kb_used<<10;

	printf(" %d Kbytes allocated for zone\n",sysgame.kb_used);

#ifdef __MINT__
	if (mxalloc_present) {
		sysgame.zone = (void *)Mxalloc(*size, MX_PREFTTRAM);
		maximal_heap_size = Mxalloc(-1,MX_STRAM);
	} else {
		sysgame.zone = (void *)Malloc(*size);
		maximal_heap_size = Malloc(-1);
	}
	printf(" (%d Kbytes left for audio/video subsystem)\n", maximal_heap_size>>10);
#else
	sysgame.zone = malloc (*size);
#endif

    return (byte *) sysgame.zone;
}
