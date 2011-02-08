/**
	\file "Object/inst/instance_placeholder.tcc"
	$Id: instance_placeholder.tcc,v 1.10 2011/02/08 22:32:48 fang Exp $
	TODO: trim includes
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_TCC__
#define	__HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overridable debug switches

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <exception>
#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>

// experimental: suppressing automatic template instantiation
#include "Object/common/extern_templates.h"

#include "Object/inst/instance_placeholder.h"
#include "Object/inst/instance_collection.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_param_expr_list.h"		// for debug only
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range_list.h"
#include "Object/def/definition_base.h"
#include "Object/def/footprint.h"
#include "Object/type/canonical_type.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/nonmeta_instance_reference_subtypes.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/unroll/instantiation_statement_base.h"
#include "Object/unroll/instantiation_statement.h"
#include "Object/unroll/unroll_context.h"
#include "Object/inst/port_actual_collection.h"
#include "Object/inst/instance_collection_pool_bundle.tcc"	// for allocate
#include "common/ICE.h"

#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"

#include "util/persistent_object_manager.tcc"
#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/static_trace.h"
#include "util/compose.h"
#include "util/binders.h"
#include "util/dereference.h"

#if ENABLE_STACKTRACE
#include <iterator>
#endif

//=============================================================================

namespace HAC {
namespace entity {
using std::string;
using std::_Select1st;
#include "util/using_ostream.h"
using util::multikey_generator;
USING_UTIL_COMPOSE
using util::dereference;
using std::mem_fun_ref;
using util::bind2nd_argval;
using util::multikey;
using util::value_writer;
using util::value_reader;
using util::write_value;
using util::read_value;
using util::indent;
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class instance_placeholder method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
INSTANCE_PLACEHOLDER_CLASS::instance_placeholder() :
		parent_type(), initial_instantiation_statement_ptr(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
INSTANCE_PLACEHOLDER_CLASS::instance_placeholder(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d), 
		initial_instantiation_statement_ptr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
INSTANCE_PLACEHOLDER_CLASS::~instance_placeholder() {
	STACKTRACE_DTOR("~instance_placeholder<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: if this->dimensions hasn't been reconstructed from 
	deserialization, then it may be reported as uninitialized garbage.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
INSTANCE_PLACEHOLDER_CLASS::what(ostream& o) const {
	return o << traits_type::tag_name << " " << this->dimensions <<
		"-D (placeholder)";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This needs to be specialized with a functor...
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
INSTANCE_PLACEHOLDER_CLASS::type_dump(ostream& o) const {
#if 0
	typename collection_type_manager_parent_type::dumper dump_it(o);
	return dump_it(*this);
#else
	o << "(TODO: infer type from inst-stmt)";
	return o;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: for port collections with relaxed types, might want to expand
	relaxed actuals at some point for diagnostics.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
INSTANCE_PLACEHOLDER_CLASS::dump_formal(ostream& o) const {
	this->get_unresolved_type_ref()->dump(o) << ' ' << this->key;
	expr_dump_context dc(expr_dump_context::default_value);
	dc.enclosing_scope = this->owner;
	if (this->dimensions) {
		const index_collection_item_ptr_type
			i(this->get_initial_instantiation_indices());
		NEVER_NULL(i);
		i->dump(o, dc);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the type-reference given by the first instantiation
	statement (which may be predicated).  
	This is not guaranteed to be the *final* type of the collection.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
INSTANCE_PLACEHOLDER_CLASS::get_unresolved_type_ref(void) const {
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Covariant return version.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
typename INSTANCE_PLACEHOLDER_CLASS::type_ref_ptr_type
INSTANCE_PLACEHOLDER_CLASS::get_unresolved_type_ref_subtype(void) const {
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_type_ref_subtype();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Associates this instance placeholder with an
	initial instantiation statement pointer. 
	For port placeholders, this is the only instantiation statement.
	For others, it's just a hint for the initial type, 
		and may not even be used.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
INSTANCE_PLACEHOLDER_CLASS::attach_initial_instantiation_statement(
		const count_ptr<const instantiation_statement_base>& i) {
	if (!this->initial_instantiation_statement_ptr) {
		this->initial_instantiation_statement_ptr =
			i.template is_a<const initial_instantiation_statement_type>();
	}
	NEVER_NULL(this->initial_instantiation_statement_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off from instance_placeholder_base::formal_size_equivalent.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
index_collection_item_ptr_type
INSTANCE_PLACEHOLDER_CLASS::get_initial_instantiation_indices(void) const {
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_indices();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param c the unroll_context, whose (read-only) lookup footprint 
		is that of the super-instance's type.  
	\return pointer to new-allocated collection, that is memory-owned
		by the managing (target) footprint.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
never_ptr<physical_instance_collection>
INSTANCE_PLACEHOLDER_CLASS::unroll_port_only(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	INVARIANT(this->initial_instantiation_statement_ptr);
	typedef	port_actual_collection<Tag>	port_collection_type;
#if 0 && ENABLE_STACKTRACE
	cerr << "looking up: " << this->key << endl;
	c.dump(cerr << "context: ") << endl;
#endif
	const never_ptr<const physical_instance_collection>
		b(c.lookup_port_collection(*this));
	NEVER_NULL(b);
	const never_ptr<const instance_collection_generic_type>
		back_ref(b.template is_a<const instance_collection_generic_type>());
	NEVER_NULL(back_ref);
	// pass unroll_context to instantiate recursively
	collection_pool_bundle_type&
		pool(c.get_target_footprint().
			template get_instance_collection_pool_bundle<Tag>());
	const never_ptr<port_collection_type>
		ret(pool.allocate_port_collection(back_ref, c));
#if 0
	NEVER_NULL(ret);
#else
	if (!ret) {
		cerr << "Error unrolling port." << endl;
	}
#endif
	// TODO: attach relaxed parameters --
	// this may be done in the unrolling of the ports now.
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just creates a simple instance_reference to this collection.  
	If indexed, the index is set by the caller.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<meta_instance_reference_base>
INSTANCE_PLACEHOLDER_CLASS::make_meta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<meta_instance_reference_base>	ptr_return_type;
	return ptr_return_type(new simple_meta_instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a nonmeta instance reference.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<nonmeta_instance_reference_base>
INSTANCE_PLACEHOLDER_CLASS::make_nonmeta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<nonmeta_instance_reference_base>	ptr_return_type;
	return ptr_return_type(new simple_nonmeta_instance_reference_type(
			never_ptr<const this_type>(this)));
		// omitting index argument, set it later...
		// done by parser::instance_array::check_build()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does exactly what it says.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
typename INSTANCE_PLACEHOLDER_CLASS::member_inst_ref_ptr_type
INSTANCE_PLACEHOLDER_CLASS::make_member_meta_instance_reference(
		const inst_ref_ptr_type& b) const {
	NEVER_NULL(b);
	return member_inst_ref_ptr_type(
		new member_simple_meta_instance_reference_type(
			b, never_ptr<const this_type>(this)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Despite the name, this doesn't copy...
	\return new instance collection for footprint.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
// typename INSTANCE_PLACEHOLDER_CLASS::instance_collection_generic_type*
instance_collection_base*
INSTANCE_PLACEHOLDER_CLASS::make_instance_collection_footprint_copy(
		footprint& f) const {
	return this->make_collection(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\returns a new collection.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
typename INSTANCE_PLACEHOLDER_CLASS::instance_collection_generic_type*
INSTANCE_PLACEHOLDER_CLASS::make_collection(footprint& f) const {
	typedef	instance_collection_generic_type*	return_type;
	collection_pool_bundle_type&
		pool(f.template get_instance_collection_pool_bundle<Tag>());
	if (this->is_port_formal()) {
		const return_type ret = pool.allocate_port_formal(
			f, never_ptr<const this_type>(this));
		NEVER_NULL(ret);
		return ret;
	} else {
		const return_type ret = pool.allocate_local_collection(
			f, never_ptr<const this_type>(this));
		NEVER_NULL(ret);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	initial_instantiation_statement_ptr is permitted to be NULL
	for instance collections that belong to footprints.  
 */
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
INSTANCE_PLACEHOLDER_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_PERSISTENT("instance_placeholder<Tag>::collect_base()");
	parent_type::collect_transient_info_base(m);
	if (this->initial_instantiation_statement_ptr) {
		initial_instantiation_statement_ptr->collect_transient_info(m);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
INSTANCE_PLACEHOLDER_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("instance_placeholder<Tag>::write_base()");
	parent_type::write_object_base(m, o);
	m.write_pointer(o, this->initial_instantiation_statement_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_PLACEHOLDER_TEMPLATE_SIGNATURE
void
INSTANCE_PLACEHOLDER_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("instance_placeholder<Tag>::load_base()");
	parent_type::load_object_base(m, i);
	m.read_pointer(i, this->initial_instantiation_statement_ptr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_PLACEHOLDER_TCC__

