// "art_parser_template_methods.h"

#ifndef	__ART_PARSER_TEMPLATE_METHODS_H__
#define	__ART_PARSER_TEMPLATE_METHODS_H__

#include <iostream>

#include "art_parser_debug.h"
#include "art_macros.h"
#include "art_parser.h"
#include "list_of_ptr_template_methods.h"
#include "art_symbol_table.h"
	// for class context

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
#define	DEBUG_NODE_LIST_BASE	1 && DEBUG_NODE_LIST

//=============================================================================
namespace ART {
namespace parser {

//=============================================================================
// TEMPLATE METHOD DEFINITIONS
//=============================================================================
// for class node_list_base<>

/// empty constructor
NODE_LIST_BASE_TEMPLATE_SPEC
node_list_base<T>::node_list_base() : node(), list_parent() {
}

/// base constructor, initialized with one element
NODE_LIST_BASE_TEMPLATE_SPEC
node_list_base<T>::node_list_base(T* n) : node_list_base() {
/***
	// not needed, now that type is matched and checked
	if(n && !IS_A(T*, n)) {
		// throw type exception
		n->what(cerr << "unexpected type: ") << endl;
		exit(1);
	}
***/
	push_back(n);
}

NODE_LIST_BASE_TEMPLATE_SPEC
node_list_base<T>::~node_list_base() {
// is virtual, will invoke list_of_ptr<> destructor
}

//-----------------------------------------------------------------------------
/// copy constructor, no transfer of ownership
NODE_LIST_BASE_TEMPLATE_SPEC
node_list_base<T>::node_list_base(const node_list_base<T>& l) : 
		list_parent(l) {
#if DEBUG_NODE_LIST_BASE
	cerr << "node_list_base<T>::node_list_base(const node_list_base<T>&);" << endl;
	cerr << "\targument's size() = " << l.size() << endl;
	cerr << "\tthis size() = " << this->size() << endl; 
#endif
}

//=============================================================================
// for class node_list<>

/// default empty constructor
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>::node_list() : parent(), open(NULL), close(NULL), delim() {
}

/// constructor initialized with first element
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>::node_list(T* n) :
		node_list_base<T>(), open(NULL), close(NULL), delim() {
/***
	// not needed, now that type is matched and checked
	if(n && !IS_A(T*, n)) {
		// throw type exception
		n->what(cerr << "unexpected type: ") << endl;
		exit(1);
	}
***/
	push_back(n);
}

//-----------------------------------------------------------------------------
/**
	List copy constructor.  Uses the parent's copy constructor, 
	which does NOT transfer ownership of the pointer elements.  
	Does not copy the open and close delimiters, which aren't applicable.  
 */
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>::node_list(const node_list<T,D>& l) : 
		node_list_base<T>(l), open(NULL), close(NULL), delim(l.delim) {
	// delim list will also copy without transfer of ownership
#if DEBUG_NODE_LIST
	cerr << "node_list<T,D>::node_list(const node_list<T,D>&);" << endl;
	cerr << "\targument's size() = " << l.size() << endl;
	cerr << "\tthis size() = " << this->size() << endl; 
#endif
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>::~node_list() {
	SAFEDELETE(open); SAFEDELETE(close);
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>*
node_list<T,D>::wrap(terminal* b, terminal* e) {
//	open = IS_A(terminal*, b);	assert(open);
	open = b;
	assert(IS_A(token_char*, open) ||
		IS_A(token_string*, open));
//	close = IS_A(terminal*, e);	assert(close);
	close = e;
	assert(IS_A(token_char*, close) ||
		IS_A(token_string*, close));
	return this;
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>*
node_list<T,D>::append(terminal* d, T* n) {
	if (d) {
		// check for delimiter character match
		// will fail if incorrect token is passed
		assert(!(d->string_compare(D)));
		// now use separate list for delimiters
		delim.push_back(d);
	} else {
		// consider using template specialization for this
		// for effective conditional compilation
		assert(D == none);	// no delimiter was expected
	}
//	assert(IS_A(T*, n));		// type-check, now redundant
	push_back(n);			// n may be null, is ok
	return this;
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
ostream&
node_list<T,D>::what(ostream& o) const {
	// print first item to get type
	const_iterator i = begin();
	o << "(node_list): ";
	if (*i) (*i)->what(o) << " ";
	return o << "...";
}

//-----------------------------------------------------------------------------
/**
	Returns position of left-most token in list, 
	and even checks delimiter token list in parallel if necessary.  
	\return the position of left-most token in list.  
 */
NODE_LIST_TEMPLATE_SPEC
line_position
node_list<T,D>::leftmost(void) const {
	const_iterator i = begin();
	delim_list::const_iterator j = delim.begin();
	if (open)
		return open->leftmost();
	for( ; i!=end(); i++) {
		if (*i) return (*i)->leftmost();
		else if (j != delim.end()) {
			if (*j) return (*j)->leftmost();
			else j++;
		}
	}
	return line_position();			// hopefully won't happen
}

//-----------------------------------------------------------------------------
/**
	Returns position of right-most token in list, 
	and even checks delimiter token list in parallel if necessary.  
	\return the position of right-most token in list.  
 */
NODE_LIST_TEMPLATE_SPEC
line_position
node_list<T,D>::rightmost(void) const {
	const_reverse_iterator i = rbegin();
	delim_list::const_reverse_iterator j = delim.rbegin();
	if (close)
		return close->rightmost();
	for( ; i!=rend(); i++) {
		if (*i) return (*i)->rightmost();
		else if (j != delim.rend()) {
			if (*j) return (*j)->rightmost();
			else j++;
		}
	}
	return line_position();			// hopefully won't happen
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
NODE_LIST_TEMPLATE_SPEC
const object*
node_list<T,D>::check_build(context* c) const {
	const object* ret = NULL;
	const_iterator i = begin();
	for( ; i!=end(); i++) {
	if (*i) {
		DEBUG(TRACE_CHECK_BUILD, 
			(*i)->what(cerr << c->auto_indent() << "checking a "))
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

//=============================================================================
};	// end namespace parser
};	// end namespace ART


#endif	// __ART_PARSER_TEMPLATE_METHODS_H__

