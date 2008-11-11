/**
	\file "Object/unroll/template_type_completion.tcc"
	$Id: template_type_completion.tcc,v 1.3 2008/11/11 22:30:03 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_TCC__
#define	__HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_TCC__

#include <iostream>
#include "Object/unroll/template_type_completion.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"	// for dynamic casting
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.tcc"	// for create_dependent_types
#include "Object/inst/instance_collection.h"
#include "Object/inst/internal_aliases_policy.h"
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
	typedef	simple_meta_instance_reference<Tag>	index_ref_type;
	alias_collection_type aliases;
	const count_ptr<const index_ref_type>
		index_ref(ref.template is_a<const index_ref_type>());
	if (!index_ref) {
		FINISH_ME(Fang);
		cerr << "Add support for non-simple_meta_index_reference."
			<< endl;
		// could include member references and aggregate references?
		return good_bool(false);
	}
	// instead of using ref
	if (index_ref->unroll_references_packed(c, aliases).bad) {
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
	INVARIANT(i!=e);
	// for index_expr, all aliases belong to the same collection
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_type;
	typedef	typename instance_collection_type::resolved_type_ref_type
					canonical_type_type;
	// expand the type once is all that is necessary
	// since alias are at least collectibly type-equivalent
	const instance_collection_type&
		coll((*i)->container->get_canonical_collection());
	canonical_type_type ct(coll.get_resolved_canonical_type());
	ct.combine_relaxed_actuals(resolved);
	INVARIANT(ct.is_strict());	// we now have a complete type
	if (!ct.create_definition_footprint(topfp).good) {
		// already have error message?
		cerr << "Instantiated by: ";
		(*i)->dump_hierarchical_name(cerr) << endl;
		return good_bool(false);
	}
	for ( ; i!=e; ++i) {
		instance_alias_info<Tag>& a(**i);	// named
		instance_alias_info<Tag>& ca(*a.find(c));	// canonical
			// this find() performs path compression
		// a may alias ca!
		const bool alias = (&a == &ca);
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
		if (!alias && !ca.container->get_canonical_collection()
				.has_relaxed_type()) {
			cerr << "Error: canonical collection `";
			ca.container->dump_hierarchical_name(cerr);
			cerr <<  "\' already has strict type, "
				"cannot bind member to relaxed parameters."
				<< endl;
			return good_bool(false);
		}
		if (!ca.attach_actuals(resolved) ||
				(!alias && !a.attach_actuals(resolved))) {
			// error, canonical reference already has actuals
			cerr << "Error: canonical alias of `";
			a.dump_hierarchical_name(cerr);
			cerr << "\' (";
			ca.dump_hierarchical_name(cerr);
			cerr << ") is already bound to relaxed parameters."
				<< endl;
			return good_bool(false);
		}
		// instantiate/unroll public ports hierarchy recursively
		// similar to instance_alias_info::instantiate(), 
		// but parent collection already established.  
		// see also instance_array::instantiate_indices()'s do-loop.
		try {
			a.instantiate_actuals_only(c);
			if (!alias) {
				ca.instantiate_actuals_only(c);
			}
			// HACK (2007-07-14):
			// what about aliases between this one and 
			// the canonical?
			// too late! find() already path-compressed :(
			// need to rely on backpatching during create phase...
		} catch (...) {
			cerr << "Error instantiating `";
			a.dump_hierarchical_name(cerr) << "\'" << endl;
			return good_bool(false);
		}
		// replay_internal_aliases
		// it is sufficient to connect just the canonical alias
		// of each group because substructures will already be aliased.
		typedef	typename instance_alias_info<Tag>::internal_alias_policy
					internal_alias_policy;
		if (!internal_alias_policy::connect(ca, ct).good) {
			cerr << "Error making internal connections of `";
			a.dump_hierarchical_name(cerr) << "\'" << endl;
			return good_bool(false);
		}
	}	// end for-each alias in collection
	return good_bool(true);
}	// end method unroll

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

