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
	object(), parent(p), key(n), subns(), open_aliases() {
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
	// the list of open/using namespaces (and their aliases) will
	// be reset each time this namespace is closed.  They will have to
	// be added back the next time it is re-opened.  
	open_aliases.clear();
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adding a namespace using directive to current namespace
// idea: use ::id[::id]* as a way of specifying absolute namespace
name_space*
name_space::add_using_directive(const id_expr& n) {
	// PROBLEM: use flattened name (string) or hierarchical?
	// hierarchical: import namespace table will be hierarchical
	// instead of flat, introduce nested class

	// see if namespace has already been declared within scope of search
	// remember: the id_expr is a suffix to be appended onto root
	// find it/them, record to list

	// if list's size > 1, ambiguity
	// else if list is empty, unresolved namespace
	// else we've narrowed it down to one

	// if not already in our list of mapped serachable namespaces,
	// add it to our map of namespaces to search
	// else maybe warning... harmless

	return NULL;
}

/// adding a namespace alias directive to current namespace

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// should be similar to add_using_directive above, but using a different name
name_space*
name_space::add_using_alias(const id_expr& n, const string& a) {
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns valid pointer to a namespace if a strict match is found.  
	This variation includes the invoking namespace in the pattern match.  
	\param the qualified/scoped name of the namespace to match
 */
name_space*
name_space::query_namespace_match(const id_expr& id) {
	// recall that id_expr is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	id_expr::const_iterator i = id.begin();	assert(*i);
	token_identifier* tid = dynamic_cast<token_identifier*>(*i);
	assert(tid);
	name_space* ns = this;
	if (ns->key.compare(*tid))
		// if names differ, already failed
		return NULL;
	// remember to skip scope tokens
	for (i++; ns && i!=id.end(); i++) {
		i++;
		tid = dynamic_cast<token_identifier*>(*i);
		assert(tid);
		ns = ns->subns[*tid];
	}
	// for loop terminates when ns is NULL or i is at the end
	// if i is not at the end, then we didn't find a matched namespace
	//	because there are still scoped id's trailing, 
	//	therefore ns is NULL, which means no match.  
	// if ns is not NULL, then i must be at the end, 
	//	which means that we've matched so far.
	// In either case, we return ns.  
	return ns;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns valid pointer to a namespace if a strict match is found.  
	This variation excludes the invoking namespace in the pattern match.  
	\param the qualified/scoped name of the namespace to match
 */
name_space*
name_space::query_subnamespace_match(const id_expr& id) {
	// recall that id_expr is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	id_expr::const_iterator i = id.begin();	assert(*i);
	token_identifier* tid = dynamic_cast<token_identifier*>(*i);
	assert(tid);
	name_space* ns = subns[*tid];		// lookup map of sub-namespaces
	// remember to skip scope tokens
	for (i++; ns && i!=id.end(); i++) {
		i++;
		tid = dynamic_cast<token_identifier*>(*i);
		assert(tid);
		ns = ns->subns[*tid];
	}
	// for loop terminates when ns is NULL or i is at the end
	// if i is not at the end, then we didn't find a matched namespace
	//	because there are still scoped id's trailing, 
	//	therefore ns is NULL, which means no match.  
	// if ns is not NULL, then i must be at the end, 
	//	which means that we've matched so far.
	// In either case, we return ns.  
	return ns;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns valid pointer to a namespace if a strict match is found.  
	This variation excludes the invoking namespace in the pattern match.  
	\param the qualified/scoped name of the namespace to match
 */
name_space*
name_space::query_import_namespace_match(const id_expr& id) {
	// for now, return first match instead of a list
	name_space* ret = NULL;
	using_map_type::const_iterator i = open_aliases.begin();
	for ( ; i!=open_aliases.end(); i++) {
		ret = (*i).second->query_subnamespace_match(id);
		if (ret)
			return ret;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// finds a namespace ending with a (optionally) scoped identifier
// should we return a list of all matches?
name_space*
name_space::find_namespace_ending_with(const id_expr& id) {
	// should we return first match, or all matches?
	//	for now: first match
	//	later, we'll complain about ambiguities that need resolving
	// search order:
	// 1) in this namespace's list of sub-namespaces
	//	(must be declared already)
	// 2) in this namespace's list of open/aliased namespaces
	// 3) upward a namespace scope, which will search its 1,2
	//	including the global scope, if reached
	// terminates (returning NULL) if not found
	name_space* ret = query_subnamespace_match(id);
	if (ret)	return ret;
	ret = query_import_namespace_match(id);
	if (ret)	return ret;
	if (parent)
		return parent->find_namespace_ending_with(id);
	else	return NULL;
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

