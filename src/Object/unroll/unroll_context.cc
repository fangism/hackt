/**
	\file "Object/unroll/unroll_context.cc"
	This file originated from "Object/art_object_unroll_context.cc"
		in a previous life.  
	$Id: unroll_context.cc,v 1.7 2005/12/13 04:15:44 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_UNROLL_CONTEXT_CC__
#define	__OBJECT_UNROLL_UNROLL_CONTEXT_CC__

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "Object/unroll/unroll_context.h"
#include "Object/expr/const_param.h"
#include "Object/def/definition_base.h"
#include "Object/def/footprint.h"
#include "Object/common/scopespace.h"
// #include "Object/inst/param_value_collection.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/ref/simple_param_meta_value_reference.h"
#include "Object/type/template_actuals.h"
#include "Object/def/template_formals_manager.h"
#include "Object/expr/param_expr_list.h"
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class unroll_context method definitions

unroll_context::unroll_context() :
		next(), template_args(), template_formals(),
		target_footprint(NULL)
		{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context::unroll_context(footprint* const f) :
		next(), template_args(), template_formals(), 
		target_footprint(f) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct a context (translator) between actuals and formals.  
 */
unroll_context::unroll_context(const template_actuals& a, 
		const template_formals_manager& f) :
		next(),
		template_args(a),
		template_formals(&f), 
		target_footprint(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called in process_definition::unroll_complete_type.
 */
unroll_context::unroll_context(const template_actuals& a, 
		const template_formals_manager& f, footprint* const fp) :
		next(), 
		template_args(a), 
		template_formals(&f), 
		target_footprint(fp) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In addition to standard construction, also chains to 
	a parent context.  
 */
unroll_context::unroll_context(const template_actuals& a, 
		const template_formals_manager& f, 
		const this_type& c) :
		next(&c),
		template_args(a),
		template_formals(&f), 
		target_footprint(NULL) {
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
	template_args.dump(o);
	if (target_footprint)
		target_footprint->dump_with_collections(
			cerr << endl << "footprint: ");
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	20051011: Fixed bug where loop context would fail to 
		translate instance reference correctly because
		target_footprint of innermost context was NULL.  
 */
const footprint*
unroll_context::get_target_footprint(void) const {
	if (!target_footprint && next)
		return next->get_target_footprint();
	else	return target_footprint;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a copy of the context with footprint pointer nullified.  
	Calledn by member_instance_reference::unroll_reference.
 */
unroll_context
unroll_context::make_member_context(void) const {
	STACKTRACE_VERBOSE;
	unroll_context ret(*this);
	ret.target_footprint = NULL;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Used only for looking up loop variables.  
	Must get the scope correct, check the template formals manager, 
	acting on behalf of the actual value of the loop variable.  
	\return NULL if not found.
 */
count_ptr<const pint_const>
unroll_context::lookup_loop_var(const pint_scalar& ps) const {
	STACKTRACE_VERBOSE;
	INVARIANT(template_args.get_strict_args()->size() == 1);
		// not true if this is called when looking up general variables
	const never_ptr<const pint_scalar>
		probe(template_formals->lookup_template_formal(ps.get_name())
			.is_a<const pint_scalar>());
	if (probe) {
		return template_args[0].is_a<const pint_const>();
	} else if (next) {
		return next->lookup_loop_var(ps);
	} else {
#if 1
		ICE(cerr, 
			cerr << "expected to resolve ";
			ps.dump(cerr) <<
				" loop variable, but failed!" << endl;
		)
#else
		return count_ptr<const pint_const>(NULL);
#endif
	}
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
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "looking up: " << p.get_name() << endl;
	dump(cerr << "with: ") << endl;
#endif
	INVARIANT(!empty());
	if (!p.is_template_formal()) {
		// could be either loop variable or local variable!
		const pint_scalar* ps(IS_A(const pint_scalar*, &p));
		if (ps && ps->is_loop_variable()) {
			return lookup_loop_var(*ps);
		} else {
			// otherwise, we have a private local parameter variable
			// need to look it up in footprint
			const footprint* const tfp = get_target_footprint();
			NEVER_NULL(tfp);
			const count_ptr<instance_collection_base>
				ic((*tfp)[p.get_name()]);
			NEVER_NULL(ic);
			FINISH_ME_EXIT(Fang);
		}
	}
	// else is a template formal, use unroll context to translate

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
			ret(template_args[index-1]);
		NEVER_NULL(ret);
		const return_type const_ret(ret.is_a<const const_param>());
		// actual may STILL be another formal reference!
		if (const_ret) {
			return const_ret;
		} else {
#if ENABLE_STACKTRACE
			ret->what(STACKTRACE_INDENT << "expr (");
			ret->dump(STACKTRACE_STREAM << ") = ") << endl;
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
	ICE(cerr, 
		cerr << "expected to resolve ";
		p.dump(cerr) << " to constant value(s), but failed!" << endl;
	)
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Another typical panic message.
 */
void
unroll_context::lookup_panic(ostream& o) {
	ICE(o, 
		o << "In unroll_context::lookup_actual(): " << endl <<
			"Help me, Obi-fang Kenobi!" << endl;
	)
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
}	// end namespace HAC

#endif	// __OBJECT_UNROLL_UNROLL_CONTEXT_CC__

