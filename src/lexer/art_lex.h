/**
	"art.l.h"
	Header for a few simple structures, useful to the 
	language front-end.  
 */

#ifndef	__ART_L_H__
#define	__ART_L_H__

/// structure for tracking token positions in the input file
// consider including file stream/pointer later...
typedef struct _token_position_ {
	long line;		///< line of start of token
	long leng;		///< length, applies for single-line tokens
	long col;		///< column position of start of token
	const long off;		///< offset: where yymore actually starts

	/**
		gcc-3.2 requires explicit constructor for class with
		non-static const members, gcc-3.3 and up don't require.
	 */
	_token_position_(const long a, const long b,
		const long c, const long d) :
		line(a), leng(b), col(c), off(d) {
	}
} token_position;


#endif	// __ART_L_H__

