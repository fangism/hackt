// "art_object.cc"

#include <iostream>

#include "art_parser_debug.h"		// need this?
#include "art_parser.h"
#include "art_symbol_table.h"

// CAUTION on ordering of the following two include files!
// including "art_object.h" first will cause compiler to complain
// about redefinition of struct hash<> template upon specialization of
// hash<string>.  

#include "hash_specializations.h"		// substitute for the following
#include "hashlist_template_methods.h"

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
// general non-member function definitions

// TO DO: give useful error messages on failure
excl_const_ptr<static_array_index_list>
make_static_array_index_list(const array_index_list& a) {
	excl_ptr<static_array_index_list>
		ret(new static_array_index_list);
	array_index_list::const_iterator i = a.begin();
	for ( ; i!=a.end(); i++) {
		assert(i->first);	// else bad arg!
		if (!i->first->is_static_constant())	// return NULL
			return excl_const_ptr<static_array_index_list>();
		if (i->second) {	// N..M
			if (!i->second->is_static_constant())	// return NULL
				return excl_const_ptr<static_array_index_list>();
			int min = i->first->static_constant_int();
			int max = i->second->static_constant_int();
			if (max >= min)
				ret->push_back(static_range_type(min, max));
			else return excl_const_ptr<static_array_index_list>();
		} else {		// N equiv. 0..N-1
			int max = i->first->static_constant_int();
			if (max > 0)
				ret->push_back(static_range_type(0, max -1));
			else return excl_const_ptr<static_array_index_list>();
		}
	}
	return excl_const_ptr<static_array_index_list>(ret);
}

//=============================================================================
// class object_handle method definitions

ostream&
object_handle::what(ostream& o) const {
	return obj.what(o);
}

ostream&
object_handle::dump(ostream& o) const {
	return obj.dump(o << "(handle)");
}

//=============================================================================
// class instance_collection_stack_item method definitions

bool
instance_collection_stack_item::static_overlap(
		const instance_collection_stack_item& c) const {
	return false;
}

//=============================================================================
// class static_collection_addition method definitions

static_collection_addition::static_collection_addition(
		excl_const_ptr<static_array_index_list>& i) :
	parent(), indices(i) {
}

bool
static_collection_addition::static_overlap(
		const instance_collection_stack_item& c) const {
	const static_collection_addition* s = 
		IS_A(const static_collection_addition*, &c);
	if (s) {
		// dimensionality should match, or else!
		assert(indices->size() == s->indices->size());
		static_array_index_list::const_iterator i = indices->begin();
		static_array_index_list::const_iterator j = s->indices->begin();
		for ( ; i!=indices->end(); i++, j++) {
			// chec for index overlap in all dimensions
			const static_range_type& ir = *i;
			const static_range_type& jr = *j;
			if (jr.first <= ir.first && ir.first <= jr.second ||
				jr.first <= ir.second && ir.second <= jr.second ||
				ir.first <= jr.first && jr.first <= ir.second ||
				ir.first <= jr.second && jr.second <= ir.second)
				continue;
			else return false;
			// if there is any dimension without overlap, false
		}
		// else there is some overlap in all dimensions
		return true;
	} else {	// argument is dynamic, not static
		// conservatively return false
		return false;
	}
}

//=============================================================================
// class dynamic_collection_addition method definitions

dynamic_collection_addition::dynamic_collection_addition(
		excl_const_ptr<array_index_list>& i) :
		parent(), indices(i) {
}

//=============================================================================
#if 0
// class sparse_index_collection method definitions

sparse_index_collection::sparse_index_collection(const int dim) :
		static_instances(
			static_instance_set_type::
			make_multidimensional_sparse_set(dim)),
		dynamic_instances() {
}

sparse_index_collection::~sparse_index_collection() { }
#endif

//=============================================================================
// class scopespace method definitions
scopespace::scopespace(const string& n, never_const_ptr<scopespace> p) : 
		object(), parent(p), key(n), 
		used_id_map(), connect_assign_list() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
scopespace::~scopespace() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for unqualified identifier.  
	Doesn't care what sub-type the object actually is.  
	This variation only searches the current namespace, and 
	never searches the parents' scopes.  
	NOTE: MUST static_cast map as a const to guarantee
	that map will not be modified.  
	Can inline this.  
	\param id the unqualified name of the object sought.  
	\return an object with the same name, if found.  
 */
never_const_ptr<object>
scopespace::lookup_object_here(const string& id) const {
	return static_cast<const used_id_map_type&>(used_id_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as regular map lookup, but returning a modifiable
	pointer.  
 */
never_ptr<object>
scopespace::lookup_object_here_with_modify(const string& id) const {
	return static_cast<const used_id_map_type&>(used_id_map)[id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for unqualified identifier.  
	Doesn't care what sub-type the object actually is.  
	The variation also queries parents's namespace and returns
	the first match found from an upward search.  
	(Consider making id a string? for cache-type/expr lookups?)
	\param id the unqualified name of the object sought.  
	\return an object with the same name, if found.  
 */
never_const_ptr<object>
scopespace::lookup_object(const string& id) const {
	never_const_ptr<object> o = lookup_object_here(id);
	if (o) return o;
	else if (parent) return parent->lookup_object(id);
	else return never_const_ptr<object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for a qualified identifier.
	If id is only a single identifier, and it is not absolute, 
	then it is considered an unqualified identifier.  
 */
never_const_ptr<object>
scopespace::lookup_object(const qualified_id_slice& id) const {
if (id.is_absolute()) {
	if (parent)
		return parent->lookup_object(id);
	else {	// we are the ROOT, start looking down namespaces
		qualified_id_slice idc(id);
		never_const_ptr<name_space> ns = 
			lookup_namespace(idc.betail()).is_a<name_space>();
		if (ns)
			return ns->lookup_object(**id.rend());
		else return never_const_ptr<object>(NULL);
	}
} else if (id.size() <= 1) {
	return lookup_object(**id.begin());
} else {
	// else need to resolve namespace portion first
	qualified_id_slice idc(id);
	never_const_ptr<name_space> ns = 
		lookup_namespace(idc.betail()).is_a<name_space>();
	if (ns)
		return ns->lookup_object(**id.rend());
	else return never_const_ptr<object>(NULL);
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
	\return pointer to the scope or namespace matched if found.  
 */
never_const_ptr<scopespace>
scopespace::lookup_namespace(const qualified_id_slice& id) const {
	return parent->lookup_namespace(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Registers a new instance of some type in the used_id_map.  
	Need to check for collisions, with previously declared instances.  
	For collectives, such as sparse arrays and dense arrays, 
	redeclarations with new indices are permitted, as long as it 
	can't be immediately determined that indices overlap.  
	Types must match for collective additions of course.  
	(What if type-parameters depend on variables?  allow?)
	\param i the new instance, possibly with sparse indices.
	\return pointer to newly created instance if successful, 
		else NULL.  
 */
never_const_ptr<instantiation_base>
scopespace::add_instance(excl_ptr<instantiation_base> i) {
	never_ptr<object> probe(
		lookup_object_here_with_modify(i->get_name()));
	if (probe) {
		never_ptr<instantiation_base> probe_inst(
			probe.is_a<instantiation_base>());
		if (probe_inst) {
			// compare types, must match!
			never_const_ptr<fundamental_type_reference> old_type(
				probe_inst->get_type_ref());
			never_const_ptr<fundamental_type_reference> new_type(
				i->get_type_ref());
			// For now, compare pointers?  
			// too strict, but more than sufficient for now.  
			// Eventually will have to do real comparison
			// based on contents.  
			if (!old_type->may_be_equivalent(new_type)) {
				cerr << "ERROR: type of redeclaration of "
					<< i->get_name() << " does not match "
					"previous declaration: " << endl <<
					"\twas: ";
				old_type->dump(cerr) << ", got: ";
				new_type->dump(cerr) << endl;
				return never_const_ptr<instantiation_base>(
					NULL);
			}	// else good to continue
			
			// compare dimensions
			if (!probe_inst->dimensions()) {
				// if original declaration was not collective, 
				// then one cannot add more.  
				probe->dump(cerr) << " was originally declared "
					"as a single instance, and thus may "
					"not be extended or re-declared, "
					"ERROR!" << endl;
				return never_const_ptr<instantiation_base>(
					NULL);
			} else if (probe_inst->dimensions()!=i->dimensions()) {
				probe->dump(cerr) << " was originally declared "
					"as a " << probe_inst->dimensions() <<
					"-D array, so the new declaration "
					"cannot add a " << i->dimensions() <<
					"-D array, ERROR!" << endl;
				return never_const_ptr<instantiation_base>(
					NULL);
			}	// else dimensions match apropriately

			// here, we know we're referring to the same collection
			// check for overlap with existing static-const indices
			if (probe_inst->merge_index_ranges(i)) {
				// returned true if there is definite overlap
				cerr << "detected overlap in indices in the "
					"collection addition for " <<
					i->get_name() << ", ERROR!" << endl;
				return never_const_ptr<instantiation_base>(
					NULL);
			}
			// else didn't detect static conflict.  
			// We discard the new instantiation, i, 
			// and let it delete itself at the end of this scope.  
			// ... happy ending, or is it?
			return probe_inst;
		} else {
			probe->what(cerr << i->get_name() <<
				" is already declared ") << ", ERROR!";
			return never_const_ptr<instantiation_base>(NULL);
		}
	} else {
		// didn't exist before, just add new instance
		never_const_ptr<instantiation_base> ret(i);
		used_id_map[i->get_name()] = i;		// transfer ownership
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void
scopespace::add_connection_to_scope(
		excl_const_ptr<connection_assignment_base> c) {
	connect_assign_list.push_back(c);
}

//=============================================================================
// class name_space method definitions

/**
	Constructor for a new namespace.  
	Inherits from its parents: type aliases to built-in types, 
	such as bool and int.  
	TO DO: is parent REDUNDANT?
	\param n the name.  
	\param p pointer to the parent namespace.  
 */
name_space::name_space(const string& n, never_const_ptr<name_space> p) : 
		scopespace(n, p), 
		parent(p), 
		open_spaces(), open_aliases() {
}

/**
	Constructor for the global namespace, which is the only
	namespace without a parent.  
	Written here instead of re-using the above constructor with
	default argument because old compilers can't accept
	default arguments (NULL) for class object formals.  
 */
name_space::name_space(const string& n) :
		scopespace(n, never_const_ptr<scopespace>(NULL)),
			// NULL parent
		parent(NULL), 
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
	// default destructors will take care of everything
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
never_const_ptr<name_space>
name_space::get_global_namespace(void) const {
	if (parent)
		return parent->get_global_namespace();
	else	// no parent
		return never_const_ptr<name_space>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
name_space::what(ostream& o) const {
	return o << "entity::namespace";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Spill contents of the used_id_map.
	\param o the output stream.
	\return the same output stream.
 */
ostream&
name_space::dump(ostream& o) const {
	used_id_map_type::const_iterator i;
//	list<never_const_ptr<...> > bin;		// later sort
	o << "In namespace \"" << key << "\", we have: {" << endl;
	for (i=used_id_map.begin(); i!=used_id_map.end(); i++) {
		o << "  " << i->first << " = ";
//		i->second->what(o) << endl;		// 1 level for now
		i->second->dump(o) << endl;
	}
	return o << "}" << endl;
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
never_ptr<name_space>
name_space::add_open_namespace(const string& n) {
	never_ptr<name_space> ret;
	never_const_ptr<object> probe = lookup_object_here(n);
	if (probe) {
		never_const_ptr<name_space> probe_ns(probe.is_a<name_space>());
		// an alias may return with valid pointer!
		if (!probe_ns) {
			probe->what(cerr << n << " is already declared as a ")
				<< ", ERROR! ";
			return never_ptr<name_space>(NULL);
		} else if (open_aliases[n]) {
		// we have a valid namespace pointer, 
		// now we see if this is an alias, or true sub-namespace
			cerr << n << " is already declared an open alias, ERROR! ";
			return never_ptr<name_space>(NULL);
		} else {
		// therefore, probe_ns is a pointer to a valid sub-namespace
			DEBUG(TRACE_NAMESPACE_NEW, 
				cerr << n << " is already exists as subspace, re-opening")

			ret = lookup_object_here_with_modify(n).is_a<name_space>();

			assert(probe_ns->key == ret->key);
		}
		assert(ret);
	} else {
		// create it, linking this as its parent
		DEBUG(TRACE_NAMESPACE_NEW, cerr << " ... creating new")
		excl_ptr<name_space> new_ns(
			new name_space(n, never_const_ptr<name_space>(this)));
		ret = new_ns;
		assert(ret);
		assert(new_ns.owned());
		// register it as a used id
		used_id_map[n] = new_ns;
			// explicit transfer
		assert(!new_ns);
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
never_const_ptr<name_space>
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
never_const_ptr<name_space>
name_space::add_using_directive(const qualified_id& n) {
	never_const_ptr<name_space> ret;
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
		case 1: 
			ret = (*i);
			open_spaces.push_back(ret);
			break;
		case 0:	{
			cerr << "namespace " << n << " not found, ERROR! ";
			// or n is not a namespace
			ret = never_const_ptr<name_space>(NULL);
			break;	// no matches
			}
		default: {	// > 1
			ret = never_const_ptr<name_space>(NULL);
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
never_const_ptr<name_space>
name_space::add_using_alias(const qualified_id& n, const string& a) {
	never_const_ptr<object> probe;
	never_const_ptr<name_space> ret;
	namespace_list::const_iterator i;
	namespace_list candidates;		// empty list

	DEBUG(TRACE_NAMESPACE_ALIAS, 
		cerr << endl << "adding using-alias in space: " 
			<< get_qualified_name() << " as " << a)

	// need to force use of the constant version of the lookup
	// because this method is non-const.  
	// else it will modify the used_id_map!
	// perhaps wrap with a probe() const method...
	probe = lookup_object_here(a);
	if (probe) {
		probe = never_const_ptr<object>(&probe->self());
		// resolve handles
	}
	if (probe) {
		// then already, it conflicts with some other id
		// we report the conflict precisely as follows:
		ret = probe.is_a<name_space>();
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
		return never_const_ptr<name_space>(NULL);
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
			// SOMETHING GOES WRONG HERE, FIX ME FIRST!!!
			ret = (*i);
			open_aliases[a] = ret;
			// ret is owned by the namespace where it belongs
			// don't need to own it here in our used_id_map
			// or even open_aliases.  
			// however, used_id_map is non-const, 
			// so we need to wrap it in a const object_handle.  
			used_id_map[a] = excl_ptr<object_handle>(
				new object_handle(ret));
			break;
			}
		case 0:	{
			cerr << " ... not found, ERROR! ";
			ret = never_const_ptr<name_space>(NULL);
			break;	// no matches
			}
		default: {	// > 1
			ret = never_const_ptr<name_space>(NULL);
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
	TO DO: re-use query_subnamespace_match
	\param id the qualified/scoped name of the namespace to match.
	\return pointer to found namespace.
 */
never_const_ptr<name_space>
name_space::query_namespace_match(const qualified_id_slice& id) const {
	// qualified_id_slice is a wrapper around qualified_id
	// recall that qualified_id is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	DEBUG(TRACE_NAMESPACE_QUERY, 
		cerr << "query_namespace_match: " << id 
			<< " in " << get_qualified_name() << endl)

	if (id.empty())	{	// what if it's absolute and empty?
		return (id.is_absolute()) ? get_global_namespace() : 
			never_const_ptr<name_space>(this);
	}
	qualified_id_slice::const_iterator i = id.begin();	assert(*i);
//	never_const_ptr<token_identifier> tid(*i);
	count_const_ptr<token_identifier> tid(*i);
	assert(tid);
	DEBUG(TRACE_NAMESPACE_SEARCH, cerr << "\ttesting: " << *tid)
	never_const_ptr<name_space> ns =
		(id.is_absolute()) ? get_global_namespace()
		: never_const_ptr<name_space>(this);
	if (ns->key.compare(*tid)) {
		// if names differ, already failed, try alias spaces
		return never_const_ptr<name_space>(NULL);
	} else {
		for (i++; ns && i!=id.end(); i++) {
			never_const_ptr<name_space> next;
			// no need to skip scope tokens anymore
			tid = i->is_a<token_identifier>();
			assert(tid);
			DEBUG(TRACE_NAMESPACE_SEARCH, cerr << scope << *tid)
			// the [] operator of map<> doesn't have const 
			// semantics, even if looking up an entry!
			next = ns->lookup_object_here(*tid).is_a<name_space>();
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
never_const_ptr<name_space>
name_space::query_subnamespace_match(const qualified_id_slice& id) const {
	// qualified_id_slice is just a wrapper around qualified_id
	// recall that qualified_id is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
	DEBUG(TRACE_NAMESPACE_QUERY, 
		cerr << endl << "query_subnamespace_match: " << id 
			<< " in " << get_qualified_name() << endl)

	// here, does NOT check for global-absoluteness
	if (id.empty())	{	// what if it's absolute and empty?
		return (id.is_absolute()) ? get_global_namespace() : 
			never_const_ptr<name_space>(this);
	}
	qualified_id_slice::const_iterator i = id.begin();
//	never_const_ptr<token_identifier> tid(*i);
	count_const_ptr<token_identifier> tid(*i);
	assert(tid);
	DEBUG(TRACE_NAMESPACE_SEARCH, cerr << "\ttesting: " << *tid)
	// no check for absoluteness
	never_const_ptr<name_space> ns;
	if (id.is_absolute()) {
		ns = get_global_namespace()->
			lookup_object_here(*tid).is_a<name_space>();
	} else {
		// force use of const probe
		never_const_ptr<object> probe(lookup_object_here(*tid));
		ns = probe.is_a<name_space>();
	}

	if (!ns) {				// else lookup in aliases
		ns = open_aliases[*tid];	// replaced for const semantics
	}
	for (i++; ns && i!=id.end(); i++) {
		never_const_ptr<name_space> next;
		tid = i->is_a<token_identifier>();
		assert(tid);
		DEBUG(TRACE_NAMESPACE_SEARCH, cerr << scope << *tid)
		next = ns->lookup_object_here(*tid).is_a<name_space>();
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
		never_const_ptr<name_space> ret = query_subnamespace_match(id);
		if (ret) m.push_back(ret);
	}
	// always search these unconditionally? or only if not found so far?
	{	// with open namespaces list
		namespace_list::const_iterator i = open_spaces.begin();
		for ( ; i!=open_spaces.end(); i++) {
			never_const_ptr<name_space> ret = 
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
	never_const_ptr<name_space> ret = query_subnamespace_match(id);
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
never_ptr<definition_base>
name_space::add_definition(excl_ptr<definition_base> db) {
	assert(db);
	string k = db->get_name();
	never_const_ptr<object> probe = lookup_object_here(k);
	if (probe) {
		probe->what(cerr << "ERROR: identifier already taken by ")
			<< "  Failed to add definition!";
		return never_ptr<definition_base>(NULL);
	} else {
		// used_id_map owns this type is reponsible for deleting it
		never_ptr<definition_base> ret = db;
		assert(db);
		assert(ret);
		assert(ret == db);
		// explicit transfer!
		used_id_map[k] = excl_ptr<definition_base>(db);
		assert(!db);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds an alias for a type, like typedef in C.  
	Later: template typedefs, ooooh!
 */
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

	probe = lookup_object_here(a);
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
			// will need excl_ptr
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
never_const_ptr<fundamental_type_reference>
name_space::add_type_reference(excl_ptr<fundamental_type_reference> tb) {
	never_const_ptr<object> o;
	never_const_ptr<fundamental_type_reference> trb;
	assert(tb);
	string k = tb->hash_string();
	o = lookup_object_here(k);
	// see what is already there...
	trb = o.is_a<fundamental_type_reference>();

	if (o)	assert(trb);
	// else a non-type-reference hashed into the used_id_map
	// using a hash for a type-reference!!!

	if (trb) {
		// then found a match!, we can delete tb
		// since tb is an excl_ptr, it will delete itself
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
		never_const_ptr<fundamental_type_reference> ret(tb);
		assert(tb);
		used_id_map[k] = tb;
		assert(!tb);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overrides scopespace::lookup_namespace.
	\param id is the entire name of the namespace.
	\return pointer to the scope or namespace matched if found.  
 */
never_const_ptr<scopespace>
name_space::lookup_namespace(const qualified_id_slice& id) const {
	return query_subnamespace_match(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks to see whether process definition or prototype already 
	exists.  
	\param s the name of the process in this namespace.  
	\return valid pointer to process_definition if found, 
		else NULL if not found.  
 */
never_const_ptr<process_definition>
name_space::probe_process_definition(const string& s) const {
	return lookup_object_here(s).is_a<process_definition>();
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
never_ptr<process_definition>
name_space::add_proc_declaration(const token_identifier& pname) {
	never_ptr<process_definition> pd;
	never_const_ptr<object> probe = lookup_object_here(pname);
	if (probe) {
		// something already exists with name...
		never_const_ptr<process_definition> probe_pd = 
			probe.is_a<process_definition>();
		if (probe_pd) {
			// see if this declaration matches EXACTLY
			// or punt check until check_build() on the templates
			//	and ports?
			return lookup_object_here_with_modify(pname)
				.is_a<process_definition>();
		} else {
			// already declared as something else in this scope.
			probe->what(cerr << pname << " is already declared as ")
				<< ", ERROR! ";
			return never_ptr<process_definition>(NULL);
		}
	} else {
		// slot is free, allocate new entry for process definition
		excl_ptr<process_definition> new_pd(
			new process_definition(
				never_const_ptr<name_space>(this), 
					pname, false));
		pd = new_pd;
		// template formals? later? earlier?
		assert(new_pd);
		assert(pd);
		used_id_map[pname] = new_pd;
		assert(!new_pd);
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
never_ptr<process_definition>
name_space::add_proc_definition(const token_identifier& pname) {
	never_ptr<process_definition> pd;
	// look up used_id_map
	never_const_ptr<object> probe(used_id_map[pname]);
	if (probe) {
		// something already exists with name...
		never_const_ptr<process_definition> probe_pd(
			probe.is_a<process_definition>());
		if (probe_pd) {
			if (probe_pd->is_defined()) {
				cerr << pname << " process already defined.  "
					"redefinition starting at " 
					<< pname.where();
				return never_ptr<process_definition>(NULL);
			} else {
			// probably already declared
			// punt check, until traversing templates/ports
				return used_id_map[pname]
					.is_a<process_definition>();
			}
		} else {
			// already declared as something else in this scope.
			probe->what(cerr << pname << " is already declared as ")
				<< ", ERROR! ";
			return never_ptr<process_definition>(NULL);
		}
	} else {
		// slot is free, allocate new entry for process definition
		excl_ptr<process_definition> new_pd(new process_definition(
			never_const_ptr<name_space>(this), pname, true));
		pd = new_pd;
		assert(new_pd);
		assert(pd);
		used_id_map[pname] = new_pd;
		assert(!new_pd);		// was transferred away
	}
	return pd;
}

//=============================================================================
// class definition_base method definitions

/**
	Definition basic constructor.  
	\param p the parent scope, a namespace.  
 */
inline
definition_base::definition_base(const string& n,
		never_const_ptr<name_space> p, 
		template_formals_set* tf) : 
		scopespace(n, p), template_formals(tf) {
	// synchronize template formals with used_id_map
}

inline
definition_base::~definition_base() {
}

ostream&
definition_base::dump(ostream& o) const {
	return what(o);
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
			never_const_ptr<param_instantiation> p(*i);
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

never_const_ptr<definition_base>
definition_base::resolve_canonical(void) const {
	return never_const_ptr<definition_base>(this);
}

/**
	Adds an instantiation to the current definition's scope, and 
	also registers it in the list of template formals for 
	template argument checking.  
	What if template formal is an array, or collective?
	TO DO: convert to pointer-classes...
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
//	const object* probe = used_id_map[pf->get_name()];
	const object* probe = used_id_map[pf->get_name()].unprotected_const_ptr();
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return NULL;
	}

	const never_const_ptr<param_instantiation>* ret =
		template_formals->append(pf->hash_string(),
			never_const_ptr<param_instantiation>(pf));
	assert(!ret);
	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!

	// COMPILE: pf is const, but used_id_map members are not
	// wrap around with object_handle?
	used_id_map[pf->hash_string()] = excl_ptr<param_instantiation>(pf);
	// later return a never_ptr<>
	return pf;
}

//=============================================================================
// class fundamental_type_reference method definitions

fundamental_type_reference::fundamental_type_reference(
		excl_ptr<template_param_list> pl)
		: type_reference_base(), 
		template_params(pl) {
}

fundamental_type_reference::fundamental_type_reference(void) :
		type_reference_base(), template_params() {
		// NULL
}

fundamental_type_reference::~fundamental_type_reference() {
}

ostream&
fundamental_type_reference::dump(ostream& o) const {
	return o << hash_string();
}

/**
	Evaluates type reference as a flat string, for caching purposes.  
	We unconditionally add the <> to the key even if there is no template
	specifier to guarantee that hash_string for a type-reference 
	cannot collide with the hash string for the non-templated definition.  
	Should we use fully qualified names for hashing?
	\return string to be used for hashing.  
 */
string
fundamental_type_reference::hash_string(void) const {
	// use fully qualified?  for hashing, no.
	// possible collision case?
	return get_base_def()->get_name() +template_param_string();
}

string
fundamental_type_reference::template_param_string(void) const {
	string ret("<");
	if (template_params) {
		template_param_list::const_iterator i =
			template_params->begin();
		never_const_ptr<param_expr> e(*i);
		if (e)	ret += e->hash_string();
		for (i++ ; i!=template_params->end(); i++) {
			ret += ",";		// add commas?
			e = *i;
			if (e)	ret += e->hash_string();
			// can e ever be NULL? yes...
		}
	}
	ret += ">";
	return ret;
}

string
fundamental_type_reference::get_qualified_name(void) const {
	return get_base_def()->get_qualified_name() +template_param_string();
}

never_const_ptr<fundamental_type_reference>
fundamental_type_reference::set_context_type_reference(context& c) const {
	return c.set_current_fundamental_type(*this);
}

/**
	Returns true if the types *may* be equivalent.  
	Easy for non-template types, but for template types, 
	sometimes parameters may be determined by other parameters
	and thus are not statically constant for comparison.  
	\return false if there is definite mismatch in type.  
	TO DO: static argument checking for template arguments.  
 */
bool
fundamental_type_reference::may_be_equivalent(
		never_const_ptr<fundamental_type_reference> t) const {
	assert(t);
	never_const_ptr<definition_base> left(get_base_def());
	never_const_ptr<definition_base> right(t->get_base_def());
	if (left != right) {
		cerr << "pointers: left = " << left.unprotected_const_ptr() <<
			", right = " << right.unprotected_const_ptr() << endl;
		return false;
	}
	// TO DO: compare template arguments
	return true;
}

//=============================================================================
// class collective_type_reference method definitions

collective_type_reference::collective_type_reference(
		const type_reference_base& b, 
		never_const_ptr<array_index_list> d) :
		type_reference_base(), base(&b), dim(d) {
}

collective_type_reference::~collective_type_reference() {
	// we don't own the members
}

ostream&
collective_type_reference::what(ostream& o) const {
	return o << "collective-type-ref";
}

ostream&
collective_type_reference::dump(ostream& o) const {
	return what(o);			// temporary
}

//=============================================================================
// class data_type_reference method definitions

data_type_reference::data_type_reference(
		never_const_ptr<datatype_definition> td) :
		fundamental_type_reference(), 
		base_type_def(td) {
	assert(base_type_def);
}

data_type_reference::data_type_reference(
		never_const_ptr<datatype_definition> td, 
		excl_ptr<template_param_list> pl) :
		fundamental_type_reference(pl), 
		base_type_def(td) {
	assert(base_type_def);
}

data_type_reference::~data_type_reference() {
}

ostream&
data_type_reference::what(ostream& o) const {
	return o << "data-type-reference";
}

#if 0
ostream&
data_type_reference::dump(ostream& o) const {
	return what(o);
}
#endif

// const definition_base*
never_const_ptr<definition_base>
data_type_reference::get_base_def(void) const {
	return base_type_def;
}

/**
	Creates an instance of a data type, 
	and adds it to a scope.
	TO DO: move all error checking into scopespace::add_instance
		for unification.  
		Handle cases for additions to sparse collections.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
never_const_ptr<instantiation_base>
data_type_reference::add_instance_to_scope(scopespace& s,
		const token_identifier& id) const {
	// make sure doesn't collide with something in s.
	// what if s is a loop-scope, not a namespace?  PUNT!
#if 1
	// PHASE OUT, PHASE INTO s.add_instance()
	never_const_ptr<object> probe = s.lookup_object_here(id);
	if (probe) {
		probe->what(cerr << id << " is already declared ") <<
			", ERROR! " << id.where() << endl;
		return never_const_ptr<instantiation_base>(NULL);
	}
	// else proceed
#endif
	excl_ptr<datatype_instantiation> di(
		new datatype_instantiation(s, *this, id));
	assert(di);
	return s.add_instance(di.as_a<instantiation_base>());
}

//=============================================================================
// class channel_type_reference method definitions

/**
	Concrete channel type reference.  
	\param cd reference to a channel definition.
	\param pl (optional) parameter list for templates.  
 */
channel_type_reference::channel_type_reference(
		never_const_ptr<channel_definition> cd, 
		excl_ptr<template_param_list> pl) :
		fundamental_type_reference(pl), 
		base_chan_def(cd) {
	assert(base_chan_def);
}

channel_type_reference::channel_type_reference(
		never_const_ptr<channel_definition> cd) :
		fundamental_type_reference(), 	// NULL
		base_chan_def(cd) {
	assert(base_chan_def);
}

channel_type_reference::~channel_type_reference() {
}

ostream&
channel_type_reference::what(ostream& o) const {
	return o << "channel-type-reference";
}

#if 0
ostream&
channel_type_reference::dump(ostream& o) const {
	return what(o);
}
#endif

// const definition_base*
never_const_ptr<definition_base>
channel_type_reference::get_base_def(void) const {
	return base_chan_def;
}

/**
	Creates an instance of a channel type, 
	and adds it to a scope.
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
// const instantiation_base*
never_const_ptr<instantiation_base>
channel_type_reference::add_instance_to_scope(scopespace& s,
		const token_identifier& id) const {
	// make sure doesn't collide with something in s.
	excl_ptr<channel_instantiation> ci(
		new channel_instantiation(s, *this, id));
	assert(ci);
	return s.add_instance(ci.as_a<instantiation_base>());
}

//=============================================================================
// class process_type_reference method definitions

process_type_reference::process_type_reference(
		never_const_ptr<process_definition> pd) :
		fundamental_type_reference(), 
		base_proc_def(pd) {
	assert(base_proc_def);
}

process_type_reference::process_type_reference(
		never_const_ptr<process_definition> pd, 
		excl_ptr<template_param_list> pl) :
		fundamental_type_reference(pl), 
		base_proc_def(pd) {
	assert(base_proc_def);
}

process_type_reference::~process_type_reference() {
}

ostream&
process_type_reference::what(ostream& o) const {
	return o << "process-type-reference";
}

#if 0
ostream&
process_type_reference::dump(ostream& o) const {
	return what(o);
}
#endif

// const definition_base*
never_const_ptr<definition_base>
process_type_reference::get_base_def(void) const {
	return base_proc_def;
}

/**
	Creates an instance of a process type, 
	and adds it to a scope.
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
// const instantiation_base*
never_const_ptr<instantiation_base>
process_type_reference::add_instance_to_scope(scopespace& s,
		const token_identifier& id) const {
	// make sure doesn't collide with something in s.
	excl_ptr<process_instantiation> ci(
		new process_instantiation(s, *this, id));
	assert(ci);
	return s.add_instance(ci.as_a<instantiation_base>());
}

//=============================================================================
// class param_type_reference method definitions

param_type_reference::param_type_reference(
		never_const_ptr<built_in_param_def> pd) : 
		fundamental_type_reference(), 	// NULL
		base_param_def(pd) {
	assert(base_param_def);
}

param_type_reference::~param_type_reference() {
}

ostream&
param_type_reference::what(ostream& o) const {
	return o << "param-type-reference";
}

#if 0
ostream&
param_type_reference::dump(ostream& o) const {
	return what(o);
}
#endif

never_const_ptr<definition_base>
param_type_reference::get_base_def(void) const {
	return base_param_def;
}

/**
	Creates an instance of a parameter type, 
	and adds it to a scope.
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
// const instantiation_base*
never_const_ptr<instantiation_base>
param_type_reference::add_instance_to_scope(scopespace& s,
		const token_identifier& id) const {
	// make sure doesn't collide with something in s.
	excl_ptr<param_instantiation> pi(
		new param_instantiation(s, *this, id));
	assert(pi);
	return s.add_instance(pi.as_a<instantiation_base>());
}

//=============================================================================
// class instantiation_base method definitions

/**
	Instantiation base constructor.  
	The first time an instance is declared, its dimensions are
	set by the array-dimension list, if provided, else 0.
	The first set of indices given will be pushed onto the 
	instance collection stack.  
	\param o the owning scope.  
	\param n the name of the instance (collection).
	\param d initial collection of indices, already resolved 
		as param_expr's.  
 */
// inline
instantiation_base::instantiation_base(const scopespace& o, 
		const string& n, const array_index_list* d) : 
		object(), owner(never_const_ptr<scopespace>(&o)),
		key(n),
//		array_dimensions(d)
		index_collection(), 
		depth(d ? d->size() : 0) {
if (d) {
	instance_collection_stack_item* coll;
	// then construct... depends on cases in the following order:
	// 4) indices all resolve to static constants
		// make static_collection_addition
// FIX ME
	excl_const_ptr<static_array_index_list> sci;	// (
//		make_static_array_index_list(*d));
	if (sci) {
		count_const_ptr<instance_collection_stack_item> icsp(
			new static_collection_addition(sci));
		index_collection.push_back(icsp);
		return;
	}

	// 3) indices depend on parameter formals
		// make dynamic_collection_addition

	// 2) instantiation is conditional
		// make conditional_collection_addition, using enclosing 
		// conditional_scope
		// PUNT!

	// 1) indices depend on induction variables
		// make loop_collection_addition, using enclosing loop_scope
		// PUNT!

	// Then, push constructed instance_collection_stack_item onto stack.
}
}

inline
instantiation_base::~instantiation_base() {
}

ostream&
instantiation_base::dump(ostream& o) const {
	return what(o);
}

string
instantiation_base::get_qualified_name(void) const {
	if (owner)
		return owner->get_qualified_name() +scope +key;
	else return key;
}

/**
	Grabs the current top of the deque of the index collection, 
	so the encapsulating instance reference know what
	instances were visibie at the time of reference.  
	QUESTION: what if it's empty because it is not collective?
 */
instantiation_base::index_collection_type::const_iterator
instantiation_base::current_collection_state(void) const {
	return index_collection.begin();
}

/**
	
	\return true if the new range *definitely* overlaps with previous
		static constant ranges.  Comparisons with dynamic ranges
		will conservatively return false; they will be resolved
		at unroll-time.  Also returns true if there was an error.  
 */
bool
instantiation_base::detect_static_overlap(
		index_collection_item_ptr_type r) const {
	assert(r);
	assert(r->dimensions() == depth);
	if (r.is_a<static_collection_addition>()) {
	index_collection_type::const_iterator i = index_collection.begin();
	for ( ; i!=index_collection.end(); i++) {
		if ((*i)->static_overlap(*r)) {
			return true;
		}
	}
	// if this point reached, then return false
	} // else just return false, can't know statically without analysis
	return false;
}

/**
	If this instance is a collection, add the new range of indices
	which may be sparse or dense.  
	This is only applicable if this instantiation was initialized
	as a collective.  
	The dimensions better damn well match!  
	\param r the index ranges to be added.  
	\return true if error condition. 
	\sa detect_static_overlap
 */
bool
instantiation_base::add_index_range(index_collection_item_ptr_type r) {
	assert(r);
	if (depth) {
		if (r->dimensions() == depth) {
			bool overlap = detect_static_overlap(r);
			index_collection.push_back(r);
			if (overlap) {
				cerr << "ERROR: detected static constant "
				"overlap in indices -- "
				"reinstantiation collision." << endl;
			}
			return overlap;
		} else {
			cerr << "ERROR: " << key << " was originally declared "
				"a " << depth << "-dimension collection, thus "
				"you cannot append with a " 
				<< r->dimensions() <<
				"-dimension index range!" << endl;
			return true;
		}
	} else {
		cerr << "ERROR: " << key << " was originally declared "
			"as a single instance, not a collective, and hence, "
			"may not be appended!" << endl;
		return true;
	}
}

/**
	Merges index ranges from another instantiation base, 
	such as a redeclaration with more indices of the same collection.  
	\return true if there is definite overlap, signaling an error.  
 */
bool
instantiation_base::merge_index_ranges(never_const_ptr<instantiation_base> i) {
	assert(i);
	// check type equality here, or push responsibility to caller?
	bool err = false;
	index_collection_type::const_reverse_iterator iter =
		i->index_collection.rbegin();
	for ( ; iter!=i->index_collection.rend(); iter++) {
		if (add_index_range(*iter)) {
			err = true;
		}
	}
	return err;
}

/**
OBSOLETE
void
instantiation_base::set_array_dimensions(excl_ptr<array_index_list> d) {
	// just in case, delete what was previously there
	array_dimensions = d;
}
**/

/**
	OBSOLETE.
	Determines whether or not the dimensions of two instantiation_base
	arrays are equivalent, and hence compatible.  
	\param i the second instantiation_base to compare against.
	\return false for now.
bool
instantiation_base::array_dimension_match(const instantiation_base& i) const {
	// TO DO: this is temporary
	// if both lists are not NULL, iterate through lists...
	return false;
}
**/

//=============================================================================
// class datatype_definition method definitions

// make sure that this constructor is never invoked outside this file
inline
datatype_definition::datatype_definition(
		never_const_ptr<name_space> o,
		const string& n, 
		template_formals_set* tf) :
		definition_base(n, o, tf) {
}

inline
datatype_definition::~datatype_definition() {
}

never_const_ptr<definition_base>
datatype_definition::set_context_definition(context& c) const {
	return c.set_current_definition_reference(*this);
}

never_const_ptr<fundamental_type_reference>
datatype_definition::set_context_fundamental_type(context& c) const {
	data_type_reference* dtr = new data_type_reference(
		never_const_ptr<datatype_definition>(this),
		c.get_current_template_arguments());
	assert(dtr);
	// CACHE the type_reference...
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

//=============================================================================
// class channel_definition method definitions

// make sure that this constructor is never invoked outside this file
inline
channel_definition::channel_definition(
		never_const_ptr<name_space> o, 
		const string& n, 
		template_formals_set* tf) :
		definition_base(n, o, tf) {
}

channel_definition::~channel_definition() {
}

never_const_ptr<definition_base>
channel_definition::set_context_definition(context& c) const {
	return c.set_current_definition_reference(*this);
}

never_const_ptr<fundamental_type_reference>
channel_definition::set_context_fundamental_type(context& c) const {
	channel_type_reference* dtr = new channel_type_reference(
		never_const_ptr<channel_definition>(this),
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
	Later: allow template typdefs!
	\param o the namespace to which this belongs.  
	\param n the name of the aliased type.  
	\param t pointer to the actual type being aliased.  
 */
type_alias::type_alias(
		never_const_ptr<name_space> o, 
		const string& n, 
		never_const_ptr<definition_base> t, 
		template_formals_set* tf) :
		definition_base(n, o, tf),
		canonical(t->resolve_canonical()) {
	assert(canonical);
	// just in case t is not a canonical type, i.e. another alias...
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
never_const_ptr<definition_base>
type_alias::resolve_canonical(void) const {
	return canonical;
}

ostream&
type_alias::what(ostream& o) const {
	return o << "aliased-type: " << key;
}

//=============================================================================
// class built_in_datatype_def method definitions

// doesn't like inlining this, linker can't find definition on gcc-3.3
// is used in "art_symbol_table.cc", unless you -fkeep-inline-functions
built_in_datatype_def::built_in_datatype_def(
		never_const_ptr<name_space> o, 
		const string& n) :
		datatype_definition(o, n) {
}

built_in_datatype_def::~built_in_datatype_def() { }

ostream&
built_in_datatype_def::what(ostream& o) const {
	return o << key;
}

never_const_ptr<definition_base>
built_in_datatype_def::set_context_definition(context& c) const {
	return c.set_current_definition_reference(*this);
}

never_const_ptr<fundamental_type_reference>
built_in_datatype_def::set_context_fundamental_type(context& c) const {
	data_type_reference* dtr = new data_type_reference(
		never_const_ptr<built_in_datatype_def>(this),
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
	never_const_ptr<built_in_datatype_def> b = 
		t.resolve_canonical().is_a<built_in_datatype_def>();
	if (b) {
		// later: check template signature! (for int<>)
		return key == b->key;
	} else {
		return false;
	}
}

//=============================================================================
// class built_in_param_def method definitions

built_in_param_def::built_in_param_def(
		never_const_ptr<name_space> p,
		const string& n) :
		definition_base(n, p) {
}

built_in_param_def::~built_in_param_def() {
}

ostream&
built_in_param_def::what(ostream& o) const {
	return o << key;
}

never_const_ptr<definition_base>
built_in_param_def::set_context_definition(context& c) const {
	return c.set_current_definition_reference(*this);
}

never_const_ptr<fundamental_type_reference>
built_in_param_def::set_context_fundamental_type(context& c) const {
	param_type_reference* dtr = new param_type_reference(
		never_const_ptr<built_in_param_def>(this));
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

//=============================================================================
// class user_def_datatype method definitions

/// constructor for user defined type
user_def_datatype::user_def_datatype(
		never_const_ptr<name_space> o,
		const string& name) :
		datatype_definition(o, name), 
		template_params(), members() {
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
	never_const_ptr<user_def_datatype> u = 
		t.resolve_canonical().is_a<user_def_datatype>();
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

never_const_ptr<fundamental_type_reference>
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
never_const_ptr<instance_reference_base>
datatype_instantiation::make_instance_reference(context& c) const {
	cerr << "datatype_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	count_ptr<datatype_instance_reference> new_ir(
		new datatype_instance_reference(*this));
		// omitting index argument
#if	UNIFIED_OBJECT_STACK
	c.push_object_stack(new_ir);
#else
	c.push_instance_reference_stack(new_ir);
#endif
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class process_definition method definitions

/**
	Constructor for a process definition symbol table entry.  
 */
process_definition::process_definition(
		never_const_ptr<name_space> o, 
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

never_const_ptr<definition_base>
process_definition::set_context_definition(context& c) const {
	return c.set_current_definition_reference(*this);
}

never_const_ptr<fundamental_type_reference>
process_definition::set_context_fundamental_type(context& c) const {
	process_type_reference* dtr = new process_type_reference(
		never_const_ptr<process_definition>(this),
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

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

never_const_ptr<fundamental_type_reference>
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
never_const_ptr<instance_reference_base>
process_instantiation::make_instance_reference(context& c) const {
	cerr << "process_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	count_ptr<process_instance_reference> new_ir(
		new process_instance_reference(*this));
		// omitting index argument
#if	UNIFIED_OBJECT_STACK
	c.push_object_stack(new_ir);
#else
	c.push_instance_reference_stack(new_ir);
#endif
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class param_instantiation method definitions

param_instantiation::param_instantiation(const scopespace& o, 
		const param_type_reference& pt, const string& n, 
		const param_expr* i) :
		instantiation_base(o, n), type(&pt), ival(i) {
}

param_instantiation::~param_instantiation() {
}

ostream&
param_instantiation::what(ostream& o) const {
	return o << "param-inst";
}

never_const_ptr<fundamental_type_reference>
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
param_instantiation::initialize(count_const_ptr<param_expr> e) {
	assert(!ival);
	assert(e);
	ival = e;
}

count_const_ptr<param_expr>
param_instantiation::default_value(void) const {
	return ival;
}

/**
	Create a param reference object.
	See if it's already registered in the current context.  
	If so, delete the new one (inefficient), 
	and return the one found.  
	Else, register the new one in the context, and return it.  
	Depends on context's method for checking references in used_id_map.  
	Different: param type reference are always referred to in the global
		scope because they cannot be templated!
		Therefore, cache them in the global (or built-in) namespace.  
	\return NULL.
 */
never_const_ptr<instance_reference_base>
param_instantiation::make_instance_reference(context& c) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	count_ptr<param_instance_reference> new_ir(
		new param_instance_reference(
			never_ptr<param_instantiation>(
			const_cast<param_instantiation*>(this))));
		// omitting index argument
#if	UNIFIED_OBJECT_STACK
	c.push_object_stack(new_ir);
#else
	c.push_instance_reference_stack(new_ir);
#endif
	return never_const_ptr<instance_reference_base>(NULL);
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

never_const_ptr<fundamental_type_reference>
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
never_const_ptr<instance_reference_base>
channel_instantiation::make_instance_reference(context& c) const {
	cerr << "channel_instantiation::make_instance_reference() "
		"INCOMPLETE, FINISH ME!" << endl;
	// depends on whether this instance is collective, 
	//	check array dimensions.  
	count_ptr<channel_instance_reference> new_ir(
		new channel_instance_reference(*this));
		// omitting index argument
#if	UNIFIED_OBJECT_STACK
	c.push_object_stack(new_ir);
#else
	c.push_instance_reference_stack(new_ir);
#endif
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class single_instance_reference method definitions

single_instance_reference::~single_instance_reference() {
}

ostream&
single_instance_reference::dump(ostream& o) const {
	o << get_inst_base()->get_name();
	if (array_indices) {
		o << "[";
		array_index_list::const_iterator i;
		i=array_indices->begin();
		for ( ; i!=array_indices->end(); i++) {
			(*i)->dump(o) << ",";
		}
		o << "]";
	}
	return o;
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
		never_const_ptr<instance_reference_base> b, 
		const param_expr* l, const param_expr* r) :
		instance_reference_base(), 
		lower_index(never_const_ptr<param_expr>(l)),
		upper_index(never_const_ptr<param_expr>(r)) {
}

collective_instance_reference::~collective_instance_reference() {
}

ostream&
collective_instance_reference::what(ostream& o) const {
	return o << "collective-inst-ref";
}

ostream&
collective_instance_reference::dump(ostream& o) const {
	return what(o);
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

/**
	\param pi needs to be modifiable for later initialization.  
 */
param_instance_reference::param_instance_reference(
		never_ptr<param_instantiation> pi,
		array_index_list* i) :
		single_instance_reference(i), param_inst_ref(pi) {
}

never_const_ptr<instantiation_base>
param_instance_reference::get_inst_base(void) const {
	return param_inst_ref;
}

ostream&
param_instance_reference::what(ostream& o) const {
	return o << "param-inst-ref";
}

/**
	For single instances references, check whether it is initialized.  
	For collective instance references, and indexed references, 
	just conservatively say that it hasn't been initialized it; 
	so don't bother checking until unroll time.  
 */
bool
param_instance_reference::is_initialized(void) const {
	if (param_inst_ref->dimensions() > 0)
		return false;
	else return param_inst_ref->is_initialized();
}

void
param_instance_reference::initialize(count_const_ptr<param_expr> i) {
	param_inst_ref->initialize(i);
}

//=============================================================================
// class process_instance_reference method definitions

never_const_ptr<instantiation_base>
process_instance_reference::get_inst_base(void) const {
	return process_inst_ref;
}

ostream&
process_instance_reference::what(ostream& o) const {
	return o << "process-inst-ref";
}

//=============================================================================
// class datatype_instance_reference method definitions

never_const_ptr<instantiation_base>
datatype_instance_reference::get_inst_base(void) const {
	return data_inst_ref;
}

ostream&
datatype_instance_reference::what(ostream& o) const {
	return o << "datatype-inst-ref";
}

ostream&
datatype_instance_reference::dump(ostream& o) const {
	return what(o);
}

//=============================================================================
// class channel_instance_reference method definitions

never_const_ptr<instantiation_base>
channel_instance_reference::get_inst_base(void) const {
	return channel_inst_ref;
}

ostream&
channel_instance_reference::what(ostream& o) const {
	return o << "channel-inst-ref";
}

ostream&
channel_instance_reference::dump(ostream& o) const {
	return what(o);
}

//=============================================================================
// class param_expression_assignment method definitions

param_expression_assignment::param_expression_assignment() :
		connection_assignment_base(), ex_list() {
}

param_expression_assignment::~param_expression_assignment() { }

void
param_expression_assignment::append_param_expression(
		count_const_ptr<param_expr> e) {
	ex_list.push_back(e);
}

void
param_expression_assignment::prepend_param_expression(
		count_const_ptr<param_expr> e) {
	ex_list.push_front(e);
}

//=============================================================================
// class instance_reference_connection method definitions

instance_reference_connection::instance_reference_connection() :
		connection_assignment_base(), inst_list() {
}

/**
	\param i instance reference to connect, may not be NULL.
 */
void
instance_reference_connection::append_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	assert(i);
	inst_list.push_back(i);
}

//=============================================================================
// class aliases_connection method definitions

aliases_connection::aliases_connection() : instance_reference_connection() { };

void
aliases_connection::prepend_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	assert(i);
	inst_list.push_front(i);
}

//=============================================================================
// class port_connection method definitions

/**
	Initial constructor for a port-connection.  
	\param i an instance of the definition that is to be connected.
 */
port_connection::port_connection(
		count_const_ptr<single_instance_reference> i) :
		instance_reference_connection(), inst(i) {
}

/**
	\param i instance reference to connect, may be NULL.
 */
void
port_connection::append_instance_reference(
		count_const_ptr<instance_reference_base> i) {
	// do not assert, may be NULL.  
	inst_list.push_back(i);
}

//=============================================================================
// class dynamic_connection_assignment method definitions

dynamic_connection_assignment::dynamic_connection_assignment(
		never_const_ptr<scopespace> s) :
		connection_assignment_base(), dscope(s) {
	// check that dscope is actually a loop or conditional
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

