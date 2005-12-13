/**
	\file "AST/common.h"
	Base set of classes for the HAC parser.  
	$Id: common.h,v 1.2 2005/12/13 04:15:06 fang Exp $
	This file was called the following prior to renaming:
	Id: art_parser_base.h,v 1.26.4.1 2005/12/11 00:45:03 fang Exp
 */

#ifndef __HAC_AST_COMMON_H__
#define __HAC_AST_COMMON_H__

#include <iosfwd>
#include "util/macros.h"
#include "lexer/hac_lex.h"	// for token_position, line_position, ...
#include "util/memory/excl_ptr.h"	// experimental pointer classes

/**
	Macro for convenient definition of the node::what member function.  
	util::what is defined in "what.h".
 */
#define PARSER_WHAT_DEFAULT_IMPLEMENTATION(T)				\
std::ostream&								\
T::what(std::ostream& o) const {					\
        return o << util::what<T >::name();				\
}

//=============================================================================
/**
	This is the general namespace for all HAC-related classes.  
 */
namespace HAC {
//=============================================================================
// forward declaration of outside namespace and classes
namespace entity {
	// defined in "common/object_base.h"
	class object;
}

using std::ostream;
using entity::object;
using util::memory::never_ptr;
using lexer::line_position;
using lexer::line_range;

//=============================================================================
/// This namespace is reserved for HAC's parser-related classes.  
/**
	This contains all of the classes for the abstract syntax tree (AST).  
	Each class should implement recursive methods of traversal.  
	The created AST will only reflect a legal instance of the grammar, 
	therefore, one should use the type-check and build phase to 
	return a more useful manipulate HAC object.  
 */
namespace parser {
//=============================================================================
// forward declarations

class context;		// defined in "AST/parse_context.h"

//=============================================================================
// The mother `node' class has been deprecated and removed, sorry Mum.  
//=============================================================================

/**
	This is the interface that must be implemented by all AST classes.  
	What? Where? and of course, a destructor for deleting.  
 */
#define	PURE_VIRTUAL_NODE_METHODS					\
virtual	ostream& what(ostream& o) const = 0;				\
virtual	line_position leftmost(void) const = 0;				\
virtual	line_position rightmost(void) const = 0;

/**
	Method prototype intended for def_body_item and root_item.  
 */
#define	ROOT_CHECK_PROTO						\
	never_ptr<const object>						\
	check_build(context&) const

/**
	Generic way of finding the span of an AST construct, appropriate
	for both terminals and non-terminal nodes.  
	\param t the AST node in question.  
 */
template <class T>
inline
line_range
where(const T& t) {
	return lexer::line_range(t.leftmost(), t.rightmost());
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_COMMON_H__

