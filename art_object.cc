// "art_object.cc"

#include "art_parser.h"
#include "map_of_ptr_template_methods.h"

// CAUTION on ordering of the following two include files!
// including "art_object.h" first will cause compiler to complain
// about redefinition of struct hash<> template upon specialization of
// hash<string>.  

#include "hashlist_template_methods.h"
	// includes "list_of_ptr_template_methods.h"
#include "art_object.h"


//=============================================================================
// template specialization, needed by some compilers

//=============================================================================
namespace ART {
namespace entity {
//-----------------------------------------------------------------------------
// class name_space methods

/**
	Constructor for a new namespace.  
	Inherits from its parents: type aliases to built-in types, 
	such as bool and int.  
	\param n the name.  
	\param p pointer to the parent namespace.  
	\sa inherit_built_in_types
 */
name_space::name_space(const string& n, name_space* p) : 
		object(), parent(p), key(n), 
		subns(), open_spaces(), open_aliases(), 
		type_defs(), type_insts(),
		proc_defs(), proc_insts() {
	inherit_built_in_types();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The only only memory we need to delete is that owned by this namespace, 	namely, the subns subnamespace map.  We created, thus we delete.  
	All other pointers are shared pointers, and will be deleted by
	their respective owners.  
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
	object* probe = used_id_map[n];
	if (probe) {
		ret = IS_A(name_space*, probe);
		// an alias may return with valid pointer!
		if (!ret) {
			probe->what(cerr << " ... already taken as a ")
				<< ", ERROR! ";
			return NULL;
		} else if (open_aliases[n]) {
		// we have a valid namespace pointer, 
		// now we see if this is an alias, or true sub-namespace
			cerr << " ... already an open alias, ERROR! ";
			return NULL;
		} else {
		// therefore, ret is a pointer to a valid sub-namespace
			cerr << " ... already exists as subspace, re-opening";
			assert(ret == subns[n]);
			// we can return this
		}
	} else {
		// consistency check: 
		// since we're keeping subns and used_id_map consistent, 
		// not finding it in used_id_map => must not exist in subns!
		assert(!subns[n]);

		// create it, linking this as its parent
		cerr << " ... creating new";
		ret = new name_space(n, this);
		subns[n] = ret;		// store it in map of sub-namespaces
		used_id_map[n] = ret;	// register it as a used id
	}

	// silly sanity checks
	assert(ret);
	assert(ret->parent == this);
	assert(ret->key == n);
	cerr << " with parent: " << ret->parent->key;
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
name_space*
name_space::leave_namespace(void) {
	// for all open_aliases, release their names from used-map
	subns_map_type::iterator i = open_aliases.begin();
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
name_space*
name_space::add_using_alias(const id_expr& n, const string& a) {
	object* probe;
	name_space* ret;
	namespace_list::iterator i;
	namespace_list candidates;		// empty list

	cerr << endl << "adding using-alias in space: " 
		<< get_qualified_name() << " as " << a;

	probe = used_id_map[a];
	if (probe) {
		// then already, it conflicts with some other id
		// we report the conflict precisely as follows:
		ret = IS_A(name_space*, probe);
		if (ret) {
			if(subns[a]) {
				cerr << " ... already a sub-namespace, ERROR! ";
			} else if(open_aliases[a]) {
				cerr << " ... already an open alias, ERROR! ";
			} else {
				// cannot possibly be anything else!
				cerr << "WTF!!???";
			}
		} else {
			probe->what(cerr << " ... already declared ")
				<< ", ERROR! ";
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
	\param id the qualified/scoped name of the namespace to match.
	\return pointer to found namespace.
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
	// always search these unconditionally? or only if not found so far?
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
name_space::query_type_def_match(type_def_list& m, const string& tid) {
	cerr << endl << "query_type_def_match: " << tid
		<< " in " << get_qualified_name();
	{
		type_definition* ret = type_defs[tid];
		if (ret) m.push_back(ret);
	}
	// always search these unconditionally? or only if not found so far?
	if (m.empty()) {
		// with open namespaces list
		namespace_list::const_iterator i = open_spaces.begin();
		for ( ; i!=open_spaces.end(); i++) {
			type_definition* ret = (*i)->type_defs[tid];
			if (ret) m.push_back(ret);
		}
		// don't search aliased imports
	}

	// until list is not empty, keep querying parents
#if 1
	if (m.empty() && parent)
#endif
		parent->query_type_def_match(m, tid);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This lookup function returns a pointer to some object that belongs to 
	this scope, be it a namespace, process, definition, or instantiation.
	This query will not look in higher namespaces or imported namespaces.
	we want to forbid reuse of identifiers among different classes
	in the same namespace, but allow local names to overshadow
	identifiers in other namespaces.  
	\param id the hash key.  
	\return pointer to object indexed by hash key, otherwise NULL 
		if object there doesn't already exist.  
 */
inline
object*
name_space::
what_is(const string& id) {
	return used_id_map[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Finds a namespace ending with a (optionally) scoped identifier
	(allows multiple matches, hence the use of a list reference).
	Currently not used.  
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a built-in type definition, generally reserved only for use 
	with the global scope.  
 */
built_in_type_def*
name_space::add_built_in_type_definition(built_in_type_def* d) {
if (parent) {
	cerr << "Adding of built-in types is reserved for the "
		<< "global namespace only!";
	return NULL;
} else {
	assert(d);
	string k = d->get_name();
	object* probe = used_id_map[k];
	assert(!probe);
	// else "ERROR: identifier already taken, failed to add built-in type!";

	// type_defs owns this type is reponsible for deleting it
	type_defs[k] = d;
	used_id_map[k] = d;
	return d;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
type_definition*
name_space::add_type_alias(const id_expr& t, const string& a) {
	return NULL;
/*** not done yet
	type_definition* ret;
	object* probe;
	namespace_list::iterator i;
	namespace_list candidates;		// empty list

	cerr << endl << "adding type-alias in space: " 
		<< get_qualified_name() << " as " << a;

	probe = used_id_map[a];
	if (probe) {
		// then already, it conflicts with some other id
		probe->what(cerr << " ... already declared ")
			<< ", ERROR! ";
		return NULL;
	}

	// else we're ok to proceed to add alias
	// first find the referenced type name...
	query_type_def_match(candidates, t);
	i = candidates.begin();

	switch (candidates.size()) {
	// if list's size > 1, ambiguity
	// else if list is empty, unresolved type
	// else we've narrowed it down to one
		case 1: {
			ret = (*i);
			type_defs[a] = ret;
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
			cerr << " ERROR: ambiguous type alias, " << 
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
	Public interface to lookup a single type_definition.  
	This version take a single identifier string.  
	Will need to make sure that template params of matched type is null.  
	\param id the unqualified name of the type
	\return pointer to matched type_definition, only if it unique, 
		otherwise returns NULL
	\sa query_type_def_match
 */

// const?
type_definition*
name_space::lookup_unique_type(const string& id) {
	// const
	type_definition* ret;
	type_def_list::iterator i;
	type_def_list candidates;

	query_type_def_match(candidates, id);

	i = candidates.begin();
	switch (candidates.size()) {
		case 1: {
			ret = (*i);
			break;
			}
		case 0:	{
			cerr << " ... not found, ERROR! ";
			ret = NULL;
			break;	// no matches
			}
		default: {	// > 1
			ret = NULL;
			cerr << " ERROR: ambiguous type definition, " << 
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
	Create an instance of a data type and add it local symbol table.  
	First checks to see if name is already taken in the used_id_map.  
	If it collides with anything, then error.  
	Doesn't check ever other namespace (imports, parents...)
	because local identifiers are allowed to overshadow.  
	\param t the type of the instance.
	\param id the names of the instance.  
 */
type_instantiation*
name_space::add_type_instantiation(const type_definition& t, 
		const string& id) {
	object* probe;
	type_instantiation* new_inst;
	probe = used_id_map[id];
	if (probe) {
		probe->what(cerr << " ... already declared ")
			<< ", ERROR! ";
		return NULL;
	}
	// consistency check
	assert(!type_insts[id]);
	// else safe to proceed
	new_inst = new type_instantiation(this, &t, id);
	assert(new_inst);
	type_insts[id] = new_inst;
	used_id_map[id] = new_inst;
	return new_inst;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This searches the parent for built-in types and creates local 
	aliases to them under the same name, for accelerated type resolution.  
	Types that qualify for inheritance are either built-in types, 
	which should only be in the global namespace, or aliases thereof.  
	To be used only by the constructor, hence private.  
 */
void
name_space::inherit_built_in_types(void) {
if (parent) {
	type_def_set::iterator i = parent->type_defs.begin();
	for ( ; i!=parent->type_defs.end(); i++) {
		const type_definition* t;
		t = (*i).second;
		assert(t);

		// is t is an alias, resolve first
		const type_alias* a = IS_A(const type_alias*, t);
		if (a) {
			t = a->resolve_canonical();
		}	// else proceed

		if (IS_A(const built_in_type_def*, t)) {
			assert(!used_id_map[(*i).first]);
			type_alias* new_alias = 
				new type_alias(this, (*i).first, t);
			assert(new_alias);
			type_defs[(*i).first] = new_alias;
		} 
		// else don't add
	}
} // end if (parent)
}

//=============================================================================
// class type_definition method definitions

inline
type_definition::type_definition(const name_space* o, const string& n) :
	definition(o), key(n) {
}

type_definition::~type_definition() {
}

string
type_definition::get_qualified_name(void) const {
	return owner->get_qualified_name() +scope +key;
}

string
type_definition::get_name(void) const {
	return key;
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
		const type_definition* t) :
		type_definition(o, n), canonical(t) {
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
const type_definition*
type_alias::resolve_canonical(void) const {
	return canonical;
}

ostream&
type_alias::what(ostream& o) const {
	return o << "aliased-type: " << key;
}


//=============================================================================
// class built_in_type_def method definitions

// doesn't like inlining this, linker can't find definition on gcc-3.3
// inline
built_in_type_def::built_in_type_def(const name_space* o, const string& n) :
	type_definition(o, n) {
}

built_in_type_def::~built_in_type_def() { }

ostream&
built_in_type_def::what(ostream& o) const {
	return o << key;
}

//=============================================================================
// class user_type_def methods

/// constructor for user defined type
user_type_def::user_type_def(const name_space* o, const string& name) :
	type_definition(o, name), template_params(), members() {
}

ostream&
user_type_def::what(ostream& o) const {
	return o << "used-defined-type: " << key;
}

//=============================================================================
// class type_instantiation method definitions

type_instantiation::type_instantiation(const name_space* o, 
		const type_definition* t, const string& n) : 
		instantiation(o), type(t), key(n) {
}

type_instantiation::~type_instantiation() { }

ostream&
type_instantiation::what(ostream& o) const {
	return o << "type-inst";
}

//=============================================================================
// class process_definition method definitions

process_definition::process_definition(const name_space* o) : 
		definition(o) {
	// fill me in...
}

process_definition::~process_definition() {
	// fill me in...
}

ostream&
process_definition::what(ostream& o) const {
	return o << "process-definition";
}

//=============================================================================
// class process_instantiation method instantiations

process_instantiation::process_instantiation(const name_space* o) : 
		instantiation(o) {
	// fill me in...
}

process_instantiation::~process_instantiation() {
	// fill me in...
}

ostream&
process_instantiation::what(ostream& o) const {
	return o << "process-inst";
}

//=============================================================================
};
};

