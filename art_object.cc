// "art_object.cc"

#include <iostream>
#include "art_parser_debug.h"
#include "art_parser.h"
#include "art_symbol_table.h"
#include "map_of_ptr_template_methods.h"
#include "hash_map_of_ptr_template_methods.h"

// CAUTION on ordering of the following two include files!
// including "art_object.h" first will cause compiler to complain
// about redefinition of struct hash<> template upon specialization of
// hash<string>.  

#include "hash_specializations.h"		// substitute for the following
#include "hashlist_template_methods.h"
	// includes "list_of_ptr_template_methods.h"
#include "art_object.h"

#include "art_object_expr.h"

//=============================================================================
// DEBUG OPTIONS -- compare to MASTER_DEBUG_LEVEL from "art_debug.h"

#define		DEBUG_NAMESPACE			1 && DEBUG_CHECK_BUILD
#define		TRACE_QUERY			0	// bool, ok to change

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if		DEBUG_NAMESPACE
// ok to change these values, but should be > TRACE_CHECK_BUILD (5)
  #define	TRACE_NAMESPACE_NEW		8
  #define	TRACE_NAMESPACE_USING		8
  #define	TRACE_NAMESPACE_ALIAS		8
  #if		TRACE_QUERY
    #define	TRACE_NAMESPACE_QUERY		10
    #define	TRACE_NAMESPACE_SEARCH		TRACE_NAMESPACE_QUERY+5
  #else
    #define	TRACE_NAMESPACE_QUERY		MASTER_DEBUG_LEVEL
    #define	TRACE_NAMESPACE_SEARCH		MASTER_DEBUG_LEVEL
  #endif
#else
// defining as >= MASTER_DEBUG_LEVEL will turn it off
  #define	TRACE_NAMESPACE_QUERY		MASTER_DEBUG_LEVEL
  #define	TRACE_NAMESPACE_SEARCH		MASTER_DEBUG_LEVEL
  #define	TRACE_NAMESPACE_USING		MASTER_DEBUG_LEVEL
  #define	TRACE_NAMESPACE_ALIAS		MASTER_DEBUG_LEVEL
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if		DEBUG_DATATYPE && TRACE_QUERY
// ok to change these values
  #define	TRACE_DATATYPE_QUERY		10
  #define	TRACE_DATATYPE_SEARCH		TRACE_DATATYPE_QUERY+5
#else
// defining as >= MASTER_DEBUG_LEVEL will turn it off
  #define	TRACE_DATATYPE_QUERY		MASTER_DEBUG_LEVEL
  #define	TRACE_DATATYPE_SEARCH		MASTER_DEBUG_LEVEL
#endif


//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// non-member function prototypes

//=============================================================================
// class scopespace method definitions
scopespace::scopespace(const string& n, const scopespace* p) : 
		object(), parent(p), key(n), 
		used_id_map() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
scopespace::~scopespace() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for unqualified identifier.  
 */
const object*
scopespace::lookup_object_here(const token_identifier& id) const {
	return used_id_map[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for unqualified identifier.  
 */
const object*
scopespace::lookup_object(const token_identifier& id) const {
	const object* o = used_id_map[id];
	if (o) return o;
	else if (parent) return parent->lookup_object(id);
	else return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for a qualified identifier.
	If id is only a single identifier, and it is not absolute, 
	then it is considered an unqualified identifier.  
 */
const object*
scopespace::lookup_object(const qualified_id& id) const {
if (id.is_absolute()) {
	if (parent)
		return parent->lookup_object(id);
	else {	// we are the ROOT, start looking down namespaces
		const name_space* ns = IS_A(const name_space*, 
			lookup_namespace(id.copy_namespace_portion()));
		if (ns)
			return ns->lookup_object(**id.rend());
		else return NULL;
	}
} else if (id.size() <= 1) {
	return lookup_object(**id.begin());
} else {
	// else need to resolve namespace portion first
	const name_space* ns = IS_A(const name_space*, 
		lookup_namespace(id.copy_namespace_portion()));
	if (ns)
		return ns->lookup_object(**id.rend());
	else return NULL;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The search for a namespace should always start at the 
	outermost namespace given the current context.  
	Since the outermost context can be something else, such as a loop, 
	we default to parent's lookup namespace if this is not a namespace. 
	The name_space::lookup_namespace will override this.  
	\param id is the entire name of the namespace.
 */

const scopespace*
scopespace::lookup_namespace(const qualified_id& id) const {
	return parent->lookup_namespace(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks in used_id_map for a registered instance name.  
	\param id name of instance to lookup, an unqualified name.  
	\return pointer to instance with matching name.  
 */
const instantiation_base*
scopespace::lookup_instance(const token_identifier& id) const {
	return IS_A(const instantiation_base*, used_id_map[id]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** NOT READY TO BE UNLEASHED YET ... or ever
const instantiation_base*
scopespace::lookup_instance(const qualified_id& id) const {
	// check absolute-ness of identifier
	...
}
**/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const instantiation_base*
scopespace::add_instance(instantiation_base& i) {
	used_id_map[i.get_name()] = &i;
	return &i;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Searches ONLY this namespace for a instance.  
	then if not found locally, searches imported (unaliased) namespaces.  
	\param m the list of accumulated matches (also returned).  
	\param tid the name of type to search for.  
 */
void
scopespace::query_instance_match(instance_list& m, const string& tid) const {
	DEBUG(TRACE_DATATYPE_QUERY, 
		cerr << endl << "scopespace::query_instance_match: " << tid
			<< " in " << get_qualified_name())

	const instantiation_base* ret = 
		IS_A(const instantiation_base*, used_id_map[tid]);
	if (ret) m.push_back(ret);
}

//=============================================================================
// class name_space method definitions

/**
	Constructor for a new namespace.  
	Inherits from its parents: type aliases to built-in types, 
	such as bool and int.  
	\param n the name.  
	\param p pointer to the parent namespace.  
 */
name_space::name_space(const string& n, const name_space* p) : 
		scopespace(n, p), 
		parent(p), 
		open_spaces(), open_aliases() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The only only memory we need to delete is that owned by 
	this namespace.  We created, thus we delete.  
	All other pointers are shared non-owned pointers, 
	and will be deleted by their respective owners.  
 */
name_space::~name_space() {
	// no longer need to explicitly delete pointers belonging
	// to map_of_ptr<> types because their default destructors, 
	// that take care of them, will be invoked automatically.  

	// default destructor for lists and maps of un-owned pointers
	// will already clear those respective sets without deleting.  
//	open_spaces.clear();		// without deleting, don't own
//	open_aliases.clear();		// without deleting, don't own
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
/// Returns pointer to global namespace by following parent pointers.
const name_space*
name_space::get_global_namespace(void) const {
	if (parent)
		return parent->get_global_namespace();
	else	// no parent
		return this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
name_space::what(ostream& o) const {
	return o << "entity::namespace";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Downward (deeper) traversal of namespace hierarchy.  
	Either creates a new sub-namespace or opens if it already exists.  
	\param n the name of the namespace to enter.
	\return pointer to the referenced namespace, if found, else NULL.
	Details: First searches list of aliased namespaces to check for
	collision, which is currently reported as an error.  
	(We want to simply head off potential ambiguity here.)
	Then sees if name is already taken by some other definition 
	or instance in the used_id_map.  
	Then searches subnamespace to determine if already exists.
	If exists, re-open, else create new and link to parent.  
	\sa leave_namespace
 */
name_space*
name_space::add_open_namespace(const string& n) {
	name_space* ret;
	const object* probe = used_id_map[n];
	if (probe) {
		const name_space* probe_ns = IS_A(const name_space*, probe);
		// an alias may return with valid pointer!
		if (!probe_ns) {
			probe->what(cerr << n << " is already declared as a ")
				<< ", ERROR! ";
			return NULL;
		} else if (open_aliases[n]) {
		// we have a valid namespace pointer, 
		// now we see if this is an alias, or true sub-namespace
			cerr << n << " is already declared an open alias, ERROR! ";
			return NULL;
		} else {
		// therefore, probe_ns is a pointer to a valid sub-namespace
			DEBUG(TRACE_NAMESPACE_NEW, 
				cerr << n << " is already exists as subspace, re-opening")
			ret = IS_A(name_space*, used_id_map[n]);
		}
		assert(ret);
	} else {
		// create it, linking this as its parent
		DEBUG(TRACE_NAMESPACE_NEW, cerr << " ... creating new")
		ret = new name_space(n, this);
		assert(ret);
		used_id_map[n] = ret;	// register it as a used id
	}

	// silly sanity checks
	assert(ret->parent == this);
	assert(ret->key == n);
	DEBUG(TRACE_NAMESPACE_NEW, 
		cerr << " with parent: " << ret->parent->key)
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Leaves the namespace.  
	The list of imported and aliased namespaces will be reset each time 
	this namespace is closed.  They will have to be added back the 
	next time it is re-opened.  
	Also reclaims the identifiers that were associated with namespace
	aliases.  
	Non-aliased imported namespace do not take up any identifier space, 
	and thus, are not checked against the used_id_map.  
	\return pointer to the parent namespace, should never be NULL.  
	\sa add_open_namespace
 */
const name_space*
name_space::leave_namespace(void) {
	// for all open_aliases, release their names from used-map
	alias_map_type::const_iterator i = open_aliases.begin();
	for ( ; i!=open_aliases.end(); i++) {
		used_id_map.erase(used_id_map.find((*i).first));
	}
	open_spaces.clear();
	open_aliases.clear();
	return parent;
	// never NULL, can't leave global namespace!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a namespace using directive (import) to current namespace.  
	\param n the qualified identifier of the referenced namespace.
	\return valid pointer to imported namespace, or NULL if error. 
	\sa add_using_alias
 */
// idea: use ::id[::id]* as a way of specifying absolute namespace path
const name_space*
name_space::add_using_directive(const qualified_id& n) {
	const name_space* ret;
	namespace_list::const_iterator i;
	namespace_list candidates;		// empty list

	DEBUG(TRACE_NAMESPACE_USING, 
		cerr << endl << "adding using-directive in space: " 
			<< get_qualified_name())
	// see if namespace has already been declared within scope of search
	// remember: the qualified_id is a suffix to be appended onto root
	// find it/them, record to list
	query_import_namespace_match(candidates, n);
	i = candidates.begin();

	switch (candidates.size()) {
	// if list's size > 1, ambiguity
	// else if list is empty, unresolved namespace
	// else we've narrowed it down to one
		case 1: ret = (*i); open_spaces.push_back(ret); break;
		case 0:	{
			cerr << "namespace " << n << " not found, ERROR! ";
			// or n is not a namespace
			ret = NULL;
			break;	// no matches
			}
		default: {	// > 1
			ret = NULL;
			cerr << " ERROR: ambiguous import of namespaces, "
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
/**
	Aliases another namespace as a pseudo-sub-namespace with a different
	name.  Similar to add_using_directive, 
	but using a different name, and taking a spot in used_id_map.  
	\param n the referenced namespace qualified identifier
	\param a the new name local identifier
	Procedure outline: 
	Check if alias name is already taken by something else
	in this namespace.  Any local collision is reported as an error.  
	Note: name clashes with namespaces in higher scopes are permitted, 
	and in imported (unaliased) spaces.  
	This allows one to overshadow identifiers in higher namespaces.  
	\sa add_using_directive
 */
const name_space*
name_space::add_using_alias(const qualified_id& n, const string& a) {
	const object* probe;
	const name_space* ret;
	namespace_list::const_iterator i;
	namespace_list candidates;		// empty list

	DEBUG(TRACE_NAMESPACE_ALIAS, 
		cerr << endl << "adding using-alias in space: " 
			<< get_qualified_name() << " as " << a)

	probe = used_id_map[a];
	if (probe)
		probe = &probe->self();		// resolve handles
	if (probe) {
		// then already, it conflicts with some other id
		// we report the conflict precisely as follows:
		ret = IS_A(const name_space*, probe);
		if (ret) {
			if(open_aliases[a]) {
				cerr << a << " is already an open alias, ERROR! ";
			} else {
				cerr << a << " is already a sub-namespace, ERROR! ";
			}
		} else {
			probe->what(cerr << a << " is already declared ") 
				<< ", ERROR! ";
			// if is another namespace, could be an alias
			//	which looks like an alias
			// perhaps make a namespace_alias class
			// to replace handle...
		}
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
		case 1: {
			ret = (*i);
			open_aliases[a] = ret;
			// remember that open_aliases owns ret, 
			// not used_id_map, thus we use a const_handle.  
			used_id_map[a] = new object_handle(ret);
			break;
			}
		case 0:	{
			cerr << " ... not found, ERROR! ";
			ret = NULL;
			break;	// no matches
			}
		default: {	// > 1
			ret = NULL;
			cerr << " ERROR: ambiguous import of namespaces, "
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
	Now honors the absolute flag of the qualified_id to start search
	from global namespace.  
	TO DO: re-use quey_subnamespace_match
	\param id the qualified/scoped name of the namespace to match.
	\return pointer to found namespace.
 */
const name_space*
name_space::query_namespace_match(const qualified_id& id) const {
	// recall that qualified_id is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	DEBUG(TRACE_NAMESPACE_QUERY, 
		cerr << "query_namespace_match: " << id 
			<< " in " << get_qualified_name() << endl)

	if (id.empty())	{	// what if it's absolute and empty?
		return (id.is_absolute()) ? get_global_namespace() : this;
	}
	qualified_id::const_iterator i = id.begin();	assert(*i);
//	const token_identifier* tid = *i;
	never_const_ptr<token_identifier> tid(*i);
	assert(tid);
	DEBUG(TRACE_NAMESPACE_SEARCH, cerr << "\ttesting: " << *tid)
	const name_space* ns = (id.is_absolute()) ? this
		: get_global_namespace();
	if (ns->key.compare(*tid)) {
		// if names differ, already failed, try alias spaces
		return NULL;
	} else {
		for (i++; ns && i!=id.end(); i++) {
			const name_space* next;
//			never_const_ptr<name_space> next;
			// no need to skip scope tokens anymore
//			tid = IS_A(token_identifier*, *i);
//			tid = i->is_a<token_identifier>();
			tid = (*i).is_a<token_identifier>();
			assert(tid);
			DEBUG(TRACE_NAMESPACE_SEARCH, cerr << scope << *tid)
			// the [] operator of map<> doesn't have const 
			// semantics, even if looking up an entry!
			next = IS_A(const name_space*, ns->used_id_map[*tid]);
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
	\param id the qualified/scoped name of the namespace to match
 */
const name_space*
name_space::query_subnamespace_match(const qualified_id& id) const {
	// recall that qualified_id is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	DEBUG(TRACE_NAMESPACE_QUERY, 
		cerr << endl << "query_subnamespace_match: " << id 
			<< " in " << get_qualified_name() << endl)
	if (id.empty())	{	// what if it's absolute and empty?
		return (id.is_absolute()) ? get_global_namespace() : this;
	}
	qualified_id::const_iterator i = id.begin();	// id may be empty!
//	const token_identifier* tid = *i;
	never_const_ptr<token_identifier> tid(*i);
	assert(tid);
	DEBUG(TRACE_NAMESPACE_SEARCH, cerr << "\ttesting: " << *tid)
	const name_space* ns = 
		IS_A(const name_space*, 
			((id.is_absolute()) ? get_global_namespace() : this)
				->used_id_map[*tid]);
	if (!ns) {				// else lookup in aliases
		ns = open_aliases[*tid];	// replaced for const semantics
	}
	// remember to skip scope tokens
	for (i++; ns && i!=id.end(); i++) {
		const name_space* next;
//		tid = IS_A(token_identifier*, *i);
		tid = (*i).is_a<token_identifier>();
		assert(tid);
		DEBUG(TRACE_NAMESPACE_SEARCH, cerr << scope << *tid)
		next = IS_A(const name_space*, ns->used_id_map[*tid]);
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
query_import_namespace_match(namespace_list& m, const qualified_id& id) const {
	DEBUG(TRACE_NAMESPACE_QUERY, 
		cerr << endl << "query_import_namespace_match: " << id 
			<< " in " << get_qualified_name())
	{
		const name_space* ret = query_subnamespace_match(id);
		if (ret) m.push_back(ret);
	}
	// always search these unconditionally? or only if not found so far?
	{	// with open namespaces list
		namespace_list::const_iterator i = open_spaces.begin();
		for ( ; i!=open_spaces.end(); i++) {
			const name_space* ret = 
				(*i)->query_subnamespace_match(id);
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
#else
	if (parent)
#endif
		parent->query_import_namespace_match(m, id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Searches this namespace for a definition, 
	then if not found locally, searches imported (unaliased) namespaces.  
	If match list is still empty, then searches parents' namespace
	until one (grand) parent finds results.  
	Does not search down subnamespaces, 
	or aliased imported namespaces.  Searching in those places
	requires that the identifier be qualified with scope.  
	The primary difference between this and other type-specific query
	methods is that this uses only the used_id_map for searching.  
	\param m the list of accumulated matches (also returned).  
	\param tid the name of type to search for.  
 */
void
name_space::query_definition_match(definition_list& m, const string& tid) const {
	DEBUG(TRACE_DATATYPE_QUERY, 
		cerr << endl << "query_definition_match: " << tid
			<< " in " << get_qualified_name())
	{
		const definition_base* ret = 
			IS_A(const definition_base*, used_id_map[tid]);
		if (ret) m.push_back(ret);
	}
	// always search these unconditionally? or only if not found so far?
	if (m.empty()) {
		// with open namespaces list
		namespace_list::const_iterator i = open_spaces.begin();
		for ( ; i!=open_spaces.end(); i++) {
			assert(*i);
			const definition_base* ret = 
				IS_A(const definition_base*,
					(*i)->used_id_map[tid]);
			if (ret) m.push_back(ret);
		}
		// don't search aliased imports
	}

	// until list is not empty, keep querying parents
	// thus, names in deeper spaces will overshadow names in outer spaces
	if (m.empty() && parent)
		parent->query_definition_match(m, tid);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Searches this namespace for a instance, 
	then if not found locally, searches imported (unaliased) namespaces.  
	If match list is still empty, then searches parents' namespace
	until one (grand) parent finds results.  
	Does not search down subnamespaces, 
	or aliased imported namespaces.  Searching in those places
	requires that the identifier be qualified with scope.  
	The primary difference between this and other type-specific query
	methods is that this uses only the used_id_map for searching.  
	\param m the list of accumulated matches (also returned).  
	\param tid the name of type to search for.  
 */
void
name_space::query_instance_match(instance_list& m, const string& tid) const {
	DEBUG(TRACE_DATATYPE_QUERY, 
		cerr << endl << "name_space::query_instance_match: " << tid
			<< " in " << get_qualified_name())

	scopespace::query_instance_match(m, tid);
	// always search these unconditionally? or only if not found so far?
	if (m.empty()) {
		// with open namespaces list
		namespace_list::const_iterator i = open_spaces.begin();
		for ( ; i!=open_spaces.end(); i++) {
			assert(*i);
			const instantiation_base* ret = 
				IS_A(const instantiation_base*,
					(*i)->used_id_map[tid]);
			if (ret) m.push_back(ret);
		}
		// don't search aliased imports
	}

	// until list is not empty, keep querying parents
	// thus, names in deeper spaces will overshadow names in outer spaces
	if (m.empty() && parent)
		parent->query_instance_match(m, tid);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Searches this namespace for a matched type, 
	then if not found locally, searches imported (unaliased) namespaces.  
	If match list is still empty, then searches parents' namespace
	until one (grand) parent finds results.  
	Does not search down subnamespaces, 
	or aliased imported namespaces.  Searching in those places
	requires that the identifier be qualified with scope.  
	\param m the list of accumulated matches (also returned).  
	\param tid the name of type to search for.  
 */
void
name_space::query_datatype_def_match(data_def_list& m, const string& tid) const {
	DEBUG(TRACE_DATATYPE_QUERY, 
		cerr << endl << "query_datatype_def_match: " << tid
			<< " in " << get_qualified_name())
	{
		const datatype_definition* ret =
			IS_A(const datatype_definition*, used_id_map[tid]);
		if (ret) m.push_back(ret);
	}
	// always search these unconditionally? or only if not found so far?
	if (m.empty()) {
		// with open namespaces list
		namespace_list::const_iterator i = open_spaces.begin();
		for ( ; i!=open_spaces.end(); i++) {
			const datatype_definition* ret = 
				IS_A(const datatype_definition*, 
					(*i)->used_id_map[tid]);
			if (ret) m.push_back(ret);
		}
		// don't search aliased imports
	}

	// until list is not empty, keep querying parents
	// thus, names in deeper spaces will overshadow names in outer spaces
	if (m.empty() && parent)
		parent->query_datatype_def_match(m, tid);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Finds a namespace ending with a (optionally) scoped identifier
	(allows multiple matches, hence the use of a list reference).
	Currently not used.  
 */
void
name_space::
find_namespace_ending_with(namespace_list& m, const qualified_id& id) const {
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
	const name_space* ret = query_subnamespace_match(id);
	if (ret)	m.push_back(ret);
	query_import_namespace_match(m, id);
	if (parent)
		parent->find_namespace_ending_with(m, id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a definition to this namespace.  
	Definition is newly created, so used_id_map is responsible
	for deleting it.  
	On failure, however, pointer is not added, so need to handle
	memory in the caller.  
	\param db the definition to add, newly created.
	\return definition added if successful, else NULL.  
 */
definition_base*
name_space::add_definition(definition_base* db) {
	assert(db);
	string k = db->get_name();
	const object* probe = used_id_map[k];
	if (probe) {
		probe->what(cerr << "ERROR: identifier already taken by ")
			<< "  Failed to add definition!";
		return NULL;
	} else {
		// used_id_map owns this type is reponsible for deleting it
		used_id_map[k] = db;
		return db;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
datatype_definition*
name_space::add_type_alias(const qualified_id& t, const string& a) {
	return NULL;
/*** not done yet
	datatype_definition* ret;
	object* probe;
	namespace_list::iterator i;
	namespace_list candidates;		// empty list

	cerr << endl << "adding type-alias in space: " 
		<< get_qualified_name() << " as " << a;

	probe = used_id_map[a];
	if (probe) {
		// then already, it conflicts with some other id
		probe->what(cerr << a << " is already declared ")
			<< ", ERROR! ";
		return NULL;
	}

	// else we're ok to proceed to add alias
	// first find the referenced type name...
	query_datatype_def_match(candidates, t);
	i = candidates.begin();

	switch (candidates.size()) {
	// if list's size > 1, ambiguity
	// else if list is empty, unresolved type
	// else we've narrowed it down to one
		case 1: {
			ret = (*i);
			used_id_map[a] = ret;
			break;
			}
		case 0:	{
			cerr << " ... not found, ERROR! ";
			ret = NULL;
			break;	// no matches
			}
		default: {	// > 1
			ret = NULL;
			cerr << " ERROR: ambiguous type alias, "
				"need to be more specific.  candidates are: ";
				for ( ; i!=candidates.end(); i++)
					cerr << endl << "\t" << 
						(*i)->get_qualified_name();
			}
	}

	return ret;		// NULL => error
	// candidates will automatically be cleared (not owned pointers)
*** not done ***/
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a fundamental_type_reference to the used_id_map, using
	the hash_string as the key.  
	Memory management: will delete tb if it is redundant!
	So tb is created before this method call.  
	Inefficiency: have to create and delete type_reference.
	TO DO: be able to lookup type in advance before creating...
	\param tb the fundamental_type_reference to lookup and add --
		MAY BE DELETED if a matching reference is found.
	\return the same fundamental_type_reference if none was previously 
		found, otherwise the existing matching reference.  
 */
const fundamental_type_reference*
name_space::add_type_reference(fundamental_type_reference* tb) {
	const object* o;
	const fundamental_type_reference* trb;
	assert(tb);
	string k = tb->hash_string();
	o = used_id_map[k];
	// see what is already there...
	trb = IS_A(const fundamental_type_reference*, o);

	if (o)	assert(trb);
	// else a non-type-reference hashed into the used_id_map
	// using a hash for a type-reference!!!

	if (trb) {
		// then found a match!, we can delete tb
		delete tb;
		return trb;
	} else {
		// if tb contains parameter literals that are only
		// local to this scope, we must add it here, 
		// else if tb contains only constants and resolved
		// parameters, then we have two options:
		// 1) search up parents' namespace until one is found.  
		//	if not found, add it locally.
		// 2) jump to the global namespace, to search for type
		//	if found, use that, else add it to global.
		//	problem: parent/global namespace is const, 
		//	so we can't modify it with dirty hack.  
		// 3) just add it locally to this namespace regardless...
		//	who gives a rat's a** about redundancy?
		// 3:
		used_id_map[k] = tb;
		return tb;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overrides scopespace::lookup_namespace.
 */
const scopespace*
name_space::lookup_namespace(const qualified_id& id) const {
	return query_subnamespace_match(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public interface to lookup a single definition_base.  
	This version takes a single identifier string (unqualified).  
	\param id the unqualified name of the definition to seek.  
	\return pointer to matched definition_base, only if it unique, 
		otherwise returns NULL.  
	\sa lookup_unqualified_datatype
 */

const definition_base*
name_space::lookup_definition(const token_identifier& id) const {
	definition_list::iterator i;
	definition_list candidates;

	// only want to query if type was unqualified
	query_definition_match(candidates, id);

	i = candidates.begin();
	switch (candidates.size()) {
		case 1: { return (*i); }	// unique match
		case 0:	{	// no matches
			cerr << "type " << id << " ... not found, ERROR! ";
			return NULL;
			}
		default: {	// too many matches
			cerr << " ERROR: ambiguous definition, "
				"need to be more specific.  candidates are: ";
				for ( ; i!=candidates.end(); i++)
					cerr << endl << "\t" << 
						(*i)->get_qualified_name();
			return NULL;
			}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public interface to lookup a single definition_base.  
	This version takes a qualified expression.  
	The main difference between this and other type-specific lookup
	methods is that this only searches used_id_map.  
	\param id the qualified name of the type.  
	\return pointer to matched definition_base, only if it unique, 
		otherwise returns NULL.  
	\sa lookup_qualified_datatype
 */
const definition_base*
name_space::lookup_definition(const qualified_id& id) const {
	qualified_id nsname = id.copy_namespace_portion();
//	cerr << "nsname = " << " " << nsname << endl;
	// what if nsname is empty? start search here
	const name_space* root = 
		((id.is_absolute()) ? get_global_namespace() : this)
		->query_subnamespace_match(nsname);
	if (root) {
		const definition_base* ret;
		qualified_id::const_reverse_iterator e = id.rbegin();
		assert(*e);
		ret = IS_A(const definition_base*, root->used_id_map[**e]);
		if (!ret)
			cerr << "definition " << id <<
				" ... not found, ERROR!" << endl;
		return ret;
	} else {
		cerr << " ERROR: namespace " << nsname <<
			" not found!" << endl;
		return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public interface to lookup a single datatype_definition.  
	This version takes a single identifier string (unqualified).  
	Will need to make sure that template params of matched type is null.  
	\param id the unqualified name of the type.  
	\return pointer to matched datatype_definition, only if it unique, 
		otherwise returns NULL.  
	\sa query_datatype_def_match
	\sa lookup_qualified_datatype
 */

const datatype_definition*
name_space::lookup_unqualified_datatype(const string& id) const {
	const datatype_definition* ret;
	data_def_list::iterator i;
	data_def_list candidates;

	// only want to query if type was unqualified
	query_datatype_def_match(candidates, id);

	i = candidates.begin();
	switch (candidates.size()) {
		case 1: {
			ret = (*i);
			break;
			}
		case 0:	{
			cerr << "type " << id << " ... not found, ERROR!";
			ret = NULL;
			break;	// no matches
			}
		default: {	// > 1
			ret = NULL;
			cerr << " ERROR: ambiguous type definition, "
				"need to be more specific.  candidates are: ";
				for ( ; i!=candidates.end(); i++)
					cerr << endl << "\t" << 
						(*i)->get_qualified_name();
			}
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public interface to lookup a single datatype_definition.  
	This version takes a qualified expression.  
	Will need to make sure that template params of matched type is null.  
	\param id the qualified name of the type.  
	\return pointer to matched datatype_definition, only if it unique, 
		otherwise returns NULL.  
	\sa query_datatype_def_match
 */
const datatype_definition*
name_space::lookup_qualified_datatype(const qualified_id& id) const {
	qualified_id nsname = id.copy_namespace_portion();
//	cerr << "nsname = " << " " << nsname << endl;
	// what if nsname is empty? start search here
	const name_space* root = 
		((id.is_absolute()) ? get_global_namespace() : this)
		->query_subnamespace_match(nsname);
	if (root) {
		const datatype_definition* ret;
		qualified_id::const_reverse_iterator e = id.rbegin();
		assert(*e);
		ret = IS_A(const datatype_definition*, root->used_id_map[**e]);
		if (!ret)
			cerr << "data-type " << id << " ... not found, ERROR! ";
		return ret;
	} else {
		cerr << " ERROR: namespace " << nsname << " not found!";
		return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Faster lookup of built-in int<> and bool data types, 
	rooted at the global namespace.  
 */
const datatype_definition*
name_space::lookup_built_in_datatype(const token_datatype& id) const {
	const built_in_datatype_def* ret;
	ret = IS_A(const built_in_datatype_def*, 
			get_global_namespace()->used_id_map[id]);
	assert(ret);		// better already be there!
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Faster lookup of built-in int<> and bool data types, 
	rooted at the global namespace.  
 */
const built_in_param_def*
name_space::lookup_built_in_paramtype(const token_paramtype& id) const {
	const built_in_param_def* ret;
	ret = IS_A(const built_in_param_def*, 
		get_global_namespace()->used_id_map[id]);
	assert(ret);		// better already be there!
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Public interface to lookup a single instantiation_base.  
	This version takes a qualified expression.  
	The main difference between this and other type-specific lookup
	methods is that this only searches used_id_map.  
	\param id the qualified name of the type.  
	\return pointer to matched instantiation_base, only if it unique, 
		otherwise returns NULL.  
 */
const instantiation_base*
name_space::lookup_instance(const qualified_id& id) const {
	qualified_id nsname = id.copy_namespace_portion();
//	cerr << "nsname = " << " " << nsname << endl;
	// what if nsname is empty? start search here
	const name_space* root = 
		((id.is_absolute()) ? get_global_namespace() : this)
		->query_subnamespace_match(nsname);
	if (root) {
		const instantiation_base* ret;
		qualified_id::const_reverse_iterator e = id.rbegin();
		assert(*e);
//		ret = IS_A(const instantiation_base*, root->used_id_map[**e]);
		ret = root->lookup_instance(**e);
		if (!ret)
			cerr << "instance " << id << " ... not found, ERROR! ";
		return ret;
	} else {
		cerr << " ERROR: namespace " << nsname << " not found!";
		return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create an instance of a data type and add it local symbol table.  
	First checks to see if name is already taken in the used_id_map.  
	If it collides with anything, then error.  
	Doesn't check ever other namespace (imports, parents...)
	because local identifiers are allowed to overshadow.  
	\param t the type of the instance.
	\param id the name of the instance.  
 */
datatype_instantiation*
name_space::add_datatype_instantiation(
		const data_type_reference& t, 
		const string& id) {
	const object* probe;
	datatype_instantiation* new_inst;
	probe = used_id_map[id];
	if (probe) {
		probe->what(cerr << id << " is already declared ")
			<< ", ERROR! ";
		return NULL;
	}
	// consistency check
	// else safe to proceed

	new_inst = new datatype_instantiation(*this, t, id);
	assert(new_inst);
	// new_inst will be owned by used_id_map
	used_id_map[id] = new_inst;
	return new_inst;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Create an instance of a param type and add it local symbol table.  
	First checks to see if name is already taken in the used_id_map.  
	If it collides with anything, then error.  
	Doesn't check ever other namespace (imports, parents...)
	because local identifiers are allowed to overshadow.  
	\param t the type of the instance.
	\param id the name of the instance.  
 */
param_instantiation*
name_space::add_paramtype_instantiation(
		const param_type_reference& t,
		const string& id) {
	const object* probe;
	param_instantiation* new_inst;
	probe = used_id_map[id];
	if (probe) {
		probe->what(cerr << id << " is already declared ")
			<< ", ERROR! ";
		return NULL;
	}
	// else safe to proceed

	new_inst = new param_instantiation(*this, t, id);
	assert(new_inst);
	// new_inst will be owned by used_id_map
	used_id_map[id] = new_inst;
	return new_inst;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks to see whether process definition or prototype already 
	exists.  
	\param s the name of the process in this namespace.  
	\return valid pointer to process_definition if found, 
		else NULL if not found.  
 */
const process_definition*
name_space::probe_process(const string& s) const {
	const object* probe = used_id_map[s];
	return IS_A(const process_definition*, probe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a process prototype.  
	Need to let the caller know whether already existed and should
	check against previous declaration, or this is a new process.  
	\param pname the process' signature, including identifier,
		optional template signature, and port signature.  
	Details: ...
 */
process_definition*
name_space::add_proc_declaration(const token_identifier& pname) {
	process_definition* pd = NULL;
	const object* probe = used_id_map[pname];
	if (probe) {
		// something already exists with name...
		const process_definition* probe_pd = 
			IS_A(const process_definition*, probe);
		if (probe_pd) {
			// see if this declaration matches EXACTLY
			// or punt check until check_build() on the templates
			//	and ports?
			return IS_A(process_definition*, used_id_map[pname]);
		} else {
			// already declared as something else in this scope.
			probe->what(cerr << pname << " is already declared as ")
				<< ", ERROR! ";
			return NULL;
		}
	} else {
		// slot is free, allocate new entry for process definition
		pd = new process_definition(this, pname, false);
		assert(pd);
		used_id_map[pname] = pd;
	}
	return pd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a process definition.  
	\param pname the process' signature, including identifier,
		optional template signature, and port signature.  
	Details: ...
 */
process_definition*
name_space::add_proc_definition(const token_identifier& pname) {
	process_definition* pd = NULL;
	const object* probe = used_id_map[pname];	// looks up used_id_map
	if (probe) {
		// something already exists with name...
		const process_definition* probe_pd = 
			IS_A(const process_definition*, probe);
		if (probe_pd) {
			if (probe_pd->is_defined()) {
				cerr << pname << " process already defined.  "
					"redefinition starting at " 
					<< pname.where();
				return NULL;
			} else {
			// probably already declared
			// punt check, until traversing templates/ports
				return IS_A(process_definition*, used_id_map[pname]);
			}
		} else {
			// already declared as something else in this scope.
			probe->what(cerr << pname << " is already declared as ")
				<< ", ERROR! ";
			return NULL;
		}
	} else {
		// slot is free, allocate new entry for process definition
		pd = new process_definition(this, pname, true);
		assert(pd);
		used_id_map[pname] = pd;
	}
	return pd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	OBSOLETE.
	This searches the parent for built-in types and creates local 
	aliases to them under the same name, for accelerated type resolution.  
	Types that qualify for inheritance are either built-in types, 
	which should only be in the global namespace, or aliases thereof.  
	To be used only by the constructor, hence private.  
void
name_space::inherit_built_in_types(void) {
if (parent) {
	type_def_set::const_iterator i = parent->data_defs.begin();
	for ( ; i!=parent->data_defs.end(); i++) {
		const datatype_definition* t;
		t = (*i).second;
		assert(t);
		t = t->resolve_canonical();	// resolve
		assert(t);

		// detect built-in types
		if (IS_A(const built_in_datatype_def*, t)) {
			assert(!used_id_map[(*i).first]);
			type_alias* new_alias = 
				new type_alias(this, (*i).first, t);
			assert(new_alias);
			data_defs[(*i).first] = new_alias;
		} 
		// else don't add
	}
} // end if (parent)
}
**/

//=============================================================================
// class definition_base method definitions

// p is parent
inline
definition_base::definition_base(const string& n, const name_space* p, 
		template_formals_set* tf) : 
		scopespace(n, p), template_formals(tf) {
}

inline
definition_base::~definition_base() {
}

/**
	Used for checking when a type should have null template arguments.  
	\return true if this definition is not templated, 
		or the template formals signature is empty.  
 */
bool
definition_base::check_null_template_argument(void) const {
	if (!template_formals)
		return true;
	else if (template_formals->empty())
		return true;
	else {
		// make sure each formal has a default parameter value
		template_formals_set::const_iterator i =
			template_formals->begin();
		for ( ; i!=template_formals->end(); i++) {
			const param_instantiation* p = *i;
			assert(p);
		// if any formal is missing a default value, then this 
		// definition cannot have null template arguments
			if (!p->default_value())
				return false;
			// else continue;	// keep checking
		}
		// if we've reached end of list, we're good!
		return true;
	}
}

string
definition_base::get_name(void) const {
	return key;
}

string
definition_base::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() +scope +key;
	else return key;
}

/**
	Adds an instantiation to the current definition's scope, and 
	also registers it in the list of template formals for 
	template argument checking.  
	What if template formal is an array, or collective?
	\param f needs to be a param_instantiation... what about array?
		need to be non-const? storing to hash_map_of_ptr...
 */
const instantiation_base*
definition_base::add_template_formal(instantiation_base* f) {
	param_instantiation* pf = 
		IS_A(param_instantiation*, f);
	assert(pf);
	if (!template_formals) {
		template_formals = new template_formals_set();
		assert(template_formals);
	}
	// check and make sure identifier wasn't repeated in formal list!
	const object* probe = used_id_map[pf->get_name()];
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return NULL;
	}

	const param_instantiation** ret =
		template_formals->append(pf->hash_string(), pf);
		// unchecked pointer dereference
	assert(!ret);
	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!

	// COMPILE: pf is const, but used_id_map members are not
	// wrap around with object_handle?
	used_id_map[pf->hash_string()] = pf;
	return pf;
}

//=============================================================================
// class fundamental_type_reference method definitions

fundamental_type_reference::fundamental_type_reference(template_param_list* pl)
		: type_reference_base(), 
		template_params(pl) {
}

fundamental_type_reference::~fundamental_type_reference() {
	SAFEDELETE(template_params);
}

/**
	Evaluates type reference as a flat string, for caching purposes.  
	We unconditionally add the <> to the key even if there is no template
	specifier to guarantee that hash_string for a type-reference 
	cannot collide with the hash string for the non-templated definition.  
	\return string to be used for hashing.  
 */
string
fundamental_type_reference::hash_string(void) const {
	// TO DO: should be fully qualified name!
	string ret(get_base_def()->get_qualified_name());
	ret += "<";
	if (template_params) {
		template_param_list::const_iterator i =
			template_params->begin();
		// add commas?
		for ( ; i!=template_params->end(); i++) {
			const param_expr* e = *i;
			if (e)
				ret += e->hash_string();
			// can e ever be NULL?
			ret += ",";
			// extra comma at the end, who cares?
		}
	}
	ret += ">";
	return ret;
}

//=============================================================================
// class collective_type_reference method definitions

collective_type_reference::collective_type_reference(
		const type_reference_base& b, 
		const array_dim_list* d) :
		type_reference_base(), base(&b), dim(d) {
}

collective_type_reference::~collective_type_reference() {
	// we don't own the members
}

ostream&
collective_type_reference::what(ostream& o) const {
	return o << "collective-type-ref";
}

//=============================================================================
// class data_type_reference method definitions

data_type_reference::data_type_reference(const datatype_definition* td, 
		template_param_list* pl) : fundamental_type_reference(pl), 
		base_type_def(td) {
}

data_type_reference::~data_type_reference() {
}

ostream&
data_type_reference::what(ostream& o) const {
	return o << "data-type-reference";
}

const definition_base*
data_type_reference::get_base_def(void) const {
	return base_type_def;
}

/**
	PHASE THIS OUT.
 */
const fundamental_type_reference*
data_type_reference::set_context_type_reference(context& c) const {
	return c.set_inst_data_type_ref(*this);
}

/**
	Creates an instance of a data type, 
	and adds it to a scope.
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
const instantiation_base*
data_type_reference::add_instance_to_scope(scopespace& s,
		const token_identifier& id) const {
	// make sure doesn't collide with something in s.
	// what if s is a loop-scope, not a namespace?  PUNT!
	const object* probe = s.lookup_object_here(id);
	if (probe) {
		probe->what(cerr << id << " is already declared ") <<
			", ERROR! " << id.where() << endl;
		return NULL;
	}
	// else proceed
	datatype_instantiation* di = 
		new datatype_instantiation(s, *this, id);
	assert(di);
	return s.add_instance(*di);
}

//=============================================================================
// class channel_type_reference method definitions

channel_type_reference::channel_type_reference(const channel_definition* cd, 
		template_param_list* pl) : fundamental_type_reference(pl), 
		base_chan_def(cd) {
}

channel_type_reference::~channel_type_reference() {
}

ostream&
channel_type_reference::what(ostream& o) const {
	return o << "channel-type-reference";
}

const definition_base*
channel_type_reference::get_base_def(void) const {
	return base_chan_def;
}

const fundamental_type_reference*
channel_type_reference::set_context_type_reference(context& c) const {
	return c.set_inst_chan_type_ref(*this);
}

/**
	Creates an instance of a channel type, 
	and adds it to a scope.
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
const instantiation_base*
channel_type_reference::add_instance_to_scope(scopespace& s,
		const token_identifier& id) const {
	// make sure doesn't collide with something in s.
	channel_instantiation* ci = 
		new channel_instantiation(s, *this, id);
	assert(ci);
	return s.add_instance(*ci);
}

//=============================================================================
// class process_type_reference method definitions

process_type_reference::process_type_reference(const process_definition* pd, 
		template_param_list* pl) : fundamental_type_reference(pl), 
		base_proc_def(pd) {
}

process_type_reference::~process_type_reference() {
}

ostream&
process_type_reference::what(ostream& o) const {
	return o << "process-type-reference";
}

const definition_base*
process_type_reference::get_base_def(void) const {
	return base_proc_def;
}

const fundamental_type_reference*
process_type_reference::set_context_type_reference(context& c) const {
	return c.set_inst_proc_type_ref(*this);
}

/**
	Creates an instance of a process type, 
	and adds it to a scope.
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
const instantiation_base*
process_type_reference::add_instance_to_scope(scopespace& s,
		const token_identifier& id) const {
	// make sure doesn't collide with something in s.
	process_instantiation* ci = 
		new process_instantiation(s, *this, id);
	assert(ci);
	return s.add_instance(*ci);
}

//=============================================================================
// class param_type_reference method definitions

param_type_reference::param_type_reference(const built_in_param_def* pd) : 
		fundamental_type_reference(NULL), 
		base_param_def(pd) {
}

param_type_reference::~param_type_reference() {
}

ostream&
param_type_reference::what(ostream& o) const {
	return o << "param-type-reference";
}

const definition_base*
param_type_reference::get_base_def(void) const {
	return base_param_def;
}

const fundamental_type_reference*
param_type_reference::set_context_type_reference(context& c) const {
	return c.set_inst_param_type_ref(*this);
}

/**
	Creates an instance of a parameter type, 
	and adds it to a scope.
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
const instantiation_base*
param_type_reference::add_instance_to_scope(scopespace& s,
		const token_identifier& id) const {
	// make sure doesn't collide with something in s.
	param_instantiation* ci = 
		new param_instantiation(s, *this, id);
	assert(ci);
	return s.add_instance(*ci);
}

//=============================================================================
// class instantiation_base method definitions

inline
instantiation_base::instantiation_base(const scopespace& o, 
		const string& n, array_dim_list* d) : 
		object(), owner(&o), key(n), array_dimensions(d) {
}

inline
instantiation_base::~instantiation_base() {
	SAFEDELETE(array_dimensions);
}

string
instantiation_base::get_qualified_name(void) const {
	if (owner)
		return owner->get_qualified_name() +scope +key;
	else return key;
}

/*** TO DO, if necessary
// reserve for instance_reference
string
instantiation_base::hash_string(void) const {
	// don't need get_type_ref()->hash_string().
	string ret(get_qualified_name());
	// TO DO: do array dimensions matter, or is name sufficient?
//	if (array_dimensions) {
//	}
	return ret;
}
***/

void
instantiation_base::set_array_dimensions(array_dim_list* d) {
	// just in case, delete what was previously there
	SAFEDELETE(array_dimensions);
	array_dimensions = d;
}

/**
	Determines whether or not the dimensions of two instantiation_base
	arrays are equivalent, and hence compatible.  
	\param i the second instantiation_base to compare against.
	\return false for now.
 */
bool
instantiation_base::array_dimension_match(const instantiation_base& i) const {
	// TO DO: this is temporary
	// if both lists are not NULL, iterate through lists...
	return false;
}

//=============================================================================
// class datatype_definition method definitions

// make sure that this constructor is never invoked outside this file
inline
datatype_definition::datatype_definition(const name_space* o, const string& n, 
		template_formals_set* tf) :
		definition_base(n, o, tf) {
}

inline
datatype_definition::~datatype_definition() {
}

const definition_base*
datatype_definition::set_context_definition(context& c) const {
	return c.set_inst_data_def(*this);
}

const fundamental_type_reference*
datatype_definition::set_context_fundamental_type(context& c) const {
	data_type_reference* dtr = new data_type_reference(this,
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

/**
	Call this to automatically resolve type, if type referenced
	is an alias or typedef.  
 */
const datatype_definition*
datatype_definition::resolve_canonical(void) const {
	return this;
}

//=============================================================================
// class channel_definition method definitions

// make sure that this constructor is never invoked outside this file
inline
channel_definition::channel_definition(const name_space* o, const string& n, 
		template_formals_set* tf) :
		definition_base(n, o, tf) {
}

channel_definition::~channel_definition() {
}

const definition_base*
channel_definition::set_context_definition(context& c) const {
	return c.set_inst_chan_def(*this);
}

const fundamental_type_reference*
channel_definition::set_context_fundamental_type(context& c) const {
	channel_type_reference* dtr = new channel_type_reference(this,
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}


//=============================================================================
// class type_alias method definitions

/**
	Type alias constructor follows the argument pointer until, 
	it encounters a canonical type, one that is not an alias.  
	\param o the namespace to which this belongs.  
	\param n the name of the aliased type.  
	\param t pointer to the actual type being aliased.  
 */
type_alias::type_alias(const name_space* o, const string& n, 
		const definition_base* t, 
		template_formals_set* tf) :
		definition_base(n, o, tf), canonical(t) {
	assert(canonical);
	// just in case t is not a canonical type, i.e. another alias...
	const type_alias* a = IS_A(const type_alias*, canonical);
	while (a) {
		canonical = a;
		a = IS_A(const type_alias*, a->canonical);
	}
}

/**
	Destructor, never deletes the canonical type pointer.  
 */
type_alias::~type_alias() { }

/**
	Fancy name for "just return the canonical pointer."
	\return the canonical pointer.  
 */
inline
const definition_base*
type_alias::resolve_canonical(void) const {
	return canonical;
}

ostream&
type_alias::what(ostream& o) const {
	return o << "aliased-type: " << key;
}

/** NOT USED, ever
bool
type_alias::type_equivalent(const datatype_definition& t) const {
	return resolve_canonical()->type_equivalent(t);
}
**/

//=============================================================================
// class built_in_datatype_def method definitions

// doesn't like inlining this, linker can't find definition on gcc-3.3
// is used in "art_symbol_table.cc", unless you -fkeep-inline-functions
built_in_datatype_def::built_in_datatype_def(const name_space* o, const string& n) :
	datatype_definition(o, n) {
}

built_in_datatype_def::~built_in_datatype_def() { }

ostream&
built_in_datatype_def::what(ostream& o) const {
	return o << key;
}

const definition_base*
built_in_datatype_def::set_context_definition(context& c) const {
	return c.set_inst_data_def(*this);
}

const fundamental_type_reference*
built_in_datatype_def::set_context_fundamental_type(context& c) const {
	data_type_reference* dtr = new data_type_reference(this,
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

/**
	Checks data type equivalence.
	TO DO:
	Currently does NOT check template signature, which need to be 
	implemented eventually, but punting for now.  
	\param t the datatype_definition to be checked.
	\return true if types are equivalent.  
 */
bool
built_in_datatype_def::type_equivalent(const datatype_definition& t) const {
	const built_in_datatype_def* b = 
		IS_A(const built_in_datatype_def*, t.resolve_canonical());
	if (b) {
		// later: check template signature! (for int<>)
		return key == b->key;
	} else {
		return false;
	}
}

//=============================================================================
// class built_in_param_def method definitions

built_in_param_def::built_in_param_def(const name_space* p, const string& n) :
		definition_base(n, p) {
}

built_in_param_def::~built_in_param_def() {
}

ostream&
built_in_param_def::what(ostream& o) const {
	return o << key;
}

const definition_base*
built_in_param_def::set_context_definition(context& c) const {
	return c.set_current_definition_reference(*this);
}

const fundamental_type_reference*
built_in_param_def::set_context_fundamental_type(context& c) const {
	param_type_reference* dtr = new param_type_reference(this);
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

//=============================================================================
// class user_def_datatype method definitions

/// constructor for user defined type
user_def_datatype::user_def_datatype(const name_space* o, const string& name) :
	datatype_definition(o, name), template_params(), members() {
}

ostream&
user_def_datatype::what(ostream& o) const {
	return o << "used-defined-datatype: " << key;
}

/**
	Equivalance operator for user-defined types.  
	TO DO: actually write comparison, for now, just always returns false.  
	\param t the datatype_definition to be checked;
	\return true if the type names match, the (optional) template
		formals match, and the data formals match.  
 */
bool
user_def_datatype::type_equivalent(const datatype_definition& t) const {
	const user_def_datatype* u = 
		IS_A(const user_def_datatype*, t.resolve_canonical());
	if (u) {
		// compare template_params
		// compare data members
		// for now...
		return false;
	} else {
		return false;
	}
}

//=============================================================================
// class datatype_instantiation method definitions

datatype_instantiation::datatype_instantiation(const scopespace& o, 
		const data_type_reference& t, 
		const string& n) : 
		instantiation_base(o, n), type(&t) {
	assert(type);
}

datatype_instantiation::~datatype_instantiation() {
}

ostream&
datatype_instantiation::what(ostream& o) const {
	return o << "datatype-inst";
}

const fundamental_type_reference*
datatype_instantiation::get_type_ref(void) const {
	return type;
}

/**
	Takes a template formal, resolves its type first, looking up
	through parents' scopes if necessary.  
	If successful, compares for type-equivalence, 
	and identifier match.  
	\param tf the template formal from the syntax tree.
	\return true if type and identifier match exactly.  
 */
bool
datatype_instantiation::equals_template_formal(
		const template_formal_decl& tf) const {

	return false;
}

/**
	Create a datatype reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
instance_reference_base*
datatype_instantiation::make_instance_reference(context& c) const {
	cerr << "datatype_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	datatype_instance_reference* ret = 
		new datatype_instance_reference(*this);
		// omitting index argument
	assert(ret);
	return ret;
}

//=============================================================================
// class process_definition method definitions

/**
	Constructor for a process definition symbol table entry.  
 */
process_definition::process_definition(const name_space* o, 
		const string& s, const bool d,
		template_formals_set* tf) : 
		definition_base(s, o, tf), def(d),
		port_formals() {
	// fill me in...
}

process_definition::~process_definition() {
	// fill me in...
}

ostream&
process_definition::what(ostream& o) const {
	return o << "process-definition";
}

const definition_base*
process_definition::set_context_definition(context& c) const {
	return c.set_inst_proc_def(*this);
}

const fundamental_type_reference*
process_definition::set_context_fundamental_type(context& c) const {
	process_type_reference* dtr = new process_type_reference(this,
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

//=============================================================================
// non-member functions related to process_definition

//=============================================================================
// class process_instantiation method definitions

process_instantiation::process_instantiation(const scopespace& o, 
		const process_type_reference& pt,
		const string& n) : 
		instantiation_base(o, n), type(&pt) {
}

process_instantiation::~process_instantiation() {
}

ostream&
process_instantiation::what(ostream& o) const {
	return o << "process-inst";
}

const fundamental_type_reference*
process_instantiation::get_type_ref(void) const {
	return type;
}

/**
	Create a process reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
instance_reference_base*
process_instantiation::make_instance_reference(context& c) const {
	cerr << "process_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	process_instance_reference* ret = 
		new process_instance_reference(*this);
		// omitting index argument
	assert(ret);
	return ret;
}

//=============================================================================
// class param_instantiation method definitions

param_instantiation::param_instantiation(const scopespace& o, 
		const param_type_reference& pt, const string& n, 
		const param_expr* i) :
		instantiation_base(o, n), type(&pt), ival(i) {
}

param_instantiation::~param_instantiation() {
	SAFEDELETE(ival);
}

ostream&
param_instantiation::what(ostream& o) const {
	return o << "param-inst";
}

const fundamental_type_reference*
param_instantiation::get_type_ref(void) const {
	return type;
}

/**
	Initializes a parameter instance with an expression.
	The ival may only be initialized once, enforced by assertions.  
	Note: a parameter is considered "usable" if it is 
	initialized OR it is a template formal.  
	\param e the rvalue expression.
	\sa is_initialized
 */
void
param_instantiation::initialize(const param_expr* e) {
	assert(!ival);
	assert(e);
	ival = e;
}

/**
	Create a param reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
instance_reference_base*
param_instantiation::make_instance_reference(context& c) const {
	cerr << "param_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	param_instance_reference* ret = 
		new param_instance_reference(*this);
		// omitting index argument
	assert(ret);
	return ret;
}

//=============================================================================
// class channel_instantiation method definitions

channel_instantiation::channel_instantiation(const scopespace& o, 
		const channel_type_reference& ct,
		const string& n) :
		instantiation_base(o, n), type(&ct) {
}

channel_instantiation::~channel_instantiation() {
}

ostream&
channel_instantiation::what(ostream& o) const {
	return o << "channel-inst";
}

const fundamental_type_reference*
channel_instantiation::get_type_ref(void) const {
	return type;
}

/**
	Create a channel reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
 */
instance_reference_base*
channel_instantiation::make_instance_reference(context& c) const {
	cerr << "channel_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	channel_instance_reference* ret = 
		new channel_instance_reference(*this);
		// omitting index argument
	assert(ret);
	return ret;
}

//=============================================================================
// class single_instance_reference method definitions

single_instance_reference::~single_instance_reference() {
	SAFEDELETE(array_indices);
}

string
single_instance_reference::hash_string(void) const {
	string ret(get_inst_base()->get_qualified_name());
	if (array_indices) {
		array_index_list::const_iterator i;
		ret += "[";
		for (i=array_indices->begin(); i!=array_indices->end(); i++) {
			assert(*i);
			ret += (*i)->hash_string();
			ret += ",";		// extra comma at end
		}
		ret += "]";
	}
	return ret;
}

//=============================================================================
// class collective_instance_reference method definitions

collective_instance_reference::collective_instance_reference(
		const instance_reference_base* b, 
		const param_expr* l, const param_expr* r) :
		instance_reference_base(), 
		lower_index(l), upper_index(r) {
}

collective_instance_reference::~collective_instance_reference() {
}

ostream&
collective_instance_reference::what(ostream& o) const {
	return o << "collective-inst-ref";
}

string
collective_instance_reference::hash_string(void) const {
	string ret(base_array->hash_string());
	ret += "[";
	ret += lower_index->hash_string();
	if (upper_index) {
		ret += "..";
		ret += upper_index->hash_string();
	}
	ret += "]";
	return ret;
}

//=============================================================================
// class param_instance_reference method definitions

const instantiation_base*
param_instance_reference::get_inst_base(void) const {
	return param_inst_ref;
}

ostream&
param_instance_reference::what(ostream& o) const {
	return o << "param-inst-ref";
}

//=============================================================================
// class process_instance_reference method definitions

const instantiation_base*
process_instance_reference::get_inst_base(void) const {
	return process_inst_ref;
}

ostream&
process_instance_reference::what(ostream& o) const {
	return o << "process-inst-ref";
}

//=============================================================================
// class datatype_instance_reference method definitions

const instantiation_base*
datatype_instance_reference::get_inst_base(void) const {
	return data_inst_ref;
}

ostream&
datatype_instance_reference::what(ostream& o) const {
	return o << "datatype-inst-ref";
}

//=============================================================================
// class channel_instance_reference method definitions

const instantiation_base*
channel_instance_reference::get_inst_base(void) const {
	return channel_inst_ref;
}

ostream&
channel_instance_reference::what(ostream& o) const {
	return o << "channel-inst-ref";
}

//=============================================================================
};	// end namespace entity
};	// end namespace ART

