/**
	\file "Object/art_object_unroll_context.cc"
	$Id: art_object_unroll_context.cc,v 1.3.14.4 2005/07/18 19:20:41 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_CC__
#define	__OBJECT_ART_OBJECT_UNROLL_CONTEXT_CC__

#include <iostream>
#include "Object/art_object_unroll_context.h"
#include "Object/expr/const_param.h"
#include "Object/art_object_instance_param.h"
#include "Object/art_object_template_actuals.h"
#include "Object/art_object_template_formals_manager.h"
#include "util/memory/count_ptr.tcc"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class unroll_context method definitions

unroll_context::unroll_context() : template_args(), template_formals() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context::~unroll_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
unroll_context::empty(void) const {
	return (!template_args && !template_formals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
unroll_context::dump(ostream& o) const {
	o << "formals: ";
	if (template_formals)
		template_formals->dump(o);
	else	o << "(none)";
	o << endl << "actuals: ";
	if (template_args)
		template_args->dump(o);
	else	o << "(none)";
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p reference the the formal instance.  
	\return actual value.  
	\pre the context has formal and actual parameters.  
	\pre the type used to expand the formals and actuals
		was canonical and has the same base definition
		as the (definition) owner of p.  
 */
count_ptr<const const_param>
unroll_context::lookup_actual(const param_instance_collection& p) const {
	INVARIANT(!empty());
	INVARIANT(template_args);
	const size_t index(p.is_template_formal());
	INVARIANT(index);
//	cerr << "I got index " << index << "!!!" << endl;
	// remember, index is 1-indexed, whereas [] is 0-indexed.
	const count_ptr<const param_expr> ret((*template_args)[index-1]);
	// assert cast
	return ret.is_a<const const_param>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
unroll_context::set_transform_context(const template_actuals& a, 
		const template_formals_manager& m) {
	INVARIANT(!template_args && !template_formals);
	template_args = never_ptr<const template_actuals>(&a);
	template_formals = never_ptr<const template_formals_manager>(&m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
unroll_context::reset_transform_context(void) {
	INVARIANT(template_args && template_formals);
	template_args = never_ptr<const template_actuals>();
	template_formals = never_ptr<const template_formals_manager>();
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_UNROLL_CONTEXT_CC__

