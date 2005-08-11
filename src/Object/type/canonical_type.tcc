/**
	\file "Object/type/canonical_type.tcc"
	Implementation of canonical_type template class.  
	$Id: canonical_type.tcc,v 1.1.2.3 2005/08/11 21:52:52 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CANONICAL_TYPE_TCC__
#define	__OBJECT_TYPE_CANONICAL_TYPE_TCC__

#include <iostream>
#include "Object/type/canonical_type.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/def/template_formals_manager.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/persistent_object_manager.tcc"
#include "common/TODO.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class canonical_type method definitions

CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type() :
		canonical_definition_ptr(NULL), param_list_ptr(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d) :
		canonical_definition_ptr(d), param_list_ptr(NULL) {
	NEVER_NULL(canonical_definition_ptr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CANONICAL_TYPE_TEMPLATE_SIGNATURE
CANONICAL_TYPE_CLASS::canonical_type(const canonical_definition_ptr_type d,
		const param_list_ptr_type& p) :
		canonical_definition_ptr(d), param_list_ptr(p) {
	NEVER_NULL(canonical_definition_ptr);
}

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
	Prints template actuals in strict-relaxed format, 
	like template_actuals.  
	TODO: Most of this code should belong to a const_param_expr_list
		member function.  
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
ostream&
CANONICAL_TYPE_CLASS::dump_template_args(ostream& o) const {
	// just local definition of const_iterator
	typedef	param_list_type::const_iterator		const_iterator;
	NEVER_NULL(canonical_definition_ptr);
	const size_t num_strict =
		canonical_definition_ptr->get_template_formals_manager()
			.num_strict_formals();
	o << '<';
	if (param_list_ptr) {
		param_list_ptr->dump_range(0, num_strict -1);
	}
	o << '>';
	const size_t s = size();
	if (num_strict < s) {
		param_list_ptr->dump_range(o << '<', num_strict, s-1) << '>';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should print template actuals in the same manner as
	template_actuals::dump().
 */
CANONICAL_TYPE_TEMPLATE_SIGNATURE
ostream&
CANONICAL_TYPE_CLASS::dump(ostream& o) const {
	NEVER_NULL(canonical_definition_ptr);
	o << canonical_definition_ptr->get_name();
	return dump_template_args(o);
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
void
CANONICAL_TYPE_CLASS::register_definition_footprint(void) const {
#if 1
	FINISH_ME(Fang);
#else
	NEVER_NULL(canonical_definition_ptr);
	canonical_definition_ptr->register_type(param_list_ptr);
#endif
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
CANONICAL_TYPE_TEMPLATE_SIGNATURE
void
CANONICAL_TYPE_CLASS::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, canonical_definition_ptr);
	m.read_pointer(i, param_list_ptr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_CANONICAL_TYPE_TCC__

