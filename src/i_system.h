#ifndef I_SYSTEM_H
#define I_SYSTEM_H

typedef struct {
	int kb_used;
	void *zone;
	boolean cpu060;
} sysgame_t;

extern sysgame_t	sysgame;

#define DEFAULT_HEAP_SIZE	8192	/* 8MB by default */
#define MINIMAL_HEAP_SIZE	2500	/* 2.5MB minimum */

#endif
