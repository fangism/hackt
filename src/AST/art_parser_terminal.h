/**
	\file "AST/art_parser_terminal.h"
	Base set of classes for the HAC parser.  
	$Id: art_parser_terminal.h,v 1.6.4.1 2005/12/11 00:45:10 fang Exp $
 */

#ifndef __AST_HAC_PARSER_TERMINAL_H__
#define __AST_HAC_PARSER_TERMINAL_H__

#include <string>
#include "lexer/hac_lex.h"
#include "AST/art_parser_base.h"

#include "lexer/file_manager.h"	
	// ugh, included everywhere, but needed for inlined operation
extern HAC::lexer::file_manager
hackt_parse_file_manager;

//=============================================================================
namespace HAC {
using std::string;

namespace parser {
//=============================================================================
/**
	Abstract base class for terminal tokens, mainly to be used by lexer.  

	Rule regarding g++ emission of vtables:
	"If the class declares any non-inline, non-pure virtual functions, 
	the first one is chosen as the "key method" for the class, 
	and the vtable is only emitted in the translation unit where 
	the key method is defined."
	This bogus method is defined in "art_parser_token.cc".
 */
class terminal {
protected:
/// The position in the file where token was found.  (pos.off is unused)
	line_position	pos;
// file name will be kept separate?
protected:
///	base constructor always records the current position of the token
	terminal() : pos(hackt_parse_file_manager.current_position()) { }

public:
///	standard virtual destructor
virtual	~terminal() { }

// so the virtual table will be emitted in the module where this is defined.
virtual	void bogus(void) const;

public:
virtual	int
	string_compare(const char* d) const = 0;

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;
};	// end class terminal

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __AST_HAC_PARSER_TERMINAL_H__

