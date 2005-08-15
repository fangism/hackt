/**
	\file "Object/type/canonical_type_base.h"
	$Id: canonical_type_base.cc,v 1.1.4.2 2005/08/15 21:12:22 fang Exp $
 */

#include "Object/type/canonical_type_base.h"
#include "Object/type/template_actuals.h"
#include "Object/expr/const_param.h"
#include "Object/expr/const_param_expr_list.h"
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
		param_list_ptr->dump_range(o, 0, num_strict);
	}
	o << '>';
	const size_t s = param_list_ptr->size();
	if (num_strict < s) {
		param_list_ptr->dump_range(o << '<', num_strict, s) << '>';
	}
	return o;
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

