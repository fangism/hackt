/**
	\file "art_parser_node_list.tcc"
	Template-only definitions for parser classes and methods.  
	Rename this to "art_parser_node_list.tcc"!
	$Id: art_parser_node_list.tcc,v 1.4 2005/03/06 22:45:50 fang Exp $
 */

#ifndef	__ART_PARSER_NODE_LIST_TCC__
#define	__ART_PARSER_NODE_LIST_TCC__

#include <ostream>

#include "STL/list.tcc"

// #include "art_parser_debug.h"
#include "art_parser_node_list.h"	// includes "ptrs.h", "count_ptr.h"
#include "art_parser_node_position.h"
#include "art_context.h"
	// for class context, uses auto_indent()

#include "what.tcc"
#include "stacktrace.h"

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
namespace ART {
namespace parser {
using util::what;
USING_STACKTRACE

/// default empty constructor
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T>::node_list() : nodes(), open(NULL), close(NULL) {
}

/// constructor initialized with first element
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T>::node_list(const T* n) : nodes(), open(NULL), close(NULL) {
	push_back(value_type(n));
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
	o << "(node_list): ";
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
NODE_LIST_TEMPLATE_SIGNATURE
void
node_list<T>::push_back(const T* p) {
	nodes.push_back(value_type(p));
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SIGNATURE
void
node_list<T>::wrap(const node_position* b, const node_position* e) {
	open = excl_ptr<const node_position>(b);
	close = excl_ptr<const node_position>(e);
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
#if 0
/**
	Releases memory owned by the list and copies over to the destination
	list.  
	Ownership of element pointers must be transferrable, hence
	excl_ptr<const T> as opposed to const excl_ptr<const T>.  
	Releasing memory also nullifies pointers, so the list will be
	unusable after this operation.  
	\param dest the destination list.  
 */
NODE_LIST_BASE_TEMPLATE_SIGNATURE
void
node_list_base<T>::release_append(node_list_base<T>& dest) {
	iterator i = this->begin();
	for ( ; i!=this->end(); i++) {
		// will release each element
		dest.push_back(*i);		// will this actually release?
//		push_back(excl_ptr<const T>(*i));	// implicit
//		push_back(some_ptr<const T>(excl_ptr<const T>(*i)));
	}
}
#endif

//=============================================================================
// for class node_list<>

//-----------------------------------------------------------------------------
#if 0
/**
	List copy constructor.  Uses the parent's copy constructor, 
	which does NOT transfer ownership of the pointer elements.  
	Does not copy the open and close delimiters, which aren't applicable.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T,D>::node_list(const node_list<T,D>& l) : 
		node(), node_list_base<T>(l),
		open(NULL), close(NULL), delim(l.delim) {
	// delim list will also copy without transfer of ownership
#if DEBUG_NODE_LIST
	cerr << "node_list<T,D>::node_list(const node_list<T,D>&);" << endl;
	cerr << "\targument's size() = " << l.size() << endl;
	cerr << "\tthis size() = " << this->size() << endl; 
#endif
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T,D>::~node_list() {
//	SAFEDELETE(open); SAFEDELETE(close);
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T,D>*
node_list<T,D>::wrap(const terminal* b, const terminal* e) {
// don't care what they are
	open = excl_ptr<const terminal>(b);
//	if (b) assert(open.is_a<token_char>() || open.is_a<token_string>());
	close = excl_ptr<const terminal>(e);
//	if (e) assert(close.is_a<token_char>() || close.is_a<token_string>());
	return this;
}
#endif

//-----------------------------------------------------------------------------
#if 0
NODE_LIST_TEMPLATE_SIGNATURE
node_list<T,D>*
node_list<T,D>::append(const terminal* d, const T* n) {
	if (d) {
		// check for delimiter character match
		// will fail if incorrect token is passed
		assert(!(d->string_compare(D)));
		// now use separate list for delimiters
#if 0
		excl_ptr<const terminal> xd(d);
		some_ptr<const terminal> sd(xd);
		assert(!xd && sd.owned());	// guarantee transfer
		delim.push_back(sd);
		assert(!sd.owned());
		// explicit conversion
#endif
		delim.push_back(count_ptr<const terminal>(d));
	} else {
		// consider using template specialization for this
		// for effective conditional compilation
		assert(D == none);	// no delimiter was expected
	}
	// push_back(const T&), but excl_ptr<const T> is destructive :/
	// either use different pointer class or introduce 
	// list sub-class, with new push_back operation.  
	// n may be null, is ok
#if 0
	excl_ptr<const T> xn(n);
	some_ptr<const T> sn(xn);
	assert(!xn  && sn.owned());		// guarantee transfer
	push_back(sn);
	assert(!sn.owned());
//	push_back(excl_ptr<const T>(n));	// if implicit constructor allowed
#endif
	push_back(count_ptr<const T>(n));

	return this;
}
#endif

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

//-----------------------------------------------------------------------------
#if 0
/**
	Releases memory owned by the list and copies over to the destination
	list.  
	\param dest the node-list to which items are transferred.  
 */
NODE_LIST_TEMPLATE_SIGNATURE
void
node_list<T,D>::release_append(node_list<T,D>& dest) {
	parent::release_append(dest);
	delim_list::iterator i = delim.begin();
	for ( ; i!=delim.end(); i++) {
		dest.delim.push_back(*i);
	}
}
#endif

//=============================================================================
}	// end namespace parser
}	// end namespace ART


#endif	// __ART_PARSER_NODE_LIST_TCC__

