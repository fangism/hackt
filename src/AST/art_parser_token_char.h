/**
	\file "art_parser_token_char.h"
	Base set of classes for the ART parser.  
	Definition of this class is in "art_parser_token.cc"
	$Id: art_parser_token_char.h,v 1.1 2005/03/06 22:45:50 fang Exp $
 */

#ifndef __ART_PARSER_TOKEN_CHAR_H__
#define __ART_PARSER_TOKEN_CHAR_H__

#include "art_parser_terminal.h"
// #include "memory/list_vector_pool_fwd.h"
#include "memory/chunk_map_pool_fwd.h"

//=============================================================================
namespace ART {
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

//	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
// private:
};      // end class token_char

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_TOKEN_CHAR_H__

