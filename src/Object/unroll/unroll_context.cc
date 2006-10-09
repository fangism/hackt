/**
	\file "Object/unroll/unroll_context.cc"
	This file originated from "Object/art_object_unroll_context.cc"
		in a previous life.  
	$Id: unroll_context.cc,v 1.17.6.9 2006/10/09 21:09:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_UNROLL_CONTEXT_CC__
#define	__HAC_OBJECT_UNROLL_UNROLL_CONTEXT_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DUMP			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include "Object/unroll/unroll_context.h"
#include "Object/expr/const_param.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/def/definition_base.h"
#include "Object/def/footprint.h"
#include "Object/common/scopespace.h"
#include "Object/common/dump_flags.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/type/template_actuals.h"
#include "Object/def/template_formals_manager.h"
#include "Object/expr/param_expr_list.h"
#include "Object/ref/meta_value_reference_base.h"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/value_placeholder.h"
#include "Object/inst/physical_instance_collection.h"
#endif
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class unroll_context method definitions

#if 0
unroll_context::unroll_context() :
		next(),
#if !RESOLVE_VALUES_WITH_FOOTPRINT
		template_args(), template_formals(),
#endif
		target_footprint(NULL)
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		, lookup_footprint(NULL)
#endif
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(NULL)
#endif
		{ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context::unroll_context(footprint* const f) :
		next(),
#if !RESOLVE_VALUES_WITH_FOOTPRINT
		template_args(), template_formals(), 
#endif
		target_footprint(f)
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		, lookup_footprint(f)
#endif
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(NULL)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#else
/**
	Constructor intended for top-level context.  
 */
unroll_context::unroll_context(footprint* const f, 
		const footprint* const t) :
		next(),
#if !RESOLVE_VALUES_WITH_FOOTPRINT
		template_args(), template_formals(), 
#endif
		target_footprint(f),
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		lookup_footprint(f), 
#endif
		top_footprint(t)
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(NULL)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Read-only context, no target footprint
 */
unroll_context::unroll_context(const footprint* const f, 
		const unroll_context& c) :
		next(&c),
#if !RESOLVE_VALUES_WITH_FOOTPRINT
		template_args(), template_formals(), 
#endif
		target_footprint(NULL),
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		lookup_footprint(f), 
#endif
		top_footprint(c.top_footprint)
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(NULL)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Intended for contexts that are scoped continuations.  
 */
unroll_context::unroll_context(footprint* const f, 
		const unroll_context& c) :
		next(&c),
#if !RESOLVE_VALUES_WITH_FOOTPRINT
		template_args(), template_formals(), 
#endif
		target_footprint(c.target_footprint),
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		lookup_footprint(f), 
#endif
		top_footprint(c.top_footprint)
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(NULL)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Intended for contexts used for unrolling temporary footprints.  
	In such cases, we don't want to unroll into the parent context's
	footprint.  
 */
unroll_context::unroll_context(footprint* const f, 
		const unroll_context& c, 
		const auxiliary_target_tag) :
		next(&c),
#if !RESOLVE_VALUES_WITH_FOOTPRINT
		template_args(), template_formals(), 
#endif
		target_footprint(f),
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
		lookup_footprint(f), 
#endif
		top_footprint(c.top_footprint)
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(NULL)
#endif
		{
}

#endif	// if 0

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !RESOLVE_VALUES_WITH_FOOTPRINT
/**
	Construct a context (translator) between actuals and formals.  
 */
unroll_context::unroll_context(const template_actuals& a, 
		const template_formals_manager& f) :
		next(),
		template_args(a),
		template_formals(&f), 
		target_footprint(NULL)
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(NULL)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called in process_definition::unroll_complete_type.
 */
unroll_context::unroll_context(const template_actuals& a, 
		const template_formals_manager& f, footprint* const fp
#if LOOKUP_GLOBAL_META_PARAMETERS
		, const never_ptr<const name_space> n
#endif
		) :
		next(), 
		template_args(a), 
		template_formals(&f), 
		target_footprint(fp)
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(n)
#endif
		{
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
		target_footprint(NULL)
#if LOOKUP_GLOBAL_META_PARAMETERS
		, parent_namespace(NULL)
#endif
		{
}
#endif	// RESOLVE_VALUES_WITH_FOOTPRINT

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context::~unroll_context() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !RESOLVE_VALUES_WITH_FOOTPRINT
/**
	What does it mean whe n one level is empty, but the next pointer
	points to a continuation?  Shouldn't allow that to happen...
 */
bool
unroll_context::empty(void) const {
	return (!template_args && !template_formals);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add recursive dump to parent scope's (next) contexts.  
	This routine is pretty much only used to aid debugging.  
 */
ostream&
unroll_context::dump(ostream& o) const {
#if STACKTRACE_DUMP
	STACKTRACE_VERBOSE;
#endif
#if !RESOLVE_VALUES_WITH_FOOTPRINT
	o << "formals: ";
	if (template_formals)
		template_formals->dump(o);
	else	o << "(none)";
	o << endl << "actuals: ";
	template_args.dump(o);
#endif
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
	if (lookup_footprint) {
		lookup_footprint->dump_with_collections(
			cerr << endl << "lookup footprint: " << endl,
			dump_flags::default_value, 
			expr_dump_context::default_value);
	}
#endif
	if (target_footprint) {
	if (target_footprint != lookup_footprint) {
		target_footprint->dump_with_collections(
			cerr << endl << "target footprint: " << endl,
			dump_flags::default_value, 
			expr_dump_context::default_value);
	} else {
		cerr << endl << "(target == lookup)" << endl;
	}
	}
	if (top_footprint) {
	if (top_footprint != lookup_footprint) {
		top_footprint->dump_with_collections(
			cerr << endl << "top footprint: " << endl,
			dump_flags::default_value, 
			expr_dump_context::default_value);
	} else {
		cerr << endl << "(top == lookup)" << endl;
	}
	}
#if 1
	// chain dump:
	if (next) {
		o << endl << "next parent context: " << endl;
		next->dump(o);
	}
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	OBSOLETE: after introducing separate target and lookup footprints.
	20051011: Fixed bug where loop context would fail to 
		translate instance reference correctly because
		target_footprint of innermost context was NULL.  
	\return (read-only) pointer to footprint.
		What does it mean if footprint returned is NULL?
		It means we're accessing a top-level instance 
		of the module, as opposed to an instance local to 
		a particular definition's footprint.  
 */
const footprint*
unroll_context::get_target_footprint(void) const {
	STACKTRACE_VERBOSE;
#if 0
	if (!target_footprint && next) {
		return next->get_target_footprint();
	} else {
		// may be NULL
		return target_footprint;
	}
#else
	if (target_footprint) {
		return target_footprint;
	} else if (next) {
		return next->get_target_footprint();
	} else {
		return NULL;
	}
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if LOOKUP_GLOBAL_META_PARAMETERS
/**
	\return pointer to the deepest namespace in which this unroll
		context was created.  
 */
never_ptr<const name_space>
unroll_context::get_parent_namespace(void) const {
	if (parent_namespace)
		return parent_namespace;
	else if (next)
		return next->get_parent_namespace();
	else	return never_ptr<const name_space>(NULL);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a copy of the context with footprint pointer nullified.  
	Called by member_instance_reference::unroll_reference.
 */
unroll_context
unroll_context::make_member_context(void) const {
	STACKTRACE_VERBOSE;
	unroll_context ret(*this);
	ret.target_footprint = NULL;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !RESOLVE_VALUES_WITH_FOOTPRINT
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
			ps.dump(cerr, dump_flags::verbose) <<
				" loop variable, but failed!" << endl;
		)
#else
		return count_ptr<const pint_const>(NULL);
#endif
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_INSTANCE_PLACEHOLDERS
/**
	Only lookup footprints, not scopespaces!
	No need to determine whether or not instance is formal.  
	However, lookup of instance_reference must remain in context, 
	and not go outside scope!  (as far as unrolling is concerned)
	NOTE: looking up of instances should only use the target-footprint, 
		and NOT the lookup-footprint.  
		Lookup should never look out-of-context.  
 */
count_ptr<physical_instance_collection>
unroll_context::lookup_instance_collection(
		const physical_instance_placeholder& p) const {
	typedef	count_ptr<physical_instance_collection>	return_type;
	STACKTRACE_VERBOSE;
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
	NEVER_NULL(target_footprint);
	return (*target_footprint)[p.get_footprint_key()]
		.is_a<physical_instance_collection>();
#else
	if (target_footprint) {
		// TODO: error-handle qualified lookups?
		const return_type
			ret((*target_footprint)[p.get_footprint_key()]
				.is_a<physical_instance_collection>());
		if (ret)
			return ret;
	}
	if (next) {
		// this might be a loop or other local scope.  
		return next->lookup_instance_collection(p);
	}
	return return_type(NULL);
#endif
}
#endif	// USE_INSTANCE_PLACEHOLDERS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_INSTANCE_PLACEHOLDERS
/**
	Only lookup footprints, not scopespaces!
	No need to determine whether or not instance is formal.  
	However, lookup of instance_reference must remain in context, 
	and not go outside scope!  (as far as unrolling is concerned)
	NOTE: rvalues may lookup parent contexts, 
		but lvalues may NOT.  
	TODO: distinguish lvalue from rvalue lookups?
 */
count_ptr<param_value_collection>
#if RVALUE_LVALUE_LOOKUPS
unroll_context::lookup_rvalue_collection
#else
unroll_context::lookup_value_collection
#endif
		(const param_value_placeholder& p) const {
	typedef	count_ptr<param_value_collection>	return_type;
	STACKTRACE_VERBOSE;
	const string key(p.get_footprint_key());
#if ENABLE_STACKTRACE
	dump(cerr << "looking up \"" << key << "\" in context:") << endl;
#endif
#if SRC_DEST_UNROLL_CONTEXT_FOOTPRINTS
	// should really only lookup rvalues with this...
	// target footprint should be covered (and searched last)
	if (lookup_footprint) {
		STACKTRACE_INDENT_PRINT("trying lookup_footprint..." << endl);
		const return_type
			ret((*lookup_footprint)[key]
				.is_a<param_value_collection>());
		if (ret) {
			STACKTRACE_INDENT_PRINT("found it." << endl);
			return ret;
		}
	}
#else
	if (target_footprint && target_footprint != lookup_footprint) {
		STACKTRACE_INDENT_PRINT("trying target_footprint..." << endl);
		// TODO: error-handle qualified lookups?
		const return_type
			ret((*target_footprint)[key]
				.is_a<param_value_collection>());
		if (ret) {
			STACKTRACE_INDENT_PRINT("found it." << endl);
			return ret;
		}
	}
#endif
	// TODO: shouldn't top-footprint be checked last???
	if (top_footprint && top_footprint != lookup_footprint) {
		STACKTRACE_INDENT_PRINT("trying top_footprint..." << endl);
		const return_type
			ret((*top_footprint)[key]
				.is_a<param_value_collection>());
		if (ret) {
			STACKTRACE_INDENT_PRINT("found it." << endl);
			return ret;
		}
	}
	if (next) {
		// this might be a loop or other local scope.  
#if RVALUE_LVALUE_LOOKUPS
		const return_type
			ret(next->lookup_rvalue_collection(p));
		if (ret) {
			STACKTRACE_INDENT_PRINT("found it." << endl);
			return ret;
		}
#else
		return next->lookup_value_collection(p);
#endif
	}
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if RVALUE_LVALUE_LOOKUPS
/**
	Lookup reserved for lvalues, which uses the target footprint.  
 */
count_ptr<param_value_collection>
unroll_context::lookup_lvalue_collection(
		const param_value_placeholder& p) const {
	typedef	count_ptr<param_value_collection>	return_type;
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	dump(cerr << "looking up in context:") << endl;
#endif
	// use the first context with a valid target footprint
	// do not use parents' target footprints
	if (target_footprint) {
		// TODO: error-handle qualified lookups?
		const return_type
			ret((*target_footprint)[p.get_footprint_key()]
				.is_a<param_value_collection>());
		if (ret)
			return ret;
	} else if (next) {
		// this might be a loop or other local scope.  
		return next->lookup_lvalue_collection(p);
	}
	return return_type(NULL);
}
#endif	// RVALUE_LVALUE_LOOKUPS
#endif	// USE_INSTANCE_PLACEHOLDERS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_INSTANCE_PLACEHOLDERS
count_ptr<physical_instance_collection>
unroll_context::lookup_collection(
		const physical_instance_placeholder& p) const {
	return lookup_instance_collection(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<param_value_collection>
unroll_context::lookup_collection(
		const param_value_placeholder& p) const {
#if RVALUE_LVALUE_LOOKUPS
	// defaults to using rvalue lookup, is this "the right thing"?
	return lookup_rvalue_collection(p);
#else
	return lookup_value_collection(p);
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_INSTANCE_PLACEHOLDERS
/**
	TODO: fix so that target (instantiating) footprints (writable)
	can be distinguished from read-only footprints.  
 */
void
unroll_context::instantiate_collection(
		const count_ptr<instance_collection_base>& p) const {
	STACKTRACE_VERBOSE;
	footprint* f = target_footprint;
	never_ptr<const this_type> c(this);
	do {
		f = c->target_footprint;
		c = c->next;
	} while (!f && c);
	NEVER_NULL(f);
	const good_bool g(f->register_collection(p));
	INVARIANT(g.good);
}
#endif	// USE_INSTANCE_PLACEHOLDERS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !USE_INSTANCE_PLACEHOLDERS
// getting rid of this shit in favor of simpler, more consistent lookup
/**
	NOTE: this method is SO CRITICAL...
	FYI: this is called by simple_meta_value_reference::unroll_resolve().
	\param p reference to the formal instance, cannot be non-formal!
	\return actual value, a bunch of constants.  
		NOT the same as lookup_const_collection (below)
	\pre the context has formal and actual parameters.  
	\pre the type used to expand the formals and actuals
		was canonical and has the same base definition
		as the (definition) owner of p.  
	TODO: completely rewrite this.
 */
count_ptr<const const_param>
unroll_context::lookup_actual(
#if USE_INSTANCE_PLACEHOLDERS
		const param_value_placeholder& p
#else
		const param_value_collection& p
#endif
		) const {
	typedef	count_ptr<const const_param>	return_type;
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("looking up: " <<
		p.get_footprint_key() << endl);
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
				ic((*tfp)[p.get_footprint_key()]);
			NEVER_NULL(ic);
			FINISH_ME_EXIT(Fang);
		}
	}
	// else is a template formal, use unroll context to translate

	// not the position of the template formal in its own list
	// but in the current context!!!
	// very awkward...
#if USE_INSTANCE_PLACEHOLDERS
	const instance_placeholder_base::owner_ptr_type
#else
	const instance_collection_base::owner_ptr_type
#endif
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
		// template formal names are never qualified
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
			{
			const expr_dump_context edc;
			ret->what(STACKTRACE_INDENT << "expr (");
			ret->dump(STACKTRACE_STREAM << ") = ", edc) << endl;
			}
#endif
			const count_ptr<const meta_value_reference_base>
				self(ret.is_a<const meta_value_reference_base>());
			if (self) {
#if USE_INSTANCE_PLACEHOLDERS
				const never_ptr<const param_value_placeholder>
#else
				const never_ptr<const param_value_collection>
#endif
					pbase(self->get_coll_base());
				if (pbase == &p) {
				// need to safeguard against self-lookup
				// may happen with value_reference
				// else will have infinite mutual recursion
				if (next) {
					// self-reference detected
					return ret->unroll_resolve_rvalues(*next, ret);
				} else {
					lookup_panic(cerr);	// no return
					return return_type(NULL);
				}
				} else {
					// check this scope again
					// for parameter-dependent-parameter
					// e.g. in default parameter exprs.
					return ret->unroll_resolve_rvalues(*this, ret);
				}
			} else {
				// not self references, safe to lookup again
				// NOTE: expressions cannot be cyclic
				// so checking self-reference is sufficient
				// for safety
				return ret->unroll_resolve_rvalues(*this, ret);
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
		p.dump(cerr, dump_flags::verbose)
			<< " to constant value(s), but failed!" << endl;
	)
		return return_type(NULL);
	}
}	// end method lookup_actual
#endif	// USE_INSTANCE_PLACEHOLDERS

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

#endif	// __HAC_OBJECT_UNROLL_UNROLL_CONTEXT_CC__

