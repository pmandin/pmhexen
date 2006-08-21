// **************************************************************************
// **
// ** st_start.c : Heretic 2 : Raven Software, Corp.
// **
// **************************************************************************

// HEADER FILES ------------------------------------------------------------

#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>				// Needed for next as well as dos

#include "h2def.h"
#include "st_start.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

// MACROS ------------------------------------------------------------------
#define ST_MAX_NOTCHES		32
#define ST_NOTCH_WIDTH		16
#define ST_NOTCH_HEIGHT		23
#define ST_PROGRESS_X		64			// Start of notches x screen pos.
#define ST_PROGRESS_Y		441			// Start of notches y screen pos.

#define ST_NETPROGRESS_X		288
#define ST_NETPROGRESS_Y		32
#define ST_NETNOTCH_WIDTH		8
#define ST_NETNOTCH_HEIGHT		16
#define ST_MAX_NETNOTCHES		8

// TYPES -------------------------------------------------------------------

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------
extern void SetVideoModeHR(void);
extern void ClearScreenHR(void);
extern void SlamHR(char *buffer);
extern void SlamBlockHR(int x, int y, int w, int h, char *src);
extern void InitPaletteHR(void);
extern void SetPaletteHR(byte *palette);
extern void GetPaletteHR(byte *palette);
extern void FadeToPaletteHR(byte *palette);
extern void FadeToBlackHR(void);
extern void BlackPaletteHR(void);
extern void I_StartupReadKeys(void);

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------
char *ST_LoadScreen(void);
void ST_UpdateNotches(int notchPosition);
void ST_UpdateNetNotches(int notchPosition);

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

// PRIVATE DATA DEFINITIONS ------------------------------------------------
char *bitmap = NULL;

char notchTable[]=
{
	// plane 0
	0x00, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40,
	0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x03, 0xC0,
	0x0F, 0x90, 0x1B, 0x68, 0x3D, 0xBC, 0x3F, 0xFC, 0x20, 0x08, 0x20, 0x08,
	0x2F, 0xD8, 0x37, 0xD8, 0x37, 0xF8, 0x1F, 0xF8, 0x1C, 0x50,

	// plane 1
	0x00, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x02, 0x40,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0xA0,
	0x30, 0x6C, 0x24, 0x94, 0x42, 0x4A, 0x60, 0x0E, 0x60, 0x06, 0x7F, 0xF6,
	0x7F, 0xF6, 0x7F, 0xF6, 0x5E, 0xF6, 0x38, 0x16, 0x23, 0xAC,

	// plane 2
	0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x02, 0x40, 0x02, 0x40,
	0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x03, 0xE0,
	0x30, 0x6C, 0x24, 0x94, 0x52, 0x6A, 0x7F, 0xFE, 0x60, 0x0E, 0x60, 0x0E,
	0x6F, 0xD6, 0x77, 0xD6, 0x56, 0xF6, 0x38, 0x36, 0x23, 0xAC,

	// plane 3
	0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
	0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0x80, 0x02, 0x40,
	0x0F, 0x90, 0x1B, 0x68, 0x3D, 0xB4, 0x1F, 0xF0, 0x1F, 0xF8, 0x1F, 0xF8,
	0x10, 0x28, 0x08, 0x28, 0x29, 0x08, 0x07, 0xE8, 0x1C, 0x50
};


// Red Network Progress notches
char netnotchTable[]=
{
	// plane 0
	0x80, 0x50, 0xD0, 0xf0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xD0, 0xF0, 0xC0,
	0x70, 0x50, 0x80, 0x60,

	// plane 1
	0x60, 0xE0, 0xE0, 0xA0, 0xA0, 0xA0, 0xE0, 0xA0, 0xA0, 0xA0, 0xE0, 0xA0,
	0xA0, 0xE0, 0x60, 0x00,

	// plane 2
	0x80, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00,
	0x10, 0x10, 0x80, 0x60,

	// plane 3
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

// CODE --------------------------------------------------------------------



//--------------------------------------------------------------------------
//
// Startup Screen Functions
//
//--------------------------------------------------------------------------


//==========================================================================
//
// ST_Init - Do the startup screen
//
//==========================================================================

void ST_Init(void)
{
}


void ST_Done(void)
{
}


//==========================================================================
//
// ST_UpdateNotches
//
//==========================================================================

void ST_UpdateNotches(int notchPosition)
{
}


//==========================================================================
//
// ST_UpdateNetNotches - indicates network progress
//
//==========================================================================

void ST_UpdateNetNotches(int notchPosition)
{
}


//==========================================================================
//
// ST_Progress - increments progress indicator
//
//==========================================================================

void ST_Progress(void)
{
	printf(".");
}


//==========================================================================
//
// ST_NetProgress - indicates network progress
//
//==========================================================================

void ST_NetProgress(void)
{
}


//==========================================================================
//
// ST_NetDone - net progress complete
//
//==========================================================================
void ST_NetDone(void)
{
	S_StartSound(NULL, SFX_PICKUP_WEAPON);
}


//==========================================================================
//
// ST_Message - gives debug message
//
//==========================================================================

void ST_Message(char *message, ...)
{
	va_list argptr;
	char buffer[80];

	va_start(argptr, message);
	vsprintf(buffer, message, argptr);
	va_end(argptr);

	if ( strlen(buffer) >= 80 )
	{
		I_Error("Long debug message has overwritten memory");
	}

	printf(buffer);
}

//==========================================================================
//
// ST_RealMessage - gives user message
//
//==========================================================================

void ST_RealMessage(char *message, ...)
{
	va_list argptr;
	char buffer[80];

	va_start(argptr, message);
	vsprintf(buffer, message, argptr);
	va_end(argptr);

	if ( strlen(buffer) >= 80 )
	{
		I_Error("Long debug message has overwritten memory\n");
	}

	printf(buffer);		// Always print these messages
}



//==========================================================================
//
// ST_LoadScreen - loads startup graphic
//
//==========================================================================


char *ST_LoadScreen(void)
{
	int length,lump;
	char *buffer;

	lump = W_GetNumForName("STARTUP");
	length = W_LumpLength(lump);
	buffer = (char *)Z_Malloc(length, PU_STATIC, NULL);
	W_ReadLump(lump, buffer);
	return(buffer);
}
