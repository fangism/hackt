/**
	\file "Object/unroll/unroll_context.cc"
	This file originated from "Object/art_object_unroll_context.cc"
		in a previous life.  
	$Id: unroll_context.cc,v 1.1.2.2 2005/07/23 01:06:02 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_UNROLL_CONTEXT_CC__
#define	__OBJECT_UNROLL_UNROLL_CONTEXT_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "Object/unroll/unroll_context.h"
#include "Object/expr/const_param.h"
#include "Object/def/definition_base.h"
#include "Object/common/scopespace.h"
#include "Object/inst/param_value_collection.h"
#include "Object/ref/simple_param_meta_value_reference.h"
#include "Object/type/template_actuals.h"
#include "Object/def/template_formals_manager.h"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class unroll_context method definitions

unroll_context::unroll_context() :
		next(), template_args(), template_formals() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct a context (translator) between actuals and formals.  
 */
unroll_context::unroll_context(const template_actuals& a, 
		const template_formals_manager& f) :
		next(), template_args(&a), template_formals(&f) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In addition to standard construction, also chains to 
	a parent context.  
 */
unroll_context::unroll_context(const template_actuals& a, 
		const template_formals_manager& f, 
		const this_type& c) :
		next(&c), template_args(&a), template_formals(&f) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context::~unroll_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	What does it mean whe n one level is empty, but the next pointer
	points to a continuation?  Shouldn't allow that to happen...
 */
bool
unroll_context::empty(void) const {
	return (!template_args && !template_formals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add recursive dump to parent scope's (next) contexts.  
 */
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
	NOTE: this method is SO CRITICAL...
	FYI: this is called by simple_meta_value_reference::unroll_resolve().
	\param p reference to the formal instance, cannont be non-formal!
	\return actual value, a bunch of constants.  
		NOT the same as lookup_const_collection (below)
	\pre the context has formal and actual parameters.  
	\pre the type used to expand the formals and actuals
		was canonical and has the same base definition
		as the (definition) owner of p.  
	TODO: completely rewrite this.
 */
count_ptr<const const_param>
unroll_context::lookup_actual(const param_value_collection& p) const {
	typedef	count_ptr<const const_param>	return_type;
	STACKTRACE("unroll_context::lookup_actual()");
#if ENABLE_STACKTRACE
	cerr << "looking up: " << p.get_name() << endl;
	dump(cerr << "with: ") << endl;
#endif
	INVARIANT(!empty());
	INVARIANT(template_args);
	INVARIANT(p.is_template_formal());
	// not the position of the template formal in its own list
	// but in the current context!!!
	// very awkward...
	const instance_collection_base::owner_ptr_type
		p_owner(p.get_owner());
	const never_ptr<const definition_base>
		p_def(p_owner.is_a<const definition_base>());
	NEVER_NULL(p_def);
	const template_formals_manager&
		p_tfm(p_def->get_template_formals_manager());
	// need to make sure we're lookin up the correct set
	// of formals...
	// why don't we just search up the context chain until
	// we find the matching template formals reference?
	if (template_formals == &p_tfm) {
		const size_t index(p_tfm.lookup_template_formal_position(
			p.get_name()));
		INVARIANT(index);
//		cerr << "I got index " << index << "!!!" << endl;
		// remember, index is 1-indexed, whereas [] is 0-indexed.
		const count_ptr<const param_expr>
			ret((*template_args)[index-1]);
		NEVER_NULL(ret);
		const return_type const_ret(ret.is_a<const const_param>());
		// actual may STILL be another formal reference!
		if (const_ret) {
			return const_ret;
		} else {
#if ENABLE_STACKTRACE
			ret->what(cerr << "expr (");
			ret->dump(cerr << ") = ") << endl;
#endif
			const count_ptr<const simple_param_meta_value_reference>
				self(ret.is_a<const simple_param_meta_value_reference>());
			if (self) {
				const never_ptr<const instance_collection_base>
					pbase(self->get_inst_base());
				if (pbase == &p) {
				// need to safeguard against self-lookup
				// may happen with value_reference
				// else will have infinite mutual recursion
				if (next) {
					// self-reference detected
					return ret->unroll_resolve(*next);
				} else {
					lookup_panic(cerr);	// no return
					return return_type(NULL);
				}
				} else {
					// check this scope again
					// for parameter-dependent-parameter
					// e.g. in default parameter exprs.
					return ret->unroll_resolve(*this);
				}
			} else {
				// not self references, safe to lookup again
				// NOTE: expressions cannot be cyclic
				// so checking self-reference is sufficient
				// for safety
				return ret->unroll_resolve(*this);
				// guaranteed that this will terminate
				// even if recursive
			}
		}
	} else if (next) {
		// STACKTRACE("checking parent context");
		return next->lookup_actual(p);
	} else {
		cerr << "Internal compiler error: expected resolve ";
		p.dump(cerr) << " to constant value(s), but failed!" << endl;
		THROW_EXIT;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Another typical panic message.
 */
void
unroll_context::lookup_panic(ostream& o) {
	o << "Internal compiler error: " << endl <<
		"In unroll_context::lookup_actual(): " << endl <<
		"Help me, Obi-fang Kenobi!" << endl;
	THROW_EXIT;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attaches a continuation scope's context to be searchable from here.  
 */
void
unroll_context::chain_context(const this_type& c) {
	INVARIANT(!next);
	next = never_ptr<const this_type>(&c);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_UNROLL_UNROLL_CONTEXT_CC__

