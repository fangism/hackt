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
#include "art_built_ins.h"

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
// class object_list method definitions

object_list::object_list() : object(), parent() {
}

object_list::~object_list() {
}

ostream&
object_list::what(ostream& o) const {
	return o << "object-list";
}

ostream&
object_list::dump(ostream& o) const {
	what(o) << ":" << endl;
	parent::const_iterator i = begin();
	for ( ; i!=end(); i++)
		dump(o) << endl;
	return o;
}

/**
	Creates an instance_collection_item.  
	The restriction with this version is that each item must
	be a single pint_expr, not a range.  
	Walks list twice, once to see what the best classification is, 
	again to copy-create.  
	Intended for use in resolving dense array dimensions
	of formal parameters and formal ports.  
	Cannot possibly be loop-dependent or conditional!
 */
count_ptr<range_expr_list>
object_list::make_formal_dense_range_list(void) const {
	// initialize some bools to true
	// and set them false approriately in iterations
	bool err = false;
	bool is_pint_expr = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_ptr<pint_expr> p(i->is_a<pint_expr>());
		if (!p) {
			is_pint_expr = false;
			cerr << "non-int expression found where single int "
				"is expected in dense range declaration.  "
				"ERROR!  " << endl;	// where?
		} else {
			if (p->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (p->is_initialized()) {
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "int expression is not initialized.  "
					"ERROR!  " << endl;	// where?
				err = true;
			}
			// can it be initialized, but non-const?
			// yes, if a dimension depends on another formal param
			assert(p->is_loop_independent());
			assert(p->is_unconditional());
		}
	}
	if (err) {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		count_ptr<const_range_list> ret(new const_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			const int n = j->is_a<pint_expr>()
				->static_constant_int();
			if (n <= 0) {
				cerr << "Integer size for a dense array "
					"must be positive, but got: "
					<< n << ".  ERROR!  " << endl;
				// where? callers figure out...
				return count_ptr<const_range_list>(NULL);
			}
			ret->push_back(const_range(n));
		}
		return ret;
//		return index_collection_item_ptr_type(
//			new static_collection_addition(const_ret));
//		assert(!const_ret);
	} else if (is_initialized) {
		const_iterator j = begin();
		count_ptr<dynamic_range_list> ret(new dynamic_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			ret->push_back(count_ptr<pint_range>(
				new pint_range(j->is_a<pint_expr>())));
		}
		return ret;
//		excl_const_ptr<dynamic_range_list> const_ret(ret);
//		return index_collection_item_ptr_type(
//			new dynamic_collection_addition(const_ret));
	} else {
		cerr << "Failed to construct a formal dense range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
//		return index_collection_item_ptr_type(NULL);
	}
}

/**
	Creates an instance_collection_item.  
	No restriction on this, may be single integer or pint_range.  
	Will automatically convert to ranges.
	Walks list twice, once to see what the best classification is, 
	again to copy-create.  
	Intended for use in resolving dense array dimensions
	of formal parameters and formal ports.  
	Cannot possibly be loop-dependent or conditional!
 */
count_ptr<range_expr_list>
object_list::make_sparse_range_list(void) const {
	// initialize some bools to true
	// and set them false approriately in iterations
	bool err = false;
	bool is_valid_range = true;
	bool is_static_constant = true;
	bool is_initialized = true;
	const_iterator i = begin();
	int k = 1;
	for ( ; i!=end(); i++, k++) {
		if (!*i) {
			cerr << "Error in dimension " << k <<
				" of array indices.  " << endl;
			continue;
		}
		count_ptr<pint_expr> p(i->is_a<pint_expr>());
		// may be pint_const or pint_literal
#if 0
		if (i->is_a<pint_const>()) {		// sanity?
			cerr << "pint_const => pint_expr" << endl;
			assert(i->is_a<pint_expr>());
			assert((*i).is_a<pint_expr>());
		}
		if (i->is_a<const_range>()) {		// sanity?
			cerr << "const_range => range_expr" << endl;
			assert(i->is_a<range_expr>());
			assert((*i).is_a<range_expr>());
		}
#endif
		count_ptr<range_expr> r(i->is_a<range_expr>());
		// may be const_range or pint_range
		if (p) {
			// later modularize to some method function...
#if 0
			if (p.is_a<pint_const>()) {		// sanity?
				cerr << "yup, is a pint_const." << endl;
			}
#endif
			if (p->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (p->is_initialized()) {	// definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "int expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;	// where?
				err = true;
			}
			// can it be initialized, but non-const?
			// yes, if a dimension depends on another formal param
			// can be loop-independent, do we need to track?
			// can be conditional, do we need to track?
		} else if (r) {
			// same thing... copy
			if (r->is_static_constant()) {
				continue;
			} else {
				is_static_constant = false;
			}
			if (r->is_initialized()) {	// definite
				continue;
			} else {
				is_initialized = false;
				// not initialized! error.  
				cerr << "range expression is definitely "
					"not initialized.  ERROR!  "
					<< endl;	// where?
				err = true;
			}
		} else {
			// is neither pint_expr nor range_expr
			assert(!p && !r);
			assert(!i->is_a<pint_const>());
			assert(!i->is_a<const_range>());
			is_valid_range = false;
			(*i)->what(cerr << "Expected integer or range "
				"expression but got a ") << 
				" in dimension " << k << " of array ranges.  "
				"ERROR!  " << endl;	// where?
			err = true;
		}
	}
	if (err || !is_valid_range) {
		cerr << "Failed to construct a sparse range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
//		return index_collection_item_ptr_type(NULL);
	} else if (is_static_constant) {
		const_iterator j = begin();
		count_ptr<const_range_list> ret(new const_range_list);
		for ( ; j!=end(); j++) {
			// should be safe to do this, since we checked above
			count_ptr<pint_expr> p(j->is_a<pint_expr>());
			count_ptr<range_expr> r(j->is_a<range_expr>());
			// don't forget to range check
			if (p) {
				const int n = p->static_constant_int();
				if (n <= 0) {
					cerr << "Integer size for a dense array "
						"must be positive, but got: "
						<< n << ".  ERROR!  " << endl;
					// where? let caller figure out
					return count_ptr<const_range_list>(NULL);
				}
				ret->push_back(const_range(n));
			} else {
				assert(r);
				if (!r->is_sane()) {
					cerr << "Range is not valid. ERROR!  "
						<< endl;
					return count_ptr<const_range_list>(NULL);
				}
				count_ptr<const_range> cr(
					r.is_a<const_range>());
				if (cr) {
					// need deep copy, b/c not pointer list
					ret->push_back(const_range(*cr));
				} else {
					count_ptr<pint_range> pr(
						r.is_a<pint_range>());
					assert(pr);
					assert(pr->is_static_constant());
					ret->push_back(
						pr->static_constant_range());
				}
			}
		}
		return ret;
//		excl_const_ptr<const_range_list> const_ret(ret);
//		assert(const_ret);
//		assert(!ret);
//		return index_collection_item_ptr_type(
//			new static_collection_addition(const_ret));
//		assert(!const_ret);
	} else if (is_initialized) {
		const_iterator j = begin();
		count_ptr<dynamic_range_list> ret(new dynamic_range_list);
		for ( ; j!=end(); j++) {
			if (j->is_a<pint_expr>()) {
				// convert N to 0..N-1
				ret->push_back(count_ptr<pint_range>(
					new pint_range(j->is_a<pint_expr>())));
			} else {
				assert(j->is_a<pint_range>());
				ret->push_back(j->is_a<pint_range>());
			}
		}
		return ret;
//		excl_const_ptr<dynamic_range_list> const_ret(ret);
//		return index_collection_item_ptr_type(
//			new dynamic_collection_addition(const_ret));
	} else {
		cerr << "Failed to construct a sparse range list!  "
			<< endl;
		return count_ptr<range_expr_list>(NULL);
//		return index_collection_item_ptr_type(NULL);
	}
}

//=============================================================================
#if 0
OBSOLETE
// class instance_collection_stack_item method definitions

bool
instance_collection_stack_item::static_overlap(
		const instance_collection_stack_item& c) const {
	return false;
}

//=============================================================================
// class static_collection_addition method definitions

static_collection_addition::static_collection_addition(
//		excl_const_ptr<static_array_index_list>& i
		excl_const_ptr<const_range_list>& i) :
	parent(), indices(i) {
}

size_t
static_collection_addition::dimensions(void) const {
	return indices->size();
}

bool
static_collection_addition::static_overlap(
		const instance_collection_stack_item& c) const {
	const static_collection_addition* s = 
		IS_A(const static_collection_addition*, &c);
	if (s) {
		return indices->static_overlap(*s->indices);
	} else {	// argument is dynamic, not static
		// conservatively return false
		return false;
	}
}

//=============================================================================
// class dynamic_collection_addition method definitions

dynamic_collection_addition::dynamic_collection_addition(
		excl_const_ptr<dynamic_range_list>& i) :
		parent(), indices(i) {
}

size_t
dynamic_collection_addition::dimensions(void) const {
	return indices->size();
}
#endif

//=============================================================================
// class scopespace method definitions
scopespace::scopespace(const string& n, never_const_ptr<scopespace> p) : 
		object(), parent(p), key(n), 
		used_id_map(), connect_assign_list() {
	// note that parent may be NULL, is this ok?
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
	assert(i);
#if 0
	// DEBUG
	cerr << "In scopespace::add_instance with this = " << this << endl;
	i->dump(cerr << "excl_ptr<instantiation_base> i = ") << endl;
#endif
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
				new_type->dump(cerr) << " ERROR!  ";
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
					"ERROR!  ";
				return never_const_ptr<instantiation_base>(
					NULL);
			} else if (probe_inst->dimensions()!=i->dimensions()) {
				probe->dump(cerr) << " was originally declared "
					"as a " << probe_inst->dimensions() <<
					"-D array, so the new declaration "
					"cannot add a " << i->dimensions() <<
					"-D array, ERROR!  ";
				return never_const_ptr<instantiation_base>(
					NULL);
			}	// else dimensions match apropriately

			assert(i);	// sanity
			// here, we know we're referring to the same collection
			// check for overlap with existing static-const indices
			const_range_list overlap(
				probe_inst->merge_index_ranges(i));
			if (!overlap.empty()) {
				// returned true if there is definite overlap
				cerr << "Detected overlap in the "
					"sparse collection for " <<
					i->get_name() << ", precisely: ";
				overlap.dump(cerr);
				cerr << ".  ERROR!  ";
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
				" is already declared ") << ", ERROR!  ";
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
/**
	Returns the flattened name of this current namespace.  
 */
string
name_space::get_qualified_name(void) const {
	if (parent) {
		if (parent->parent)
			return parent->get_qualified_name() +scope +key;
		else	return key;
	} else return "<global>";		// global
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns pointer to global namespace by following parent pointers.
 */
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
			cerr << n << " is already declared an open alias, "
				"ERROR! ";
			return never_ptr<name_space>(NULL);
		} else {
		// therefore, probe_ns is a pointer to a valid sub-namespace
			DEBUG(TRACE_NAMESPACE_NEW, 
				cerr << n << " is already exists as subspace, "
					"re-opening")
			ret = lookup_object_here_with_modify(n)
				.is_a<name_space>();
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
				cerr << a << " is already an open alias, "
					"ERROR! ";
			} else {
				cerr << a << " is already a sub-namespace, "
					"ERROR! ";
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
	THIS IS KEY.  
	Adds a definition to this namespace.  
	Can be used by declaration prototypes as well.  
	Need to check whether existing definition is_defined, 
		as opposed to just being declared.  
	Definition is newly created, so used_id_map is responsible
	for deleting it.  
	On failure, however, pointer is not added, so need to handle
	memory in the caller.  
	
	\param db the definition to add, newly created.
	\return modifiable pointer to definition if successful, else NULL.  
 */
never_ptr<definition_base>
name_space::add_definition(excl_ptr<definition_base> db) {
	assert(db);
	string k = db->get_name();
	never_const_ptr<object> probe(lookup_object_here(k));
	if (probe) {
		never_const_ptr<definition_base> probe_def(
			probe.is_a<definition_base>());
		if (probe_def) {
			assert(k == probe_def->get_name());	// consistency
			if (probe_def->require_signature_match(db)) {
				// definition signatures match
				// can discard new declaration
				// db will self-delete (excl_ptr)
				return never_ptr<definition_base>(db);
			} else {
				// signature mismatch!
				// also catches class type mismatch
				cerr << "new declaration for " << k <<
					"doesn't match previous declaration. "
					"ERROR! ";
				// give details...
				return never_ptr<definition_base>(NULL);
			}
		} else {
			probe->what(cerr << "Identifier already taken by ")
				<< endl << "ERROR: Failed to add definition! ";
			return never_ptr<definition_base>(NULL);
		}
	} else {
		// used_id_map owns this type is reponsible for deleting it
		never_ptr<definition_base> ret = db;
		assert(ret);
		used_id_map[k] = db;
		assert(!db);		// after explicit transfer of ownership
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
		scopespace(n, p), template_formals(tf), defined(false) {
	// synchronize template formals with used_id_map
}

inline
definition_base::~definition_base() {
}

ostream&
definition_base::dump(ostream& o) const {
	return what(o) << " " << key;
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

/**
	Need template_formal_set to be a queryable-hashlist...
 */
never_const_ptr<param_instantiation>
definition_base::lookup_template_formal(const string& id) const {
	if (template_formals) {
		return never_const_ptr<param_instantiation>(
			(static_cast<const template_formals_set&>
			(*template_formals))[id]);
	} else {
		return never_const_ptr<param_instantiation>(NULL);
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
	Sub-classes only need to re-implement if behavior is different.  
	e.g. an assertion fail for built-in types.  
 */
never_const_ptr<definition_base>
definition_base::set_context_definition(context& c) const {
	return c.set_current_definition_reference(*this);
}

/**
	Only definition aliases will return a different pointer, 
	the one for the original definition.  
	Remember to use this in type-checking.  
 */
never_const_ptr<definition_base>
definition_base::resolve_canonical(void) const {
	return never_const_ptr<definition_base>(this);
}

/**
	DO ME NOW!
	Adds an instantiation to the current definition's scope, and 
	also registers it in the list of template formals for 
	template argument checking.  
	What if template formal is an array, or collective?
	TO DO: convert to pointer-classes...
	\param f needs to be a param_instantiation... what about array?
		need to be non-const? storing to hash_map_of_ptr...
		must be modifiable for used_id_map
 */
never_const_ptr<instantiation_base>
definition_base::add_template_formal(excl_ptr<instantiation_base> f) {
	never_const_ptr<param_instantiation> pf(
		f.is_a<param_instantiation>());
	assert(pf);
	if (!template_formals) {
		template_formals = new template_formals_set();
		assert(template_formals);
	}
	// check and make sure identifier wasn't repeated in formal list!
	never_const_ptr<object> probe(lookup_object_here(pf->get_name()));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_const_ptr<instantiation_base>(NULL);
	}

	const never_const_ptr<param_instantiation>* ret =
		template_formals->append(pf->hash_string(),
			never_const_ptr<param_instantiation>(pf));
	assert(!ret);
	// since we already checked used_id_map, there cannot be a repeat
	// in the template_formals_list!

	// COMPILE: pf is const, but used_id_map members are not
	// wrap around with object_handle?
	used_id_map[pf->hash_string()] = f;
	// later return a never_ptr<>
	return pf;
}

/**
	Virtually pure, not purely virtual...
	Only temporary.
	Override in appropriate subclasses.  
 */
never_const_ptr<instantiation_base>
definition_base::add_port_formal(excl_ptr<instantiation_base> f) {
	assert(0);
	return never_const_ptr<instantiation_base>(NULL);
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
		return false;
	}
	// TO DO: compare template arguments
	return true;
}

//=============================================================================
#if 0
MAY BE OBSOLETE
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
#endif

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

never_const_ptr<definition_base>
data_type_reference::get_base_def(void) const {
	return base_type_def;
}

/**
	Returns a newly constructed data instance object.  
	TO DO: move all error checking into scopespace::add_instance
		for unification.  
		Handle cases for additions to sparse collections.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
excl_ptr<instantiation_base>
data_type_reference::make_instantiation(never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new datatype_instantiation(*s, *this, id, d));
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

never_const_ptr<definition_base>
channel_type_reference::get_base_def(void) const {
	return base_chan_def;
}

/**
	Returns a newly constructed channel instance object.  
 */
excl_ptr<instantiation_base>
channel_type_reference::make_instantiation(never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new channel_instantiation(*s, *this, id, d));
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

never_const_ptr<definition_base>
process_type_reference::get_base_def(void) const {
	return base_proc_def;
}

/**
	Returns a newly constructed process instance object.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
excl_ptr<instantiation_base>
process_type_reference::make_instantiation(
		never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	return excl_ptr<instantiation_base>(
		new process_instantiation(*s, *this, id, d));
}

//=============================================================================
// class param_type_reference method definitions

/**
	Only used in construction of built-in types.  
 */
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

never_const_ptr<definition_base>
param_type_reference::get_base_def(void) const {
	return base_param_def;
}

/**
	Returns a newly constructed param instance object.  
	Sort of kludged... built-in type case... YUCK, poor style.  
	\param s the scope to which to add this instance.
	\param id the local name of this instance.  
	\return pointer to the created instance.  
 */
excl_ptr<instantiation_base>
param_type_reference::make_instantiation(never_const_ptr<scopespace> s, 
		const token_identifier& id, 
		index_collection_item_ptr_type d) const {
	// hard coded... yucky, but efficient.  
	if (this == &pbool_type)
		return excl_ptr<instantiation_base>(
			new pbool_instantiation(*s, id, d));
	else if (this == &pint_type)
		return excl_ptr<instantiation_base>(
			new pint_instantiation(*s, id, d));
	else {
		pbool_type.dump(cerr) << " at " << &pbool_type << endl;
		pint_type.dump(cerr) << " at " << &pint_type << endl;
		dump(cerr) << " at " << this << endl;
		assert(0);		// WTF?
		return excl_ptr<instantiation_base>(NULL);
	}
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
		const string& n,
		index_collection_item_ptr_type d) : 
		object(), owner(never_const_ptr<scopespace>(&o)),
		key(n),
		index_collection(), 
		depth(d ? d->dimensions() : 0) {
if (d) {
	index_collection.push_front(d);
} else {
	// push a NULL pointer?
}
}

instantiation_base::~instantiation_base() {
}

ostream&
instantiation_base::dump(ostream& o) const {
	get_type_ref()->dump(o) << " " << key;
	index_collection_type::const_iterator i = index_collection.begin();
	for ( ; i!=index_collection.end(); i++) {
		assert(*i);
		(*i)->dump(o) << endl;
	}
	return o;
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
	instances were visible at the time of reference.  
	QUESTION: what if it's empty because it is not collective?
		will begin() = end()? should be...
 */
instantiation_state
instantiation_base::current_collection_state(void) const {
	return index_collection.begin();
}

/**
	By "end", we mean the beginning of the collection state deque, 
	the first item added to the the collection stack.  
	Can't actually dereference the returned iterator, 
	it's only useful for ending looped iterations.  
 */
instantiation_state
instantiation_base::collection_state_end(void) const {
	return index_collection.end();
}

/**
	Will need two flavors: may and must?
	
	\return true if the new range *definitely* overlaps with previous
		static constant ranges.  Comparisons with dynamic ranges
		will conservatively return false; they will be resolved
		at unroll-time.  Also returns true if there was an error.  
		By "true", we mean a valid precise range of overlap.  
 */
const_range_list
instantiation_base::detect_static_overlap(
		index_collection_item_ptr_type r) const {
	assert(r);
	assert(r->dimensions() == depth);
#if 0
	// DEBUG
	cerr << "In instantiation_base::detect_static_overlap with this = "
		<< this << endl;
	r->dump(cerr << "index_collection_item_ptr_type r = ") << endl;
#endif
	if (r.is_a<const_range_list>()) {
	index_collection_type::const_iterator i = index_collection.begin();
	for ( ; i!=index_collection.end(); i++) {
		// return upon first overlap error
		// later accumulate all overlaps.  
		const_range_list ovlp((*i)->static_overlap(*r));
		if (!ovlp.empty()) {
			return ovlp;
		}
		// else keep checking...
	}
	// if this point reached, then return false
	} // else just return false, can't know statically without analysis
	return const_range_list();	// empty constructed list
}

/**
	If this instance is a collection, add the new range of indices
	which may be sparse or dense.  
	This is only applicable if this instantiation was initialized
	as a collective.  
	Pre-condition: The dimensions better damn well match!  
	\param r the index ranges to be added.  
	\return Overlapping range (true) if error condition. 
	\sa detect_static_overlap
 */
const_range_list
instantiation_base::add_index_range(index_collection_item_ptr_type r) {
	assert(r);
	assert(depth);
	assert(depth == r->dimensions());
#if 0
	// DEBUG
	cerr << "In instantiation_base::add_index_range with this = "
		<< this << endl;
	r->dump(cerr << "index_collection_item_ptr_type r = ") << endl;
#endif
	const_range_list overlap(detect_static_overlap(r));
	index_collection.push_back(r);
	return overlap;
}

/**
	Merges index ranges from another instantiation base, 
	such as a redeclaration with more indices of the same collection.  
	Pre-condition: dimensions of i must match this!
	The only type of error caught from here are overlap errors.  
	\return true if there is definite overlap, signaling an error.  
 */
const_range_list
instantiation_base::merge_index_ranges(never_const_ptr<instantiation_base> i) {
	assert(i);
	assert(dimensions() == i->dimensions());
#if 0
	// DEBUG
	cerr << "In instantiation_base::merge_index_range with this = " <<
		this << endl;
	i->dump(cerr << "never_const_ptr<instantiation_base> i = ") << endl;
#endif
	// check type equality here, or push responsibility to caller?
	index_collection_type::const_reverse_iterator iter =
		i->index_collection.rbegin();
	for ( ; iter!=i->index_collection.rend(); iter++) {
		const_range_list ret(add_index_range(*iter));
		if (!ret.empty())
			return ret;
		// else keep checking...
	}
	return const_range_list();
}

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
// class user_def_chan method definitions

user_def_chan::user_def_chan(never_const_ptr<name_space> o, 
		const string& name) : channel_definition(o, name) {
	// FINISH ME
}

user_def_chan::~user_def_chan() {
}

ostream&
user_def_chan::what(ostream& o) const {
	return o << "user-def-chan";
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

/**
	Built-in data type marks itself as already defined.  
 */
built_in_datatype_def::built_in_datatype_def(
		never_const_ptr<name_space> o, 
		const string& n) :
		datatype_definition(o, n) {
	mark_defined();
}

/**
	Same constructor, but with one template formal parameter.  
	This constructor is dedicated to int<pint width>.  
 */
built_in_datatype_def::built_in_datatype_def(
		never_const_ptr<name_space> o, 
		const string& n, 
		excl_ptr<param_instantiation> p) :
		datatype_definition(o, n) {
	add_template_formal(p.as_a<instantiation_base>());
	mark_defined();
}

built_in_datatype_def::~built_in_datatype_def() { }

ostream&
built_in_datatype_def::what(ostream& o) const {
	return o << key;
}

/**
	Built-in data types should never be opened for modification.  
	Assert fails.  
 */
never_const_ptr<definition_base>
built_in_datatype_def::set_context_definition(context& c) const {
	assert(0);
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

/**
	Built-in param marks itself as already defined.  
 */
built_in_param_def::built_in_param_def(
		never_const_ptr<name_space> p,
		const string& n, 
		const param_type_reference& t) :
		definition_base(n, p), type_ref(&t) {
	mark_defined();
}

built_in_param_def::~built_in_param_def() {
}

ostream&
built_in_param_def::what(ostream& o) const {
	return o << key;
}

/**
	Really this should never be called, as built-in definitions
	cannot be opened for modification.  
	Assert fails.
 */
never_const_ptr<definition_base>
built_in_param_def::set_context_definition(context& c) const {
	assert(0);
	return c.set_current_definition_reference(*this);
}

/**
	Consider built-in type references, is this even used?
	Now uses hard-coded param_type_references.  
	Kludge object comparison...
	Consider passing default type_reference into constructor
		as a forward pointer.  
 */
never_const_ptr<fundamental_type_reference>
built_in_param_def::set_context_fundamental_type(context& c) const {
	return c.set_current_fundamental_type(*type_ref);
}

//=============================================================================
// class enum_member method definitions

enum_member::enum_member(const string& n) : object(), id(n) { }

enum_member::~enum_member() { }

ostream&
enum_member::what(ostream& o) const {
	return o << "enum-member";
}

ostream&
enum_member::dump(ostream& o) const {
	return o << id;
}

//=============================================================================
// class enum_datatype_def method definitions

enum_datatype_def::enum_datatype_def(never_const_ptr<name_space> o, 
		const string& n) : 
		datatype_definition(o, n) {
}

enum_datatype_def::~enum_datatype_def() {
}

ostream&
enum_datatype_def::what(ostream& o) const {
	return o << key;
}

never_const_ptr<fundamental_type_reference>
enum_datatype_def::set_context_fundamental_type(context& c) const {
	data_type_reference* dtr = new data_type_reference(
		never_const_ptr<enum_datatype_def>(this));
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

/**
	Type equivalence of enumerated types:
	must point to same definition, namely this!
 */
bool
enum_datatype_def::type_equivalent(const datatype_definition& t) const {
	never_const_ptr<enum_datatype_def> b = 
		t.resolve_canonical().is_a<enum_datatype_def>();
	return b == this;
}

/**
	Not the same as type-equivalence, which requires precise 
	pointer equality.  
	This is used for comparing prototypes and signatures of 
	declarations and definitions.  
	Report errors to stderr or stdout?
	Template this?  Nah...
	\param d the definition (signature) to compare, 
		the name MUST match, else comparison is pointless!
 */
bool
enum_datatype_def::require_signature_match(
		never_const_ptr<definition_base> d) const {
	assert(d);
	assert(key == d->get_name());
	never_const_ptr<enum_datatype_def>
		ed(d.is_a<enum_datatype_def>());
	if (ed) {
		// only names need to match...
		// no other signature information!  easy.
		return true;
	} else {
		// class type doesn't even match!  report error.
		d->what(cerr << key << " is already declared as a ")
			<< " but is being redeclared as a ";
		what(cerr) << "  ERROR!  ";
		return false;
	}
}

/**
	\return true if successfully added, false if there was conflict.  
 */
bool
enum_datatype_def::add_member(const token_identifier& em) {
	never_const_ptr<object> probe(lookup_object_here(em));
	if (probe) {
		never_const_ptr<enum_member> probe_em(
			probe.is_a<enum_member>());
		assert(probe_em);	// can't contain enything else
		return false;
	} else {
		used_id_map[em] = excl_ptr<enum_member>(
			new enum_member(em));
		return true;
	}
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
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(&t) {
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
		new datatype_instance_reference(*this, 
			excl_ptr<index_list>(NULL)));
		// omitting index argument, set it later...
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class process_definition method definitions

/**
	Constructor for a process definition symbol table entry.  
 */
process_definition::process_definition(
		never_const_ptr<name_space> o, 
		const string& s,
		template_formals_set* tf) : 
		definition_base(s, o, tf),
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

never_const_ptr<fundamental_type_reference>
process_definition::set_context_fundamental_type(context& c) const {
	process_type_reference* dtr = new process_type_reference(
		never_const_ptr<process_definition>(this),
		c.get_current_template_arguments());
	assert(dtr);
	// type reference check checking? where?
	return c.set_current_fundamental_type(*dtr);
}

/**
	Adds a port formal instance to this process definition.  
 */
never_const_ptr<instantiation_base>
process_definition::add_port_formal(excl_ptr<instantiation_base> f) {
	assert(f);
	assert(!f.is_a<param_instantiation>());
	// check and make sure identifier wasn't repeated in formal list!
	never_const_ptr<object> probe(lookup_object_here(f->get_name()));
	if (probe) {
		probe->what(cerr << " already taken as a ") << " ERROR!";
		return never_const_ptr<instantiation_base>(NULL);
	}

	{
	const never_const_ptr<instantiation_base>* ret =
		port_formals.append(f->hash_string(),
			never_const_ptr<instantiation_base>(f));
	assert(!ret);
	// since we already checked used_id_map, there cannot be a repeat
	// in the port_formals_list!
	}

	never_const_ptr<instantiation_base> ret(f);
	assert(ret);
	used_id_map[f->hash_string()] = f;
	assert(!f);		// ownership transferred
	return ret;
}

//=============================================================================
// class process_instantiation method definitions

process_instantiation::process_instantiation(const scopespace& o, 
		const process_type_reference& pt,
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(&pt) {
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
		new process_instance_reference(*this, 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class param_instantiation method definitions

param_instantiation::param_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d) {
}

param_instantiation::~param_instantiation() {
}

/**
	Override instantiation_base's what() to suppress output of <>.
ostream&
param_instantiation::what(ostream& o) const {
	return o << 
}
**/

/**
	To determine whether or not this is a formal parameter, 
	look itself up in the owning namespace.  
 */
bool
param_instantiation::is_template_formal(void) const {
	// look itself up in owner namespace
	never_const_ptr<definition_base> def(owner.is_a<definition_base>());
	if (def) {
		return def->lookup_template_formal(key);
	} else {
		assert(owner.is_a<name_space>());
		// is owned by a namespace, i.e. actually instantiated
		return false;
	}
}

/**
	Checks whether or not instance has been assigned.
	Collectives -- conservatively return false.  
	In this context, "default_value" refers to the initialized
	value.  
	Should check whether or not this is a template formal first!

	Later distinguish between may_be_init_ and definitely_init_...
 */
bool
param_instantiation::is_initialized(void) const {
//	return default_value();		// used to be just this

	if (dimensions()) {
		// for may_be_initialized, return true, 
		// for definitely_initialized, return false.  
		return false;
	} else if (is_template_formal()) {
	// first check whether or not this is a template formal parameter
		return false;
	} else {
		// then is not a formal, default_value field is 
		// interpreted as an initial value.  
		count_const_ptr<param_expr> ret(default_value());
		if (ret)
			return ret->is_initialized();
		else return false;
	}
}

bool
param_instantiation::is_static_constant(void) const {
	if (dimensions()) {
		// conservatively return... depends on may or must...
		return false;
	} else if (is_template_formal()) {
		return false;
	} else {
		count_const_ptr<param_expr> ret(default_value());
		if (ret)
			return ret->is_static_constant();
		else return false;
	}
}

#if 0
/**
	Note: only one flavor needed (hopefully).  
	One should be able to statically determine whether or not
	something is loop-dependent.  
	Wait, does it even make sense for an "instantiation"?
	This should only be applicable to instance_references...
	put this on hold...
 */
bool
param_instantiation::is_loop_independent(void) const {
	
}
#endif

//=============================================================================
// class pbool_instantiation method definitions

pbool_instantiation::pbool_instantiation(const scopespace& o, 
		const string& n, 
		const pbool_expr* i) :
		param_instantiation(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(i) {
}

pbool_instantiation::pbool_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d, 
		const pbool_expr* i) :
		param_instantiation(o, n, d), ival(i) {
}

ostream&
pbool_instantiation::what(ostream& o) const {
	return o << "pbool-inst";
}

never_const_ptr<fundamental_type_reference>
pbool_instantiation::get_type_ref(void) const {
	return never_const_ptr<fundamental_type_reference>(&pbool_type);
		// defined in "art_built_ins.h"
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
pbool_instantiation::initialize(count_const_ptr<param_expr> e) {
	assert(!ival);
	count_const_ptr<pbool_expr> b(e.is_a<pbool_expr>());
	assert(b);
	ival = b;
}

count_const_ptr<param_expr>
pbool_instantiation::default_value(void) const {
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
pbool_instantiation::make_instance_reference(context& c) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	count_ptr<param_instance_reference> new_ir(
		new pbool_instance_reference(
			never_ptr<pbool_instantiation>(
			const_cast<pbool_instantiation*>(this)), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

#if 0
bool
pbool_instantiation::is_initialized(void) const {
	if (ival)
		return ival->is_initialized();
	else	return false;
#if 0
	// used to be just this...
	return ival;
#endif
}
#endif

//=============================================================================
// class pint_instantiation method definitions

pint_instantiation::pint_instantiation(const scopespace& o, 
		const string& n, 
		const pint_expr* i) :
		param_instantiation(o, n,
			index_collection_item_ptr_type(NULL)),
		ival(i) {
}

pint_instantiation::pint_instantiation(const scopespace& o, 
		const string& n, 
		index_collection_item_ptr_type d, 
		const pint_expr* i) :
		param_instantiation(o, n, d), ival(i) {
}

ostream&
pint_instantiation::what(ostream& o) const {
	return o << "pint-inst";
}

never_const_ptr<fundamental_type_reference>
pint_instantiation::get_type_ref(void) const {
	return never_const_ptr<fundamental_type_reference>(&pint_type);
		// defined in "art_built_ins.h"
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
pint_instantiation::initialize(count_const_ptr<param_expr> e) {
	assert(!ival);
	count_const_ptr<pint_expr> b(e.is_a<pint_expr>());
	assert(b);
	ival = b;
}

count_const_ptr<param_expr>
pint_instantiation::default_value(void) const {
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
pint_instantiation::make_instance_reference(context& c) const {
	// depends on whether this instance is collective, 
	//	check array dimensions.  

	// problem: needs to be modifiable for later initialization
	count_ptr<param_instance_reference> new_ir(
		new pint_instance_reference(
			never_ptr<pint_instantiation>(
			const_cast<pint_instantiation*>(this)), 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class channel_instantiation method definitions

channel_instantiation::channel_instantiation(const scopespace& o, 
		const channel_type_reference& ct,
		const string& n, 
		index_collection_item_ptr_type d) : 
		instantiation_base(o, n, d), type(&ct) {
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
		new channel_instance_reference(*this, 
			excl_ptr<index_list>(NULL)));
		// omitting index argument
	c.push_object_stack(new_ir);
	return never_const_ptr<instance_reference_base>(NULL);
}

//=============================================================================
// class single_instance_reference method definitions

single_instance_reference::single_instance_reference(
		excl_ptr<index_list> i, 
		const instantiation_state& st) :
		array_indices(i), 
		inst_state(st) {
	// in sub-type constructors, 
	// assert(array_indices->size < get_inst_base->dimensions());
}

single_instance_reference::~single_instance_reference() {
}

/**
	Dimensionality of an indexed references depends on
	which indexed dimensions are collapsed x[i], and which
	are not x[i..j].  
	A fully collapsed index list is equivalent to a 0-d array
	or a single instance.  
	\return the dimensions of the referenced array.  
 */
size_t
single_instance_reference::dimensions(void) const {
	// THIS NEEDS FIXING
	if (array_indices)
		return array_indices->dimensions();
	else return get_inst_base()->dimensions();
}

ostream&
single_instance_reference::dump(ostream& o) const {
	o << get_inst_base()->get_name();
	if (array_indices) {
		array_indices->dump(o);
	}
	return o;
}

string
single_instance_reference::hash_string(void) const {
	string ret(get_inst_base()->get_qualified_name());
	if (array_indices) {
		ret += array_indices->hash_string();
	}
	return ret;
}

//=============================================================================
#if 0
PHASE IN later...
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
#endif

//=============================================================================
// class param_instance_reference method definitions

/**
	\param pi needs to be modifiable for later initialization.  
 */
param_instance_reference::param_instance_reference(
		excl_ptr<index_list> i, 
		const instantiation_state& st) :
		single_instance_reference(i, st) {
}

/**
	For single instances references, check whether it is initialized.  
	For collective instance references, and indexed references, 
	just conservatively say that it hasn't been initialized yet; 
	so don't bother checking until unroll time.  
 */
bool
param_instance_reference::is_initialized(void) const {
	never_const_ptr<instantiation_base> i(get_inst_base());
	assert(i);
	if (i->dimensions() > 0)
		return false;
	else 
		return i.is_a<param_instantiation>()->is_initialized();
}

/**
	Under what conditions is a reference to a param instance
	static and constant?
	Resolved to a compile-time constant value.  
	May refer to parameters that are unconditionally initialized
	exactly once.  
	For collective variables, we just conservatively return false.  
	Oh yeah, how do we keep track whether or not a variable
	has been conditionally initialized?
	We may need some assignment stack.... PUNT!
 */
bool
param_instance_reference::is_static_constant(void) const {
	// "collective": if either reference is indexed, 
	// 	(mind, this is conservative and not precise because
	//	we don't track values of arrays at compile-time)
	//	More thoughts later on how to be more precise...
	if (array_indices)
		return false;
	// or the instantiation_base is collective (not 0-dimensional)
	else if (get_inst_base()->dimensions())
		return false;
	// else if singular, whether or not it is initialized once
	// to another static constant (parameter variable or value).  
	else 
		return get_param_inst_base()->is_static_constant();
}

/**
	A reference is loop independent if it's indices
	contain no references to index loop variables.  
	Or if it's simply not in a loop, since references are type-checked.  
	BEWARE: referring to a collective instance without indexing
	can also be loop-variant if instances are added to the collection
	within some loop.  
	See if the iterator into instance_collection_stack points
	to a loop-scope.  
	(Of course, we need to actually use the instance_collection stack...)
 */
bool
param_instance_reference::is_loop_independent(void) const {
	if (array_indices)
		return array_indices->is_loop_independent();
	else 
		// no array indices, see if instantiation_base is collective
	if (get_inst_base()->dimensions()) {
		// if collective, check if the instance_reference itself 
		// is found within a loop that adds to the collection...
		// ... but I'm too lazy to do this entirely now
		return false;
		// FIX ME later
	} else {
		// is 0-dimension, look up and see if it happens to be
		// a loop index variable.  
		// Who owns the param_inst_base?
		never_const_ptr<scopespace>
			owner(get_inst_base()->get_owner());
		return !owner.is_a<loop_scope>();
	}
}

/**
	Whether or not this instance reference is found inside a 
	conditional body.  
	Check the iterator into the instantiation base's
	collection stack.  
	Is point of reference in some conditional body
		(may be nested in loop, don't forget!)
	What about the indices themselves?  (shouldn't need to check?)
 */
bool
param_instance_reference::is_unconditional(void) const {
	if (array_indices)
		return array_indices->is_unconditional();
	// else see if point of reference is within some conditional scope
	else if (get_inst_base()->dimensions()) {
		// if collective, see if the instance_reference itself
		// is found within a conditional body, by walking the 
		// collection_state_iterator forward.
		// ... but I'm too lazy to do this now, FIX ME later
		// what about checking history of assignments???
		//	implies assignment stack...
		return false;		// extremely conservative
	} else {
		// also need to check assignment stack...
		return true;
	}
}

//=============================================================================
// class pbool_instance_reference method definitions

pbool_instance_reference::pbool_instance_reference(
		never_ptr<pbool_instantiation> pi,
		excl_ptr<index_list> i) :
		param_instance_reference(i, pi->current_collection_state()),
		pbool_inst_ref(pi) {
}

never_const_ptr<instantiation_base>
pbool_instance_reference::get_inst_base(void) const {
	return pbool_inst_ref;
}

never_const_ptr<param_instantiation>
pbool_instance_reference::get_param_inst_base(void) const {
	return pbool_inst_ref;
}

ostream&
pbool_instance_reference::what(ostream& o) const {
	return o << "pbool-inst-ref";
}

void
pbool_instance_reference::initialize(count_const_ptr<param_expr> i) {
	pbool_inst_ref->initialize(i.is_a<pbool_expr>());
}

/**
	\return newly constructed pbool literal if successful, 
		returns NULL if type mismatches.  
 */
count_ptr<param_expr>
pbool_instance_reference::make_param_literal(
		count_ptr<param_instance_reference> pr) {
	// make sure passed pointer is a self-ref count
	assert(pr == this);
	count_ptr<pbool_instance_reference> br(
		pr.is_a<pbool_instance_reference>());
	if (br)	return count_ptr<param_expr>(new pbool_literal(br));
	else	return count_ptr<param_expr>(NULL);
}

//=============================================================================
// class pint_instance_reference method definitions

pint_instance_reference::pint_instance_reference(
		never_ptr<pint_instantiation> pi,
		excl_ptr<index_list> i) :
		param_instance_reference(i, pi->current_collection_state()),
		pint_inst_ref(pi) {
}

never_const_ptr<instantiation_base>
pint_instance_reference::get_inst_base(void) const {
	return pint_inst_ref;
}

never_const_ptr<param_instantiation>
pint_instance_reference::get_param_inst_base(void) const {
	return pint_inst_ref;
}

ostream&
pint_instance_reference::what(ostream& o) const {
	return o << "pint-inst-ref";
}

void
pint_instance_reference::initialize(count_const_ptr<param_expr> i) {
	pint_inst_ref->initialize(i.is_a<pint_expr>());
}

/**
	\return newly constructed pint literal if successful, 
		returns NULL if type mismatches.  
 */
count_ptr<param_expr>
pint_instance_reference::make_param_literal(
		count_ptr<param_instance_reference> pr) {
	// make sure passed pointer is a self-ref count
	assert(pr == this);
	count_ptr<pint_instance_reference> ir(
		pr.is_a<pint_instance_reference>());
	if (ir)	return count_ptr<param_expr>(new pint_literal(ir));
	else	return count_ptr<param_expr>(NULL);
}

//=============================================================================
// class process_instance_reference method definitions

process_instance_reference::process_instance_reference(
		const process_instantiation& pi,
		excl_ptr<index_list> i) :
		single_instance_reference(i, pi.current_collection_state()),
		process_inst_ref(&pi) {
}

process_instance_reference::~process_instance_reference() {
}

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

datatype_instance_reference::datatype_instance_reference(
		const datatype_instantiation& di,
		excl_ptr<index_list> i) :
		single_instance_reference(i, di.current_collection_state()),
		data_inst_ref(&di) {
}

datatype_instance_reference::~datatype_instance_reference() {
}

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

channel_instance_reference::channel_instance_reference(
		const channel_instantiation& ci,
		excl_ptr<index_list> i) :
		single_instance_reference(i, ci.current_collection_state()),
		channel_inst_ref(&ci) {
}

channel_instance_reference::~channel_instance_reference() {
}

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

