/**
	\file "AST/node_list.tcc"
	Template-only definitions for parser classes and methods.  
	$Id: node_list.tcc,v 1.4.60.1 2007/07/09 02:40:16 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_node_list.tcc,v 1.10.34.1 2005/12/11 00:45:09 fang Exp
 */

#ifndef	__HAC_AST_NODE_LIST_TCC__
#define	__HAC_AST_NODE_LIST_TCC__

#include <ostream>
#include <algorithm>
#include <functional>

#include "AST/node_list.h"
#include "AST/node_position.h"
#include "util/what.tcc"
#include "util/stacktrace.h"
#include "util/type_traits.h"
// #include "util/binders.h"
#include "util/memory/count_ptr.tcc"

// DO NOT INCLUDE THIS FILE IN OTHER HEADER FILES
// unless you want the contained class methods to be inlined!!!
// Including in other .cc source files is acceptable.  

// Why is this file separate from "art_parser.h" and "art_parser.cc"?
// Template class methods belong in this fuzzy zone between header files 
// and source files.  Defining template methods in the .h file will
// result in all files using that header file to instantiate their own
// private instantiations of the template -- causing code bloat.  
// If the method definition in the class definition (at declaration time), 
// then it is even inlined, which is great for performance.  
// Sometimes this is desirable, but we would like to have some control
// over when this happens.  The template method definitions do not belong
// in source files (.cc) either because the template export mechanism 
// hasn't been implemented in even modern versions of gcc.  
// (Please correct me when this is no longer true.)  Without the export
// mechanism, other users will have no access to the template method 
// definitions, unless they explicitly include the .cc file, which is poor
// style.  With no other home for these definitions, we created this file
// so that other source files that depend on these template definitions
// may use them in explicitly instantiation classes.  
// This is not the place for explicit template instantiations, that belongs
// in the .cc source files.  
// As a reminder, non-template methods should be defined in the .cc file.

//=============================================================================
// debug flags
#define	DEBUG_NODE_LIST		0

//=============================================================================
namespace HAC {
namespace parser {
class context;
using util::what;
using std::transform;
// using std::bind2nd;
// using util::bind2nd_argval;
using std::mem_fun;
using std::mem_fun_ref;

//-----------------------------------------------------------------------------
// default definitions for syntax

template <class L>
const char node_list_traits<L>::open[] = "(";

template <class L>
const char node_list_traits<L>::close[] = ")";

template <class L>
const char node_list_traits<L>::delim[] = ", ";

//-----------------------------------------------------------------------------
/// default empty constructor
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T>::node_list() : nodes(), open(NULL), close(NULL) { }

/// default destructor
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T>::~node_list() { }

/// constructor initialized with first element
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T>::node_list(const T* n) : nodes(), open(NULL), close(NULL) {
	nodes.push_back(value_type(n));
}

NODE_LIST_TEMPLATE_SIGNATURE
node_list<T>::node_list(const count_ptr<const T>& n) :
		nodes(), open(NULL), close(NULL) {
	nodes.push_back(n);
}

//-----------------------------------------------------------------------------
/**
	Keeping as the old-style for legacy reasons.  
	Don't want to bother updating tons of parser expected-output
	tests yet.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
ostream&
node_list<T>::what(ostream& o) const {
	// print first item to get type
	const_iterator i = this->begin();
	o << "list<" <<
		util::what<typename util::remove_const<T>::type>::name() <<
		">: ";
	if (i == this->end()) {
//		o << "<empty> ";
	} else if (*i) { 
		// possible for list to be empty
		(*i)->what(o) << " ";
	} else {
//		o << "<nothing> ";
	}
	return o << "...";
}

//-----------------------------------------------------------------------------
/**
	Sequentially type-checks and builds list of elements in order.  
	No longer need to skip every other delimiter token because
	delimiters are now in separate list.  
	\return useless pointer, which only reflects the error status of the
		last object in the list, and thus should be disregarded.  
		The context object collects the necessary error information.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
never_ptr<const object>
node_list<T>::check_build(context& c) const {
	// note: these static consts become multiply defined with gcc4
	// must do something about this...
#if ENABLE_STACKTRACE
	static const string trace_root(util::what<T>::name());
	static const char trace_suffix[] = "_list::check_build()";
	static const string trace_str(trace_root + trace_suffix);
	STACKTRACE(trace_str);
#endif

	never_ptr<const object> ret(NULL);
	const_iterator i = begin();
	for( ; i!=end(); i++) {
		if (*i) {
			// check returned value for failure
			ret = (*i)->check_build(c);
			// context will be updated if there is an error
		}
	// What if null argument needs to be checked, say, against a list?
	// especially for optional argument placeholders, need the following:
	// c->null_check_build();
	// Alternatively, use template specialization to fix special cases.  
	// port connectivity checking is a key example...
	}
	return ret;
}

//-----------------------------------------------------------------------------
/**
	Template check method, where each element is required to be 
	non-NULL prior to checking.  
	Results are accumulated in a sequence, passed in by reference.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
template <class R, class A>
void
node_list<T>::check_list(R& r, 
		typename R::value_type (T::*f)(A&) const, A& a) const {
	INVARIANT(r.empty());
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		r.push_back(mem_fun_ref(f)(**i, a));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Another check method, but input elements are allowed to 
	be NULL, in which case, null checked values are appended
	to the result list.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
template <class R, class A>
void
node_list<T>::check_list_optional(R& r, 
		typename R::value_type (T::*f)(A&) const, A& a) const {
	INVARIANT(r.empty());
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++) {
		r.push_back((*i) ? mem_fun_ref(f)(**i, a) :
			typename R::value_type());
	}
}

//-----------------------------------------------------------------------------
/**
	OBSOLETE: because lists contain count_ptrs, no need to release.

	Releases memory owned by the list and copies over to the destination
	list.  
	Ownership of element pointers must be transferrable, hence
	excl_ptr<const T> as opposed to const excl_ptr<const T>.  
	Releasing memory also nullifies pointers, so the list will be
	unusable after this operation.  
	\param dest the destination list.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
void
node_list<T>::release_append(this_type& dest) {
	iterator i = this->begin();
	for ( ; i!=this->end(); i++) {
		// will release each element
		dest.nodes.push_back(*i);	// will this actually release?
		INVARIANT(!*i);
	}
}

//-----------------------------------------------------------------------------
/**
	Returns position of left-most token in list, 
	and even checks delimiter token list in parallel if necessary.  
	\return the position of left-most token in list.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
line_position
node_list<T>::leftmost(void) const {
	const_iterator i = nodes.begin();
	if (open)
		return open->position;
	for( ; i!=nodes.end(); i++) {
		if (*i)
			return (*i)->leftmost();
	}
	return line_position();			// hopefully won't happen
}

//-----------------------------------------------------------------------------
/**
	Returns position of right-most token in list, 
	and even checks delimiter token list in parallel if necessary.  
	\return the position of right-most token in list.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
line_position
node_list<T>::rightmost(void) const {
	const_reverse_iterator i = nodes.rbegin();
	if (close)
		return close->position;
	for( ; i!=nodes.rend(); i++) {
		if (*i)
			return (*i)->rightmost();
	}
	return line_position();			// hopefully won't happen
}

//=============================================================================
// class list_checked method definitions

#if 0
/**
 */
template <class R, class L, class A>
void
list_checker<R,L,A>::operator () (R& r, const L& l) {
	transform(l.begin(), l.end(), back_inserter(r), 
		bind2nd(mem_fun(this->mem_func), this->bound_arg)
	);
}
#endif

//=============================================================================
}	// end namespace parser
}	// end namespace HAC


#endif	// __HAC_AST_NODE_LIST_TCC__

