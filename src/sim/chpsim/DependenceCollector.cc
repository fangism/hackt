/**
	\file "sim/chpsim/DependenceCollector.cc"
	$Id: DependenceCollector.cc,v 1.13 2010/09/21 00:18:39 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <algorithm>
#include <iterator>
#include "sim/chpsim/DependenceCollector.hh"
#include "sim/chpsim/StateConstructor.hh"
#include "sim/chpsim/State.hh"

#include "Object/expr/int_negation_expr.hh"
#include "Object/expr/int_arith_expr.hh"
#include "Object/expr/int_relational_expr.hh"
#include "Object/expr/bool_negation_expr.hh"
#include "Object/expr/bool_logical_expr.hh"
#include "Object/expr/loop_nonmeta_expr.hh"
#include "Object/expr/enum_expr.hh"
#include "Object/expr/real_expr.hh"
#include "Object/expr/string_expr.hh"
// #include "Object/expr/real_negation_expr.hh"
// #include "Object/expr/real_arith_expr.hh"
// #include "Object/expr/real_relational_expr.hh"
#include "Object/expr/channel_probe.hh"
#include "Object/expr/nonmeta_index_list.hh"
#include "Object/expr/nonmeta_expr_list.hh"
#include "Object/expr/nonmeta_func_call.hh"
#include "Object/expr/nonmeta_cast_expr.hh"
#include "Object/global_entry.hh"
#include "Object/global_entry_context.hh"
#include "Object/module.hh"

#include "Object/ref/meta_instance_reference_subtypes.hh"
#include "Object/ref/simple_meta_instance_reference.hh"
#include "Object/ref/member_meta_instance_reference.hh"
#include "Object/ref/aggregate_meta_instance_reference.hh"
#include "Object/ref/simple_nonmeta_instance_reference.hh"
#include "Object/ref/simple_nonmeta_value_reference.hh"
#include "Object/ref/data_nonmeta_instance_reference.hh"
#include "Object/traits/classification_tags.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/traits/int_traits.hh"
#include "Object/traits/enum_traits.hh"
#include "Object/traits/string_traits.hh"
#include "Object/traits/chan_traits.hh"
#include "Object/traits/proc_traits.hh"
#include "Object/traits/value_traits.hh"

#include "common/ICE.hh"
#include "common/TODO.hh"
#include "util/iterator_more.hh"
#include "util/stacktrace.hh"

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
#include "util/using_ostream.hh"
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

DependenceSetCollector::DependenceSetCollector(const StateConstructor& s) : 
		global_entry_context(
			s.state. get_module().get_context_cache().top_context),
	 	// don't default to top-level
		dependence_collector_base<bool_tag>(), 
		dependence_collector_base<int_tag>(), 
		dependence_collector_base<enum_tag>(), 
		dependence_collector_base<channel_tag>() {
	STACKTRACE_VERBOSE;
	// all processes except the top-level should have valid footprint-frame
	if (s.current_process_index) {
#if !CACHE_GLOBAL_FOOTPRINT_FRAMES
#error	"Caching of global footprint frames is required!"
	// because a return by reference is necessary, not return by value
#endif
		fpf = &s.state.get_module().get_context_cache()
			.get_global_context(
				s.current_process_index).value.frame;
		// do anything about global offset?
		// probably don't need if process traversal is flat
	}
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
DEFINE_TRIVIAL_VISIT(pstring_const)
DEFINE_TRIVIAL_VISIT(pint_const_collection)
DEFINE_TRIVIAL_VISIT(pbool_const_collection)
DEFINE_TRIVIAL_VISIT(preal_const_collection)
DEFINE_TRIVIAL_VISIT(pstring_const_collection)
DEFINE_TRIVIAL_VISIT(pint_meta_func_call)
DEFINE_TRIVIAL_VISIT(pbool_meta_func_call)
DEFINE_TRIVIAL_VISIT(preal_meta_func_call)
DEFINE_TRIVIAL_VISIT(pstring_meta_func_call)
DEFINE_NEVER_VISIT(int_range_expr)

DEFINE_NEVER_VISIT(param_defined)
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
DEFINE_NEVER_VISIT(pstring_relational_expr)
DEFINE_NEVER_VISIT(convert_pint_to_preal_expr)

#if 0
// TODO: finish these!
DEFINE_NEVER_VISIT(bool_return_cast_expr)
DEFINE_NEVER_VISIT(int_return_cast_expr)
#else
void
DependenceSetCollector::visit(const bool_return_cast_expr& e) {
	e.get_operand()->accept(*this);
}

void
DependenceSetCollector::visit(const int_return_cast_expr& e) {
	e.get_operand()->accept(*this);
}
#endif

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

void
DependenceSetCollector::visit(const simple_pstring_nonmeta_value_reference& r) {
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
DEFINE_TRIVIAL_VISIT(simple_pstring_meta_value_reference)
DEFINE_TRIVIAL_VISIT(aggregate_pint_meta_value_reference)
DEFINE_TRIVIAL_VISIT(aggregate_pbool_meta_value_reference)
DEFINE_TRIVIAL_VISIT(aggregate_preal_meta_value_reference)
DEFINE_TRIVIAL_VISIT(aggregate_pstring_meta_value_reference)

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

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

