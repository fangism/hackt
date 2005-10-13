/**
	\file "Object/type/canonical_type_base.h"
	$Id: canonical_type_base.cc,v 1.2.8.1 2005/10/13 01:27:11 fang Exp $
 */

#include <algorithm>
#include <iterator>
#include "Object/type/canonical_type_base.h"
#include "Object/type/template_actuals.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/expr_dump_context.h"
#include "util/memory/count_ptr.tcc"

namespace ART {
namespace entity {
//=============================================================================
// class canonical_type_base method definitions

canonical_type_base::canonical_type_base() : param_list_ptr(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
canonical_type_base::canonical_type_base(const const_param_list_ptr_type& p) :
		param_list_ptr(p) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_type_base::~canonical_type_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param num_strict the number of strict template formals
		for the sake of splitting into strict and relaxed lists.  
 */
template_actuals
canonical_type_base::get_template_params(const size_t num_strict) const {
	if (!param_list_ptr)
		return template_actuals();
	const param_list_ptr_type
		sp(new param_list_type(
			param_list_ptr->begin(),
			param_list_ptr->begin() +num_strict));
	const size_t s = param_list_ptr->size();
	const const_param_list_ptr_type
		rp((num_strict < s) ?
			new param_list_type(
			param_list_ptr->begin() +num_strict,
			param_list_ptr->begin() +s) : NULL);
	return template_actuals(sp, rp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
        Prints template actuals in strict-relaxed format, 
        like in template_actuals.  
	\param num_strict the number of strict template formals
		for the sake of splitting into strict and relaxed lists.  
 */
ostream&
canonical_type_base::dump_template_args(ostream& o, 
		const size_t num_strict) const {
	// just local definition of const_iterator
	typedef param_list_type::const_iterator         const_iterator;
	o << '<';
	if (param_list_ptr) {
#if USE_EXPR_DUMP_CONTEXT
		param_list_ptr->dump_range(o, 
			expr_dump_context::default_value, 0, num_strict);
#else
		param_list_ptr->dump_range(o, 0, num_strict);
#endif
	}
	o << '>';
	const size_t s = param_list_ptr->size();
	if (num_strict < s) {
#if USE_EXPR_DUMP_CONTEXT
		param_list_ptr->dump_range(o << '<',
			expr_dump_context::default_value, num_strict, s) << '>';
#else
		param_list_ptr->dump_range(o << '<', num_strict, s) << '>';
#endif
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre the actuals for this type are incomplete.
	\post the actuals for this type will be complete.  
	This is not checked locally, is caller's responsibility to check.
	\see instance_alias_info_actuals::complete_type_actuals().  
 */
void
canonical_type_base::combine_relaxed_actuals(
		const const_param_list_ptr_type& p) {
if (p) {
	if (!param_list_ptr) {
		param_list_ptr = p;
	} else if (param_list_ptr->size()) {
		// need to make a deep copy: copy-on-write
		param_list_ptr_type pp(new param_list_type(*param_list_ptr));
		std::copy(p->begin(), p->end(), std::back_inserter(*pp));
		param_list_ptr = pp;
	}
}
// else nothing to add
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
canonical_type_base::match_relaxed_actuals(
		const const_param_list_ptr_type& p) const {
	if (param_list_ptr) {
		return p && param_list_ptr->is_tail_equivalent(*p);
	} else {
		return !p || p->empty();
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

