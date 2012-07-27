/**
	\file "Object/ref/simple_meta_instance_reference.cc"
	Method definitions for the meta_instance_reference family of objects.
	This file was reincarnated from "Object/art_object_inst_ref.cc".
 	$Id: simple_meta_instance_reference.tcc,v 1.39 2011/02/25 23:19:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_TCC__

#include <iostream>
#include <vector>

#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_value_reference.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/meta_index_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/expr_visitor.h"
#include "Object/common/dump_flags.h"
#include "Object/unroll/unroll_context.h"
#include "Object/def/footprint.h"
#include "Object/type/fundamental_type_reference.h"
#include "Object/global_entry_context.h"
#include "common/TODO.h"
#include "common/ICE.h"
#include "util/what.h"
#include "util/persistent_object_manager.tcc"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/ref/inst_ref_implementation.h"
#include "Object/unroll/port_connection.h"
#include "util/stacktrace.h"
#include "Object/inst/instance_placeholder.h"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity {
using std::vector;
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class simple_meta_instance_reference method definitions

/**
	Private empty constructor.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_INSTANCE_REFERENCE_CLASS::simple_meta_instance_reference() :
		common_base_type(), 
		parent_type(), inst_collection_ref() {
	// no assert
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_INSTANCE_REFERENCE_CLASS::simple_meta_instance_reference(
		const instance_placeholder_ptr_type pi) :
		common_base_type(), 
		parent_type(), 
		inst_collection_ref(pi) {
	NEVER_NULL(inst_collection_ref);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Special ctor that attaches indices initially. 
	\param pi placeholder instance pointer.
	\param i indices.  
	\pre in this variation, we require dimensions to be exact
		and not underspecified with the indices, i.e. only
		allowed for scalar references (for now).  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_INSTANCE_REFERENCE_CLASS::simple_meta_instance_reference(
		const instance_placeholder_ptr_type pi, 
		indices_ptr_arg_type i) :
		common_base_type(i), 
		parent_type(), 
		inst_collection_ref(pi) {
	NEVER_NULL(inst_collection_ref);
#if 0
	INVARIANT(i && (i->size() == this->inst_collection_ref->get_dimensions())
		|| !this->inst_collection_ref->get_dimensions());
#else
	INVARIANT(!this->dimensions());
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
SIMPLE_META_INSTANCE_REFERENCE_CLASS::~simple_meta_instance_reference() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const instance_placeholder_base>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::get_inst_base(void) const {
	return inst_collection_ref;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_INSTANCE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just wrapped around common base class implmentation.  
	Ripped off of the old simple_meta_indexed_reference_base::dump()
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_INSTANCE_REFERENCE_CLASS::dump(ostream& o, 
		const expr_dump_context& c) const {
	if (c.include_type_info)
		this->what(o) << " ";
	NEVER_NULL(this->inst_collection_ref);
	if (c.parent_instance_name.size()) {
		o << c.parent_instance_name << ".";
		this->inst_collection_ref->dump_qualified_name(o,
			dump_flags::no_definition_owner);
	} else if (c.enclosing_scope) {
		this->inst_collection_ref->dump_qualified_name(o,
			dump_flags::no_definition_owner);
	} else {
		this->inst_collection_ref->dump_qualified_name(o,
			dump_flags::default_value);
	}
	return simple_meta_indexed_reference_base::dump_indices(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Simplified from simple_meta_indexed_reference_base::dump_type_size().
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
SIMPLE_META_INSTANCE_REFERENCE_CLASS::dump_type_size(ostream& o) const {
	this->get_unresolved_type_ref()->dump(o);
	const size_t d = this->dimensions();
	if (d) {
		o << '{' << d << "-dim}";
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_META_INSTANCE_REFERENCE_CLASS::dimensions(void) const {
	size_t dim = this->inst_collection_ref->get_dimensions();
	if (array_indices) {
		const size_t c = this->array_indices->dimensions_collapsed();
		INVARIANT(c <= dim);
		return dim -c;
	} else	return dim;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const definition_base>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::get_base_def(void) const {
	return this->inst_collection_ref->get_base_def();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::get_unresolved_type_ref(void) const {
	return this->inst_collection_ref->get_unresolved_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy-reduced from simple_meta_indexed_reference_base.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::attach_indices(indices_ptr_arg_type i) {
	INVARIANT(!array_indices);
	NEVER_NULL(i);
	// dimension-check:
	// number of indices must be <= dimension of instance collection.  
	const size_t max_dim = dimensions();    // depends on indices
	if (i->size() > max_dim) {
		cerr << "ERROR: instance collection " <<
			this->inst_collection_ref->get_name()
			<< " is " << max_dim << "-dimensional, and thus, "
			"cannot be indexed " << i->size() <<
			"-dimensionally!  ";
			// caller will say where
		return good_bool(false);
	}
	// no static dimension checking
	array_indices = i;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this is called, we're at the top-level of the instance hierarchy.
	This should work regardless of whether this type has substructure.  
	Only called from top-level, context-free.  
	NOTE: this can also be used to lookup up a local footprint
	if the footprint is a not the top-level.  In this case, the
	index returned is a local index which needs to be translated
	to a global index through a footprint_frame lookup.  
	Will private subinstances still work on local references? no
	\param sm is not used here, but is needed for member lookups, 
		implemented in the virtual override.  
	\return 1-based global index, 0 on error.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_META_INSTANCE_REFERENCE_CLASS::lookup_globally_allocated_index(
		const global_entry_context& gc, 
		const unroll_context* ucp) const {
	STACKTRACE_VERBOSE;
	const footprint& top(gc.get_top_footprint());
	const footprint_frame* const fpf = gc.get_footprint_frame();
	const unroll_context uc(fpf ? fpf->_footprint : &top, &top);
	const unroll_context* cp = ucp ? ucp : &uc;
	// should not be virtual call (one hopes)
	// translate local index to global
	const size_t lid = this->lookup_locally_allocated_index(*cp);
	if (lid) {
		return gc.template lookup_global_id<Tag>(lid);
	} else {
		return 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
global_indexed_reference
SIMPLE_META_INSTANCE_REFERENCE_CLASS::lookup_top_level_reference(
		const global_entry_context& gc) const {
	STACKTRACE_VERBOSE;
#if 0
	// also works
	return global_indexed_reference(traits_type::type_tag_enum_value, 
		this->lookup_globally_allocated_index(gc));
#else
	const size_t d = this->dimensions();
	if (d) {
		cerr << "Error: expecting scalar reference, but got " <<
			d << "-dimensional array reference." << endl;
		return global_indexed_reference(META_TYPE_NONE, 0);
	}
	global_reference_array_type tmp;
	if (lookup_top_level_references(gc, tmp).good) {
		return tmp.front();
	} else {
		return global_indexed_reference(META_TYPE_NONE, 0);
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::lookup_top_level_references(
		const global_entry_context& gc,
		global_reference_array_type& ret) const {
	STACKTRACE_VERBOSE;
	vector<size_t> tmp;
	const footprint& top(gc.get_top_footprint());
	const footprint_frame* const fpf = gc.get_footprint_frame();
	const footprint& fp(fpf ? *fpf->_footprint : top);
	if (this->lookup_globally_allocated_indices(fp, tmp).good) {
		ret.reserve(tmp.size());
		transform(tmp.begin(), tmp.end(), back_inserter(ret), 
			std::bind1st(std::ptr_fun(&make_global_reference),
				size_t(traits_type::type_tag_enum_value)));
		return good_bool(true);
	} else {
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up a local index, local to lookup-footprint
	in the unroll_context argument.  
	To convert to a global index, caller is responsible
	for translating using the footprint_frame.  
	\return 1-based index for local alias, 0 for error.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
size_t
SIMPLE_META_INSTANCE_REFERENCE_CLASS::lookup_locally_allocated_index(
		const unroll_context& uc) const {
	STACKTRACE_VERBOSE;
	const instance_alias_info_ptr_type
		alias(__unroll_generic_scalar_reference(
			*this->inst_collection_ref, this->array_indices,
			uc));
	if (!alias) {
		cerr << "Error resolving a single instance alias." << endl;
		return 0;
	}
	const size_t ret = alias->instance_index;
	INVARIANT(ret);
	STACKTRACE_INDENT_PRINT("local-index = " << ret << endl);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if AGGREGATE_PARENT_REFS
/**
	Collects a bunch of ids at once.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::lookup_locally_allocated_indices(
		const unroll_context& uc, vector<size_t>& ids) const {
	STACKTRACE_VERBOSE;
	alias_collection_type aliases;
	if (!__unroll_generic_scalar_references(
			*this->inst_collection_ref, this->array_indices,
			uc, aliases).good) {
		cerr << "Error resolving instance aliases." << endl;
		return good_bool(false);
	}
	ids.resize(aliases.sizes_product());
	transform(aliases.begin(), aliases.end(), ids.begin(), 
		instance_index_extractor());
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const SIMPLE_META_INSTANCE_REFERENCE_CLASS>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::__unroll_resolve_copy(
		const unroll_context& c, 
		const count_ptr<const this_type>& p) const {
	typedef	count_ptr<this_type>	return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	if (this->array_indices) {
		const count_ptr<const const_index_list>
			resolved_indices(unroll_resolve_indices(c));
		if (!resolved_indices) {
			cerr << "Error resolving meta indices." << endl;
			return return_type(NULL);
		}
		const return_type
			ret(new this_type(this->inst_collection_ref));
		ret->attach_indices(resolved_indices);
		return ret;
	} else {
		return p;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Covariant wrapper.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const meta_instance_reference_base>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_resolve_copy(
		const unroll_context& c, 
		const count_ptr<const meta_instance_reference_base>& p) const {
	return __unroll_resolve_copy(c, p.template is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the reference into aliases.  
	\param c the context of unrolling.
	\param a the destination collection in which to return
		resolved instance aliases, must be densely packed.  
	\return true on error, else false.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_references_packed(
		const unroll_context& c, alias_collection_type& a) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	this->dump(STACKTRACE_INDENT, expr_dump_context::default_value) << endl;
#endif
	return unroll_references_packed_helper(c, *this->inst_collection_ref,
		this->array_indices, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::lookup_global_reference_indices(
		const global_entry_context& gc, 
		typed_index_array_reference& ret,
		const unroll_context* uc) const {
	ret.first = traits_type::type_tag_enum_value;
	if (this->unroll_subindices_packed(gc, *uc, ret.second).bad) {
		ret.first = META_TYPE_NONE;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRIVATE_MEMBER_REFERENCES
/**
	Since this is not a member reference, it is relative to the 
	current scope/lookup_footprint.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
bad_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_subindices_packed(
		const global_entry_context&,
		const unroll_context& u,
		index_array_reference& a) const {
	STACKTRACE_VERBOSE;
	// doesn't work with private member references
	alias_collection_type aliases;
	// does this need global_entry_context at all?
	if (this->unroll_references_packed(u, aliases).bad) {
		return bad_bool(true);
	}
	a.resize(aliases.size());
	transform(aliases.begin(), aliases.end(), a.begin(), 
		instance_index_extractor());
	return bad_bool(false);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This does the real work.
	(Many other calls are forwarded to this.)
	\param lookup whether or not need to translate reference to
		local instance collection to actual footprint
		instance collection.  
	NOTE: not used anymore with placeholders!
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_INSTANCE_REFERENCE_CLASS::instance_alias_info_ptr_type
SIMPLE_META_INSTANCE_REFERENCE_CLASS::__unroll_generic_scalar_reference(
		const instance_placeholder_type& inst, 
		const count_ptr<const index_list_type>& ind, 
		const unroll_context& c) {
	typedef instance_alias_info_ptr_type 	return_type;
	STACKTRACE_VERBOSE;
	alias_collection_type aliases;
	const bad_bool
		bad(unroll_references_packed_helper(c, inst, ind, aliases));
	if (bad.bad) {
		return return_type(NULL);
	} else if (aliases.dimensions()) {
		cerr << "ERROR: got a " << aliases.dimensions() <<
			"-dimension collection where a scalar was required."
			<< endl;
		return return_type(NULL);
	} else {
		return aliases.front();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_INSTANCE_REFERENCE_CLASS::instance_alias_info_ptr_type
SIMPLE_META_INSTANCE_REFERENCE_CLASS::__unroll_generic_scalar_reference_no_lookup(
		const collection_interface_type& inst, 
		const count_ptr<const index_list_type>& ind, 
		const unroll_context& c) {
	typedef instance_alias_info_ptr_type 	return_type;
	STACKTRACE_VERBOSE;
	alias_collection_type aliases;
	const bad_bool
		bad(unroll_references_packed_helper_no_lookup(
			c, inst, ind, aliases));
	if (bad.bad) {
		return return_type(NULL);
	} else if (aliases.dimensions()) {
		cerr << "ERROR: got a " << aliases.dimensions() <<
			"-dimension collection where a scalar was required."
			<< endl;
		return return_type(NULL);
	} else {
		return aliases.front();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as the above, but collects a bunch of aliases, as opposed
	to a single scalar alias.  Note the plurality in the name.  
	\param aliases dimensionality is set by the caller (me).  
	\param lookup is OBSOLETE after re-writing using placeholders.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::__unroll_generic_scalar_references(
		const instance_placeholder_type& inst, 
		const count_ptr<const index_list_type>& ind, 
		const unroll_context& c, 
		alias_collection_type& aliases) {
	STACKTRACE_VERBOSE;
//	INVARIANT(lookup);	// until caller is re-written
	const bad_bool
		bad(unroll_references_packed_helper(c, inst, ind, aliases));
	// already have some error message
	return bad;	// implicit conversion to good
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
SIMPLE_META_INSTANCE_REFERENCE_CLASS::__unroll_generic_scalar_references_no_lookup(
		const collection_interface_type& inst, 
		const count_ptr<const index_list_type>& ind, 
		const unroll_context& c, 
		alias_collection_type& aliases) {
	STACKTRACE_VERBOSE;
	return unroll_references_packed_helper_no_lookup(c, inst, ind, aliases);
	// already have some error message
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by member_instance_reference::unroll_references_packed.
	This implementation should be policy-determined.  
	\return a single instance alias to a substructure_alias.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_INSTANCE_REFERENCE_CLASS::instance_alias_info_ptr_type
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_generic_scalar_reference(
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return __unroll_generic_scalar_reference(
			*this->inst_collection_ref,
			this->array_indices, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by member_instance_reference::unroll_references_packed.
	This implementation should be policy-determined.  
	\return a single instance alias to a substructure_alias.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<substructure_alias>
SIMPLE_META_INSTANCE_REFERENCE_CLASS::unroll_scalar_substructure_reference(
		const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	return substructure_implementation_policy::
		template simple_unroll_generic_scalar_substructure_reference<Tag>(
			*this, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
typename SIMPLE_META_INSTANCE_REFERENCE_CLASS::port_connection_ptr_type
SIMPLE_META_INSTANCE_REFERENCE_CLASS::make_port_connection_private(
		const count_ptr<const meta_instance_reference_base>& r) const {
	INVARIANT(r == this);
	return substructure_implementation_policy::make_port_connection(
		r.template is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	common_base_type::collect_transient_info_base(m);
	inst_collection_ref->collect_transient_info(m);
	// instantiation_state has no pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits children nodes and register pointers to object manager
	for serialization.  
	\param m the persistent object manager.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	this->collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common functionality for this->write_object and 
	member_SIMPLE_META_INSTANCE_REFERENCE_CLASS::write_object.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, inst_collection_ref);
	common_base_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the instance reference to output stream, translating
	pointers to indices as it goes along.  
	Note: the instantiation base must be written before the
		state information, for reconstruction purposes.  
	\param m the persistent object manager.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& f) const {
	this->write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common functionality for this->load_object and
	member_SIMPLE_META_INSTANCE_REFERENCE_CLASS::load_object.
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, inst_collection_ref);
	NEVER_NULL(inst_collection_ref);
#if 0
	// necessary? nope, just let the object_manager do it
	m.load_object_once(const_cast<instance_collection_generic_type*>(
		&*inst_collection_ref));
#endif
	common_base_type::load_object_base(m, i);
}
// else already visited

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Loads the instance reference from an input stream, translating
	indices to pointers.  
	Note: the instantiation base must be loaded before the
		state information, because the instantiation state
		depends on the instantiation base being complete.  
	\param m the persistent object manager.  
 */
SIMPLE_META_INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
void
SIMPLE_META_INSTANCE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& f) {
	this->load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_META_INSTANCE_REFERENCE_TCC__

