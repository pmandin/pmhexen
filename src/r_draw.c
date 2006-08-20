// **************************************************************************
// **
// ** r_draw.c : Heretic 2 : Raven Software, Corp.
// **
// ** $RCSfile: r_draw.c,v $
// ** $Revision: 1.2 $
// ** $Date: 2006/08/20 17:42:06 $
// ** $Author: patrice $
// **
// **************************************************************************

#include "h2def.h"
#include "r_local.h"
#include "i_video.h"

/*

All drawing to the view buffer is accomplished in this file.  The other refresh
files only know about ccordinates, not the architecture of the frame buffer.

*/

byte *viewimage;
int viewwidth, scaledviewwidth, viewheight, viewwindowx, viewwindowy;
byte **ylookup=NULL;
int *columnofs=NULL;
//byte translations[3][256]; // color tables for different players
byte *tinttable; // used for translucent sprites

/*
==================
=
= R_DrawColumn
=
= Source is the top of the column to scale
=
==================
*/

lighttable_t	*dc_colormap;
int				dc_x;
int				dc_yl;
int				dc_yh;
fixed_t			dc_iscale;
fixed_t			dc_texturemid;
byte			*dc_source;		// first pixel in a column (possibly virtual)

int				dccount;		// just for profiling

void R_DrawColumn (void)
{
	int			count;
	byte		*dest;
	fixed_t		frac, fracstep;	

	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x]; 
	
	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

# define RENDER_PIXEL	\
	{	\
		*dest = dc_colormap[dc_source[(frac>>FRACBITS)&127]];	\
		dest += sysvideo.pitch; 	\
		frac += fracstep;	\
	}

	{
		int n = count>>2;
		switch (count & 3) {
			case 3: do {
					RENDER_PIXEL;
			case 2:		RENDER_PIXEL;
			case 1:		RENDER_PIXEL;
			case 0:		RENDER_PIXEL;
				} while (n--); /* FIXME subq/bne */
		}
	}
#undef RENDER_PIXEL
}

void R_DrawColumnLow (void)
{
	int			count, n;
	byte		*dest;
	fixed_t		frac, fracstep;	

	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
//	dccount++;
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x<<1];
	
	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

#define RENDER_PIXEL	\
	{	\
		int spot;	\
		spot = dc_colormap[dc_source[(frac>>FRACBITS)&127]];	\
		*(unsigned short *)dest = spot|(spot<<8);	\
		dest += sysvideo.pitch; 	\
		frac += fracstep;	\
	}	

	n = count>>2;
	switch (count & 3) {
		case 3: do {
				RENDER_PIXEL;
		case 2:		RENDER_PIXEL;
		case 1:		RENDER_PIXEL;
		case 0:		RENDER_PIXEL;
			} while (n--); /* FIXME subq/bne */
	}
#undef RENDER_PIXEL
}

void R_DrawFuzzColumn (void)
{
	int			count, n;
	byte		*dest;
	fixed_t		frac, fracstep;	

	if (!dc_yl)
		dc_yl = 1;
	if (dc_yh == viewheight-1)
		dc_yh = viewheight - 2;
		
	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawFuzzColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

#define RENDER_PIXEL	\
	{	\
		*dest = tinttable[(*dest)+dc_colormap[dc_source[(frac>>FRACBITS)&127]]];	\
		dest += sysvideo.pitch;	\
		frac += fracstep;	\
	}

	n = count>>2;
	switch (count & 3) {
		case 3: do {
				RENDER_PIXEL;
		case 2:		RENDER_PIXEL;
		case 1:		RENDER_PIXEL;
		case 0:		RENDER_PIXEL;
			} while (n--); /* FIXME subq/bne */
	}
#undef RENDER_PIXEL
}

void R_DrawFuzzColumnLow (void)
{
	int			count, n;
	byte		*dest;
	fixed_t		frac, fracstep;	

	if (!dc_yl)
		dc_yl = 1;
	if (dc_yh == viewheight-1)
		dc_yh = viewheight - 2;
		
	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawFuzzColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x<<1];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

#define RENDER_PIXEL	\
	{	\
		int color;	\
		\
		color = tinttable[(*dest)+dc_colormap[dc_source[(frac>>FRACBITS)&127]]];	\
		*((unsigned short *) dest) = color|(color<<8);	\
		dest += sysvideo.pitch;	\
		frac += fracstep;	\
	}

	n = count>>2;
	switch (count & 3) {
		case 3: do {
				RENDER_PIXEL;
		case 2:		RENDER_PIXEL;
		case 1:		RENDER_PIXEL;
		case 0:		RENDER_PIXEL;
			} while (n--); /* FIXME subq/bne */
	}
#undef RENDER_PIXEL
}

//============================================================================
//
// R_DrawAltFuzzColumn
//
//============================================================================

void R_DrawAltFuzzColumn (void)
{
	int			count, n;
	byte		*dest;
	fixed_t		frac, fracstep;	

	if (!dc_yl)
		dc_yl = 1;
	if (dc_yh == viewheight-1)
		dc_yh = viewheight - 2;
		
	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawFuzzColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

#define RENDER_PIXEL	\
	{	\
		*dest = tinttable[((*dest)<<8)+dc_colormap[dc_source[(frac>>FRACBITS)&127]]];	\
		dest += sysvideo.pitch;	\
		frac += fracstep;	\
	}

	n = count>>2;
	switch (count & 3) {
		case 3: do {
				RENDER_PIXEL;
		case 2:		RENDER_PIXEL;
		case 1:		RENDER_PIXEL;
		case 0:		RENDER_PIXEL;
			} while (n--); /* FIXME subq/bne */
	}
#undef RENDER_PIXEL
}

void R_DrawAltFuzzColumnLow (void)
{
	int			count, n;
	byte		*dest;
	fixed_t		frac, fracstep;	

	if (!dc_yl)
		dc_yl = 1;
	if (dc_yh == viewheight-1)
		dc_yh = viewheight - 2;
		
	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawFuzzColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x<<1];

	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

#define RENDER_PIXEL	\
	{	\
		int color;	\
		\
		color = tinttable[((*dest)<<8)+dc_colormap[dc_source[(frac>>FRACBITS)&127]]];	\
		*((unsigned short *) dest) = color|(color<<8);	\
		dest += sysvideo.pitch;	\
		frac += fracstep;	\
	}

	n = count>>2;
	switch (count & 3) {
		case 3: do {
				RENDER_PIXEL;
		case 2:		RENDER_PIXEL;
		case 1:		RENDER_PIXEL;
		case 0:		RENDER_PIXEL;
			} while (n--); /* FIXME subq/bne */
	}
#undef RENDER_PIXEL
}

/*
========================
=
= R_DrawTranslatedColumn
=
========================
*/

byte *dc_translation;
byte *translationtables;

void R_DrawTranslatedColumn (void)
{
	int			count;
	byte		*dest;
	fixed_t		frac, fracstep;	

	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];
	
	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

#define RENDER_PIXEL	\
	*dest = dc_colormap[dc_translation[dc_source[(frac>>FRACBITS)&127]]];	\
	dest += sysvideo.pitch; 	\
	frac += fracstep;

	{
		int n = count>>2;
		switch (count & 3) {
			case 3: do {
					RENDER_PIXEL;
			case 2:		RENDER_PIXEL;
			case 1:		RENDER_PIXEL;
			case 0:		RENDER_PIXEL;
				} while (n--); /* FIXME subq/bne */
		}
	}
#undef RENDER_PIXEL
}

void R_DrawTranslatedColumnLow (void)
{
	int			count;
	byte		*dest;
	fixed_t		frac, fracstep;	

	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x<<1];
	
	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

# define RENDER_PIXEL	\
	{	\
		int spot;	\
		\
		spot = dc_colormap[dc_translation[dc_source[(frac>>FRACBITS)&127]]]; 	\
		\
		*((unsigned short *) dest) = spot|(spot<<8);	\
		\
		dest += sysvideo.pitch;	\
		frac += fracstep;	\
	}

	{
		int n = count>>2;
		switch (count & 3) {
			case 3: do {
					RENDER_PIXEL;
			case 2:		RENDER_PIXEL;
			case 1:		RENDER_PIXEL;
			case 0:		RENDER_PIXEL;
				} while (n--); /* FIXME subq/bne */
		}
	}
#undef RENDER_PIXEL
}

//============================================================================
//
// R_DrawTranslatedFuzzColumn
//
//============================================================================

void R_DrawTranslatedFuzzColumn (void)
{
	int			count;
	byte		*dest;
	fixed_t		frac, fracstep;	

	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];
	
	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

#define RENDER_PIXEL	\
	{	\
		*dest =  tinttable[((*dest)<<8)	\
			+dc_colormap[dc_translation[dc_source[frac>>FRACBITS]]]];	\
		dest += sysvideo.pitch; 	\
		frac += fracstep;	\
	}

	{
		int n = count>>2;
		switch (count & 3) {
			case 3: do {
					RENDER_PIXEL;
			case 2:		RENDER_PIXEL;
			case 1:		RENDER_PIXEL;
			case 0:		RENDER_PIXEL;
				} while (n--); /* FIXME subq/bne */
		}
	}
#undef RENDER_PIXEL
}

void R_DrawTranslatedFuzzColumnLow (void)
{
	int			count;
	byte		*dest;
	fixed_t		frac, fracstep;	

	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= sysvideo.width || dc_yl < 0 || dc_yh >= sysvideo.height)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x<<1];
	
	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

# define RENDER_PIXEL	\
	{	\
		int spot;	\
		\
		spot = tinttable[((*dest)<<8)	\
			+dc_colormap[dc_translation[dc_source[frac>>FRACBITS]]]];	\
		\
		*((unsigned short *) dest) = spot|(spot<<8);	\
		\
		dest += sysvideo.pitch;	\
		frac += fracstep;	\
	}

	{
		int n = count>>2;
		switch (count & 3) {
			case 3: do {
					RENDER_PIXEL;
			case 2:		RENDER_PIXEL;
			case 1:		RENDER_PIXEL;
			case 0:		RENDER_PIXEL;
				} while (n--); /* FIXME subq/bne */
		}
	}
#undef RENDER_PIXEL
}

//--------------------------------------------------------------------------
//
// PROC R_InitTranslationTables
//
//--------------------------------------------------------------------------

void R_InitTranslationTables (void)
{
	int i;
	byte *transLump;

	// Load tint table
	tinttable = W_CacheLumpName("TINTTAB", PU_STATIC);

	// Allocate translation tables
	translationtables = Z_Malloc(256*3*(MAXPLAYERS-1)+255, 
		PU_STATIC, 0);
	translationtables = (byte *)(((int)translationtables+255)&~255);

	for(i = 0; i < 3*(MAXPLAYERS-1); i++)
	{
		transLump = W_CacheLumpNum(W_GetNumForName("trantbl0")+i, PU_STATIC);
		memcpy(translationtables+i*256, transLump, 256);
		Z_Free(transLump);
	}
}

/*
================
=
= R_DrawSpan
=
================
*/

int				ds_y;
int				ds_x1;
int				ds_x2;
lighttable_t	*ds_colormap;
fixed_t			ds_xfrac;
fixed_t			ds_yfrac;
fixed_t			ds_xstep;
fixed_t			ds_ystep;
byte			*ds_source;		// start of a 64*64 tile image

int				dscount;		// just for profiling

void R_DrawSpan (void)
{
	fixed_t		xfrac, yfrac;
	byte		*dest;
	int			count;
	
#ifdef RANGECHECK
	if (ds_x2 < ds_x1 || ds_x1<0 || ds_x2>=sysvideo.width 
	|| (unsigned)ds_y>sysvideo.height)
		I_Error ("R_DrawSpan: %i to %i at %i",ds_x1,ds_x2,ds_y);
//	dscount++;
#endif
	
	xfrac = ds_xfrac;
	yfrac = ds_yfrac;
	
	dest = ylookup[ds_y] + columnofs[ds_x1];	
	count = ds_x2 - ds_x1;

# define RENDER_PIXEL	\
	{	\
	    int spot;	\
		\
		/* Current texture index in u,v. */	\
		spot = ((yfrac>>(16-6))&(63*64)) + ((xfrac>>16)&63); \
		\
		/* Lookup pixel from flat texture tile, */ \
		/*  re-index using light/colormap. */ \
		*dest++ = ds_colormap[ds_source[spot]];	\
		\
		/* Next step in u,v. */ \
		xfrac += ds_xstep;	\
		yfrac += ds_ystep;	\
	}

	{
		fixed_t xfrac = ds_xfrac, yfrac = ds_yfrac;

		int n = count>>2;
		switch (count & 3) {
			case 3: do {
					RENDER_PIXEL;
			case 2:		RENDER_PIXEL;
			case 1:		RENDER_PIXEL;
			case 0:		RENDER_PIXEL;
				} while (n--); /* FIXME subq/bne */
		}
	}
#undef RENDER_PIXEL
}

void R_DrawSpanFlat (void) 
{ 
    byte*		dest; 
	 
#ifdef RANGECHECK 
    if (ds_x2 < ds_x1
	|| ds_x1<0
	|| ds_x2>=sysvideo.width  
	|| (unsigned)ds_y>sysvideo.height)
    {
	I_Error( "R_DrawSpanFlat: %i to %i at %i",
		 ds_x1,ds_x2,ds_y);
    }
#endif 
	 
	dest = ylookup[ds_y] + columnofs[ds_x1];

	memset(dest, ds_colormap[*ds_source], ds_x2 - ds_x1 +1);
}

void R_DrawSpanLow (void)
{
	fixed_t		xfrac, yfrac;
	byte		*dest;
	int			count;
	
#ifdef RANGECHECK
	if (ds_x2 < ds_x1 || ds_x1<0 || ds_x2>=sysvideo.width 
	|| (unsigned)ds_y>sysvideo.height)
		I_Error ("R_DrawSpan: %i to %i at %i",ds_x1,ds_x2,ds_y);
//	dscount++;
#endif
	
	xfrac = ds_xfrac;
	yfrac = ds_yfrac;
	
	dest = ylookup[ds_y] + columnofs[ds_x1<<1];
	count = ds_x2 - ds_x1;

# define RENDER_PIXEL	\
	{	\
	    int spot;	\
		\
		/* Current texture index in u,v. */	\
		spot = ((yfrac>>(16-6))&(63*64)) + ((xfrac>>16)&63); \
		spot = ds_colormap[ds_source[spot]];	\
		\
		/* Lookup pixel from flat texture tile, */ \
		/*  re-index using light/colormap. */ \
		*(unsigned short *)dest = spot|(spot<<8);	\
		dest += 2;	\
		\
		/* Next step in u,v. */ \
		xfrac += ds_xstep;	\
		yfrac += ds_ystep;	\
	}

	{
		fixed_t		xfrac = ds_xfrac, yfrac = ds_yfrac;
		int n = count>>2;
		switch (count & 3) {
			case 3: do {
					RENDER_PIXEL;
			case 2:		RENDER_PIXEL;
			case 1:		RENDER_PIXEL;
			case 0:		RENDER_PIXEL;
				} while (n--); /* FIXME subq/bne */
		}
	}
#undef RENDER_PIXEL
}

void R_DrawSpanLowFlat (void) 
{ 
    byte*		dest; 
	 
#ifdef RANGECHECK 
    if (ds_x2 < ds_x1
	|| ds_x1<0
	|| ds_x2>=sysvideo.width  
	|| (unsigned)ds_y>sysvideo.height)
    {
	I_Error( "R_DrawSpan: %i to %i at %i",
		 ds_x1,ds_x2,ds_y);
    }
#endif 

	dest = ylookup[ds_y] + columnofs[ds_x1<<1];

	memset(dest, ds_colormap[*ds_source], (ds_x2 - ds_x1 + 1)<<1);
}

/*
================
=
= R_InitBuffer
=
=================
*/

void R_InitBuffer (int width, int height)
{
	int		i;
	
	if (ylookup) {
		Z_Free(ylookup);
	}
	ylookup=Z_Malloc(sysvideo.height*sizeof(byte *),PU_STATIC,NULL);

	if (columnofs)
		Z_Free(columnofs);
	columnofs=Z_Malloc(sysvideo.width*sizeof(int),PU_STATIC,NULL);

	viewwindowx = (sysvideo.width-width) >> 1;
	for (i=0 ; i<width ; i++)
		columnofs[i] = viewwindowx + i;
	if (width == sysvideo.width)
		viewwindowy = 0;
	else
		viewwindowy = (sysvideo.height-((SBARHEIGHT*sysvideo.height)/SCREENHEIGHT)-height) >> 1;
	for (i=0 ; i<height ; i++)
		ylookup[i] = screen + (i+viewwindowy)*sysvideo.pitch;
}


/*
==================
=
= R_DrawViewBorder
=
= Draws the border around the view for different size windows
==================
*/

boolean BorderNeedRefresh;

void R_DrawViewBorder (void)
{
	byte	*src, *dest;
	int		x,y;
	
	if (scaledviewwidth == sysvideo.width)
		return;

	src = W_CacheLumpName("F_022", PU_CACHE);
	dest = screen;
	
	for (y=0 ; y<((SCREENHEIGHT-SBARHEIGHT)*sysvideo.height)/SCREENHEIGHT ; y++)
	{
		byte *dest_line=dest;

		for (x=0 ; x<sysvideo.width/64 ; x++)
		{
			memcpy (dest_line, src+((y&63)<<6), 64);
			dest_line += 64;
		}
		if (sysvideo.width&63)
		{
			memcpy (dest_line, src+((y&63)<<6), sysvideo.width&63);
		}
		dest += sysvideo.pitch;
	}
	for(x=viewwindowx; x < viewwindowx+(viewwidth<<detailshift); x += 16)
	{
		V_DrawPatchDirect(x, viewwindowy-4, W_CacheLumpName("bordt", PU_CACHE));
		V_DrawPatchDirect(x, viewwindowy+viewheight, W_CacheLumpName("bordb", 
			PU_CACHE));
	}
	for(y=viewwindowy; y < viewwindowy+viewheight; y += 16)
	{
		V_DrawPatchDirect(viewwindowx-4, y, W_CacheLumpName("bordl", PU_CACHE));
		V_DrawPatchDirect(viewwindowx+(viewwidth<<detailshift), y, W_CacheLumpName("bordr", 
			PU_CACHE));
	}
	V_DrawPatchDirect(viewwindowx-4, viewwindowy-4, W_CacheLumpName("bordtl", 
		PU_CACHE));
	V_DrawPatchDirect(viewwindowx+(viewwidth<<detailshift), viewwindowy-4, 
		W_CacheLumpName("bordtr", PU_CACHE));
	V_DrawPatchDirect(viewwindowx+(viewwidth<<detailshift), viewwindowy+viewheight, 
		W_CacheLumpName("bordbr", PU_CACHE));
	V_DrawPatchDirect(viewwindowx-4, viewwindowy+viewheight, 
		W_CacheLumpName("bordbl", PU_CACHE));
}

/*
==================
=
= R_DrawTopBorder
=
= Draws the top border around the view for different size windows
==================
*/

boolean BorderTopRefresh;

void R_DrawTopBorder (void)
{
	byte	*src, *dest;
	int		x,y;
	
	if (scaledviewwidth == sysvideo.width)
		return;

	src = W_CacheLumpName("F_022", PU_CACHE);
	dest = screen;
	
	for (y=0 ; y<(34*sysvideo.height)/SCREENHEIGHT ; y++)
	{
		byte *dest_line=dest;

		for (x=0 ; x<sysvideo.width/64 ; x++)
		{
			memcpy (dest_line, src+((y&63)<<6), 64);
			dest_line += 64;
		}
		if (sysvideo.width&63)
		{
			memcpy (dest_line, src+((y&63)<<6), sysvideo.width&63);
		}
		dest += sysvideo.pitch;
	}
	if(viewwindowy < (35*sysvideo.height)/SCREENHEIGHT)
	{
		for(x=viewwindowx; x < viewwindowx+(viewwidth<<detailshift); x += 16)
		{
			V_DrawPatchDirect(x, viewwindowy-4, W_CacheLumpName("bordt", PU_CACHE));
		}
		V_DrawPatchDirect(viewwindowx-4, viewwindowy, W_CacheLumpName("bordl", 
			PU_CACHE));
		V_DrawPatchDirect(viewwindowx+(viewwidth<<detailshift), viewwindowy, 
			W_CacheLumpName("bordr", PU_CACHE));
		V_DrawPatchDirect(viewwindowx-4, viewwindowy+16, W_CacheLumpName("bordl", 
			PU_CACHE));
		V_DrawPatchDirect(viewwindowx+(viewwidth<<detailshift), viewwindowy+16, 
			W_CacheLumpName("bordr", PU_CACHE));

		V_DrawPatchDirect(viewwindowx-4, viewwindowy-4, W_CacheLumpName("bordtl", 
			PU_CACHE));
		V_DrawPatchDirect(viewwindowx+(viewwidth<<detailshift), viewwindowy-4, 
			W_CacheLumpName("bordtr", PU_CACHE));
	}
}
