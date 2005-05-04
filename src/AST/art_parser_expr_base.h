/**
	\file "art_parser_base.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_expr_base.h,v 1.2 2005/05/04 17:54:10 fang Exp $
 */

#ifndef __ART_PARSER_EXPR_BASE_H__
#define __ART_PARSER_EXPR_BASE_H__

#include "art_parser_base.h"

namespace ART {
//=============================================================================
// global variable
namespace lexer {
extern	token_position current;
}

// forward declaration of outside namespace and classes
namespace entity {
	// defined in "art_object_base.h"
	class object;
}

using std::ostream;
using entity::object;
using namespace util::memory;		// for experimental pointer classes

namespace parser {
//=============================================================================
/**
	Abstract base class for general expressions.
	Expressions may be terminal or nonterminal.
	Defined in "art_parser_expr.h".
 */
class expr {
public:
	expr() { }
virtual ~expr() { }

virtual ostream&
	what(ostream& o) const = 0;

virtual line_position
	leftmost(void) const = 0;

virtual line_position
	rightmost(void) const = 0;
/**
	In all implementations, must create a param_expr object.
	The created object will be pushed onto the context's stack.
 */
virtual never_ptr<const object>
	check_build(context& c) const = 0;
};	// end class expr

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_EXPR_BASE_H__

