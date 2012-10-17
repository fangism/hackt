/**
	\file "hac_lex.hh"
	Header for a few simple structures, useful to the 
	language front-end.  
	Note, this is a C++ source file.  
	$Id: hac_lex.hh,v 1.4 2007/08/15 01:08:18 fang Exp $
	This file's earlier revision history originates from:
	Id: art_lex.h,v 1.5 2005/02/27 22:12:00 fang Exp
 */

#ifndef	__LEXER_HAC_LEX_H__
#define	__LEXER_HAC_LEX_H__

#include <cassert>
#include <iosfwd>

// switches
// goal: 1
#define	USE_TOKEN_POSITION_FILE_NUMBER		0

// goal: 0
#define	USE_TOKEN_POSITION_OFFSET		0

namespace HAC {
namespace lexer {
using std::ostream;

/// structure for tracking token positions in the input file
// consider including file stream/pointer later...
struct token_position {
	long line;		///< line of start of token
	long leng;		///< length, applies for single-line tokens
	long col;		///< column position of start of token
#if USE_TOKEN_POSITION_OFFSET
	const long off;		///< offset: where yymore actually starts
#endif

	/**
		gcc-3.2 requires explicit constructor for class with
		non-static const members, gcc-3.3 and up don't require.
	 */
	token_position(const long a, const long b, const long c
#if USE_TOKEN_POSITION_OFFSET
		, const long d = 0
#endif
		) :
		line(a), leng(b), col(c)
#if USE_TOKEN_POSITION_OFFSET
		, off(d)
#endif
		{
	}
};	// end struct token_position

//=============================================================================
/// just a line number and column position
struct line_position {
	long            line;
	long            col;
#if USE_TOKEN_POSITION_FILE_NUMBER
	long		fn;	///< file number, will correspond to string
#endif

	line_position() : line(0), col(0) { }
	line_position(const long l, const long c) : line(l), col(c) {
		// assert(line>0);	// allow 0-line compensation
		assert(col>0);
	}
	line_position(const line_position& lp) :
		line(lp.line), col(lp.col) { }		// no assertion yet
	line_position(const token_position& tp) :
		line(tp.line), col(tp.col)
		{ assert(line>0); assert(col>0); }
// default destructor

};      // end struct line_position

//=============================================================================
// a pair of line positions constitutes a range
struct line_range {
	line_position   start;
	line_position   end;

	line_range() : start(), end() { }
	line_range(const line_position& l, const line_position& r) :
		start(l), end(r) { }
// default destructor

friend ostream& operator << (ostream& o, const line_range& l);
};      // end struct line_range

//=============================================================================

}	// end namespace lexer
}	// end namespace HAC

#endif	// __LEXER_HAC_LEX_H__

