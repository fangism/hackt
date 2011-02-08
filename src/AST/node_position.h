/**
	\file "AST/node_position.h"
	Node whose sole purpose is to record a position of a token.  
	$Id: node_position.h,v 1.3 2011/02/08 02:06:45 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_node_position.h,v 1.6.42.1 2005/12/11 00:45:09 fang Exp
 */

#ifndef	__HAC_AST_NODE_POSITION_H__
#define	__HAC_AST_NODE_POSITION_H__

#include <cstring>		// for strncpy
#include "AST/common.h"
#include "util/memory/chunk_map_pool_fwd.h"

namespace HAC {
namespace parser {
using util::memory::chunk_map_pool;
using lexer::token_position;

//=============================================================================
/**
	Generic class containing information about a token's position.  
	sizeof this object == 16, includes text field.
	Appropriate for strings shorter than 4 characters, like punctuation.  
 */
class node_position {
	typedef	node_position		this_type;
public:
	enum { MAXLEN = 4 };
	const line_position		position;
	// fixed length char array makes for efficient destruction
	char				text[MAXLEN];
public:
	/**
		The strncpy() function copies at most len characters from 
		src into dst.  If src is less than len characters long, 
		the remainder of dst is filled with `\0' characters.  
		Otherwise, dst is not terminated.

		strlcpy() will copy n-1 chars and alwaus NUL-terminate
		but it is less portable.  :(  Thus, we do it by hand.  
	 */
	node_position(const char* s, const line_position& p) :
		position(p) {
		strncpy(text, s, MAXLEN);
		text[MAXLEN-1] = 0;
	}

	node_position(const char* s, const token_position& p) :
		position(p.line, p.col) {
		strncpy(text, s, MAXLEN);
		text[MAXLEN-1] = 0;
	}

	~node_position() { }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream& o) const {
		return what(o);
	}

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)

};	// end class node_position

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic keyword node.  
	Consider template <char[]> ?
	Only tracking the leftmost position of the keyword.  
	Keywords are typically not found at the end of a 
	grammar production rule.
	String length limit is 12.  
 */
class keyword_position {
	typedef	keyword_position	this_type;
public:
	enum { MAXLEN = 12 };
	const line_position		position;
	char				text[MAXLEN];
public:
	keyword_position(const char* s, const line_position& p) :
		position(p) {
		strncpy(text, s, MAXLEN);
		text[MAXLEN-1] = 0;
	}

	keyword_position(const char* s, const token_position& p) :
		position(p.line, p.col) {
		strncpy(text, s, MAXLEN);
		text[MAXLEN-1] = 0;
	}

	~keyword_position() { }

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)

};	// end class keyword_position

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_NODE_POSITION_H__

