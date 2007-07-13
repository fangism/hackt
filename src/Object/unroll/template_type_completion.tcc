/**
	\file "Object/unroll/template_type_completion.tcc"
	$Id: template_type_completion.tcc,v 1.1.2.4 2007/07/13 01:08:06 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_TCC__
#define	__HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_TCC__

#include <iostream>
#include "Object/unroll/template_type_completion.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.tcc"	// for create_dependent_types
#include "Object/inst/instance_collection.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/unroll/unroll_context.h"

#include "common/TODO.h"
#include "util/persistent_object_manager.h"
#include "util/stacktrace.h"
#include "util/what.tcc"
#include "util/packed_array.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::persistent_traits;

//=============================================================================
// class template_type_completion method definitions

template <class Tag>
template_type_completion<Tag>::template_type_completion() :
		ref(), relaxed_args() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param r is the instance reference
	\param a is the list of template arguments to attach
 */
template <class Tag>
template_type_completion<Tag>::template_type_completion(
		const reference_ptr_type& r, 
		const const_relaxed_args_type& a) :
		ref(r), relaxed_args(a) {
	NEVER_NULL(ref);
	NEVER_NULL(relaxed_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
template_type_completion<Tag>::~template_type_completion() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
template_type_completion<Tag>::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
template_type_completion<Tag>::dump(ostream& o,
		const expr_dump_context& c) const {
	ref->dump(o, c);
	return relaxed_args->dump(o << '<', c) << '>';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolve referenced aliases and bind relaxed template
	parameters to complete its type.  
	This should work similarly to commit_type_first_time.  
	Q: can port hierarchy depend on relaxed parameters? 
		not in type/size/dimensions, but in internal aliasing shape.
 */
template <class Tag>
good_bool
template_type_completion<Tag>::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	typedef	typename reference_type::alias_collection_type
			alias_collection_type;
	alias_collection_type aliases;
	if (ref->unroll_references_packed(c, aliases).bad) {
		// Error message?
		return good_bool(false);
	}
	const count_ptr<const const_param_expr_list>
		resolved(relaxed_args->unroll_resolve_rvalues(c, relaxed_args));
	if (!resolved) {
		// Error message?
		return good_bool(false);
	}
	const footprint& topfp(*c.get_top_footprint());
	typedef	typename alias_collection_type::const_iterator	const_iterator;
	const_iterator i(aliases.begin()), e(aliases.end());
	for ( ; i!=e; ++i) {
		instance_alias_info<Tag>& a(**i);	// named
		instance_alias_info<Tag>& ca(*a.find());	// canonical
		if (!a.container->get_canonical_collection()
				.has_relaxed_type()) {
			cerr << "Error: collection `";
			a.container->dump_hierarchical_name(cerr);
			cerr <<  "\' already has strict type, "
				"cannot bind member to relaxed parameters."
				<< endl;
			return good_bool(false);
		}
		// canonical check is rather redundant because we forbid
		// aliasing between strict/relaxed container members.
		if (!ca.container->get_canonical_collection()
				.has_relaxed_type()) {
			cerr << "Error: canonical collection `";
			ca.container->dump_hierarchical_name(cerr);
			cerr <<  "\' already has strict type, "
				"cannot bind member to relaxed parameters."
				<< endl;
			return good_bool(false);
		}
		if (ca.attach_actuals(resolved) && a.attach_actuals(resolved)) {
			// error, canonical reference already has actuals
			cerr << "Error: canonical alias of `";
			a.dump_hierarchical_name(cerr);
			cerr << "\' (";
			ca.dump_hierarchical_name(cerr);
			cerr << ") is already bound to relaxed parameters."
				<< endl;
		}
		// create footprint of complete type:
		// only need to do once, because all aliases referenced
		// should have the same complete type, use the first.
		// rather wasteful to place this inside loop...
		if (!instance_alias_info<Tag>::create_dependent_types(
				a, topfp).good) {
			// already have error message
			return good_bool(false);
		}
		// instantiate/unroll public ports hierarchy recursively
		// similar to instance_alias_info::instantiate(), 
		// but parent collection already established.  
		// see also instance_array::instantiate_indices()'s do-loop.
		a.instantiate_actuals_only(c);
		// throws exception on error
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
template_type_completion<Tag>::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	ref->collect_transient_info(m);
	relaxed_args->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
template_type_completion<Tag>::write_object(const persistent_object_manager& m,
		ostream& o) const {
	m.write_pointer(o, ref);
	m.write_pointer(o, relaxed_args);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
template_type_completion<Tag>::load_object(const persistent_object_manager& m,
		istream& i) {
	m.read_pointer(i, ref);
	m.read_pointer(i, relaxed_args);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_TCC__

