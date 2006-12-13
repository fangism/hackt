/**
	\file "sim/chpsim/DependenceCollector.cc"
	$Id: DependenceCollector.cc,v 1.1.2.4 2006/12/13 07:47:43 fang Exp $
 */

#include <iostream>
#include "sim/chpsim/DependenceCollector.h"
#include "sim/chpsim/StateConstructor.h"

#include "Object/expr/int_negation_expr.h"
#include "Object/expr/int_arith_expr.h"
#include "Object/expr/int_relational_expr.h"
#include "Object/expr/bool_negation_expr.h"
#include "Object/expr/bool_logical_expr.h"
#include "Object/expr/real_expr.h"
// #include "Object/expr/real_negation_expr.h"
// #include "Object/expr/real_arith_expr.h"
// #include "Object/expr/real_relational_expr.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"

#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/member_meta_instance_reference.h"
#include "Object/ref/aggregate_meta_instance_reference.h"
#include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/ref/simple_nonmeta_value_reference.h"
#include "Object/ref/data_nonmeta_instance_reference.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/traits/value_traits.h"

#include "common/ICE.h"
#include "common/TODO.h"
#include "util/iterator_more.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
#include "util/using_ostream.h"
using entity::bool_tag;
using entity::int_tag;
using entity::channel_tag;
using entity::process_tag;
using entity::global_entry_pool;
using entity::footprint_frame;
using entity::simple_nonmeta_instance_reference_base;
using util::set_inserter;
using util::memory::never_ptr;

//=============================================================================
// class DependenceSetCollector method definitions

DependenceSetCollector::DependenceSetCollector(const StateConstructor& s) : 
		_sm(s.get_state_manager()), 
		_fp(s.get_process_footprint()), 
		_ff(s.current_process_index ?
			&_sm.get_pool<process_tag>()[s.current_process_index]
			._frame
			: NULL), 
		bool_set(), int_set(), channel_set() {
	if (s.current_process_index) {
		NEVER_NULL(_ff);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DependenceSetCollector::~DependenceSetCollector() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
DependenceSetCollector::clear(void) {
	bool_set.clear();
	int_set.clear();
	channel_set.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op visits.
 */
#define	DEFINE_TRIVIAL_VISIT(type)					\
void									\
DependenceSetCollector::visit(const type& t) { }

/**
	Should never be reached.  (Will ICE if reached)
	We expect meta-parameter expressions to have been folded
	into constants.  
 */
#define	DEFINE_NEVER_VISIT(type)					\
void									\
DependenceSetCollector::visit(const type& t) {				\
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
DEFINE_NEVER_VISIT(int_range_expr)

DEFINE_NEVER_VISIT(pint_arith_expr)
DEFINE_NEVER_VISIT(pint_unary_expr)
DEFINE_NEVER_VISIT(pint_relational_expr)
DEFINE_NEVER_VISIT(pbool_unary_expr)
DEFINE_NEVER_VISIT(pbool_logical_expr)
DEFINE_NEVER_VISIT(preal_arith_expr)
DEFINE_NEVER_VISIT(preal_unary_expr)
DEFINE_NEVER_VISIT(preal_relational_expr)


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEFINE_UNARY_VISIT(type)					\
void									\
DependenceSetCollector::visit(const type& t) {				\
	t.get_operand()->accept(*this);					\
}

#define	DEFINE_BINARY_VISIT(type)					\
void									\
DependenceSetCollector::visit(const type& t) {				\
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


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Meta-references can be fully resolved at compile time.
	These are not likely to be called from CHP, which contains
	nonmeta-typed references (some of which may qualify as meta).
	\param r scalar reference to a bool.  
 */
void
DependenceSetCollector::visit(const simple_bool_meta_instance_reference& r) {
	bool_set.insert(r.lookup_globally_allocated_index(_sm, _fp));
}

void
DependenceSetCollector::visit(const simple_int_meta_instance_reference& r) {
	int_set.insert(r.lookup_globally_allocated_index(_sm, _fp));
}

void
DependenceSetCollector::visit(const simple_channel_meta_instance_reference& r) {
	channel_set.insert(r.lookup_globally_allocated_index(_sm, _fp));
}

void
DependenceSetCollector::visit(const bool_member_meta_instance_reference& r) {
	bool_set.insert(r.lookup_globally_allocated_index(_sm, _fp));
}

void
DependenceSetCollector::visit(const int_member_meta_instance_reference& r) {
	int_set.insert(r.lookup_globally_allocated_index(_sm, _fp));
}

void
DependenceSetCollector::visit(const channel_member_meta_instance_reference& r) {
	channel_set.insert(r.lookup_globally_allocated_index(_sm, _fp));
}

void
DependenceSetCollector::visit(
		const aggregate_bool_meta_instance_reference& r) {
	vector<node_index_type> indices;
	if (r.lookup_globally_allocated_indices(_sm, _fp, indices).good) {
		copy(indices.begin(), indices.end(), set_inserter(bool_set));
	} else {
		// TODO: error handling
		FINISH_ME(Fang);
		THROW_EXIT;
	}
}

void
DependenceSetCollector::visit(
		const aggregate_int_meta_instance_reference& r) {
	vector<node_index_type> indices;
	if (r.lookup_globally_allocated_indices(_sm, _fp, indices).good) {
		copy(indices.begin(), indices.end(), set_inserter(int_set));
	} else {
		// TODO: error handling
		FINISH_ME(Fang);
		THROW_EXIT;
	}
}

void
DependenceSetCollector::visit(
		const aggregate_channel_meta_instance_reference& r) {
	vector<node_index_type> indices;
	if (r.lookup_globally_allocated_indices(_sm, _fp, indices).good) {
		copy(indices.begin(), indices.end(), set_inserter(channel_set));
	} else {
		// TODO: error handling
		FINISH_ME(Fang);
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// nonmeta references

// not yet supported
DEFINE_NEVER_VISIT(simple_process_nonmeta_instance_reference);
DEFINE_NEVER_VISIT(simple_datastruct_nonmeta_instance_reference);
DEFINE_NEVER_VISIT(simple_enum_nonmeta_instance_reference);

/**
	Collects all may-references of nonmeta-referenced channel.  
	Remember: also visit the array_indices.  
 */
#define	DEFINE_NONMETA_INSTANCE_VISIT(type, set)			\
void									\
DependenceSetCollector::visit(const type& r) {				\
	vector<node_index_type> indices;				\
	if (r.lookup_may_reference_global_indices(			\
			_sm, _fp, _ff, indices).good) {			\
		copy(indices.begin(), indices.end(), set_inserter(set)); \
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
	simple_bool_nonmeta_instance_reference, bool_set)
DEFINE_NONMETA_INSTANCE_VISIT(
	simple_int_nonmeta_instance_reference, int_set)
DEFINE_NONMETA_INSTANCE_VISIT(
	simple_channel_nonmeta_instance_reference, channel_set)

// meta-value references have no aliases, and thus are only index dependent
void
DependenceSetCollector::visit(const simple_pbool_nonmeta_value_reference& r) {
	const never_ptr<const nonmeta_index_list> ind(r.get_indices());
	if (ind) { ind->accept(*this); }
}

void
DependenceSetCollector::visit(const simple_pint_nonmeta_value_reference& r) {
	const never_ptr<const nonmeta_index_list> ind(r.get_indices());
	if (ind) { ind->accept(*this); }
}

void
DependenceSetCollector::visit(const simple_preal_nonmeta_value_reference& r) {
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
DEFINE_NEVER_VISIT(simple_enum_meta_instance_reference)
DEFINE_NEVER_VISIT(process_member_meta_instance_reference)
DEFINE_NEVER_VISIT(datastruct_member_meta_instance_reference)
DEFINE_NEVER_VISIT(enum_member_meta_instance_reference)
DEFINE_NEVER_VISIT(aggregate_process_meta_instance_reference)
DEFINE_NEVER_VISIT(aggregate_datastruct_meta_instance_reference)
DEFINE_NEVER_VISIT(aggregate_enum_meta_instance_reference)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#undef	DEFINE_TRIVIAL_VISIT
#undef	DEFINE_NEVER_VISIT
#undef	DEFINE_UNARY_VISIT
#undef	DEFINE_BINARY_VISIT

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

