/**
	\file "sim/chpsim/DependenceCollector.cc"
	$Id: DependenceCollector.cc,v 1.9.46.2 2010/02/10 06:43:13 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <algorithm>
#include <iterator>
#include "sim/chpsim/DependenceCollector.h"
#include "sim/chpsim/StateConstructor.h"
#include "sim/chpsim/State.h"

#include "Object/expr/int_negation_expr.h"
#include "Object/expr/int_arith_expr.h"
#include "Object/expr/int_relational_expr.h"
#include "Object/expr/bool_negation_expr.h"
#include "Object/expr/bool_logical_expr.h"
#include "Object/expr/loop_nonmeta_expr.h"
#include "Object/expr/enum_expr.h"
#include "Object/expr/real_expr.h"
// #include "Object/expr/real_negation_expr.h"
// #include "Object/expr/real_arith_expr.h"
// #include "Object/expr/real_relational_expr.h"
#include "Object/expr/channel_probe.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/nonmeta_expr_list.h"
#include "Object/expr/nonmeta_func_call.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/global_entry_context.h"

#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "Object/traits/classification_tags.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/traits/value_traits.h"

#include "common/ICE.h"
#include "common/TODO.h"
#include "util/iterator_more.h"
#include "util/stacktrace.h"

/**
	Goal: 1
	Disabled because the calls to lookup_globally_allocated_index
	are suspiciously wrong (top-level vs. local).  
	Also possible that they are never reached since CHP
	contains nonmeta references only (even for non-indexed references).
	Don't forget to add support for enum references.  
 */
#define	HANDLE_META_REFERENCES				0

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using std::vector;
using std::copy;
using std::transform;
using util::set_inserter;
using entity::channel_tag;
using entity::process_tag;
using entity::global_entry_pool;
using entity::footprint_frame;
using entity::footprint_frame_transformer;
using entity::simple_nonmeta_instance_reference_base;
using entity::global_entry_context;
using entity::simple_meta_instance_reference;
using entity::member_meta_instance_reference;
using entity::aggregate_meta_instance_reference;
using util::memory::never_ptr;

//=============================================================================
// class dependence_collector_base method definitions

template <class Tag>
dependence_collector_base<Tag>::dependence_collector_base() : _set() { }

template <class Tag>
dependence_collector_base<Tag>::~dependence_collector_base() { }

//=============================================================================
// class DependenceSetCollector method definitions

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
// TEMPORARY
DependenceSetCollector::DependenceSetCollector(const StateConstructor& s) : 
		global_entry_context(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			s.get_state_manager(), 
#endif
			s.get_process_footprint(), 	// is top-level!
			(s.current_process_index ?
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
			NULL	// TODO: FINISH ME
#else
			&s.get_state_manager().get_pool<process_tag>()
				[s.current_process_index]._frame
#endif
			: NULL)), 	// don't default to top-level
		dependence_collector_base<bool_tag>(), 
		dependence_collector_base<int_tag>(), 
		dependence_collector_base<enum_tag>(), 
		dependence_collector_base<channel_tag>() {
	// all processes except the top-level should have valid footprint-frame
	if (s.current_process_index) {
		NEVER_NULL(fpf);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	With this constructor, footprint_frame pointer is not set, 
	and should be set using global_entry_context::footprint_frame_setter.
 */
DependenceSetCollector::DependenceSetCollector(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		const state_manager& _sm, 
#endif
		const footprint& _topfp) : 
		global_entry_context(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			_sm, 
#endif
			_topfp), 
		dependence_collector_base<bool_tag>(), 
		dependence_collector_base<int_tag>(), 
		dependence_collector_base<enum_tag>(), 
		dependence_collector_base<channel_tag>() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DependenceSetCollector::~DependenceSetCollector() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
DependenceSetCollector::clear(void) {
	dependence_collector_base<bool_tag>::_set.clear();
	dependence_collector_base<int_tag>::_set.clear();
	dependence_collector_base<enum_tag>::_set.clear();
	dependence_collector_base<channel_tag>::_set.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op visits.
 */
#define	DEFINE_TRIVIAL_VISIT(type)					\
void									\
DependenceSetCollector::visit(const type&) {				\
	STACKTRACE_VERBOSE;						\
}

/**
	Should never be reached.  (Will ICE if reached)
	We expect meta-parameter expressions to have been folded
	into constants.  
 */
#define	DEFINE_NEVER_VISIT(type)					\
void									\
DependenceSetCollector::visit(const type&) {				\
	ICE_NEVER_CALL(cerr);						\
}

DEFINE_TRIVIAL_VISIT(pint_const)
DEFINE_NEVER_VISIT(pint_range)
DEFINE_TRIVIAL_VISIT(const_index)
DEFINE_TRIVIAL_VISIT(const_range)
DEFINE_TRIVIAL_VISIT(pbool_const)
DEFINE_TRIVIAL_VISIT(preal_const)
DEFINE_TRIVIAL_VISIT(pint_const_collection)
DEFINE_TRIVIAL_VISIT(pbool_const_collection)
DEFINE_TRIVIAL_VISIT(preal_const_collection)
DEFINE_TRIVIAL_VISIT(string_expr)
DEFINE_NEVER_VISIT(int_range_expr)

DEFINE_NEVER_VISIT(pint_arith_expr)
DEFINE_NEVER_VISIT(pint_unary_expr)
DEFINE_NEVER_VISIT(pint_relational_expr)
DEFINE_NEVER_VISIT(pbool_unary_expr)
DEFINE_NEVER_VISIT(pbool_logical_expr)
DEFINE_NEVER_VISIT(preal_arith_expr)
DEFINE_NEVER_VISIT(preal_unary_expr)
DEFINE_NEVER_VISIT(preal_relational_expr)
DEFINE_NEVER_VISIT(pint_arith_loop_expr)
DEFINE_NEVER_VISIT(pbool_logical_loop_expr)
DEFINE_NEVER_VISIT(preal_arith_loop_expr)
DEFINE_NEVER_VISIT(convert_pint_to_preal_expr)
DEFINE_NEVER_VISIT(bool_return_cast_expr)
DEFINE_NEVER_VISIT(int_return_cast_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEFINE_UNARY_VISIT(type)					\
void									\
DependenceSetCollector::visit(const type& t) {				\
	STACKTRACE_VERBOSE;						\
	t.get_operand()->accept(*this);					\
}

#define	DEFINE_BINARY_VISIT(type)					\
void									\
DependenceSetCollector::visit(const type& t) {				\
	STACKTRACE_VERBOSE;						\
	t.get_first()->accept(*this);					\
	t.get_second()->accept(*this);					\
}

DEFINE_UNARY_VISIT(int_negation_expr)
DEFINE_UNARY_VISIT(bool_negation_expr)
// DEFINE_UNARY_VISIT(real_negation_expr)
DEFINE_BINARY_VISIT(int_arith_expr)
DEFINE_BINARY_VISIT(int_relational_expr)
DEFINE_BINARY_VISIT(bool_logical_expr)
// DEFINE_BINARY_VISIT(real_arith_expr)
// DEFINE_BINARY_VISIT(real_relational_expr)
DEFINE_UNARY_VISIT(int_arith_loop_expr)
DEFINE_UNARY_VISIT(bool_logical_loop_expr)
// DEFINE_UNARY_VISIT(real_arith_loop_expr)

void
DependenceSetCollector::visit(const channel_probe& t) {
	STACKTRACE_VERBOSE;
	t.get_channel()->accept(*this);
}

void
DependenceSetCollector::visit(const nonmeta_func_call& f) {
	f.get_func_args()->accept(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Meta-references can be fully resolved at compile time.
	These are not likely to be called from CHP, which contains
	nonmeta-typed references (some of which may qualify as meta).
	\param r scalar reference to a bool.  
	Could define this as template in base classes...
 */

#if HANDLE_META_REFERENCES
#define	DEFINE_SINGLE_META_REFERENCE_VISIT(reference, Tag)		\
void									\
DependenceSetCollector::visit(const reference<Tag>& r) {		\
	STACKTRACE_VERBOSE;						\
	const node_index_type i = 					\
		r.lookup_globally_allocated_index(*sm, *topfp);		\
	dependence_collector_base<Tag>::_set.insert(			\
			fpf ? footprint_frame_transformer(		\
			*fpf, Tag())(i) : i);				\
}
#else
#define	DEFINE_SINGLE_META_REFERENCE_VISIT(reference, Tag)		\
void									\
DependenceSetCollector::visit(const reference<Tag>&) {			\
	FINISH_ME(Fang);						\
}
#endif

DEFINE_SINGLE_META_REFERENCE_VISIT(simple_meta_instance_reference, bool_tag)
DEFINE_SINGLE_META_REFERENCE_VISIT(simple_meta_instance_reference, int_tag)
DEFINE_SINGLE_META_REFERENCE_VISIT(simple_meta_instance_reference, enum_tag)
DEFINE_SINGLE_META_REFERENCE_VISIT(simple_meta_instance_reference, channel_tag)
DEFINE_SINGLE_META_REFERENCE_VISIT(member_meta_instance_reference, bool_tag)
DEFINE_SINGLE_META_REFERENCE_VISIT(member_meta_instance_reference, int_tag)
DEFINE_SINGLE_META_REFERENCE_VISIT(member_meta_instance_reference, enum_tag)
DEFINE_SINGLE_META_REFERENCE_VISIT(member_meta_instance_reference, channel_tag)

#undef	DEFINE_SINGLE_META_REFERENCE_VISIT

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if HANDLE_META_REFERENCES
#define	DEFINE_AGGREGATE_META_REFERENCE_VISIT(Tag)			\
void									\
DependenceSetCollector::visit(						\
		const aggregate_meta_instance_reference<Tag>& r) {	\
	STACKTRACE_VERBOSE;						\
	vector<node_index_type> indices;				\
if (r.lookup_globally_allocated_indices(*sm, *topfp, indices).good) {	\
	if (fpf) {							\
		transform(indices.begin(), indices.end(),		\
			set_inserter(dependence_collector_base<Tag>::_set), \
			footprint_frame_transformer(*fpf, Tag()));	\
	} else {							\
		copy(indices.begin(), indices.end(), 			\
			set_inserter(dependence_collector_base<Tag>::_set)); \
	}								\
} else {								\
	/* TODO: error handling */					\
	FINISH_ME(Fang);						\
	THROW_EXIT;							\
}									\
}
#else
#define	DEFINE_AGGREGATE_META_REFERENCE_VISIT(Tag)			\
void									\
DependenceSetCollector::visit(						\
		const aggregate_meta_instance_reference<Tag>&) {	\
	FINISH_ME(Fang);						\
}
#endif

DEFINE_AGGREGATE_META_REFERENCE_VISIT(bool_tag)
DEFINE_AGGREGATE_META_REFERENCE_VISIT(int_tag)
DEFINE_AGGREGATE_META_REFERENCE_VISIT(enum_tag)
DEFINE_AGGREGATE_META_REFERENCE_VISIT(channel_tag)

#undef	DEFINE_AGGREGATE_META_REFERENCE_VISIT

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// nonmeta references

// not yet supported
DEFINE_NEVER_VISIT(simple_process_nonmeta_instance_reference)
DEFINE_NEVER_VISIT(simple_datastruct_nonmeta_instance_reference)

/**
	Collects all may-references of nonmeta-referenced channel.  
	Remember: also visit the array_indices.  
 */
#define	DEFINE_NONMETA_INSTANCE_VISIT(type, Tag)			\
void									\
DependenceSetCollector::visit(const type& r) {				\
	STACKTRACE_VERBOSE;						\
	vector<node_index_type> indices;				\
	if (r.lookup_may_reference_global_indices(*this, indices).good) { \
		copy(indices.begin(), indices.end(),			\
			set_inserter(dependence_collector_base<Tag>::_set)); \
		const never_ptr<const type::index_list_type>		\
			ind(r.get_indices());				\
		if (ind) { ind->accept(*this); }			\
	} else {							\
		/* TODO: error handling */				\
		FINISH_ME(Fang);					\
		THROW_EXIT;						\
	}								\
}

DEFINE_NONMETA_INSTANCE_VISIT(
	simple_bool_nonmeta_instance_reference, bool_tag)
DEFINE_NONMETA_INSTANCE_VISIT(
	simple_int_nonmeta_instance_reference, int_tag)
DEFINE_NONMETA_INSTANCE_VISIT(
	simple_enum_nonmeta_instance_reference, enum_tag)
DEFINE_NONMETA_INSTANCE_VISIT(
	simple_channel_nonmeta_instance_reference, channel_tag)

#undef	DEFINE_NONMETA_INSTANCE_VISIT

// meta-value references have no aliases, and thus are only index dependent
void
DependenceSetCollector::visit(const simple_pbool_nonmeta_value_reference& r) {
	STACKTRACE_VERBOSE;
	const never_ptr<const nonmeta_index_list> ind(r.get_indices());
	if (ind) { ind->accept(*this); }
}

void
DependenceSetCollector::visit(const simple_pint_nonmeta_value_reference& r) {
	STACKTRACE_VERBOSE;
	const never_ptr<const nonmeta_index_list> ind(r.get_indices());
	if (ind) { ind->accept(*this); }
}

void
DependenceSetCollector::visit(const simple_preal_nonmeta_value_reference& r) {
	STACKTRACE_VERBOSE;
	const never_ptr<const nonmeta_index_list> ind(r.get_indices());
	if (ind) { ind->accept(*this); }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// meta value references never change, and thus do not need to be
// added to any dependence sets -- they are constant.

DEFINE_TRIVIAL_VISIT(simple_pint_meta_value_reference)
DEFINE_TRIVIAL_VISIT(simple_pbool_meta_value_reference)
DEFINE_TRIVIAL_VISIT(simple_preal_meta_value_reference)
DEFINE_TRIVIAL_VISIT(aggregate_pint_meta_value_reference)
DEFINE_TRIVIAL_VISIT(aggregate_pbool_meta_value_reference)
DEFINE_TRIVIAL_VISIT(aggregate_preal_meta_value_reference)

// enums and structs are so far unsupported, so we ICE for now
DEFINE_NEVER_VISIT(simple_process_meta_instance_reference)
DEFINE_NEVER_VISIT(simple_datastruct_meta_instance_reference)
DEFINE_NEVER_VISIT(process_member_meta_instance_reference)
DEFINE_NEVER_VISIT(datastruct_member_meta_instance_reference)
DEFINE_NEVER_VISIT(aggregate_process_meta_instance_reference)
DEFINE_NEVER_VISIT(aggregate_datastruct_meta_instance_reference)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#undef	DEFINE_TRIVIAL_VISIT
#undef	DEFINE_NEVER_VISIT
#undef	DEFINE_UNARY_VISIT
#undef	DEFINE_BINARY_VISIT
#endif	// TEMPORARY

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

