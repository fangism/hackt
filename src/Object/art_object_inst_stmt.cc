/**
	\file "art_object_inst_stmt.cc"
	Method definitions for instantiation statement classes.  
 	$Id: art_object_inst_stmt.cc,v 1.16.6.6 2005/03/11 05:16:40 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_STMT_CC__
#define	__ART_OBJECT_INST_STMT_CC__

// for debugging only, before inclusion of any header files
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS				0 && ENABLE_STACKTRACE

#include <iostream>
#include <algorithm>

#include "art_object_type_ref.h"
#include "art_object_instance.h"
#include "art_object_instance_param.h"
#include "art_object_inst_stmt.h"
#include "art_object_inst_ref_base.h"
#include "art_object_expr_base.h"
#include "art_built_ins.h"
#include "art_object_type_hash.h"
#include "art_object_unroll_context.h"
#include "art_object_classification_details.h"
#include "art_object_instance_collection.h"
#include "art_object_value_collection.h"

#if USE_INST_STMT_TEMPLATE
#include "art_object_inst_stmt_param.h"
#include "art_object_inst_stmt_data.h"
#include "art_object_inst_stmt_chan.h"
#include "art_object_inst_stmt_proc.h"
#endif

#include "what.tcc"
#include "memory/list_vector_pool.tcc"
#include "persistent_object_manager.tcc"
#include "stacktrace.h"
#include "static_trace.h"

// conditional defines, after inclusion of "stacktrace.h"
#if STACKTRACE_DESTRUCTORS
	#define	STACKTRACE_DTOR(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_DTOR(x)
#endif

#if STACKTRACE_PERSISTENTS
	#define	STACKTRACE_PERSISTENT(x)	STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif

#if USE_INST_STMT_TEMPLATE
#include "art_object_inst_stmt.tcc"
#endif

//=============================================================================
// local specializations
// Alternatively, explicit specialization here guarantees that the
// static initialization occurs in the correct order in this module.  
namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::data_instantiation_statement,
	"data_instantiation_statement")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_instantiation_statement,
	"pint_instantiation_statement")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_instantiation_statement,
	"pbool_instantiation_statement")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pbool_instantiation_statement, 
		PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::pint_instantiation_statement, 
		PINT_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_instantiation_statement, 
		PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_instantiation_statement, 
		CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::data_instantiation_statement, 
		DATA_INSTANTIATION_STATEMENT_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
// start of static initializations
STATIC_TRACE_BEGIN("inst_stmt")

//=============================================================================
namespace ART {
namespace entity {
USING_STACKTRACE
using util::persistent_traits;
#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// class instantiation_statement_base method definitions

#if 0
/**	Private empty constructor. */
instantiation_statement_base::instantiation_statement_base(void) :
//		inst_base(NULL), type_base(NULL),
		indices(NULL) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement_base::instantiation_statement_base(
		const index_collection_item_ptr_type& i) :
		indices(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement_base::~instantiation_statement_base() {
	STACKTRACE_DTOR("~instantiation_statement_base()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instantiation_statement_base::dump(ostream& o) const {
//	STACKTRACE("instantation_statement::dump()");
	const count_ptr<const fundamental_type_reference>
		type_base(get_type_ref());
	NEVER_NULL(type_base);
	type_base->dump(o) << " ";
	never_ptr<const instance_collection_base>
		inst_base(get_inst_base());
	if(inst_base) {
		o << inst_base->get_name();
	} else {
		o << "<unknown>";
	}
	if (indices)
		indices->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
instantiation_statement_base::get_name(void) const {
	const never_ptr<const instance_collection_base>
		inst_base(get_inst_base());
	NEVER_NULL(inst_base);
	return inst_base->get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference-counted pointer to instantiating indices, 
		which contains expressions, and may be null.  
 */
index_collection_item_ptr_type
instantiation_statement_base::get_indices(void) const {
	return indices;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This sets the range list passed by reference according to 
	the resolved values at unroll-time.  
	\pre Only call this if indices is NOT null.  
 */
good_bool
instantiation_statement_base::resolve_instantiation_range(
		const_range_list& r, const unroll_context& c) const {
if (indices) {
	INVARIANT(r.empty());	// not already constructed
	const good_bool ret(indices->unroll_resolve(r, c));
	if (!ret.good) {
		// ranges is passed and returned by reference
		// fail
		cerr << "ERROR: unable to resolve indices of " <<
			get_inst_base()->get_qualified_name() <<
			" for instantiation: ";
		indices->dump(cerr) << endl;
		THROW_EXIT;	// temporary non-error-handling
	}
	return ret;
} else {
	INVARIANT(r.empty());
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
instantiation_statement_base::dimensions(void) const {
	if (indices)
		return indices->dimensions();
	else return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary, should be pure virtual in the end.
 */
void
instantiation_statement_base::unroll(unroll_context& c) const {
	cerr << "instantiation_statement_base::unroll(): Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT(
		"instantiation_statement_base::collect_transient_info_base()");
	if (indices)
		indices->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement_base::load_object_base(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, indices);
}

//=============================================================================
// class param_instantiation_statement method definitions

#if 0
/**
	Private empty constructor.
 */
param_instantiation_statement::param_instantiation_statement() :
		parent_type() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation_statement::param_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		parent_type(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
param_instantiation_statement::~param_instantiation_statement() {
}
#endif

//=============================================================================
// class pbool_instantiation_statement method definitions

#if !USE_INST_STMT_TEMPLATE
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(pbool_instantiation_statement, 128)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pbool_instantiation_statement::pbool_instantiation_statement() :
		parent_type(), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation_statement::pbool_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		parent_type(i), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation_statement::~pbool_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pbool_instantiation_statement)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pbool_instantiation_statement::dump(ostream& o) const {
	return parent_type::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
pbool_instantiation_statement::attach_collection(
		const never_ptr<instance_collection_base> i) {
	INVARIANT(!inst_base);
	inst_base = i.is_a<collection_type>();
	NEVER_NULL(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
pbool_instantiation_statement::get_inst_base(void) {
	NEVER_NULL(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
pbool_instantiation_statement::get_inst_base(void) const {
	NEVER_NULL(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
pbool_instantiation_statement::get_type_ref(void) const {
	return pbool_type_ptr;		// built-in type pointer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::unroll(unroll_context& c) const {
	NEVER_NULL(inst_base);
	inst_base->instantiate_indices(indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(inst_base);
	// let the scopespace take care of it
	// inst_base->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, inst_base);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_instantiation_statement::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, inst_base);
	parent_type::load_object_base(m, f);
}

//=============================================================================
// class pint_instantiation_statement method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(pint_instantiation_statement, 256)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pint_instantiation_statement::pint_instantiation_statement() :
		parent_type(), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation_statement::pint_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		parent_type(i), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation_statement::~pint_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_instantiation_statement)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pint_instantiation_statement::dump(ostream& o) const {
//	STACKTRACE("pint_instantation_statement::dump()");
	return parent_type::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
pint_instantiation_statement::attach_collection(
		const never_ptr<instance_collection_base> i) {
	INVARIANT(!inst_base);
	inst_base = i.is_a<collection_type>();
	INVARIANT(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
pint_instantiation_statement::get_inst_base(void) {
	INVARIANT(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
pint_instantiation_statement::get_inst_base(void) const {
	INVARIANT(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
pint_instantiation_statement::get_type_ref(void) const {
	return pint_type_ptr;		// built-in type pointer
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::unroll(unroll_context& c) const {
	NEVER_NULL(inst_base);
	inst_base->instantiate_indices(indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
STACKTRACE_PERSISTENT("pint_instantiation_statement::collect_transient_info()");
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(inst_base);
	// let the scopespace take care of it
	// inst_base->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, inst_base);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_instantiation_statement::load_object(const persistent_object_manager& m, 
		istream&f ) {
	m.read_pointer(f, inst_base);
	parent_type::load_object_base(m, f);
}
#endif	// USE_INST_STMT_TEMPLATE

//=============================================================================
// class process_instantiation_statement method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
process_instantiation_statement::process_instantiation_statement() :
		parent_type(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation_statement::process_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		parent_type(i),
		type(t), inst_base(NULL) {
	NEVER_NULL(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation_statement::~process_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instantiation_statement::what(ostream& o) const {
	return o << "process-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instantiation_statement::dump(ostream& o) const {
	return parent_type::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
process_instantiation_statement::attach_collection(
		const never_ptr<instance_collection_base> i) {
	INVARIANT(!inst_base);
	inst_base = i.is_a<collection_type>();
	INVARIANT(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
process_instantiation_statement::get_inst_base(void) {
	INVARIANT(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
process_instantiation_statement::get_inst_base(void) const {
	INVARIANT(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
process_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::unroll(unroll_context& c) const {
	STACKTRACE("process_instantiation_statement::unroll()");
	NEVER_NULL(inst_base);
	// we need to type-check against template parameters!
	// perhaps this should be made virtual...
	const count_ptr<const process_type_reference>
		final_type_ref(type->unroll_resolve(c));
	if (!final_type_ref) {
		cerr << "ERROR resolving proces type reference during unroll."
			<< endl;
		return;
	}
	const bad_bool err(inst_base->commit_type(final_type_ref));
	if (err.bad) {
		cerr << "ERROR during process_instantiation_statement::unroll()"
			<< endl;
		THROW_EXIT;
	}
#if 0
	inst_base->instantiate_indices(indices);
#else
	const_range_list crl;
	const good_bool rr(resolve_instantiation_range(crl, c));
	if (rr.good) {
		inst_base->instantiate_indices(crl);
	} else {
		cerr << "ERROR: resolving index range of instantiation!"
			<< endl;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(inst_base);
	NEVER_NULL(type);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.write_pointer(f, type);		NEVER_NULL(type);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.read_pointer(f, type);		NEVER_NULL(type);
	parent_type::load_object_base(m, f);
#if 0
	m.load_object(type);
	m.load_object(inst_base);
	if (indices)
		m.load_object(indices);
#endif
}

//=============================================================================
// class channel_instantiation_statement method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
channel_instantiation_statement::channel_instantiation_statement() :
		parent_type(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation_statement::channel_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		parent_type(i),
		type(t), inst_base(NULL) {
	NEVER_NULL(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation_statement::~channel_instantiation_statement() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instantiation_statement::what(ostream& o) const {
	return o << "channel-instantiation_statement";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instantiation_statement::dump(ostream& o) const {
	return parent_type::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
channel_instantiation_statement::attach_collection(
		const never_ptr<instance_collection_base> i) {
	INVARIANT(!inst_base);
	inst_base = i.is_a<collection_type>();
	INVARIANT(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
channel_instantiation_statement::get_inst_base(void) {
	INVARIANT(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
channel_instantiation_statement::get_inst_base(void) const {
	INVARIANT(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
channel_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	INVARIANT(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.write_pointer(f, type);		NEVER_NULL(type);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instantiation_statement::load_object(
		const persistent_object_manager& m, istream& f) {
	m.read_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.read_pointer(f, type);		NEVER_NULL(type);
	parent_type::load_object_base(m, f);
#if 0
	m.load_object(type);
	m.load_object(inst_base);
	if (indices)
		m.load_object(indices);
#endif
}

//=============================================================================
// class data_instantiation_statement method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(data_instantiation_statement, 64)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
data_instantiation_statement::data_instantiation_statement() :
		parent_type(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_instantiation_statement::data_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		parent_type(i),
		type(t), inst_base(NULL) {
	NEVER_NULL(type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_instantiation_statement::~data_instantiation_statement() {
	STACKTRACE_DTOR("~data_instantiation_statement()");
#if 0
	cerr << "data-type-ref has " << type.refs() << " references." << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(data_instantiation_statement)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
data_instantiation_statement::dump(ostream& o) const {
	return parent_type::dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre inst_base is not yet set.
 */
void
data_instantiation_statement::attach_collection(
		const never_ptr<instance_collection_base> i) {
	INVARIANT(!inst_base);
	inst_base = i.is_a<collection_type>();
	NEVER_NULL(inst_base);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<instance_collection_base>
data_instantiation_statement::get_inst_base(void) {
	NEVER_NULL(inst_base);
	return inst_base.as_a<instance_collection_base>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const instance_collection_base>
data_instantiation_statement::get_inst_base(void) const {
	NEVER_NULL(inst_base);
	return inst_base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const fundamental_type_reference>
data_instantiation_statement::get_type_ref(void) const {
	return type;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this is the first instantiation statement, then
	this will determine the parameters.  
	Otherwise, this will check the statement's resolved
	parameters against the previously determined parameters.  
	\param c the context information for expression resolution.  
 */
void
data_instantiation_statement::unroll(unroll_context& c) const {
	STACKTRACE("data_instantiation_statement::unroll()");
	NEVER_NULL(inst_base);
	// we need to type-check against template parameters!
	// perhaps this should be made virtual...
	const count_ptr<const data_type_reference>
		final_type_ref(type->unroll_resolve(c));
	if (!final_type_ref) {
		cerr << "ERROR resolving data type reference during unroll."
			<< endl;
		return;
	}
	const bad_bool err(inst_base->commit_type(final_type_ref));
	if (err.bad) {
		cerr << "ERROR during data_instantiation_statement::unroll()"
			<< endl;
		THROW_EXIT;
	}
#if 0
	inst_base->instantiate_indices(indices);
#else
	const_range_list crl;
	const good_bool rr(resolve_instantiation_range(crl, c));
	if (rr.good) {
		inst_base->instantiate_indices(crl);
	} else {
		cerr << "ERROR: resolving index range of instantiation!"
			<< endl;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
// STACKTRACE("data_instantiation_statement::collect_transient_info()");
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::write_object(
		const persistent_object_manager& m, ostream& f) const {
	m.write_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.write_pointer(f, type);		NEVER_NULL(type);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::load_object(const persistent_object_manager& m, 
		istream& f) {
	m.read_pointer(f, inst_base);		NEVER_NULL(inst_base);
	m.read_pointer(f, type);		NEVER_NULL(type);
	parent_type::load_object_base(m, f);
}

//=============================================================================
// explicit template class instantiations

#if USE_INST_STMT_TEMPLATE
template class instantiation_statement<pbool_tag>;
template class instantiation_statement<pint_tag>;
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("inst_stmt")

// responsibly, anally, undefining macros local to this module
#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT

#endif	// __ART_OBJECT_INST_STMT_CC__

