/**
	\file "Object/common/namespace.cc"
	Method definitions for base classes for semantic objects.  
	This file was "Object/common/namespace.cc"
		in a previous lifetime.  
 	$Id: namespace.cc,v 1.20.4.3 2006/09/03 02:33:28 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_NAMESPACE_CC__
#define	__HAC_OBJECT_COMMON_NAMESPACE_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <string>

#include "util/STL/functional.h"
#include "util/ptrs_functional.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/conditional.h"

// CAUTION on ordering of the following two include files!
// including "art_object.h" first will cause compiler to complain
// about redefinition of struct hash<> template upon specialization of
// hash<string>.  

// include this as early as possible
#include "util/hash_specializations.h"		// substitute for the following
#include "util/STL/hash_map_utils.h"

#include "AST/token_string.h"
#include "AST/identifier.h"

#include "Object/devel_switches.h"
#include "Object/common/namespace.h"
#include "Object/common/dump_flags.h"
#include "Object/def/typedef_base.h"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/param_value_placeholder.h"
#else
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/param_value_collection.h"
#endif
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/type/fundamental_type_reference.h"
#include "Object/persistent_type_hash.h"

#include "util/memory/count_ptr.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
#if 0
SPECIALIZE_UTIL_WHAT_DEFINITION(HAC::entity::name_space, "namespace")
#endif

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::name_space, NAMESPACE_TYPE_KEY, 0)
}	// end namespace util

namespace HAC {
namespace entity {

#include "util/using_ostream.h"
using parser::scope;
using std::_Select1st;
using std::_Select2nd;
using std::bind1st;
using util::mem_fun;
using std::not1;
USING_UTIL_COMPOSE
using util::indent;
using util::auto_indent;
using util::disable_indent;
using util::write_value;
using util::read_value;
using util::write_string;
using util::read_string;
using util::persistent_traits;
using HASH_MAP_NAMESPACE::copy_map_reverse_bucket;

//=============================================================================
// general non-member function definitions

//=============================================================================
// class scopespace method definitions
scopespace::scopespace() : 
		persistent(), used_id_map() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
scopespace::~scopespace() {
#if 0
	STACKTRACE("~scopespace()");
	cerr << "\t@ " << this << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Egregious quick and dirty patch.  :\
 */
bool
scopespace::is_global_namespace(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for unqualified identifier.  
	This is overrideable.  
	Doesn't care what sub-type the object actually is.  
	This variation only searches the current namespace, and 
	never searches the parents' scopes.  
	NOTE: MUST static_cast map as a const to guarantee
	that map will not be modified.  
	Can inline this.  
	\param id the unqualified name of the object sought.  
	\return an object with the same name, if found.  
 */
never_ptr<const object>
scopespace::lookup_member(const string& id) const {
	return __lookup_member(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non-overrideable lookup_member.  
 */
never_ptr<const object>
scopespace::__lookup_member(const string& id) const {
	const const_map_iterator f(used_id_map.find(id));
	return (f != used_id_map.end()) ? f->second :
		never_ptr<const object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as regular map lookup, but returning a modifiable pointer.  
 */
never_ptr<object>
scopespace::lookup_member_with_modify(const string& id) const {
	const const_map_iterator f(used_id_map.find(id));
	return (f != used_id_map.end()) ?
		never_ptr<object>(f->second) : never_ptr<object>(NULL);
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
	NOTE: recursive through parents, but need not be.  
 */
never_ptr<const object>
scopespace::lookup_object(const string& id) const {
	const never_ptr<const object> o(lookup_member(id));
	const never_ptr<const scopespace> parent(get_parent());
	if (o) return o;
	else if (parent) return parent->lookup_object(id);
	else return never_ptr<const object>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic object lookup for a qualified identifier.
	If id is only a single identifier, and it is not absolute, 
	then it is considered an unqualified identifier.  
 */
never_ptr<const object>
scopespace::lookup_object(const qualified_id_slice& id) const {
	typedef	never_ptr<const object>		return_type;
	STACKTRACE_VERBOSE;
if (id.is_absolute()) {
	const never_ptr<const scopespace> parent(get_parent());
	if (parent)
		return parent->lookup_object(id);
	else {	// we are the ROOT, start looking down namespaces
		qualified_id_slice idc(id);
		const never_ptr<const name_space>
			ns = lookup_namespace(
				idc.betail()).is_a<const name_space>();
		if (ns)
			return ns->lookup_object(**(--id.end()));
		else return return_type(NULL);
	}
} else if (id.size() <= 1) {
	return lookup_object(**id.begin());
} else {
	// else need to resolve namespace portion first
	qualified_id_slice idc(id);
	const never_ptr<const name_space>
		ns(lookup_namespace(idc.betail()).is_a<const name_space>());
	if (ns)
		return ns->lookup_object(**(--id.end()));
	else return return_type(NULL);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
scopespace::__dump_map_keys(ostream& o, const used_id_map_type& m) {
	std::ostream_iterator<string> osi(o, "\n");
	std::transform(m.begin(), m.end(), osi, 
		_Select1st<used_id_map_type::value_type>()
	);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Standard formatted dump for definitions.  
	Prints members by section, and alphabetically sorted.  
 */
ostream&
scopespace::dump_for_definitions(ostream& o) const {
	// to canonicalize the dump, we bin and sort into maps
	const_bin_sort bins;
	const_bin_sort& bins_ref(bins);
	bins_ref =
#if 1
	for_each_if(used_id_map.begin(), used_id_map.end(), 
		not1(bind1st(mem_fun(&scopespace::exclude_object_val), this)),
		bins_ref	// explicitly pass by REFERENCE not VALUE
	);
#else
	// unpredicated
	for_each(used_id_map.begin(), used_id_map.end(), bins_ref);
#endif

	// indentation scope
	// INDENT_SECTION(o);
	// bins.stats(o);

	// maps are already sorted by key
	if (!bins.param_bin.empty()) {
		o << auto_indent << "Parameters:" << endl;
		INDENT_SECTION(o);
		for_each(bins.param_bin.begin(), bins.param_bin.end(), 
		unary_compose(
			bind2nd_argval(
#if USE_INSTANCE_PLACEHOLDERS
				mem_fun(&instance_placeholder_base::pair_dump, 
					instance_placeholder_base::null), 
#else
				mem_fun(&instance_collection_base::pair_dump, 
					instance_collection_base::null), 
#endif
				o), 
			_Select2nd<const_bin_sort::param_bin_type::value_type>()
		)
		);
	}
#if 0	
	// consider nested typedefs later...
	if (!bins.alias_bin.empty()) {
		o << auto_indent << "Typedefs:" << endl;
		INDENT_SECTION(o);
		for_each(bins.alias_bin.begin(), bins.alias_bin.end(), 
		unary_compose(
			bind2nd_argval(
				mem_fun(&definition_base::pair_dump,
					definition_base::null),
				o), 
			_Select2nd<const_bin_sort::alias_bin_type::value_type>()
		)
		);
	}
#endif
	if (!bins.inst_bin.empty()) {
		o << auto_indent << "Instances:" << endl;
		INDENT_SECTION(o);
		for_each(bins.inst_bin.begin(), bins.inst_bin.end(), 
		unary_compose(
			bind2nd_argval(
#if USE_INSTANCE_PLACEHOLDERS
				mem_fun(&instance_placeholder_base::pair_dump, 
					instance_placeholder_base::null), 
#else
				mem_fun(&instance_collection_base::pair_dump,
					instance_collection_base::null),
#endif
				o), 
			_Select2nd<const_bin_sort::inst_bin_type::value_type>()
		)
		);
	}
	return o;
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
never_ptr<const scopespace>
scopespace::lookup_namespace(const qualified_id_slice& id) const {
	STACKTRACE("scopespace::lookup_namespace()");
	const never_ptr<const scopespace> parent(get_parent());
	NEVER_NULL(parent);
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
	\param inst_stmt the instantiation statement to process.
		non-const because we attach the back-reference 
		to the instance_collection.  
	\param cond whether or not the instantiation statement
		is in a conditional scope context.
		If true, then do not add it to the list of instantiation
		statement (of collection) when checking for range overlap.  
	\return pointer to newly created instance if successful, 
		else NULL.  
 */
#if USE_INSTANCE_PLACEHOLDERS
never_ptr<const instance_placeholder_base>
#else
never_ptr<const instance_collection_base>
#endif
scopespace::add_instance(
		const never_ptr<instantiation_statement_base> inst_stmt, 
		const token_identifier& id, const bool cond) {
	STACKTRACE("scopespace::add_instance(never_ptr<inst_stmt>, id)");
#if USE_INSTANCE_PLACEHOLDERS
	typedef never_ptr<const instance_placeholder_base>	return_type;
#else
	typedef never_ptr<const instance_collection_base>	return_type;
#endif
	INVARIANT(id != "");
	NEVER_NULL(inst_stmt);
	// inst_stmt won't have a name yet!
	// const string id(inst_stmt->get_name());
	const size_t dim = inst_stmt->dimensions();
	const never_ptr<object> probe(lookup_member_with_modify(id));
if (probe) {
#if USE_INSTANCE_PLACEHOLDERS
	const never_ptr<instance_placeholder_base>
		probe_inst(probe.is_a<instance_placeholder_base>());
#else
	const never_ptr<instance_collection_base>
		probe_inst(probe.is_a<instance_collection_base>());
#endif
	if (probe_inst) {
		// make sure is not a template or port formal instance!
		// can't append to those.  
#if USE_INSTANCE_PLACEHOLDERS
		const never_ptr<const physical_instance_placeholder>
			phi(probe_inst.is_a<const physical_instance_placeholder>());
		const never_ptr<const param_value_placeholder>
			pvp(probe_inst.is_a<const param_value_placeholder>());
		if (pvp->is_template_formal()) {
			cerr << "ERROR: cannot redeclare or append to "
				"a template formal parameter." << endl;
			return return_type(NULL);
		}
		if (phi->is_port_formal()) {
			cerr << "ERROR: cannot redeclare or append to "
				"a port formal instance." << endl;
			return return_type(NULL);
		}
#else
		if (probe_inst->is_template_formal()) {
			cerr << "ERROR: cannot redeclare or append to "
				"a template formal parameter." << endl;
			return return_type(NULL);
		}
		if (probe_inst->is_port_formal()) {
			cerr << "ERROR: cannot redeclare or append to "
				"a port formal instance." << endl;
			return return_type(NULL);
		}
#endif
		// compare types, must match!
		// just change these to references to avoid ref_count
		const count_ptr<const fundamental_type_reference>
			old_type(probe_inst->get_type_ref());
		const count_ptr<const fundamental_type_reference>
			new_type(inst_stmt->get_type_ref());
		// 2005-07-08 decision: 
		// strictness must be the same as original declaration, 
		// the first to appear in text, not necessarily first 
		// to be unrolled, in the case of conditional scopes.
		// In the future, these constraints may be revisited
		// and changed.  
		if (!old_type->get_template_params()
				.is_strictly_compatible_with(
				new_type->get_template_params())) {
			cerr << "ERROR: type redeclaration of "
				"collection " << id <<
				" does not match in strictness "
				"to the original declaration." << endl;
			return return_type(NULL);

		}
		// type comparison is conservative, in the 
		// case of dynamic template parameters.  
		if (!old_type->may_be_collectibly_type_equivalent(*new_type)) {
			cerr << "ERROR: type of redeclaration of "
				<< id << " does not match "
				"previous declaration: " << endl <<
				"\twas: ";
			old_type->dump(cerr) << ", got: ";
			new_type->dump(cerr) << " ERROR!  ";
			return return_type(NULL);
		}	// else good to continue
		
		// compare dimensions
		const size_t p_dim = probe_inst->get_dimensions();
#if 0
		cerr << "original dimensions = " << p_dim << 
			", new dimensions = " << dim << endl;
#endif
		if (!p_dim) {
			// if original declaration was not collective, 
			// then one cannot add more.  
			probe->dump(cerr) << " was originally declared "
				"as a single instance, and thus may "
				"not be extended or re-declared, "
				"ERROR!  ";
			return return_type(NULL);
		} else if (p_dim != dim) {
			probe->dump(cerr) << " was originally declared "
				"as a " << p_dim <<
				"-D array, so the new declaration "
				"cannot add a " << dim <<
				"-D array, ERROR!  ";
			return return_type(NULL);
		}	// else dimensions match apropriately
		/**
			We used to detect statically at compile time
			some instantiation collisions, but the implementation's
			little benefit didn't warrant the effort. 
		**/
		inst_stmt->attach_collection(probe_inst);
		return probe_inst;
	} else {
		probe->what(cerr << id << " is already declared ")
			<< ", ERROR!  ";
		return return_type(NULL);
	}
} else {
	// didn't exist before, just create and add new instance
#if USE_INSTANCE_PLACEHOLDERS
	excl_ptr<instance_placeholder_base> new_inst =
#else
	excl_ptr<instance_collection_base> new_inst =
#endif
		inst_stmt->get_type_ref()->make_instance_collection(
			never_ptr<const scopespace>(this), id, dim);
	// attach non-const back-reference
	inst_stmt->attach_collection(new_inst);
	// attaching collection will automatically set the first
	// instantiation_statement pointer to it.  
	INVARIANT(inst_stmt->get_name() == id);
	NEVER_NULL(new_inst);
#if USE_INSTANCE_PLACEHOLDERS
	const never_ptr<const instance_placeholder_base>
#else
	const never_ptr<const instance_collection_base>
#endif
		ret(add_instance(new_inst));
	INVARIANT(!new_inst.owned());
	NEVER_NULL(ret);
	return ret;
}
}	// end method add_instance

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The unsafe version of adding an instance_collection to the 
	named scope space.  
 */
#if USE_INSTANCE_PLACEHOLDERS
never_ptr<const instance_placeholder_base>
#else
never_ptr<const instance_collection_base>
#endif
scopespace::add_instance(
#if USE_INSTANCE_PLACEHOLDERS
		excl_ptr<instance_placeholder_base>& i
#else
		excl_ptr<instance_collection_base>& i
#endif
		) {
	STACKTRACE("scopespace::add_instance(excl_ptr<instance_collection_base>&)");
#if USE_INSTANCE_PLACEHOLDERS
	typedef never_ptr<const instance_placeholder_base>	return_type;
#else
	typedef never_ptr<const instance_collection_base>	return_type;
#endif
	return_type ret(i);
	NEVER_NULL(i);
	const string id(i->get_name());
	INVARIANT(id != "");		// cannot be empty string
	used_id_map[id] = i;
	// IS THE NEW ENTRY OWNED? IT SHOULD BE
	INVARIANT(used_id_map[id].owned());
	INVARIANT(!i);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a definition name alias to this scope.  
 */
good_bool
scopespace::add_definition_alias(const never_ptr<const definition_base> d, 
		const string& a) {
	const never_ptr<const object> probe(lookup_member(a));
		// or __lookup_member
	if (probe) {
		cerr << "Identifier \"" << a << "\" already taken by a ";
		probe->what(cerr) << " in ";
		what(cerr) << " " << get_qualified_name() << ".  ERROR!  ";
		return good_bool(false);
	} else {
#if 0
		// gcc-3.4.0 rejects, thinking that excl_ptr is const!
		used_id_map[a] = excl_ptr<object_handle>(new object_handle(d));
#else
		excl_ptr<object_handle> handle_ptr(new object_handle(d));
		used_id_map[a] = handle_ptr;
		INVARIANT(!handle_ptr);
#endif
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overrideable hook to exclude objects from dumping and persisting.  
	By default, nothing is excluded, so this returns false.  
 */
bool
scopespace::exclude_object(const used_id_map_type::value_type& i) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper to pass by value, instead of by reference.  
 */
bool
scopespace::exclude_object_val(const used_id_map_type::value_type i) const {
	return exclude_object(i);	// is virtual
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Count how many objects in the used_id_map will be excluded by
	exclude_object().  
 */
size_t
scopespace::exclude_population(void) const {
	return count_if(used_id_map.begin(), used_id_map.end(), 
		bind1st(mem_fun(&scopespace::exclude_object_val), this)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Register all pointers in the used_id_map with the 
	serial object manager.  
 */
inline
void
scopespace::collect_used_id_map_pointers(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT("scopespace::collect_used_id_map_pointers()");
	used_id_map_type::const_iterator m_iter(used_id_map.begin());
	const used_id_map_type::const_iterator m_end(used_id_map.end());
	for ( ; m_iter!=m_end; m_iter++) {
		const never_ptr<const object> m_obj(m_iter->second);
		NEVER_NULL(m_obj);		// no NULLs in hash_map
		// checks for excluded objects, virtual call
		if (!exclude_object(*m_iter)) {
			const never_ptr<const persistent>
				m_p(m_obj.is_a<const persistent>());
			if (m_p)
				m_p->collect_transient_info(m);
			// else skip non-persistent objects, 
			// such as namespace aliases
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
scopespace::collect_transient_info_base(persistent_object_manager& m) const {
	collect_used_id_map_pointers(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serializes the used_id_map hash_map's pointers to an 
	output stream, translating pointers to indices.  
	NOTE: we use a copy of the hash-table with its buckets reversed
	so that it will be reconstructed in the correct order.  
 */
inline
void
scopespace::write_object_used_id_map(const persistent_object_manager& m, 
		ostream& f) const {
	STACKTRACE_PERSISTENT("scopespace::write_object_used_id_map()");
	MUST_BE_A(const persistent*, this);
	// filter any objects out? yes
	// how many objects to exclude? need to subtract
	size_t s = used_id_map.size();
	size_t ex = exclude_population();
	INVARIANT(ex <= s);		// sanity check b/c unsigned
	write_value(f, s -ex);
	used_id_map_type temp;
	copy_map_reverse_bucket(used_id_map, temp);
	const used_id_map_type::const_iterator m_end(temp.end());
	used_id_map_type::const_iterator m_iter(temp.begin());
	for ( ; m_iter!=m_end; m_iter++) {
		// any distinction between aliases and non-owners?
		if (!exclude_object(*m_iter)) {
			const never_ptr<const object> m_obj(m_iter->second);
			m.write_pointer(f, m_obj);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
scopespace::write_object_base(const persistent_object_manager& m, 
	      ostream& o) const {
	write_object_used_id_map(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes out an empty map.
 */
void
scopespace::write_object_base_fake(const persistent_object_manager& m, 
		ostream& o) {
	static const size_t zero = 0;
	write_value(o, zero);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Deserializes a set of pointers and restores them into the
	used_id_map.  
	The key is that each object pointed to should have some associated
	key used for hashing, thus the objects must be at least partially
	reconstructed before adding them back to the hash map.  
	This means that "alias" entries of the hash_map, those that
	are mapped with a name that's not their own, cannot be restored
	as aliases since keys are not kept.  
	That is intentional since we don't intend to keep around aliases.  
 */
void
scopespace::load_object_used_id_map(
		const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("scopespace::load_object_used_id_map()");
	size_t s, i=0;
	read_value(f, s);
	for ( ; i<s; i++) {
		excl_ptr<persistent> m_obj;
		m.read_pointer(f, m_obj);	// replaced, b/c need index
		// need to add it back through hash_map.  
		if (!m_obj) {
			// this really should never happen...
			if (warn_unimplemented) {
				cerr << "Skipping a NULL object at index "
#if 0
					<< index << endl;
#else
					<< "???" << endl;
#endif
			}
		} else {
			m.load_object_once(m_obj);	// recursion!!!
			// need to reconstruct it to get its key, 
			// then add this object to the used_id_map
			load_used_id_map_object(m_obj);	// pure virtual
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
scopespace::load_object_base(const persistent_object_manager& m, istream& f) {
	load_object_used_id_map(m, f);
}

//=============================================================================
// class scopespace::bin_sort method definitions

scopespace::bin_sort::bin_sort() :
		ns_bin(), def_bin(), alias_bin(), inst_bin(), param_bin() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pred is a boolean functor.  
	\param i is essentially a pair<const string, some_ptr<object> >.  
		See definition of used_id_map_type.  
 */
void
scopespace::bin_sort::operator () (const used_id_map_type::value_type& i) {
	const never_ptr<object> o_p(i.second);
	NEVER_NULL(o_p);
	const never_ptr<name_space>
		n_b(o_p.is_a<name_space>());
	const never_ptr<definition_base>
		d_b(o_p.is_a<definition_base>());
#if USE_INSTANCE_PLACEHOLDERS
	const never_ptr<instance_placeholder_base>
		i_b(o_p.is_a<instance_placeholder_base>());
#else
	const never_ptr<instance_collection_base>
		i_b(o_p.is_a<instance_collection_base>());
#endif
	const string& k(i.first);
	if (n_b) {
		ns_bin[k] = n_b;
	} else if (d_b) {
		const never_ptr<typedef_base>
			t_b(d_b.is_a<typedef_base>());
		if (t_b)
			alias_bin[k] = t_b;
		else	def_bin[k] = d_b;
	} else if (i_b) {
#if USE_INSTANCE_PLACEHOLDERS
		const never_ptr<param_value_placeholder>
			p_b(i_b.is_a<param_value_placeholder>());
#else
		const never_ptr<param_value_collection>
			p_b(i_b.is_a<param_value_collection>());
#endif
		if (p_b)
			param_bin[k] = p_b;
		else	inst_bin[k] = i_b;
	} else {
		o_p->dump(cerr << "object ") << 
			"not binned for modification." << endl;
	}
}

//=============================================================================
// class scopespace::const_bin_sort method definitions

scopespace::const_bin_sort::const_bin_sort() :
		ns_bin(), def_bin(), alias_bin(), inst_bin(), param_bin() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pred is a boolean functor.  
	\param i is essentially a pair<const string, some_ptr<object> >.  
		See definition of used_id_map_type.  
 */
void
scopespace::const_bin_sort::operator () (
		const used_id_map_type::value_type& i) {
	const never_ptr<const object> o_p(i.second);
	NEVER_NULL(o_p);
	const never_ptr<const name_space>
		n_b(o_p.is_a<const name_space>());
	const never_ptr<const definition_base>
		d_b(o_p.is_a<const definition_base>());
#if USE_INSTANCE_PLACEHOLDERS
	const never_ptr<const instance_placeholder_base>
		i_b(o_p.is_a<const instance_placeholder_base>());
#else
	const never_ptr<const instance_collection_base>
		i_b(o_p.is_a<const instance_collection_base>());
#endif
	const string& k = i.first;
	if (n_b) {
		ns_bin[k] = n_b;		INVARIANT(ns_bin[k]);
	} else if (d_b) {
		const never_ptr<const typedef_base>
			t_b(d_b.is_a<const typedef_base>());
		if (t_b) {
			alias_bin[k] = t_b;	INVARIANT(alias_bin[k]);
		} else {
			def_bin[k] = d_b;	INVARIANT(def_bin[k]);
		}
	} else if (i_b) {
#if USE_INSTANCE_PLACEHOLDERS
		const never_ptr<const param_value_placeholder>
			p_b(i_b.is_a<const param_value_placeholder>());
#else
		const never_ptr<const param_value_collection>
			p_b(i_b.is_a<const param_value_collection>());
#endif
		if (p_b) {
			param_bin[k] = p_b;	INVARIANT(param_bin[k]);
		} else {
			inst_bin[k] = i_b;	INVARIANT(inst_bin[k]);
		}
	} else {
		o_p->dump(cerr << "object ") << 
			"not binned for reading." << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void
scopespace::const_bin_sort::stats(ostream& o) const {
	o << auto_indent << param_bin.size() << " parameter-collections" << endl;
	o << auto_indent << inst_bin.size() << " instantiation-collections" << endl;
	o << auto_indent << ns_bin.size() << " sub-namespaces" << endl;
	o << auto_indent << def_bin.size() << " definitions" << endl;
	o << auto_indent << alias_bin.size() << " typedefs" << endl;
}

//=============================================================================
// class name_space method definitions

LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(name_space, 8)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const never_ptr<const name_space>
name_space::null(NULL);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor, just allocate with bogus fields.
 */
name_space::name_space() :
		object(), 
		scopespace(), key(), parent(),
		open_spaces(), open_aliases() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructor for a new namespace.  
	Inherits from its parents: type aliases to built-in types, 
	such as bool and int.  
	\param n the name.  
	\param p pointer to the parent namespace.  
 */
name_space::name_space(const string& n, never_ptr<const name_space> p) : 
		object(), 
		scopespace(), key(n), parent(p), 
		open_spaces(), open_aliases() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructor for the global namespace, which is the only
	namespace without a parent.  
	Written here instead of re-using the above constructor with
	default argument because old compilers can't accept
	default arguments (NULL) for class object formals.  
 */
name_space::name_space(const string& n) :
		object(), 
		scopespace(), 
		key(n), 
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
	STACKTRACE_DTOR_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary hideous hack to print namespaces in a certain way.  
 */
bool
name_space::is_global_namespace(void) const {
	return !parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the key.
 */
const string&
name_space::get_key(void) const {
	return key;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the parent namespace.
 */
never_ptr<const scopespace>
name_space::get_parent(void) const {
	return parent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the flattened name of this current namespace.  
 */
string
name_space::get_qualified_name(void) const {
	if (parent)
		return parent->get_qualified_name() +scope +key;
	else return "";			// global e.g. ::foo
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: if df.no_global_namespace, then omit leading ::
 */
ostream&
name_space::dump_qualified_name(ostream& o, const dump_flags& df) const {
	if (parent) {
		INVARIANT(parent.is_a<const name_space>());
#if 0
		return parent->dump_qualified_name(o, df) << scope << key;
#else
		if (df.show_leading_scope || !parent->is_global_namespace()) {
			return parent->dump_qualified_name(o, df)
				<< scope << key;
		} else {
			// parent IS the global namespace
			// return o << parent->key << scope << key;
			return o << key;
		}
#endif
	} else	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns pointer to global namespace by following parent pointers.
 */
never_ptr<const name_space>
name_space::get_global_namespace(void) const {
	if (parent)
		return parent->get_global_namespace();
	else	// no parent
		return never_ptr<const name_space>(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
name_space::what(ostream& o) const {
#if 0
	return util::what<this_type>::name();
#else
	return o << "entity::namespace";
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Spill contents of the used_id_map.
	\param o the output stream.
	\return the same output stream.
 */
ostream&
name_space::dump(ostream& o) const {
	// to canonicalize the dump, we bin and sort into maps
	const_bin_sort bins;
	const_bin_sort& bins_ref(bins);
	bins_ref =
	for_each_if(used_id_map.begin(), used_id_map.end(), 
		not1(bind1st(mem_fun(&name_space::exclude_object_val), this)),
		bins_ref	// explicitly pass by REFERENCE not VALUE
	);

	o << auto_indent <<
		"In namespace \"" << key << "\", we have: {" << endl;
{
	// indentation scope
	INDENT_SECTION(o);
	bins.stats(o);

	// maps are already sorted by key
	if (!bins.param_bin.empty()) {
		o << auto_indent << "Parameters:" << endl;
		INDENT_SECTION(o);
		for_each(bins.param_bin.begin(), bins.param_bin.end(), 
		unary_compose(
			bind2nd_argval(
#if USE_INSTANCE_PLACEHOLDERS
				mem_fun(&instance_placeholder_base::pair_dump, 
					instance_placeholder_base::null), 
#else
				mem_fun(&instance_collection_base::pair_dump, 
					instance_collection_base::null), 
#endif
				o), 
			_Select2nd<const_bin_sort::param_bin_type::value_type>()
		)
		);
	}

	if (!bins.ns_bin.empty()) {
		o << auto_indent << "Namespaces:" << endl;
		INDENT_SECTION(o);
		for_each(bins.ns_bin.begin(), bins.ns_bin.end(), 
		unary_compose(
			bind2nd_argval(
				mem_fun(&name_space::pair_dump, 
					name_space::null), 
				o), 
			_Select2nd<const_bin_sort::ns_bin_type::value_type>()
		)
//			This does the following (in pair_dump):
//			o << "  " << i->first << " = ";
//			i->second->dump(o) << endl;
		);
	}
	
	if (!bins.def_bin.empty()) {
		o << auto_indent << "Definitions:" << endl;
		INDENT_SECTION(o);
		for_each(bins.def_bin.begin(), bins.def_bin.end(), 
		unary_compose(
			bind2nd_argval(
				mem_fun(&definition_base::pair_dump,
					definition_base::null),
				o), 
			_Select2nd<const_bin_sort::def_bin_type::value_type>()
		)
		);
	}
	
	if (!bins.alias_bin.empty()) {
		o << auto_indent << "Typedefs:" << endl;
		INDENT_SECTION(o);
		for_each(bins.alias_bin.begin(), bins.alias_bin.end(), 
		unary_compose(
			bind2nd_argval(
				mem_fun(&definition_base::pair_dump,
					definition_base::null),
				o), 
			_Select2nd<const_bin_sort::alias_bin_type::value_type>()
		)
		);
	}
	// would like to show instance names prefixed with "::"
	// to clarify the absolute name of top-level instances
	if (!bins.inst_bin.empty()) {
		o << auto_indent << "Instances:" << endl;
		INDENT_SECTION(o);
		for_each(bins.inst_bin.begin(), bins.inst_bin.end(), 
		unary_compose(
			bind2nd_argval(
#if USE_INSTANCE_PLACEHOLDERS
				mem_fun(&instance_placeholder_base::pair_dump_top_level,
					instance_placeholder_base::null),
#else
				mem_fun(&instance_collection_base::pair_dump_top_level,
					instance_collection_base::null),
#endif
				o), 
			_Select2nd<const_bin_sort::inst_bin_type::value_type>()
		)
		);
	}
}	// end of indentation scope
	return o << auto_indent << "}" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
name_space::pair_dump(ostream& o) const {
        o << auto_indent << get_key() << " = ";
        return dump(o);
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
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("opening: " << n << endl);
	never_ptr<name_space> ret;
	const never_ptr<const object> probe(__lookup_member(n));
	if (probe) {
		const never_ptr<const name_space>
			probe_ns(probe.is_a<const name_space>());
		// an alias may return with valid pointer!
		if (!probe_ns) {
			probe->what(cerr << n << " is already declared as a ")
				<< ", ERROR! ";
			return never_ptr<name_space>(NULL);
		} else if (lookup_open_alias(n)) {
		// we have a valid namespace pointer, 
		// now we see if this is an alias, or true sub-namespace
			cerr << n << " is already declared an open alias, "
				"ERROR! ";
			return never_ptr<name_space>(NULL);
		} else {
		// therefore, probe_ns is a pointer to a valid sub-namespace
#if 0
			cerr << n << " is already exists as subspace, "
					"re-opening";
#endif
			ret = lookup_member_with_modify(n).is_a<name_space>();
//			INVARIANT(lookup_member(n).is_a<name_space>());
			INVARIANT(probe_ns->key == ret->key);
		}
		INVARIANT(ret);
	} else {
		// create it, linking this as its parent
//		cerr << " ... creating new";
		excl_ptr<name_space>
			new_ns(new name_space(
				n, never_ptr<const name_space>(this)));
		ret = add_namespace(new_ns);
		// explicit transfer
		INVARIANT(!new_ns);
	}

	// silly sanity checks
	INVARIANT(ret->parent == this);
	INVARIANT(ret->key == n);
//	cerr << " with parent: " << ret->parent->key;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private method for adding a namespace to the used_id_map.
	Ideally, should already be checked before calling this.  
 */
never_ptr<name_space>
name_space::add_namespace(excl_ptr<name_space>& new_ns) {
	STACKTRACE_VERBOSE;
	const never_ptr<name_space> ret(new_ns);
	NEVER_NULL(ret);
	INVARIANT(new_ns.owned());
	// register it as a used id
	used_id_map[new_ns->key] = new_ns;
		// explicit transfer
	INVARIANT(!new_ns);
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
never_ptr<const name_space>
name_space::leave_namespace(void) {
	STACKTRACE_VERBOSE;
#if 0
	cerr << "Beginning of name_space::leave_namespace(): " << endl;
	dump(cerr) << endl;
#endif
	// for all open_aliases, release their names from used-map
	alias_map_type::const_iterator i(open_aliases.begin());
	const alias_map_type::const_iterator a_end(open_aliases.end());
	for ( ; i!=a_end; i++) {
#if 0
		cerr << "Removing \"" << i->first << "\" from used_id_map."
			<< endl;
#endif
		used_id_map.erase(used_id_map.find(i->first));
	}
	open_spaces.clear();
	open_aliases.clear();
#if 0
	cerr << "End of name_space::leave_namespace(): " << endl;
	dump(cerr) << endl;
#endif
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
never_ptr<const name_space>
name_space::add_using_directive(const qualified_id& n) {
	typedef	never_ptr<const name_space>	return_type;
	STACKTRACE_VERBOSE;
	return_type ret;
	namespace_list candidates;		// empty list

#if 0
	cerr << endl << "adding using-directive in space: " 
		<< get_qualified_name();
#endif
	// see if namespace has already been declared within scope of search
	// remember: the qualified_id is a suffix to be appended onto root
	// find it/them, record to list
	query_import_namespace_match(candidates, n);
	namespace_list::const_iterator i(candidates.begin());

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
			ret = return_type(NULL);
			break;	// no matches
			}
		default: {	// > 1
			ret = return_type(NULL);
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
never_ptr<const name_space>
name_space::add_using_alias(const qualified_id& n, const string& a) {
	STACKTRACE_VERBOSE;
	never_ptr<const object> probe;
	never_ptr<const name_space> ret;
	namespace_list::const_iterator i;
	namespace_list candidates;		// empty list

#if 0
	cerr << endl << "adding using-alias in space: " 
		<< get_qualified_name() << " as " << a;
#endif

	// need to force use of the constant version of the lookup
	// because this method is non-const.  
	// else it will modify the used_id_map!
	// perhaps wrap with a probe() const method...
	probe = __lookup_member(a);	// not lookup_member?
	if (probe) {
		probe = never_ptr<const object>(&probe->self());
		// resolve handles
	}
	if (probe) {
		// then already, it conflicts with some other id
		// we report the conflict precisely as follows:
		ret = probe.is_a<const name_space>();
		if (ret) {
			if(lookup_open_alias(a)) {
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
		return never_ptr<const name_space>(NULL);
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
#if 0
			// gcc-3.4.0 doesn't like, assumes excl_ptr is const
			used_id_map[a] = excl_ptr<object_handle>(
				new object_handle(ret));
#else
			excl_ptr<object_handle>
				handle_ptr(new object_handle(ret));
			used_id_map[a] = handle_ptr;
			INVARIANT(!handle_ptr);
#endif
			break;
			}
		case 0:	{
			cerr << " ... not found, ERROR! ";
			ret = never_ptr<const name_space>(NULL);
			break;	// no matches
			}
		default: {	// > 1
			ret = never_ptr<const name_space>(NULL);
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
never_ptr<const name_space>
name_space::query_namespace_match(const qualified_id_slice& id) const {
	typedef	never_ptr<const name_space>	return_type;
	// qualified_id_slice is a wrapper around qualified_id
	// recall that qualified_id is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
#if 0
	cerr << "query_namespace_match: " << id 
		<< " in " << get_qualified_name() << endl;
#endif

	if (id.empty())	{	// what if it's absolute and empty?
		return (id.is_absolute()) ? get_global_namespace() : 
			return_type(this);
	}
	qualified_id_slice::const_iterator i(id.begin());
	NEVER_NULL(*i);
	const count_ptr<const token_identifier>& tidp(*i);
	NEVER_NULL(tidp);
	const token_identifier& tid(*tidp);
	never_ptr<const name_space>
		ns = (id.is_absolute()) ? get_global_namespace()
		: never_ptr<const name_space>(this);
	if (ns->key.compare(tid)) {
		// if names differ, already failed, try alias spaces
		return return_type(NULL);
	} else {
		for (i++; ns && i!=id.end(); i++) {
			// no need to skip scope tokens anymore
			const count_ptr<const token_identifier>&
				tidp2(i->is_a<const token_identifier>());
			NEVER_NULL(tidp2);
			const token_identifier& tid2(*tidp2);
			// the [] operator of map<> doesn't have const 
			// semantics, even if looking up an entry!
			const never_ptr<const name_space>
				next(ns->__lookup_member(tid2).is_a<const name_space>());
			// if not found in subspaces, check aliases list
			// or should we not search aliases?
			ns = (next) ? next : ns->lookup_open_alias(tid2);
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
never_ptr<const name_space>
name_space::query_subnamespace_match(const qualified_id_slice& id) const {
	// qualified_id_slice is just a wrapper around qualified_id
	// recall that qualified_id is a node_list<token_identifier,scope>
	// and that token_identifier is a sub-type of string
#if 0
	cerr << endl << "query_subnamespace_match: " << id 
		<< " in " << get_qualified_name() << endl;
#endif

	// here, does NOT check for global-absoluteness
	if (id.empty())	{	// what if it's absolute and empty?
		return (id.is_absolute()) ? get_global_namespace() : 
			never_ptr<const name_space>(this);
	}
	qualified_id_slice::const_iterator i(id.begin());
	NEVER_NULL(*i);		// *i is a count_ptr<const token_identifier>
	const token_identifier& tid(**i);
	// no check for absoluteness
	never_ptr<const name_space> ns;
	if (id.is_absolute()) {
		ns = get_global_namespace()->
			__lookup_member(tid).is_a<const name_space>();
	} else {
		// force use of const probe
		const never_ptr<const object> probe(__lookup_member(tid));
		ns = probe.is_a<const name_space>();
	}

	if (!ns) {				// else lookup in aliases
		ns = lookup_open_alias(tid);	// replaced for const semantics
	}
	for (i++; ns && i!=id.end(); i++) {
		NEVER_NULL(*i);
		const token_identifier& tid2(**i);
		const never_ptr<const name_space>
			next(ns->__lookup_member(tid2).is_a<const name_space>());
		// if not found in subspaces, check aliases list
		ns = (next) ? next : ns->lookup_open_alias(tid2);
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
name_space::query_import_namespace_match(
		namespace_list& m, const qualified_id& id) const {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("inside " << get_qualified_name() << endl);
	{
		const never_ptr<const name_space>
			ret(query_subnamespace_match(id));
		if (ret) m.push_back(ret);
	}
	// always search these unconditionally? or only if not found so far?
	{	// with open namespaces list
		namespace_list::const_iterator i(open_spaces.begin());
		for ( ; i!=open_spaces.end(); i++) {
			const never_ptr<const name_space>
				ret((*i)->query_subnamespace_match(id));
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
name_space::find_namespace_ending_with(
		namespace_list& m, const qualified_id& id) const {
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
	const never_ptr<const name_space>
		ret(query_subnamespace_match(id));
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

	If the signature matches an existing definition, 
	then the new one is a duplicate, thus we delete it.  
	
	\param db the definition to add, newly created.
	\return modifiable pointer to definition if successful, else NULL.  
 */
never_ptr<definition_base>
name_space::add_definition(excl_ptr<definition_base>& db) {
	typedef	never_ptr<definition_base>	return_type;
	NEVER_NULL(db);
	const string k = db->get_name();
	const never_ptr<const object> probe(lookup_member(k));
		// not __lookup_member
	if (probe) {
		const never_ptr<const definition_base>
			probe_def(probe.is_a<const definition_base>());
		if (probe_def) {
			INVARIANT(k == probe_def->get_name());	// consistency
			if (probe_def->require_signature_match(db).good) {
				// definition signatures match
				// can discard new declaration
				// to delete db, we steal ownership, 
				// and deallocate it with a local excl_ptr
				const excl_ptr<definition_base>
					release_db(db);
				return return_type(release_db);
			} else {
				// signature mismatch!
				// also catches class type mismatch
				cerr << "new declaration for \"" << k <<
					"\" doesn't match previous "
					"declaration.  ERROR! ";
				// give details...
				return return_type(NULL);
			}
		} else {
			probe->what(cerr << "Identifier already taken by ")
				<< endl << "ERROR: Failed to add definition! ";
			return return_type(NULL);
		}
	} else {
		// used_id_map owns this type is reponsible for deleting it
		const never_ptr<definition_base> ret = db;
		NEVER_NULL(ret);
		used_id_map[k] = db;
		INVARIANT(!db);		// after explicit transfer of ownership
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overrides scopespace::lookup_namespace.
	\param id is the entire name of the namespace.
	\return pointer to the scope or namespace matched if found.  
 */
never_ptr<const scopespace>
name_space::lookup_namespace(const qualified_id_slice& id) const {
	STACKTRACE("namespace::lookup_namespace()");
	return query_subnamespace_match(id);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up a namespace in ONLY the open_aliases set.  
 */
never_ptr<const name_space>
name_space::lookup_open_alias(const string& id) const {
	// need static cast to guarantee non-modification
	typedef	never_ptr<const name_space>	return_type;
	const alias_map_type::const_iterator f(open_aliases.find(id));
	return (f != open_aliases.end()) ? f->second : return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overrides scopespace's lookup_member by also searching aliases.  
	Search open_spaces in addition to own namespace.  
	Don't search open_aliases because those are treated
	like member (nested) namespaces.
	TODO: Do aliases have priority, or treated equal?
	If treated equal, then need to detect ambiguity.  
	Recursive search should not lookup aliases, so call __lookup_member.  
 */
never_ptr<const object>
name_space::lookup_member(const string& id) const {
	typedef	never_ptr<const object>	return_type;
	typedef	std::set<return_type>	return_set;
	typedef	namespace_list::const_iterator		const_iterator;
	STACKTRACE_VERBOSE;
	return_type o(parent_type::lookup_member(id));
	if (o)
		return o;
	// else search open namespace aliases
	return_set candidates;
{
	const_iterator i(open_spaces.begin()), e(open_spaces.end());
	for ( ;i!=e; ++i) {
		const return_type a((*i)->__lookup_member(id));
		if (a) {
			const bool b __ATTRIBUTE_UNUSED__ =
				candidates.insert(a).second;
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT_PRINT("a = " << &*a << endl);
			a->dump(cerr) << endl;
			STACKTRACE_INDENT_PRINT("found match " << candidates.size() << endl);
#endif
			// INVARIANT(b);
			// not true, can be found through different
			// namespace paths
		}
		// else skip
	}
}
	if (candidates.empty()) {
		// no matches found
		return return_type(NULL);
	} else if (candidates.size() == 1) {
		return *candidates.begin();
	} else {
		// multiple ambiguous matches
		cerr << "Error: multiple matches found for \'" << id << "\'."
			<< endl;
		cerr << "Candidates are: " << endl;
		typedef	return_set::const_iterator		set_iterator;
		set_iterator i(candidates.begin()), e(candidates.end());
		for ( ; i!=e; ++i) {
			// dump qualified name enough?
			(*i)->dump(cerr) << endl;
		}
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive collection of all namespaces into a flat list
	of namespace pointers.  
	\param l the list in which to accumulate namespace pointers.  
 */
void
name_space::collect_namespaces(namespace_collection_type& l) const {
	used_id_map_type::const_iterator i(used_id_map.begin());
	const used_id_map_type::const_iterator e(used_id_map.end());
	// consider transform_if
	for ( ; i!=e; i++) {
		const namespace_collection_type::value_type
			p(i->second.is_a<name_space>());
		if (p) {
			l.push_back(p);
			p->collect_namespaces(l);	// recursion
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively collect pointer information about constituents.  
 */
void
name_space::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_PERSISTENT("namespace::collect_transients()");
#if 0
	cerr << "Found namespace \"" << get_key() << "\" whose address is: "
		<< this << endl;
#endif
	scopespace::collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes out object binary.  
	Not recursive, as manager will sequentially visit each object
	exactly once.  
	Temporarily only write out namespaces, will extend later...
	Make sure read_object reverses this process exactly.  
	\param f output file stream, must be in binary mode.  
	\param m object manager, must already be loaded with all pointer info, 
		only modifies the flagged state of the entries.  
 */
void
name_space::write_object(const persistent_object_manager& m, ostream& f) const {
	// Second, write out the name of this namespace.
	// name MUST be available for use by other visitors right away
	STACKTRACE_PERSISTENT("namespace::write_object()");
	write_string(f, key);

	m.write_pointer(f, parent);

	// do we need to sort objects into bins?
	scopespace::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initializes and loads this namespace with actual contents.  
	Have to cast some const qualifiers away because object
	was only partially initialized on allocation.  
 */
void
name_space::load_object(const persistent_object_manager& m, istream& f) {
	STACKTRACE_PERSISTENT("namespace::load_object()");
	// Second, read in the name of the namespace.  
	read_string(f, const_cast<string&>(key));	// coercive cast

	// Next, read in the parent namespace pointer.  
	m.read_pointer(f, parent);

	scopespace::load_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
name_space::exclude_object(const used_id_map_type::value_type& i) const {
	if (i.second.is_a<const object_handle>())
		return true;
	else return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specific way of adding objects for a namespace.  
	\pre object must already be loaded, so its hash key can be used.  
 */
void
name_space::load_used_id_map_object(excl_ptr<persistent>& o) {
	STACKTRACE("name_space::load_used_id_map_object()");
	NEVER_NULL(o);
	if (o.is_a<name_space>()) {
		excl_ptr<name_space>
			nsp = o.is_a_xfer<name_space>();
		add_namespace(nsp);
		INVARIANT(!nsp);
	} else if (o.is_a<definition_base>()) {
		excl_ptr<definition_base>
			defp = o.is_a_xfer<definition_base>();
		add_definition(defp);
		INVARIANT(!defp);
	// ownership restored here!
#if USE_INSTANCE_PLACEHOLDERS
	} else if (o.is_a<instance_placeholder_base>()) {
#else
	} else if (o.is_a<instance_collection_base>()) {
#endif
#if USE_INSTANCE_PLACEHOLDERS
		excl_ptr<instance_placeholder_base>
			icbp = o.is_a_xfer<instance_placeholder_base>();
#else
		excl_ptr<instance_collection_base>
			icbp = o.is_a_xfer<instance_collection_base>();
#endif
		add_instance(icbp);
		INVARIANT(!icbp);
		// NEED TO GUARANTEE THAT IT IS OWNED!
	} else {
		o->what(cerr << "TO DO: define method for adding ")
			<< " back to namespace." << endl;
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

#endif	// __HAC_OBJECT_COMMON_NAMESPACE_CC__

