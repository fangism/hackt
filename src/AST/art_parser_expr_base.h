/**
	\file "AST/art_parser_base.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_expr_base.h,v 1.3.2.2 2005/05/12 00:43:48 fang Exp $
 */

#ifndef __AST_ART_PARSER_EXPR_BASE_H__
#define __AST_ART_PARSER_EXPR_BASE_H__

#include "AST/art_parser_base.h"

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
	class param_expr;
	class index_list;
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
	typedef	count_ptr<entity::param_expr>	return_type;
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

/**
	Prototype for expression check method.  
 */
#define	CHECK_EXPR_PROTO						\
	expr::return_type						\
	check_expr(context& c) const

virtual CHECK_EXPR_PROTO = 0;

};	// end class expr

//-----------------------------------------------------------------------------
/**
	Bah! breaks circular dependence of nested typedefs.  
 */
typedef	count_ptr<entity::index_list>	range_list_return_type;

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_EXPR_BASE_H__

