// "art_object.cc"

#include "art_object.h"

namespace ART {
namespace entity {
//=============================================================================
// class name_space methods

/**
	Constructor for a new namespace.  
	\param n the name.  
	\param p pointer to the parent namespace.  
 */
name_space::name_space(const string& n, name_space* p) : 
	object(), parent(p), key(n), subns() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destructor.
 */
name_space::~name_space() {

	// iterate through and delete subspaces
	subns.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// either creates a new sub-namespace or opens if it already exists
// effectively context shifts
name_space*
name_space::add_open_namespace(const string& n) {
	// see if namespace already exists
	//	(searching up and in open namespaces, as well?)
	//	check for potential confusion and ambiguity?
	name_space* ret = subns[n];
	// if sub-namespace already exists, return reference to it
	if (ret) {
		cerr << " ... already exists";
		return ret;
	}
	// else create it, linking this as its parent
	cerr << " ... creating new";
	ret = new name_space(n,this);
	assert(ret);
	subns[n] = ret;		// store it in the map of sub-namespaces
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

name_space*
name_space::leave_namespace(void) {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adding a namespace using directive to current namespace
// idea: use ::id[::id]* as a way of specifying absolute namespace
void
name_space::add_using_directive(const id_expr& n) {
	// see if namespace has already been declared within scope of search
	// remember: the id_expr is a suffix to be appended onto root
	// find it/them, record to list

	// if list's size > 1, ambiguity
	// else if list is empty, unresolved namespace
	// else we've narrowed it down to one

	// if not already in our list of mapped serachable namespaces,
	// add it to our map of namespaces to search
	// else maybe warning... harmless
}

/// adding a namespace alias directive to current namespace

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
name_space::add_using_alias(const id_expr& n, const string& a) {
	// should be similar to add_using_directive above
}

//=============================================================================
// class user_type_def methods

/// constructor for user defined type
user_type_def::user_type_def(const string& name) :
	type_definition(), key(name), members() {
}

//=============================================================================
};
};

