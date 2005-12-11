/**
	\file "AST/art_parser_token_char.h"
	Base set of classes for the HAC parser.  
	Definition of this class is in "art_parser_token.cc"
	$Id: art_parser_token_char.h,v 1.3.52.1 2005/12/11 00:45:11 fang Exp $
 */

#ifndef __AST_HAC_PARSER_TOKEN_CHAR_H__
#define __AST_HAC_PARSER_TOKEN_CHAR_H__

#include "AST/art_parser_terminal.h"
#include "util/memory/chunk_map_pool_fwd.h"

//=============================================================================
namespace HAC {
namespace parser {
//=============================================================================
/**
	Single token characters.
	Methods defined in "art_parser_token.cc"
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

#endif	// __AST_HAC_PARSER_TOKEN_CHAR_H__

