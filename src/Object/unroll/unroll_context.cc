/**
	\file "Object/unroll/unroll_context.cc"
	This file originated from "Object/art_object_unroll_context.cc"
		in a previous life.  
	$Id: unroll_context.cc,v 1.24.8.3 2006/12/26 21:26:11 fang Exp $
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
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/value_placeholder.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/global_entry_context.h"
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/memory/count_ptr.tcc"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class unroll_context method definitions

/**
	Constructor intended for top-level context.  
 */
unroll_context::unroll_context(footprint* const f, 
		const footprint* const t) :
		next(),
		target_footprint(f),
		const_target_footprint(f), 
		lookup_footprint(f), 
		top_footprint(t) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by reference::lookup_globally_allocated_index
	No target, just pure lookup.  
 */
unroll_context::unroll_context(const footprint* const f, 
		const footprint* const t) :
		next(),
		target_footprint(NULL),
		const_target_footprint(f), 
		lookup_footprint(f), 
		top_footprint(t) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context::unroll_context(const global_entry_context_base& c) :
		next(),
		target_footprint(NULL),
		const_target_footprint(c.get_top_footprint_ptr()), 
		lookup_footprint(c.get_top_footprint_ptr()), 
		top_footprint(c.get_top_footprint_ptr()) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Read-only context, no target footprint
 */
unroll_context::unroll_context(const footprint* const f, 
		const unroll_context& c) :
		next(&c),
		target_footprint(NULL),
		const_target_footprint(f), 
		lookup_footprint(f), 
		top_footprint(c.top_footprint) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Intended for contexts that are scoped continuations.  
 */
unroll_context::unroll_context(footprint* const f, 
		const unroll_context& c) :
		next(&c),
		target_footprint(c.target_footprint),
		const_target_footprint(c.const_target_footprint), 
		lookup_footprint(f), 
		top_footprint(c.top_footprint) {
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
		target_footprint(f),
		const_target_footprint(f), 
		lookup_footprint(f), 
		top_footprint(c.top_footprint) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context::~unroll_context() { }

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
	if (lookup_footprint) {
		lookup_footprint->dump_with_collections(
			cerr << endl << "lookup footprint: " << endl,
			dump_flags::default_value, 
			expr_dump_context::default_value);
	}
	if (const_target_footprint) {
	if (const_target_footprint != lookup_footprint) {
		const_target_footprint->dump_with_collections(
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
/**
	\return a copy of the context with footprint pointer nullified.  
	Called by member_instance_reference::unroll_reference.
 */
unroll_context
unroll_context::make_member_context(void) const {
	STACKTRACE_VERBOSE;
	unroll_context ret(*this);
	ret.target_footprint = NULL;
	ret.const_target_footprint = NULL;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only lookup footprints, not scopespaces!
	No need to determine whether or not instance is formal.  
	However, lookup of instance_reference must remain in context, 
	and not go outside scope!  (as far as unrolling is concerned)
	NOTE: looking up of instances should only use the target-footprint, 
		and NOT the lookup-footprint.  
		Lookup should never look out-of-context.  
 */
never_ptr<physical_instance_collection>
unroll_context::lookup_instance_collection(
		const physical_instance_placeholder& p) const {
	typedef	count_ptr<physical_instance_collection>	return_type;
	STACKTRACE_VERBOSE;
	NEVER_NULL(const_target_footprint);
	return (*const_target_footprint)[p.get_footprint_key()]
		.is_a<physical_instance_collection>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Intended for use with looking up canonical_collection belonging
	for footprint for the type that is being recursively unrolled
	(instantiating public ports).  
	This only uses the outermost lookup footprint. 
	Called from instance_placeholder::unroll_port_only().  
 */
never_ptr<const physical_instance_collection>
unroll_context::lookup_port_collection(
		const physical_instance_placeholder& p) const {
	typedef	count_ptr<physical_instance_collection>	return_type;
	STACKTRACE_VERBOSE;
	NEVER_NULL(lookup_footprint);
	return (*lookup_footprint)[p.get_footprint_key()]
		.is_a<const physical_instance_collection>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only lookup footprints, not scopespaces!
	No need to determine whether or not instance is formal.  
	However, lookup of instance_reference must remain in context, 
	and not go outside scope!  (as far as unrolling is concerned)
	NOTE: rvalues may lookup parent contexts, 
		but lvalues may NOT.  
	TODO: distinguish lvalue from rvalue lookups?
 */
never_ptr<param_value_collection>
unroll_context::lookup_rvalue_collection
		(const param_value_placeholder& p) const {
	typedef	never_ptr<param_value_collection>	return_type;
	STACKTRACE_VERBOSE;
	const string key(p.get_footprint_key());
#if ENABLE_STACKTRACE
	dump(cerr << "looking up \"" << key << "\" in context:") << endl;
#endif
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
		const return_type
			ret(next->lookup_rvalue_collection(p));
		if (ret) {
			STACKTRACE_INDENT_PRINT("found it." << endl);
			return ret;
		}
	}
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Lookup reserved for lvalues, which uses the target footprint.  
 */
never_ptr<param_value_collection>
unroll_context::lookup_lvalue_collection(
		const param_value_placeholder& p) const {
	typedef	never_ptr<param_value_collection>	return_type;
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	dump(cerr << "looking up in context:") << endl;
#endif
	// use the first context with a valid target footprint
	// do not use parents' target footprints
	if (const_target_footprint) {
		// TODO: error-handle qualified lookups?
		const return_type
			ret((*const_target_footprint)[p.get_footprint_key()]
				.is_a<param_value_collection>());
		if (ret)
			return ret;
	} else if (next) {
		// this might be a loop or other local scope.  
		return next->lookup_lvalue_collection(p);
	}
	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<physical_instance_collection>
unroll_context::lookup_collection(
		const physical_instance_placeholder& p) const {
	return lookup_instance_collection(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<param_value_collection>
unroll_context::lookup_collection(
		const param_value_placeholder& p) const {
	// defaults to using rvalue lookup, is this "the right thing"?
	return lookup_rvalue_collection(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return reference to modifiable footprint.  
 */
footprint&
unroll_context::get_target_footprint(void) const {
	footprint* f = target_footprint;
	never_ptr<const this_type> c(this);
	do {
		f = c->target_footprint;
		c = c->next;
	} while (!f && c);
	NEVER_NULL(f);
	return *f;
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

