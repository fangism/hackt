/**
	\file "Object/inst/dummy_placeholder.tcc"
	$Id: dummy_placeholder.tcc,v 1.3 2011/02/08 22:32:47 fang Exp $
	TODO: trim includes
 */

#ifndef	__HAC_OBJECT_INST_DUMMY_PLACEHOLDER_TCC__
#define	__HAC_OBJECT_INST_DUMMY_PLACEHOLDER_TCC__

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// overridable debug switches

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#include <iostream>
#include <vector>

// experimental: suppressing automatic template instantiation
#include "Object/common/extern_templates.h"

#include "Object/inst/dummy_placeholder.h"
#include "Object/expr/const_range_list.h"
#include "Object/ref/meta_instance_reference_base.h"
#include "Object/ref/nonmeta_instance_reference_base.h"

#include "common/TODO.h"
#include "common/ICE.h"

#include "util/memory/count_ptr.tcc"
// #include "util/memory/chunk_map_pool.tcc"

#include "util/persistent_object_manager.tcc"
// #include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
// #include "util/static_trace.h"
// #include "util/compose.h"
// #include "util/binders.h"
// #include "util/dereference.h"

#if ENABLE_STACKTRACE
#include <iterator>
#endif

//=============================================================================

namespace HAC {
namespace entity {
// using std::string;
// using std::_Select1st;
#include "util/using_ostream.h"
// using util::multikey_generator;
// USING_UTIL_COMPOSE
// using util::dereference;
// using std::mem_fun_ref;
// using util::bind2nd_argval;
// using util::multikey;
// using util::value_writer;
// using util::value_reader;
// using util::write_value;
// using util::read_value;
// using util::indent;
// using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class dummy_placeholder method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
DUMMY_PLACEHOLDER_CLASS::dummy_placeholder() :
		parent_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
DUMMY_PLACEHOLDER_CLASS::dummy_placeholder(const scopespace& o, 
		const string& n, const size_t d) :
		parent_type(o, n, d) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
DUMMY_PLACEHOLDER_CLASS::~dummy_placeholder() {
	STACKTRACE_DTOR("~dummy_placeholder<>()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: if this->dimensions hasn't been reconstructed from 
	deserialization, then it may be reported as uninitialized garbage.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
DUMMY_PLACEHOLDER_CLASS::what(ostream& o) const {
	return o << traits_type::tag_name << " " << this->dimensions <<
		"-D (placeholder)";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This needs to be specialized with a functor...
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
DUMMY_PLACEHOLDER_CLASS::type_dump(ostream& o) const {
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
	Dummy instances should never appear in formals.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
ostream&
DUMMY_PLACEHOLDER_CLASS::dump_formal(ostream& o) const {
	o << "node " << this->key;
	ICE_NEVER_CALL(cerr);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns the type-reference given by the first instantiation
	statement (which may be predicated).  
	This is not guaranteed to be the *final* type of the collection.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
DUMMY_PLACEHOLDER_CLASS::get_unresolved_type_ref(void) const {
#if 0
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_type_ref();
#else
	return traits_type::built_in_type_ptr;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Covariant return version.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
typename DUMMY_PLACEHOLDER_CLASS::type_ref_ptr_type
DUMMY_PLACEHOLDER_CLASS::get_unresolved_type_ref_subtype(void) const {
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_type_ref_subtype();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Associates this instance placeholder with an
	initial instantiation statement pointer. 
	For port placeholders, this is the only instantiation statement.
	For others, it's just a hint for the initial type, 
		and may not even be used.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
void
DUMMY_PLACEHOLDER_CLASS::attach_initial_instantiation_statement(
		const count_ptr<const instantiation_statement_base>& i) {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off from dummy_placeholder_base::formal_size_equivalent.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
index_collection_item_ptr_type
DUMMY_PLACEHOLDER_CLASS::get_initial_instantiation_indices(void) const {
#if 0
	NEVER_NULL(this->initial_instantiation_statement_ptr);
	return this->initial_instantiation_statement_ptr->get_indices();
#else
	ICE_NEVER_CALL(cerr);
	return index_collection_item_ptr_type(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param c the unroll_context, whose (read-only) lookup footprint 
		is that of the super-instance's type.  
	\return pointer to new-allocated collection, that is memory-owned
		by the managing (target) footprint.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
never_ptr<physical_instance_collection>
DUMMY_PLACEHOLDER_CLASS::unroll_port_only(const unroll_context& c) const {
	ICE_NEVER_CALL(cerr);
	return never_ptr<physical_instance_collection>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just creates a simple instance_reference to this collection.  
	If indexed, the index is set by the caller.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<meta_instance_reference_base>
DUMMY_PLACEHOLDER_CLASS::make_meta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<meta_instance_reference_base>	ptr_return_type;
#if 0
	return ptr_return_type(new simple_meta_instance_reference_type(
		never_ptr<const this_type>(this)));
#else
	this->what(cerr << "Error: creating explicit reference to ") <<
		" is not allowed." << endl;
	return ptr_return_type(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a nonmeta instance reference.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
count_ptr<nonmeta_instance_reference_base>
DUMMY_PLACEHOLDER_CLASS::make_nonmeta_instance_reference(void) const {
	// depends on whether this instance is collective, 
	//      check array dimensions -- when attach_indices() invoked
	typedef	count_ptr<nonmeta_instance_reference_base>	ptr_return_type;
	ICE_NEVER_CALL(cerr);
	return ptr_return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does exactly what it says.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
typename DUMMY_PLACEHOLDER_CLASS::member_inst_ref_ptr_type
DUMMY_PLACEHOLDER_CLASS::make_member_meta_instance_reference(
		const inst_ref_ptr_type& b) const {
	ICE_NEVER_CALL(cerr);
	return member_inst_ref_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Despite the name, this doesn't copy...
	\return new instance collection for footprint.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
instance_collection_base*
DUMMY_PLACEHOLDER_CLASS::make_instance_collection_footprint_copy(
		footprint& f) const {
	ICE_NEVER_CALL(cerr);
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\returns a new collection.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
typename DUMMY_PLACEHOLDER_CLASS::instance_collection_generic_type*
DUMMY_PLACEHOLDER_CLASS::make_collection(footprint& f) const {
	ICE_NEVER_CALL(cerr);
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	initial_instantiation_statement_ptr is permitted to be NULL
	for instance collections that belong to footprints.  
 */
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
void
DUMMY_PLACEHOLDER_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	STACKTRACE_PERSISTENT("dummy_placeholder<Tag>::collect_base()");
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
void
DUMMY_PLACEHOLDER_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT("dummy_placeholder<Tag>::write_base()");
	parent_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DUMMY_PLACEHOLDER_TEMPLATE_SIGNATURE
void
DUMMY_PLACEHOLDER_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT("dummy_placeholder<Tag>::load_base()");
	parent_type::load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_DUMMY_PLACEHOLDER_TCC__

