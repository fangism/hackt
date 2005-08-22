/**
	\file "Object/type/canonical_type.tcc"
	Implementation of canonical_type template class.  
	$Id: canonical_type.tcc,v 1.1.2.7 2005/08/22 00:44:17 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_TCC__
#define	__OBJECT_TYPE_CANONICAL_TYPE_TCC__

#include <iostream>
#include "Object/type/canonical_type.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/def/typedef_base.h"
#include "Object/def/footprint.h"
#include "Object/def/template_formals_manager.h"
#include "Object/type/template_actuals.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/unroll/unroll_context.h"
#include "util/persistent_object_manager.tcc"
#include "common/TODO.h"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class canonical_type method definitions

CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type() :
		base_type(), canonical_definition_ptr(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d) :
		base_type(), canonical_definition_ptr(d) {
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.template is_a<const typedef_base>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d,
		const param_list_ptr_type& p) :
		base_type(p), canonical_definition_ptr(d) {
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.template is_a<const typedef_base>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d,
		const template_actuals& p) :
		base_type(p.make_const_param_list()),
		canonical_definition_ptr(d) {
	NEVER_NULL(canonical_definition_ptr);
	INVARIANT(!canonical_definition_ptr.template is_a<const typedef_base>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use checked_cast<>
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
template <class DefType2>
CANONICAL_TYPE_CLASS::canonical_type(
		const canonical_type<DefType2>& c) :
		base_type(c.get_raw_template_params()), 
		canonical_definition_ptr(c.get_base_def()
			.template is_a<const canonical_definition_type>()) {
	NEVER_NULL(canonical_definition_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::~canonical_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// not needed yet
CANONICAL_TYPE_TEMPLATE_SIGNATURE
ostream&
CANONICAL_TYPE_CLASS::what(ostream& o) const {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should print template actuals in the same manner as
	template_actuals::dump().
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
ostream&
CANONICAL_TYPE_CLASS::dump(ostream& o) const {
	STACKTRACE_VERBOSE;
	NEVER_NULL(canonical_definition_ptr);
	o << canonical_definition_ptr->get_key();	// get_name()
	return base_type::dump_template_args(o,
		canonical_definition_ptr->num_strict_formals());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: FIX ME
	BUG!? template-params are constructed ephemerally!
	but unroll context returns by reference!
	FIX: unroll_context's template_params should make a copy
		not just a pointer!
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
unroll_context
CANONICAL_TYPE_CLASS::make_unroll_context(void) const {
	STACKTRACE_VERBOSE;
	return unroll_context(this->get_template_params(), 
		canonical_definition_ptr->get_template_formals_manager());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Creates a generic fundamental type reference from a canonical one.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
count_ptr<const typename CANONICAL_TYPE_CLASS::type_reference_type>
CANONICAL_TYPE_CLASS::make_type_reference(void) const {
	typedef	count_ptr<const type_reference_type>	return_type;
	return canonical_definition_ptr->
		make_canonical_fundamental_type_reference(
			this->get_template_params())
			.template is_a<const type_reference_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is implemented for the sake of transition compatibility.  
	\return template params split into strict and relaxed lists.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
template_actuals
CANONICAL_TYPE_CLASS::get_template_params(void) const {
	if (!param_list_ptr)
		return template_actuals();
	const size_t num_strict =
		canonical_definition_ptr->num_strict_formals();
	return base_type::get_template_params(num_strict);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Whetier or not this type reference is strict or relaxed.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
bool
CANONICAL_TYPE_CLASS::is_strict(void) const {
	NEVER_NULL(canonical_definition_ptr);
	const template_formals_manager&
		tfm(canonical_definition_ptr->get_template_formals_manager());
	const bool expects = tfm.has_relaxed_formals();
	const bool has = (param_list_ptr) ?
		param_list_ptr->size() > tfm.num_strict_formals() : false;
	if (has) {
		INVARIANT(expects);
		return true;
	} else {
		return !expects;
	}
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Standard error message for mismatched types.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
ostream&
CANONICAL_TYPE_CLASS::type_mismatch_error(ostream& o, 
		const this_type& l, const this_type& r) {
	o << "ERROR: types mismatch!" << endl;
	l.dump(o << "\tgot: ") << endl;
	r.dump(o << "\tand: ") << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers the complete type as a used type in the
	definition's footprint map.  
	Q: should it unroll 'instantiate' that definition?
	A: IMHO, yes, but it is not truly required.  
	A: could use a compiler flag to conditionally postpone... oooh.
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
good_bool
CANONICAL_TYPE_CLASS::unroll_definition_footprint(void) const {
	NEVER_NULL(canonical_definition_ptr);
	canonical_definition_ptr->register_complete_type(param_list_ptr);
	return canonical_definition_ptr->unroll_complete_type(param_list_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	copy-modified from fundamental_type_reference::must_be_collectibly_type_equivalent
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
bool
CANONICAL_TYPE_CLASS::must_be_collectibly_type_equivalent(
		const this_type& t) const {
	if (this->canonical_definition_ptr != t.canonical_definition_ptr) {
		// type_mismatch_error(cerr, *this, t);
		return false;
	} else {
		if (this->param_list_ptr && t.param_list_ptr) {
			const size_t num_strict =
				canonical_definition_ptr->num_strict_formals();
			// only strict parameters must match
			const bool ret(this->param_list_ptr->
				must_be_equivalent(*t.param_list_ptr, 
					num_strict));
			if (!ret) {
				type_mismatch_error(cerr, *this, t);
			}
			return ret;
		} else {
			// not sure if this is true
			INVARIANT(!this->canonical_definition_ptr);
			INVARIANT(!t.canonical_definition_ptr);
			return true;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	copy-modified from fundamental_type_reference::must_be_connectibly_type_equivalent
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
bool
CANONICAL_TYPE_CLASS::must_be_connectibly_type_equivalent(
		const this_type& t) const {
	if (this->canonical_definition_ptr != t.canonical_definition_ptr) {
		// type_mismatch_error(cerr, *this, t);
		return false;
	} else {
		if (this->param_list_ptr && t.param_list_ptr) {
			// all parameters must match
			const bool ret(this->param_list_ptr->
				must_be_equivalent(*t.param_list_ptr));
			if (!ret) {
				type_mismatch_error(cerr, *this, t);
			}
			return ret;
		} else {
			// not sure if this is true
			INVARIANT(!this->canonical_definition_ptr);
			INVARIANT(!t.canonical_definition_ptr);
			return true;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::unroll_port_instances(const unroll_context& c, 
		subinstance_manager& sub) const {
	unroll_port_instances_policy<DefType>()(*this, c, sub);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::collect_transient_info_base(
		persistent_object_manager& m) const {
	if (canonical_definition_ptr)
		canonical_definition_ptr->collect_transient_info(m);
	if (param_list_ptr)
		param_list_ptr->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, canonical_definition_ptr);
	m.write_pointer(o, param_list_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	AAAARRRRGGG.... CALL THE HACK POLICE.
	data_type_reference::load_object employs a dirty hack for dealing
	with built-in definitions (until we implement them the right way).
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, canonical_definition_ptr);
	m.read_pointer(i, param_list_ptr);
	canonical_definition_load_policy<DefType>()
		(m, canonical_definition_ptr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_CANONICAL_TYPE_TCC__

