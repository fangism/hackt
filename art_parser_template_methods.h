// "art_parser_template_methods.h"

#ifndef	__ART_PARSER_TEMPLATE_METHODS_H__
#define	__ART_PARSER_TEMPLATE_METHODS_H__

#include "art_parser.h"
#include "list_of_ptr_template_methods.h"

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

namespace ART {
namespace parser {

// TEMPLATE METHOD DEFINITIONS
//=============================================================================
// for class node_list_base<>

/// base constructor, initialized with one element
NODE_LIST_BASE_TEMPLATE_SPEC
node_list_base<T>::node_list_base(node* n) : node_list_base() {
	if(n && !dynamic_cast<T*>(n)) {
		// throw type exception
		n->what(cerr << "unexpected type: ") << endl;
		exit(1);
	}
	push_back(n);
}

//-----------------------------------------------------------------------------
NODE_LIST_BASE_TEMPLATE_SPEC
ostream&
node_list_base<T>::what(ostream& o) const {
	return o << "(node_list_base)";
}

//=============================================================================
// for class node_list<>

NODE_LIST_TEMPLATE_SPEC
node_list<T,D>::node_list(node* n) :
		node_list_base<T>(), open(NULL), close(NULL) {
	if(n && !dynamic_cast<T*>(n)) {
		// throw type exception
		n->what(cerr << "unexpected type: ") << endl;
		exit(1);
	}
	push_back(n);
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>::~node_list() {
	SAFEDELETE(open); SAFEDELETE(close);
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>*
node_list<T,D>::wrap(node* b, node* e) {
	open = dynamic_cast<terminal*>(b);
	assert(open);
	assert(dynamic_cast<token_char*>(open) ||
		dynamic_cast<token_string*>(open));
	close = dynamic_cast<terminal*>(e);
	assert(close);
	assert(dynamic_cast<token_char*>(close) ||
		dynamic_cast<token_string*>(close));
	return this;
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
node_list<T,D>*
node_list<T,D>::append(node* d, node* n) {
	if (d) {
		// check for delimiter character match
		terminal* t = dynamic_cast<terminal*>(d);
		assert(t);		// throw exception
		// will fail if incorrect type is passed
		assert(!(t->string_compare(D)));
		push_back(d);
	} else {
		assert(D == none);	// no delimiter was expected
	}
	assert(dynamic_cast<T*>(n));	// type-check
	push_back(n);			// n may be null
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
NODE_LIST_TEMPLATE_SPEC
line_position
node_list<T,D>::leftmost(void) const {
	const_iterator i = begin();
	if (open)
		return open->leftmost();
	for( ; i!=end(); i++) {
		if (*i) return (*i)->leftmost();
	}
	return line_position();			// hopefully won't happen
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
line_position
node_list<T,D>::rightmost(void) const {
	const_reverse_iterator i = rbegin();
	if (close)
		return close->rightmost();
	for( ; i!=rend(); i++) {
		if (*i) return (*i)->rightmost();
	}
	return line_position();			// hopefully won't happen
}

//-----------------------------------------------------------------------------
NODE_LIST_TEMPLATE_SPEC
object*
node_list<T,D>::check_build(context* c) const {
	object* o = NULL;
	const_iterator i = begin();
	if (*i) {
		(*i)->what(cerr << c->auto_indent() << "checking a ");
		// check returned value for failure
		o = (*i)->check_build(c);
		// update context c with o, should be done inside
	}
	for(i++; i!=end(); i++) {
		// remember to skip every other token if there was one!
		if (D != none)
			i++;
		if (*i) {
			(*i)->what(cerr << c->auto_indent() << "checking a ");
			// check returned value for failure
			o = (*i)->check_build(c);
			// update context c with o, should be done inside
		}
	}
	return NULL;
}

//=============================================================================
};	// end namespace parser
};	// end namespace ART


#endif	// __ART_PARSER_TEMPLATE_METHODS_H__

