/**
	\file "AST/node_list.h"
	Base set of classes for the HAC parser.  
	$Id: node_list.h,v 1.4 2006/02/10 21:50:35 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_node_list.h,v 1.10.12.1 2005/12/11 00:45:08 fang Exp
 */

#ifndef __HAC_AST_NODE_LIST_H__
#define __HAC_AST_NODE_LIST_H__

#include <list>
#include "AST/AST_fwd.h"
#include "AST/common.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace parser {

using std::list;
using util::memory::excl_ptr;
using util::memory::count_ptr;

//=============================================================================
// replacement node list

class node_position;

/**
	Characteristics of syntax details and appearance of 
	node_list template classes.  
	This is just the default definition.
	Each list type may specialize with its own delimiters.  
 */
template <class L>
class node_list_traits {
	static const char open[];
	static const char close[];
	static const char delim[];
	// other useful traits?
};	// end class node_list_traits

#define	NODE_LIST_TEMPLATE_SIGNATURE					\
template <class T>

/**
	To make this print the desired delimiter, 
	we will define an overrideable template that depends on this, 
	OR rely on inferring the delimiter from the value_type parameter.  
	Delimiter is only useful for spitting out source.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
class node_list {
	typedef	node_list<T>				this_type;
protected:
	// consider using a vector
	typedef	list<count_ptr<T> >			list_type;
public:
	typedef	typename list_type::value_type		value_type;
	typedef	typename list_type::reference		reference;
	typedef	typename list_type::const_reference	const_reference;
	typedef	typename list_type::iterator		iterator;
	typedef	typename list_type::const_iterator	const_iterator;
	typedef	typename list_type::reverse_iterator	reverse_iterator;
	typedef	typename list_type::const_reverse_iterator
							const_reverse_iterator;
protected:
	list_type					nodes;
	// consider count_ptr to make copy-constructible
	count_ptr<const node_position>			open;
	count_ptr<const node_position>			close;
public:
	node_list();

	explicit
	node_list(const T*);

	~node_list();

	const list_type&
	raw_list(void) const { return nodes; }

	iterator
	begin(void) { return nodes.begin(); }

	const_iterator
	begin(void) const { return nodes.begin(); }

	iterator
	end(void) { return nodes.end(); }

	const_iterator
	end(void) const { return nodes.end(); }

	bool
	empty(void) const { return nodes.empty(); }

	size_t
	size(void) const { return nodes.size(); }

#if 0
	this_type*
	append(const T*);		// basically push_back
#endif

	reference
	front(void) { return nodes.front(); }

	const_reference
	front(void) const { return nodes.front(); }

	reference
	back(void) { return nodes.back(); }

	const_reference
	back(void) const { return nodes.back(); }

	void
	push_back(const T* p) { nodes.push_back(value_type(p)); }

	void
	push_front(const T* p) { nodes.push_front(value_type(p)); }

	void
	pop_back(void) { nodes.pop_back(); }

	void
	pop_front(void) { nodes.pop_front(); }

	void
	wrap(const node_position* o, const node_position* c) {
		open = count_ptr<const node_position>(o);
		close = count_ptr<const node_position>(c);
	}

	void
	release_append(this_type&);

	ostream&
	what(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	/**
		PHASING OUT.
		Consider giving this an aggregate return type.  
		Note that this is NON-VIRTUAL.  
		Determine the return type from the template paraameter.
	 */
	never_ptr<const object>
	check_build(context&) const;

	template <class R, class A>
	void
	check_list(R&, typename R::value_type (T::*)(A&) const, A&) const;

	template <class R, class A>
	void
	check_list_optional(
		R&, typename R::value_type (T::*)(A&) const, A&) const;

#if 0
	template <class R, class A>
	good_bool
	check_list_while_good(R&, 
		typename R::value_type (T::*)(A&) const, A&) const;

	template <class R, class A>
	good_bool
	check_list_optional_while_good(
		R&, typename R::value_type (T::*)(A&) const, A&) const;
#endif

};	// end class node_list

//=============================================================================
// non-member functions

#if 0
/**
	Dedicated functor for checking a whole list of elements.  
	\param L the sequence type to check.
	\param R the sequence type to return.  
	\param A is the type passed to 
 */
template <class R, class L, class A>
class list_checker {
	typedef	R					return_list_type;
	typedef	L					base_list_type;
	typedef	typename R::value_type			return_type;
	/**
		Assumes that is a list of pointers.  
	 */
	typedef	typename L::value_type			base_ptr_type;
	/**
		Could use pointer_traits pointee type.  
	 */
	typedef	typename base_ptr_type::element_type	base_type;
	/**
		Mostly likely some parse context object.  
	 */
	typedef	A					arg_type;
	/**
		Checking member function type.  
	 */
	typedef	return_type (base_type::*mem_func_type)(arg_type) const;
private:
	/**
		mem_func is the member funct used to check 
		and accumulate results.
	 */
	mem_func_type					mem_func;
	/**
		Bound context object.  
		Assumes that argument is passed alike to each invocation
		of the leemnets' check routine.  
		Should be a reference type.  
	 */
	arg_type					bound_arg;
public:
	list_checker(mem_func_type m, arg_type a) :
		mem_func(m), arg_type(a) { }

	void
	operator () (R&, const L&);

};	// end class list_checker
#endif

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_NODE_LIST_H__

