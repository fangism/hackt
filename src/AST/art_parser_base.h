/**
	\file "art_parser_base.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_base.h,v 1.19.26.1 2005/02/22 08:15:18 fang Exp $
 */

#ifndef __ART_PARSER_BASE_H__
#define __ART_PARSER_BASE_H__

#include <iosfwd>
#include "macros.h"
#include "art_lex.h"		// for token_position, line_position, ...
#include "memory/excl_ptr.h"	// experimental pointer classes

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
	This is the general namespace for all ART-related classes.  
 */
namespace ART {
//=============================================================================
// forward declaration of outside namespace and classes
namespace entity {
	// defined in "art_object_base.h"
	class object;
}

using std::ostream;
using entity::object;
using util::memory::never_ptr;

//=============================================================================
/// This namespace is reserved for ART's parser-related classes.  
/**
	This contains all of the classes for the abstract syntax tree (AST).  
	Each class should implement recursive methods of traversal.  
	The created AST will only reflect a legal instance of the grammar, 
	therefore, one should use the type-check and build phase to 
	return a more useful manipulate ART object.  
 */
namespace parser {
//=============================================================================
// forward declarations

class context;		// defined in "Object/art_context.h"

//=============================================================================
// some constant delimiter strings, defined in art_parser.cc
// these are used as delimiter template arguments for node_list
extern	const char	none[];
extern	const char	comma[];
extern	const char	semicolon[];
extern	const char	scope[];
extern	const char	thickbar[];
extern	const char	colon[];
extern	const char	alias[];
extern	const char	pound[];	// calling it "hash" would be confusing

//=============================================================================
/// the abstract base class for parser nodes, universal return type
/**
	The mother class.  
	Rather than have to rely on unions in the lexer and parser, 
	we prefer to use a single type of node for manipulation.  
	This will be useful when we wish to dump the parser's value stack
	in error reporting and debugging.  
	All immediate subclasses of node must be virtually inherited.  
 */
class node {
private:
/**
	Rule regarding g++ emission of vtables:
	"If the class declares any non-inline, non-pure virtual functions, 
	the first one is chosen as the "key method" for the class, 
	and the vtable is only emitted in the translation unit where 
	the key method is defined."
	This bogus method is defined in "art_parser_base.cc".
 */
virtual	void
	bogus(void) const;
public:

/** Standard virtual destructor, mother-of-all virtual destructors */
virtual	~node() { }

/**
	Shows representation without recursive descent.  
	Derived classes of non-terminals should just print their type name.  
	\param o the output stream.  
	\return the output stream.
 */
virtual	ostream&
	what(ostream& o) const = 0;

/**
	Shows the position where the node's subtree starts.  
	\return the starting position.
 */
virtual	line_position
	leftmost(void) const = 0;

/**
	Shows the position where the node's subtree ends.  
	\return the ending position.
 */
virtual	line_position
	rightmost(void) const = 0;

/**
	Shows the range of file position covered by a particular node's
	subtree.  
 */
virtual	line_range
	where(void) const;

/**
	Type-check and return a usable ART::entity::object, which 
	contains a hierarchical symbol table.  
	\return pointer to resulting object.  
 */
virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class node

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_BASE_H__

