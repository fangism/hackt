// "art_object.cc"

#include "art_parser.h"
#include "art_object.h"
#include "list_of_ptr_template_methods.h"

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
		object(), parent(p), key(n), subns(), 
		open_spaces(), open_aliases() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The only only memory we need to delete is that owned by this namespace, 	namely, the subns subnamespace map.  We created, thus we delete.  
	All other pointers are shared pointers, and will be deleted by
	their respective owners.  
 */
name_space::~name_space() {
	ns_map_type::iterator i;

	open_spaces.clear();		// without deleting, don't own
	open_aliases.clear();		// without deleting, don't own

	// iterate through and delete subnamespaces, this object owns them
	for (i=subns.begin(); i!=subns.end(); i++) {
		SAFEDELETE((*i).second);
	}
	subns.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// Returns the flattened name of this current namespace
string
name_space::get_qualified_name(void) const {
	if (parent) {
		if (parent->parent)
			return parent->get_qualified_name() +scope +key;
		else	return key;
	} else return "<global>";		// global
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// either creates a new sub-namespace or opens if it already exists
name_space*
name_space::add_open_namespace(const string& n) {
	// see if namespace already exists
	//	note: name clashes are permitted with namespaces in higher
	//	scopes, and in open (unaliased) spaces, however, 
	//	name conflict with existing subspaces and ALIASED
	//	namespaces (alias names) will be reported as errors, 
	//	to head off certain ambiguity.  

	name_space* ret = open_aliases[n];
	if (ret) {
		cerr << " ... already an open alias, ERROR! ";
		return NULL;
	}
	ret = subns[n];
	// if sub-namespace already exists, return reference to it
	if (ret) {
		cerr << " ... already exists as subspace";
	} else {	// else create it, linking this as its parent
		cerr << " ... creating new";
		ret = new name_space(n, this);
		subns[n] = ret;		// store it in map of sub-namespaces
	}
	assert(ret);
	assert(ret->parent == this);
	assert(ret->key == n);
	cerr << " with parent: " << ret->parent->key;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
name_space*
name_space::leave_namespace(void) {
	// the list of open/using namespaces (and their aliases) will
	// be reset each time this namespace is closed.  They will have to
	// be added back the next time it is re-opened.  
	open_spaces.clear();
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// adding a namespace using directive to current namespace
// idea: use ::id[::id]* as a way of specifying absolute namespace
name_space*
name_space::add_using_directive(const id_expr& n) {
	name_space* ret;
	namespace_list::iterator i;
	namespace_list candidates;		// empty list

	cerr << endl << "adding using-directive in space: " 
		<< get_qualified_name();
	// see if namespace has already been declared within scope of search
	// remember: the id_expr is a suffix to be appended onto root
	// find it/them, record to list
	query_import_namespace_match(candidates, n);
	i = candidates.begin();

	switch (candidates.size()) {
	// if list's size > 1, ambiguity
	// else if list is empty, unresolved namespace
	// else we've narrowed it down to one
		case 1: ret = (*i); open_spaces.push_back(ret); break;
		case 0:	{
			cerr << " ... not found, ERROR! ";
			ret = NULL;
			break;	// no matches
			}
		default: {	// > 1
			ret = NULL;
			cerr << " ERROR: ambiguous import of namespaces, " << 
				"need to be more specific.  candidates are: ";
				for ( ; i!=candidates.end(); i++)
					cerr << endl << "\t" << 
						(*i)->get_qualified_name();
			}
	}

	// if not already in our list of mapped serachable namespaces,
	// add it to our map of namespaces to search
	// else maybe warning... harmless

	return ret;		// NULL => error
}

/// adding a namespace alias directive to current namespace

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// should be similar to add_using_directive above, but using a different name
name_space*
name_space::add_using_alias(const id_expr& n, const string& a) {
	name_space* ret;
	namespace_list::iterator i;
	namespace_list candidates;		// empty list

	// see if namespace already exists
	//	note: name clashes are permitted with namespaces in higher
	//	scopes, and in open (unaliased) spaces, however, 
	//	name conflict with existing subspaces and ALIASED
	//	namespaces (alias names) will be reported as errors, 
	//	to head off certain ambiguity.  

	cerr << endl << "adding using-alias in space: " 
		<< get_qualified_name() << " as " << a;

	ret = subns[a];
	if (ret) {
		cerr << " ... already as open alias, ERROR! ";
		return NULL;
	}
	ret = open_aliases[a];
	if (ret) {
		cerr << " ... already exists as subspace, ERROR! ";
		return NULL;
	}
	// else we're ok to proceed to add alias
	// first find the referenced namespace...
	query_import_namespace_match(candidates, n);
	i = candidates.begin();

	switch (candidates.size()) {
	// if list's size > 1, ambiguity
	// else if list is empty, unresolved namespace
	// else we've narrowed it down to one
		case 1: ret = (*i); open_aliases[a] = ret; break;
		case 0:	{
			cerr << " ... not found, ERROR! ";
			ret = NULL;
			break;	// no matches
			}
		default: {	// > 1
			ret = NULL;
			cerr << " ERROR: ambiguous import of namespaces, " << 
				"need to be more specific.  candidates are: ";
				for ( ; i!=candidates.end(); i++)
					cerr << endl << "\t" << 
						(*i)->get_qualified_name();
			}
	}

	return ret;		// NULL => error
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns valid pointer to a namespace if a strict match is found.  
	(at most one precise match)
	This will serach both true subnamespaces and aliased subspaces.  
	This variation includes the invoking namespace in the pattern match.  
	\param id the qualified/scoped name of the namespace to match
 */
name_space*
name_space::query_namespace_match(const id_expr& id) {
	// recall that id_expr is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	cerr << "query_namespace_match: " << id 
		<< " in " << get_qualified_name() << endl;

	id_expr::const_iterator i = id.begin();	assert(*i);
	token_identifier* tid = dynamic_cast<token_identifier*>(*i);
	assert(tid);
	cerr << "\ttesting: " << *tid;
	name_space* ns = this;
	if (ns->key.compare(*tid)) {
		// if names differ, already failed, try alias spaces
		return NULL;
	} else {
		for (i++; ns && i!=id.end(); i++) {
			name_space* next;
			i++;	// remember to skip scope tokens
			tid = dynamic_cast<token_identifier*>(*i); assert(tid);
			cerr << scope << *tid;
			next = ns->subns[*tid];
			// if not found in subspaces, check aliases list
			// or should we not search aliases?
			ns = (next) ? next : ns->open_aliases[*tid];
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns valid pointer to a namespace if a strict match is found.  
	(at most one precise match)
	This will treat open aliased namespaces as valid subspaces for search.
	This variation excludes the invoking namespace in the pattern match.  
	\param the qualified/scoped name of the namespace to match
 */
name_space*
name_space::query_subnamespace_match(const id_expr& id) {
	// recall that id_expr is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	cerr << endl << "query_subnamespace_match: " << id 
		<< " in " << get_qualified_name() << endl;
	id_expr::const_iterator i = id.begin();	assert(*i);
	token_identifier* tid = dynamic_cast<token_identifier*>(*i);
	assert(tid);
	cerr << "\ttesting: " << *tid;
	name_space* ns = subns[*tid];		// lookup map of sub-namespaces
	if (!ns) ns = open_aliases[*tid];	// else lookup in aliases
	// remember to skip scope tokens
	for (i++; ns && i!=id.end(); i++) {
		name_space* next;
		i++;
		tid = dynamic_cast<token_identifier*>(*i); assert(tid);
		cerr << scope << *tid;
		next = ns->subns[*tid];
		// if not found in subspaces, check aliases list
		ns = (next) ? next : ns->open_aliases[*tid];
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
	(allows multiple matches, hence the use of a list reference)
	This variation checks whether the head of the identifier is the name
	of this scope, and if so, searches with the beheaded id.  
	This will catch: open foo::bar...
	\param m the accumulating list of matches
	\param id the qualified/scoped name of the namespace to match
 */
void
name_space::
query_import_namespace_match(namespace_list& m, const id_expr& id) {
	cerr << endl << "query_import_namespace_match: " << id 
		<< " in " << get_qualified_name();
	{
		name_space* ret = query_subnamespace_match(id);
		if (ret) m.push_back(ret);
	}
	{	// with open namespaces list
		namespace_list::const_iterator i = open_spaces.begin();
		for ( ; i!=open_spaces.end(); i++) {
			name_space* ret = (*i)->query_subnamespace_match(id);
			if (ret) m.push_back(ret);
		}
	}
	// When searching for imported namespaces matches found
	// in the deepest scopes will override those found in higher
	// scope without ambiguity... for now
	// (can be easily changed)
	// Only if the match list is empty, ask the parent to do the 
	// same query.  
#if 1
	if (m.empty() && parent)
#endif
		parent->query_import_namespace_match(m, id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	finds a namespace ending with a (optionally) scoped identifier
	(allows multiple matches, hence the use of a list reference)
 */
void
name_space::
find_namespace_ending_with(namespace_list& m, const id_expr& id) {
	// should we return first match, or all matches?
	//	for now: first match
	//	later, we'll complain about ambiguities that need resolving
	// search order:
	// 1) in this namespace's list of sub-namespaces
	//	(must be declared already)
	// 2) in this namespace's list of open/aliased namespaces
	//	*without* following its imports
	// 3) upward a namespace scope, which will search its 1,2
	//	including the global scope, if reached
	// terminates (returning NULL) if not found
	name_space* ret = query_subnamespace_match(id);
	if (ret)	m.push_back(ret);
	query_import_namespace_match(m, id);
	if (parent)
		parent->find_namespace_ending_with(m, id);
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

