/**
	\file "art_parser_node_list.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_node_list.h,v 1.1 2005/02/22 08:15:20 fang Exp $
 */

#ifndef __ART_PARSER_NODE_LIST_H__
#define __ART_PARSER_NODE_LIST_H__

#include "STL/list.h"
#include "art_parser_fwd.h"
#include "art_parser_base.h"
#include "memory/count_ptr.h"

namespace ART {
//=============================================================================
USING_LIST
using util::memory::excl_ptr;
using util::memory::count_ptr;

namespace parser {
//=============================================================================
// From here below, begins the definitions of the fundamental base classes
// for the ART parser.  
// Most children classes that derive from them are defined in
// headers "art_parser_*.h".  
//=============================================================================
#define	NODE_LIST_BASE_TEMPLATE_SIGNATURE				\
	template <class T>

/**
	This is the general class for list structures of nodes in the
	syntax tree.  What is unique about this implementation is that
	its derived classes keep track of the delimiter tokens that 
	separated the lists, if applicable.  
	The specifier T, a derived class from node, is only used for 
	type-checking.  
	Consider deriving from list<base_ptr<const T> > to allow 
	copyable lists, using never_ptr<const T>'s.  
	Then dynamically casting elements may be a pain?
 */
template <class T>
class node_list_base : virtual public node, public list<count_ptr<T> > {
private:
	/**
		Base class.  (was derived from excl_const_ptr, 
		then some_count_ptr)
	 */
	typedef		list<count_ptr<T> >	list_parent;
	// read-only, but transferrable ownership
public:
	typedef	typename list_parent::value_type	value_type;
	typedef	typename list_parent::iterator		iterator;
	typedef	typename list_parent::const_iterator	const_iterator;
public:
	/**
		Default empty constructor.  
	 */
	node_list_base();

	/**
		Non-owner-transfer copy constructor.  
	 */

	node_list_base(const node_list_base<T>& l);
	/**
		Constructor with initializing first element, 
		T should be subclass of node!
	 */
	explicit
	node_list_base(const T* n);

virtual	~node_list_base();

using	list_parent::begin;
using	list_parent::end;

// later, use static functions (operator <<) to determine type name...
/// Prints out type of first element in list, if not null.  
virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

virtual	never_ptr<const object>
	check_build(context& c) const;

/// Releases memory to the destination list, transfering ownership
virtual	void
	release_append(node_list_base<T>& dest);
};	// end template class node_list_base

//-----------------------------------------------------------------------------
#define	NODE_LIST_TEMPLATE_SIGNATURE_DEFAULT				\
	template <class T, const char D[] = none>
#define	NODE_LIST_TEMPLATE_SIGNATURE					\
	template <class T, const char D[]>
#define	NODE_LIST_TEMPLATE_SIGNATURE_EXPORT				\
	export NODE_LIST_TEMPLATE_SIGNATURE
	// "export" is ignored by gcc, even 3.3

/**
	The delimiter specifier, D, is used for checking that every other
	token is separated by a D character (if D is not '\0').  
 */
NODE_LIST_TEMPLATE_SIGNATURE_DEFAULT
class node_list : public node_list_base<T> {
private:
	typedef		node_list_base<T>	parent;
public:
	typedef	typename parent::iterator	iterator;
	typedef	typename parent::const_iterator	const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;
	typedef	list<count_ptr<const terminal> >	delim_list;
protected:
	excl_ptr<const terminal>	open;	///< wrapping string, e.g. "("
	excl_ptr<const terminal>	close;	///< wrapping string, e.g. ")"
	/**
		We now keep the delimiter tokens in a separate list
		so they no longer collide with the useful elements.
		Alternative is to just keep a list of file positions, 
		since the D template parameter tells us what the delimiter 
		token is already.  
		If we really care about saving memory, can just delete
		the delimiter tokens as they are passed in.  
		If we really care about performance, just don't produce
		allocated tokens and ignore the arguments.  
	 */
	delim_list	delim;
public:
	node_list();

	node_list(const node_list<T,D>& l);

	explicit
	node_list(const T* n);

virtual	~node_list();

using	parent::begin;
using	parent::end;

/**
	This attaches enclosing text, such as brackets, braces, parentheses, 
	around a node_list.  The arguments are type-checked as token_strings
	or token_chars.  
	\param b the beginning token such as open-parenthesis.  
	\param e the end token such as open-parenthesis.  
	\return this.
 */
virtual	node_list<T,D>*
	wrap(const terminal* b, const terminal* e);

/**
	Adds an element to a node list, along with the delimiting
	character token.  Also checks that delimiter matchs, and
	that type of argument matches.
	\param d the delimiter token, such as a comma, must match D.
	\param n the useful node.  
	\return this.
 */
virtual	node_list<T,D>*
	append(const terminal* d, const T* n);

// the following methods are defined in "art_parser_template_methods.h"

/// Prints out type of first element in list, if not null.  
// virtual	ostream& what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

virtual	void
	release_append(node_list<T,D>& dest);
};	// end of template class node_list<>

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_NODE_LIST_H__

