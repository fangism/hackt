/**
	\file "art_parser_terminal.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_terminal.h,v 1.3 2005/03/06 22:45:50 fang Exp $
 */

#ifndef __ART_PARSER_TERMINAL_H__
#define __ART_PARSER_TERMINAL_H__

#include <string>
#include "art_lex.h"
#include "art_parser_base.h"

//=============================================================================
namespace ART {
//=============================================================================
// global variable
namespace lexer {
extern	token_position current;
}

using lexer::current;			// current token position
using std::string;

namespace parser {
//=============================================================================
/**
	Abstract base class for terminal tokens, mainly to be used by lexer.  
 */
class terminal : virtual public node {
protected:
/// The position in the file where token was found.  (pos.off is unused)
	line_position	pos;
// file name will be kept separate?
protected:
///	base constructor always records the current position of the token
	terminal() : node(), pos(current) { }

public:
///	standard virtual destructor
virtual	~terminal() { }

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
}	// end namespace ART

#endif	// __ART_PARSER_TERMINAL_H__

