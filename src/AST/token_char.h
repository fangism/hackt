/**
	\file "AST/token_char.h"
	Base set of classes for the HAC parser.  
	Definition of this class is in "AST/token.cc"
	$Id: token_char.h,v 1.2 2005/12/13 04:15:13 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_token_char.h,v 1.3.52.1 2005/12/11 00:45:11 fang Exp
 */

#ifndef __HAC_AST_TOKEN_CHAR_H__
#define __HAC_AST_TOKEN_CHAR_H__

#include "AST/terminal.h"
#include "util/memory/chunk_map_pool_fwd.h"

//=============================================================================
namespace HAC {
namespace parser {
//=============================================================================
/**
	Single token characters.
	Methods defined in "AST/token.cc"
 */
class token_char : public terminal {
private:
	typedef	token_char			this_type;
protected:
/// the character
	int c;
public:
	/// inlined, intended only for allocator use
	token_char();

	explicit
	token_char(const int i) : terminal(), c(i) { }

	~token_char() { }

	int
	get_char(void) const { return c; }

	int
	string_compare(const char* d) const;

	ostream&
	what(ostream& o) const;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};      // end class token_char

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_TOKEN_CHAR_H__

