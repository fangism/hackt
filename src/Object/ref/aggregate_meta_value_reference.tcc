/**
	\file "Object/ref/aggregate_meta_value_reference.tcc"
	Implementation of aggregate_meta_value_reference class.  
	$Id: aggregate_meta_value_reference.tcc,v 1.1.2.4 2006/02/19 06:09:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_TCC__
#define	__HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_TCC__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE			0
#endif

#include <iostream>
#include "Object/ref/aggregate_meta_value_reference.h"
#include "Object/def/definition_base.h"
#include "Object/common/multikey_index.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/type/param_type_reference.h"
#include "common/TODO.h"
#include "common/ICE.h"
#include "util/persistent_object_manager.h"
#include "util/multikey.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::persistent_traits;

//=============================================================================
// class aggregate_meta_value_reference method definitions

AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_VALUE_REFERENCE_CLASS::aggregate_meta_value_reference() :
		aggregate_meta_value_reference_base(), 
		parent_type(), 
		subreferences() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
AGGREGATE_META_VALUE_REFERENCE_CLASS::~aggregate_meta_value_reference() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
AGGREGATE_META_VALUE_REFERENCE_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
AGGREGATE_META_VALUE_REFERENCE_CLASS::dump(ostream& o, 
		const expr_dump_context& c) const {
	FINISH_ME(Fang);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
ostream&
AGGREGATE_META_VALUE_REFERENCE_CLASS::dump_type_size(ostream& o) const {
	FINISH_ME(Fang);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aggregate constituents should have the same type of course, 
	so we use any one's base definition as the representative
	definition for the entire collection. 
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const definition_base>
AGGREGATE_META_VALUE_REFERENCE_CLASS::get_base_def(void) const {
//	return traits_type::built_in_definition;	// private!?
	ICE_NEVER_CALL(cerr);
	return never_ptr<const definition_base>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: account for relaxed types vs. strict types?
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const fundamental_type_reference>
AGGREGATE_META_VALUE_REFERENCE_CLASS::get_type_ref(void) const {
	return traits_type::built_in_type_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Aggregates have more than one base collection!
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
never_ptr<const param_value_collection>
AGGREGATE_META_VALUE_REFERENCE_CLASS::get_coll_base(void) const {
	ICE_NEVER_CALL(cerr);
	return never_ptr<const param_value_collection>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just adds a reference to the end of the list.  (push_back)
	Dimension checking done later.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::append_meta_value_reference(
		const count_ptr<const expr_base_type>& p) {
	NEVER_NULL(p);
	if (!subreferences.empty()) {
		// then we check against the first's dimensions
		const size_t fd = subreferences.front()->dimensions();
		const size_t pd = p->dimensions();
		if (fd != pd) {
			cerr << "Error: value reference " <<
				subreferences.size() +1 << " is " << pd
				<< "-dimensional, but the former references "
				"in the aggregate are " << fd <<
				"-dimensional." << endl;
			return good_bool(false);
		}
		// punt detail dimension checks until unroll-time.
	}
	subreferences.push_back(p);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds a subreference to the end of the list.
	\return good if argument was of the correct type.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::append_meta_value_reference(
		const count_ptr<const param_expr>& p) {
	const count_ptr<const expr_base_type>
		next(p.template is_a<const expr_base_type>());
	if (next) {
		return this->append_meta_value_reference(next);
	} else {
		cerr << "Can\'t append a ";
		p->what(cerr) << " to a ";
		this->what(cerr) << ".  " << endl;
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If this is a concatenation, then resulting dimension is the 
	same as constituents.  If is array-construction, then 
	dimension is 1 greater than constituents.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
size_t
AGGREGATE_META_VALUE_REFERENCE_CLASS::dimensions(void) const {
	return subreferences.front()->dimensions()
		+(_is_concatenation ? 0 : 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't bother checking at parse time, leave until unroll time.  
 */
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::may_be_initialized(void) const {
	// FINISH_ME(Fang);
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::must_be_initialized(void) const {
	// FINISH_ME(Fang);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::is_static_constant(void) const {
	FINISH_ME(Fang);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::is_relaxed_formal_dependent(void) const {
	FINISH_ME_EXIT(Fang);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
typename AGGREGATE_META_VALUE_REFERENCE_CLASS::value_type
AGGREGATE_META_VALUE_REFERENCE_CLASS::static_constant_value(void) const {
	FINISH_ME(Fang);
	return value_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::must_be_equivalent(
		const expr_base_type& b) const {
	FINISH_ME(Fang);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve_value(
		const unroll_context& c, value_type& i) const {
	ICE_NEVER_CALL(cerr);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
good_bool
AGGREGATE_META_VALUE_REFERENCE_CLASS::resolve_value(value_type& i) const {
	FINISH_ME(Fang);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_index_list
AGGREGATE_META_VALUE_REFERENCE_CLASS::resolve_dimensions(void) const {
	FINISH_ME(Fang);
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
const_index_list
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve_dimensions(
		const unroll_context& c) const {
	FINISH_ME(Fang);
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
count_ptr<const_param>
AGGREGATE_META_VALUE_REFERENCE_CLASS::unroll_resolve(
		const unroll_context& c) const {
	FINISH_ME(Fang);
	return count_ptr<const_param>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	m.collect_pointer_list(subreferences);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, _is_concatenation);
	m.write_pointer_list(o, subreferences);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
AGGREGATE_META_VALUE_REFERENCE_TEMPLATE_SIGNATURE
void
AGGREGATE_META_VALUE_REFERENCE_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	read_value(i, _is_concatenation);
	m.read_pointer_list(i, subreferences);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_AGGREGATE_META_VALUE_REFERENCE_TCC__

