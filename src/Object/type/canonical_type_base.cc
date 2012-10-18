/**
	\file "Object/type/canonical_type_base.hh"
	$Id: canonical_type_base.cc,v 1.9 2011/03/23 00:36:18 fang Exp $
 */

#include <algorithm>
#include <iterator>
#include "Object/type/canonical_type_base.hh"
#include "Object/type/template_actuals.hh"
#include "Object/expr/const_param.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
#include "Object/expr/expr_dump_context.hh"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity {
using std::copy;
using std::back_inserter;
//=============================================================================
// class canonical_type_base method definitions

canonical_type_base::canonical_type_base() : param_list_ptr(NULL)
#if PROCESS_CONNECTIVITY_CHECKING
		, direction(CHANNEL_DIRECTION_DEFAULT)	// not NULL
#endif
	{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
canonical_type_base::canonical_type_base(const const_param_list_ptr_type& p) :
		param_list_ptr(p)
#if PROCESS_CONNECTIVITY_CHECKING
		, direction(CHANNEL_DIRECTION_DEFAULT)	// not NULL
#endif
	{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
/**
	Concatenates expression lists into one.  
 */
canonical_type_base::canonical_type_base(const const_param_list_ptr_type& p, 
		const const_param_list_ptr_type& r) :
		param_list_ptr(p)
#if PROCESS_CONNECTIVITY_CHECKING
		, direction(CHANNEL_DIRECTION_DEFAULT)	// not NULL
#endif
		{
	if (p && r) {
		const param_list_ptr_type
			temp(new const_param_expr_list(*param_list_ptr));
		NEVER_NULL(temp);
		copy(r->begin(), r->end(), back_inserter(*temp));
		param_list_ptr = temp;
	}
}

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
	const template_actuals::arg_list_ptr_type
		sp(new template_actuals::expr_list_type());
	copy(param_list_ptr->begin(), param_list_ptr->begin() +num_strict, 
		back_inserter(*sp));
	const size_t s = param_list_ptr->size();
	const template_actuals::arg_list_ptr_type
		rp((num_strict < s) ?
			new template_actuals::expr_list_type() : NULL);
	if (rp) {
		copy(param_list_ptr->begin() +num_strict,
			param_list_ptr->begin() +s, back_inserter(*rp));
	}
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
		param_list_ptr->dump_range(o, 
			expr_dump_context::default_value, 0, num_strict);
	}
	o << '>';
	if (param_list_ptr) {
		const size_t s = param_list_ptr->size();
		if (num_strict < s) {
			param_list_ptr->dump_range(o << '<',
				expr_dump_context::default_value,
				num_strict, s) << '>';
		}
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
	} else {
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
}	// end namespace HAC

