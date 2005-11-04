/**
	\file "hac_lex.h"
	Header for a few simple structures, useful to the 
	language front-end.  
	Note, this is a C++ source file.  
	$Id: hac_lex.h,v 1.1.2.1 2005/11/04 22:23:28 fang Exp $
	This file's earlier revision history originates from:
	Id: art_lex.h,v 1.5 2005/02/27 22:12:00 fang Exp
 */

#ifndef	__LEXER_HAC_LEX_H__
#define	__LEXER_HAC_LEX_H__

#include <cassert>
#include <iosfwd>

namespace ART {
namespace lexer {
using std::ostream;

/// structure for tracking token positions in the input file
// consider including file stream/pointer later...
struct token_position {
	long line;		///< line of start of token
	long leng;		///< length, applies for single-line tokens
	long col;		///< column position of start of token
	const long off;		///< offset: where yymore actually starts

	/**
		gcc-3.2 requires explicit constructor for class with
		non-static const members, gcc-3.3 and up don't require.
	 */
	token_position(const long a, const long b,
		const long c, const long d) :
		line(a), leng(b), col(c), off(d) {
	}
};	// end struct token_position

//=============================================================================
/// just a line number and column position
struct line_position {
	long            line;
	long            col;

	line_position() : line(0), col(0) { }
	line_position(const long l, const long c) : line(l), col(c)
		{ assert(line>0); assert(col>0); }
	line_position(const line_position& lp) :
		line(lp.line), col(lp.col) { }		// no assertion yet
	line_position(const token_position& tp) :
		line(tp.line), col(tp.col)
		{ assert(line>0); assert(col>0); }
// default destructor

};      // end struct line_position

//=============================================================================
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
}	// end namespace ART

#endif	// __LEXER_HAC_LEX_H__

