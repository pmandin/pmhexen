// **************************************************************************
// **
// ** w_wad.c : Heretic 2 : Raven Software, Corp.
// **
// ** $RCSfile: w_wad.c,v $
// ** $Revision: 1.1 $
// ** $Date: 2006/08/18 20:49:17 $
// ** $Author: patrice $
// **
// **************************************************************************

// HEADER FILES ------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "h2def.h"

// MACROS ------------------------------------------------------------------

#ifndef O_BINARY
#define O_BINARY 0
#endif

// TYPES -------------------------------------------------------------------

typedef struct
{
	char identification[4];
	int numlumps;
	int infotableofs;
} wadinfo_t;

typedef struct
{
	int filepos;
	int size;
	char name[8];
} filelump_t;

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

#define DEFAULT_LUMPINFO_SIZE 16

static unsigned long lumpinfo_size=DEFAULT_LUMPINFO_SIZE;
lumpinfo_t *lumpinfo=NULL;
int numlumps;
void **lumpcache;

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static lumpinfo_t *PrimaryLumpInfo;
static int PrimaryNumLumps;
static void **PrimaryLumpCache;
static lumpinfo_t *AuxiliaryLumpInfo;
static int AuxiliaryNumLumps;
static void **AuxiliaryLumpCache;
static int AuxiliaryHandle = 0;
boolean AuxiliaryOpened = false;

// CODE --------------------------------------------------------------------

#ifndef HAVE_STRUPR
void strupr (char *s)
{
	while (*s) {
		*s = toupper(*s);
		s++;
	}
}
#endif

//==========================================================================
//
// filelength
//
//==========================================================================

int filelength(int handle)
{
    struct stat fileinfo;

    if(fstat(handle, &fileinfo) == -1)
	{
		I_Error("Error fstating");
	}
    return fileinfo.st_size;
}

//==========================================================================
//
// W_AddFile
//
// Files with a .wad extension are wadlink files with multiple lumps,
// other files are single lumps with the base filename for the lump name.
//
//==========================================================================

void W_AddFile(char *filename)
{
	wadinfo_t header;
	lumpinfo_t *lump_p, *oldlump;
	unsigned i;
	int handle, length, newsize;
	int startlump;
	filelump_t *fileinfo, singleinfo;
	filelump_t *freeFileInfo;

	if((handle = open(filename, O_RDONLY|O_BINARY)) == -1)
	{ // Didn't find file
		return;
	}
	startlump = numlumps;
	if(strcasecmp(filename+strlen(filename)-3, "wad"))
	{ // Single lump file
		fileinfo = &singleinfo;
		freeFileInfo = NULL;
		singleinfo.filepos = 0;
		singleinfo.size = LONG(filelength(handle));
		M_ExtractFileBase(filename, singleinfo.name);
		numlumps++;
	}
	else
	{ // WAD file
		read(handle, &header, sizeof(header));
		if(strncmp(header.identification, "IWAD", 4))
		{
			if(strncmp(header.identification, "PWAD", 4))
			{ // Bad file id
				I_Error("Wad file %s doesn't have IWAD or PWAD id\n",
					filename);
			}
		}
		header.numlumps = LONG(header.numlumps);
		header.infotableofs = LONG(header.infotableofs);
		length = header.numlumps*sizeof(filelump_t);
		fileinfo = Z_Malloc(length, PU_STATIC, NULL);
		freeFileInfo = fileinfo;
		lseek(handle, header.infotableofs, SEEK_SET);
		read(handle, fileinfo, length);
		numlumps += header.numlumps;
	}

	// Fill in lumpinfo
	oldlump = lumpinfo;
	newsize = numlumps*sizeof(lumpinfo_t);
	lumpinfo = Z_Malloc(newsize, PU_STATIC, NULL);
	memcpy((void *)lumpinfo, (void *)oldlump, lumpinfo_size);
	Z_Free(oldlump);
	lumpinfo_size = newsize;

//	lumpinfo = realloc(lumpinfo, numlumps*sizeof(lumpinfo_t));
	if(!lumpinfo)
	{
		I_Error("Couldn't realloc lumpinfo");
	}
	lump_p = &lumpinfo[startlump];
	for(i = startlump; i < numlumps; i++, lump_p++, fileinfo++)
	{
		lump_p->handle = handle;
		lump_p->position = LONG(fileinfo->filepos);
		lump_p->size = LONG(fileinfo->size);
		strncpy(lump_p->name, fileinfo->name, 8);
	}
	if(freeFileInfo)
	{
		Z_Free(freeFileInfo);
	}
}

//==========================================================================
//
// W_InitMultipleFiles
//
// Pass a null terminated list of files to use.  All files are optional,
// but at least one file must be found.  Lump names can appear multiple
// times.  The name searcher looks backwards, so a later file can
// override an earlier one.
//
//==========================================================================

void W_InitMultipleFiles(char **filenames)
{
	int size;

	// Open all the files, load headers, and count lumps
	numlumps = 0;
	lumpinfo = Z_Malloc(DEFAULT_LUMPINFO_SIZE, PU_STATIC, NULL);	// will be realloced as lumps are added

	for(; *filenames; filenames++)
	{
		W_AddFile(*filenames);
	}
	if(!numlumps)
	{
		I_Error("W_InitMultipleFiles: no files found");
	}

	// Set up caching
	size = numlumps*sizeof(*lumpcache);
	lumpcache = Z_Malloc(size, PU_STATIC, NULL);
	if(!lumpcache)
	{
		I_Error("Couldn't allocate lumpcache");
	}
	memset(lumpcache, 0, size);

	PrimaryLumpInfo = lumpinfo;
	PrimaryLumpCache = lumpcache;
	PrimaryNumLumps = numlumps;
}

//==========================================================================
//
// W_InitFile
//
// Initialize the primary from a single file.
//
//==========================================================================

void W_InitFile(char *filename)
{
	char *names[2];

	names[0] = filename;
	names[1] = NULL;
	W_InitMultipleFiles(names);
}

//==========================================================================
//
// W_OpenAuxiliary
//
//==========================================================================

void W_OpenAuxiliary(char *filename)
{
	int i;
	int size;
	wadinfo_t header;
	int handle;
	int length;
	filelump_t *fileinfo;
	filelump_t *sourceLump;
	lumpinfo_t *destLump;

	if(AuxiliaryOpened)
	{
		W_CloseAuxiliary();
	}
	if((handle = open(filename, O_RDONLY|O_BINARY)) == -1)
	{
		I_Error("W_OpenAuxiliary: %s not found.", filename);
		return;
	}
	AuxiliaryHandle = handle;
	read(handle, &header, sizeof(header));
	if(strncmp(header.identification, "IWAD", 4))
	{
		if(strncmp(header.identification, "PWAD", 4))
		{ // Bad file id
			I_Error("Wad file %s doesn't have IWAD or PWAD id\n",
				filename);
		}
	}
	header.numlumps = LONG(header.numlumps);
	header.infotableofs = LONG(header.infotableofs);
	length = header.numlumps*sizeof(filelump_t);
	fileinfo = Z_Malloc(length, PU_STATIC, 0);
	lseek(handle, header.infotableofs, SEEK_SET);
	read(handle, fileinfo, length);
	numlumps = header.numlumps;

	// Init the auxiliary lumpinfo array
	lumpinfo = Z_Malloc(numlumps*sizeof(lumpinfo_t), PU_STATIC, 0);
	sourceLump = fileinfo;
	destLump = lumpinfo;
	for(i = 0; i < numlumps; i++, destLump++, sourceLump++)
	{
		destLump->handle = handle;
		destLump->position = LONG(sourceLump->filepos);
		destLump->size = LONG(sourceLump->size);
		strncpy(destLump->name, sourceLump->name, 8);
	}
	Z_Free(fileinfo);

	// Allocate the auxiliary lumpcache array
	size = numlumps*sizeof(*lumpcache);
	lumpcache = Z_Malloc(size, PU_STATIC, 0);
	memset(lumpcache, 0, size);

	AuxiliaryLumpInfo = lumpinfo;
	AuxiliaryLumpCache = lumpcache;
	AuxiliaryNumLumps = numlumps;
	AuxiliaryOpened = true;
}

//==========================================================================
//
// W_CloseAuxiliary
//
//==========================================================================

void W_CloseAuxiliary(void)
{
	int i;

	if(AuxiliaryOpened)
	{
		W_UseAuxiliary();
		for(i = 0; i < numlumps; i++)
		{
			if(lumpcache[i])
			{
				Z_Free(lumpcache[i]);
			}
		}
		Z_Free(AuxiliaryLumpInfo);
		Z_Free(AuxiliaryLumpCache);
		W_CloseAuxiliaryFile();
		AuxiliaryOpened = false;
	}
	W_UsePrimary();
}

//==========================================================================
//
// W_CloseAuxiliaryFile
//
// WARNING: W_CloseAuxiliary() must be called before any further
// auxiliary lump processing.
//
//==========================================================================

void W_CloseAuxiliaryFile(void)
{
	if(AuxiliaryHandle)
	{
		close(AuxiliaryHandle);
		AuxiliaryHandle = 0;
	}
}

//==========================================================================
//
// W_UsePrimary
//
//==========================================================================

void W_UsePrimary(void)
{
	lumpinfo = PrimaryLumpInfo;
	numlumps = PrimaryNumLumps;
	lumpcache = PrimaryLumpCache;
}

//==========================================================================
//
// W_UseAuxiliary
//
//==========================================================================

void W_UseAuxiliary(void)
{
	if(AuxiliaryOpened == false)
	{
		I_Error("W_UseAuxiliary: WAD not opened.");
	}
	lumpinfo = AuxiliaryLumpInfo;
	numlumps = AuxiliaryNumLumps;
	lumpcache = AuxiliaryLumpCache;
}

//==========================================================================
//
// W_NumLumps
//
//==========================================================================

int	W_NumLumps(void)
{
	return numlumps;
}

//==========================================================================
//
// W_CheckNumForName
//
// Returns -1 if name not found.
//
//==========================================================================

int W_CheckNumForName(char *name)
{
	union {
		char	s[9];
		int	x[2];
	} name8;
	int		v1,v2;
	lumpinfo_t	*lump_p;

	// make the name into two integers for easy compares
	strncpy (name8.s,name,8);

	// in case the name was a fill 8 chars
	name8.s[8] = 0;

	// case insensitive
	strupr (name8.s);		

	v1 = name8.x[0];
	v2 = name8.x[1];

	// Scan backwards so patch lump files take precedence
	lump_p = lumpinfo+numlumps;
	while(lump_p-- != lumpinfo)
	{
		if(*(int *)lump_p->name == v1 && *(int *)&lump_p->name[4] == v2)
		{
			return lump_p-lumpinfo;
		}
	}
	return -1;
}

//==========================================================================
//
// W_GetNumForName
//
// Calls W_CheckNumForName, but bombs out if not found.
//
//==========================================================================

int	W_GetNumForName (char *name)
{
	int	i;

	i = W_CheckNumForName(name);
	if(i != -1)
	{
		return i;
	}
	I_Error("W_GetNumForName: %s not found!", name);
	return -1;
}

//==========================================================================
//
// W_LumpLength
//
// Returns the buffer size needed to load the given lump.
//
//==========================================================================

int W_LumpLength(int lump)
{
	if(lump >= numlumps)
	{
		I_Error("W_LumpLength: %i >= numlumps", lump);
	}
	return lumpinfo[lump].size;
}

//==========================================================================
//
// W_ReadLump
//
// Loads the lump into the given buffer, which must be >= W_LumpLength().
//
//==========================================================================

void W_ReadLump(int lump, void *dest)
{
	int c;
	lumpinfo_t *l;

	if(lump >= numlumps)
	{
		I_Error("W_ReadLump: %i >= numlumps", lump);
	}
	l = lumpinfo+lump;
	//I_BeginRead();
	lseek(l->handle, l->position, SEEK_SET);
	c = read(l->handle, dest, l->size);
	if(c < l->size)
	{
		I_Error("W_ReadLump: only read %i of %i on lump %i",
			c, l->size, lump);
	}
	//I_EndRead();
}

//==========================================================================
//
// W_CacheLumpNum
//
//==========================================================================

void *W_CacheLumpNum(int lump, int tag)
{
	byte *ptr;

	if((unsigned)lump >= numlumps)
	{
		I_Error("W_CacheLumpNum: %i >= numlumps", lump);
	}
	if(!lumpcache[lump])
	{ // Need to read the lump in
		ptr = Z_Malloc(W_LumpLength(lump), tag, &lumpcache[lump]);
		W_ReadLump(lump, lumpcache[lump]);
	}
	else
	{
		Z_ChangeTag(lumpcache[lump], tag);
	}
	return lumpcache[lump];
}

//==========================================================================
//
// W_CacheLumpName
//
//==========================================================================

void *W_CacheLumpName(char *name, int tag)
{
	return W_CacheLumpNum(W_GetNumForName(name), tag);
}
