/**
	\file "art_object_inst_stmt.cc"
	Method definitions for instantiation statement classes.  
 	$Id: art_object_inst_stmt.cc,v 1.12.2.1.10.2 2005/02/18 06:07:44 fang Exp $
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
	#define	STACKTRACE_PERSISTENT(x)		STACKTRACE(x)
#else
	#define	STACKTRACE_PERSISTENT(x)
#endif

//=============================================================================
// local specializations
#if 0
// need to explicitly instantiate here because list_vector_pool's
// static initialization requires that the ::name be initialized first.
// Without this, the name is automatically instantiated, but too late.
template struct util::what<ART::entity::data_instantiation_statement>;
#else
// Alternatively, explicit specialization here guarantees that the
// static initialization occurs in the correct order in this module.  
namespace util {
SPECIALIZE_UTIL_WHAT(ART::entity::data_instantiation_statement,
	"data_instantiation_statement")
SPECIALIZE_UTIL_WHAT(ART::entity::pint_instantiation_statement,
	"pint_instantiation_statement")
SPECIALIZE_UTIL_WHAT(ART::entity::pbool_instantiation_statement,
	"pbool_instantiation_statement")
}
#endif

//=============================================================================
// start of static initializations
STATIC_TRACE_BEGIN("inst_stmt")

//=============================================================================
namespace ART {
namespace entity {
USING_STACKTRACE
#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
#endif

//=============================================================================
// class instantiation_statement method definitions

#if 0
/**	Private empty constructor. */
instantiation_statement::instantiation_statement(void) :
//		inst_base(NULL), type_base(NULL),
		indices(NULL) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
instantiation_statement::instantiation_statement(
		const index_collection_item_ptr_type& i) :
		indices(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
instantiation_statement::~instantiation_statement() {
	STACKTRACE_DTOR("~instantiation_statement()");
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
instantiation_statement::dump(ostream& o) const {
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
instantiation_statement::get_name(void) const {
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
instantiation_statement::get_indices(void) const {
	return indices;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
instantiation_statement::dimensions(void) const {
	if (indices)
		return indices->dimensions();
	else return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Temporary, should be pure virtual in the end.
 */
void
instantiation_statement::unroll(unroll_context& c) const {
	cerr << "instantiation_statement::unroll(): Fang, finish me!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT(
		"instantiation_statement::collect_transient_info_base()");
	if (indices)
		indices->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
instantiation_statement::load_object_base(
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
		instantiation_statement() {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_instantiation_statement::param_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		instantiation_statement(i) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
param_instantiation_statement::~param_instantiation_statement() {
}
#endif

//=============================================================================
// class pbool_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pbool_instantiation_statement, 
	PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(pbool_instantiation_statement, 128)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pbool_instantiation_statement::pbool_instantiation_statement() :
		param_instantiation_statement(), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pbool_instantiation_statement::pbool_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		param_instantiation_statement(i), 
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
	return instantiation_statement::dump(o);
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
if (!m.register_transient_object(this, PBOOL_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	NEVER_NULL(inst_base);
	// let the scopespace take care of it
	// inst_base->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pbool_instantiation_statement::construct_empty(const int i) {
	return new pbool_instantiation_statement();
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

DEFAULT_PERSISTENT_TYPE_REGISTRATION(pint_instantiation_statement, 
	PINT_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(pint_instantiation_statement, 256)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
pint_instantiation_statement::pint_instantiation_statement() :
		param_instantiation_statement(), 
		inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_instantiation_statement::pint_instantiation_statement(
		const index_collection_item_ptr_type& i) :
		param_instantiation_statement(i), 
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
	return instantiation_statement::dump(o);
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
if (!m.register_transient_object(this, PINT_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	NEVER_NULL(inst_base);
	// let the scopespace take care of it
	// inst_base->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
pint_instantiation_statement::construct_empty(const int i) {
	return new pint_instantiation_statement();
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

//=============================================================================
// class process_instantiation_statement method definitions

DEFAULT_PERSISTENT_TYPE_REGISTRATION(process_instantiation_statement, 
	PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
process_instantiation_statement::process_instantiation_statement() :
		instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instantiation_statement::process_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		instantiation_statement(i),
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
	return instantiation_statement::dump(o);
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
	const bool err = inst_base->commit_type(final_type_ref);
	if (err) {
		cerr << "ERROR during process_instantiation_statement::unroll()"
			<< endl;
		THROW_EXIT;
	}
	inst_base->instantiate_indices(indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, PROCESS_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	NEVER_NULL(inst_base);
	NEVER_NULL(type);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
process_instantiation_statement::construct_empty(const int i) {
	return new process_instantiation_statement();
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

DEFAULT_PERSISTENT_TYPE_REGISTRATION(channel_instantiation_statement, 
	CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
channel_instantiation_statement::channel_instantiation_statement() :
		instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instantiation_statement::channel_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		instantiation_statement(i),
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
	return instantiation_statement::dump(o);
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
if (!m.register_transient_object(this, CHANNEL_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	INVARIANT(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
channel_instantiation_statement::construct_empty(const int i) {
	return new channel_instantiation_statement();
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

DEFAULT_PERSISTENT_TYPE_REGISTRATION(data_instantiation_statement, 
	DATA_INSTANTIATION_STATEMENT_TYPE_KEY)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
LIST_VECTOR_POOL_DEFAULT_STATIC_DEFINITION(data_instantiation_statement, 64)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
data_instantiation_statement::data_instantiation_statement() :
		instantiation_statement(), 
		type(NULL), inst_base(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
data_instantiation_statement::data_instantiation_statement(
		const type_ptr_type& t, 
		const index_collection_item_ptr_type& i) :
		instantiation_statement(i),
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
	return instantiation_statement::dump(o);
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
	const bool err = inst_base->commit_type(final_type_ref);
	if (err) {
		cerr << "ERROR during data_instantiation_statement::unroll()"
			<< endl;
		THROW_EXIT;
	}
	inst_base->instantiate_indices(indices);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
data_instantiation_statement::collect_transient_info(
		persistent_object_manager& m) const {
// STACKTRACE("data_instantiation_statement::collect_transient_info()");
if (!m.register_transient_object(this, DATA_INSTANTIATION_STATEMENT_TYPE_KEY)) {
	NEVER_NULL(inst_base);
	inst_base->collect_transient_info(m);
	type->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}	// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent*
data_instantiation_statement::construct_empty(const int i) {
	return new data_instantiation_statement();
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

