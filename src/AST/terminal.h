/**
	\file "AST/terminal.h"
	Base set of classes for the HAC parser.  
	$Id: terminal.h,v 1.1.2.1 2005/12/11 04:41:31 fang Exp $
	This file used to be the following before it was renamed:
	$Id: terminal.h,v 1.1.2.1 2005/12/11 04:41:31 fang Exp $
 */

#ifndef __HAC_AST_TERMINAL_H__
#define __HAC_AST_TERMINAL_H__

#include <string>
#include "lexer/hac_lex.h"
#include "AST/common.h"

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
	This bogus method is defined in "AST/token.cc".
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

#endif	// __HAC_AST_TERMINAL_H__

