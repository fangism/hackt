// "art.l.h"
// header for a few simple structures

#ifndef	__ART_L_H__
#define	__ART_L_H__

typedef struct _token_position_ {
	long line;		// line of start of token
	long leng;		// length, applies for single-line tokens
	long col;		// column position of start of token
	const long off;		// offset: where yymore actually starts
} token_position;


#endif	// __ART_L_H__

