/**
	\file "Object/unroll/unroll_context_value_resolver.cc"
	Rationale: separate definition to control eager instantiation.  
	$Id: unroll_context_value_resolver.cc,v 1.5.6.2 2006/09/02 00:46:10 fang Exp $
 */

#include "Object/unroll/unroll_context_value_resolver.h"
#include "Object/expr/pint_const.h"
#include "Object/inst/pint_value_collection.h"
#if USE_INSTANCE_PLACEHOLDERS
#include "Object/inst/value_placeholder.h"
#endif
#if LOOKUP_GLOBAL_META_PARAMETERS
#include "Object/common/namespace.h"
#endif
#include "common/TODO.h"

namespace HAC {
namespace entity {

//=============================================================================
// class unroll_context_value_resolver<pint_tag> method definitions

/**
	TODO: use a consistent scoped lookup function (up footprints)
		to avoid this hackery, now that we have proper placeholders.  
	2006-09-01

	Looks up a value for reading and resolving.  
	\return true if lookup was resolved a loop variable.  
 */
unroll_context_value_resolver<pint_tag>::const_return_type
unroll_context_value_resolver<pint_tag>::operator ()
		(const unroll_context& c, const value_placeholder_type& v,
		value_type& i) const {
	// hack: intercept loop variable lookups
	// TODO: can't cast placeholder to scalar!
	const value_scalar_type* const
		vsp(IS_A(const value_scalar_type*, &v));
	if (vsp && vsp->is_loop_variable()) {
		// icc: this causes premature instantiation of 
		// value_array<pint_tag, 0>
		const count_ptr<const pint_const>
			pc(c.lookup_loop_var(*vsp));
		if (pc) {
			i = pc->static_constant_value();
			// stupid LWG defect...
			// return std::make_pair(true, v);
			// stupid reference-to-reference...
			// return return_type(true, v);
#if USE_INSTANCE_PLACEHOLDERS
			FINISH_ME(Fang);
			return const_return_type(true, &v);
#else
			return const_return_type(true, &v);
#endif
		}
	}
	// end hack
	const footprint* const f(c.get_target_footprint());
#if USE_INSTANCE_PLACEHOLDERS
	NEVER_NULL(f);
	const value_collection_type*
		_vals(IS_A(const value_collection_type*, &*(*f)[v.get_name()]));
#else
	const value_collection_type*
		_vals(f ? IS_A(const value_collection_type*,
				&*(*f)[v.get_name()])
			: &v);
#endif
#if LOOKUP_GLOBAL_META_PARAMETERS
	// don't expect footprint lookup to find globals, only locals
	if (!_vals) {
		// not found in current footprint
		// then lookup parent namespaces until found...
		// need to get namespace stack from context...
		const never_ptr<const name_space> n(c.get_parent_namespace());
		NEVER_NULL(n);
		const never_ptr<const value_collection_type>
			_v(n->lookup_object(v.get_name())
				.is_a<const value_collection_type>());
		INVARIANT(_v);
		_vals = &*_v;
	}
#else
	INVARIANT(_vals);
#endif
	return const_return_type(false, _vals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up a value-collection for modification purposes.  
	Does NOT perform lookup of globals, because globals
	should not be modified through outside contexts.  
 */
unroll_context_value_resolver<pint_tag>::value_collection_type&
unroll_context_value_resolver<pint_tag>::operator ()
	(const unroll_context& c, value_placeholder_type& v) const {
	// no specialization, can't assign to loop vars.  
	const footprint* const f(c.get_target_footprint());
#if USE_INSTANCE_PLACEHOLDERS
	NEVER_NULL(f);
	value_collection_type&
		_vals(IS_A(value_collection_type&, *(*f)[v.get_name()]));
#else
	value_collection_type&
		_vals(f ? IS_A(value_collection_type&, *(*f)[v.get_name()])
			: v);
#endif
	return _vals;
}

//-----------------------------------------------------------------------------
// class unroll_context_value_resolver<pbool_tag> method definitions

unroll_context_value_resolver<pbool_tag>::const_return_type
unroll_context_value_resolver<pbool_tag>::operator ()
		(const unroll_context& c, const value_placeholder_type& v,
		value_type& i) const {
	const footprint* const f(c.get_target_footprint());
#if USE_INSTANCE_PLACEHOLDERS
	NEVER_NULL(f);
	const value_collection_type*
		_vals(IS_A(const value_collection_type*, &*(*f)[v.get_name()]));
#else
	const value_collection_type*
		_vals(f ? IS_A(const value_collection_type*,
				&*(*f)[v.get_name()])
			: &v);
#endif
#if LOOKUP_GLOBAL_META_PARAMETERS
	// don't expect footprint lookup to find globals, only locals
	if (!_vals) {
		const never_ptr<const name_space> n(c.get_parent_namespace());
		NEVER_NULL(n);
		const never_ptr<const value_collection_type>
			_v(n->lookup_object(v.get_name())
				.is_a<const value_collection_type>());
		INVARIANT(_v);
		_vals = &*_v;
	}
#else
	INVARIANT(_vals);
#endif
	return const_return_type(false, _vals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context_value_resolver<pbool_tag>::value_collection_type&
unroll_context_value_resolver<pbool_tag>::operator ()
	(const unroll_context& c, value_placeholder_type& v) const {
	// no specialization, can't assign to loop vars.  
	const footprint* const f(c.get_target_footprint());
#if USE_INSTANCE_PLACEHOLDERS
	NEVER_NULL(f);
	value_collection_type&
		_vals(IS_A(value_collection_type&, *(*f)[v.get_name()]));
#else
	value_collection_type&
		_vals(f ? IS_A(value_collection_type&, *(*f)[v.get_name()])
			: v);
#endif
	return _vals;
}

//-----------------------------------------------------------------------------
/**
	No need to check if variable is for flow control, 
		because those are not declared and referenced.  
	\param i is never used.  
 */
unroll_context_value_resolver<preal_tag>::const_return_type
unroll_context_value_resolver<preal_tag>::operator ()
		(const unroll_context& c, const value_placeholder_type& v,
		value_type& i) const {
	const footprint* const f(c.get_target_footprint());
#if USE_INSTANCE_PLACEHOLDERS
	NEVER_NULL(f);
	const value_collection_type*
		_vals(IS_A(const value_collection_type*, &*(*f)[v.get_name()]));
#else
	const value_collection_type*
		_vals(f ? IS_A(const value_collection_type*,
				&*(*f)[v.get_name()])
			: &v);
#endif
#if LOOKUP_GLOBAL_META_PARAMETERS
	// don't expect footprint lookup to find globals, only locals
	if (!_vals) {
		const never_ptr<const name_space> n(c.get_parent_namespace());
		NEVER_NULL(n);
		const never_ptr<const value_collection_type>
			_v(n->lookup_object(v.get_name())
				.is_a<const value_collection_type>());
		INVARIANT(_v);
		_vals = &*_v;
	}
#else
	INVARIANT(_vals);
#endif
	return const_return_type(false, _vals);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unroll_context_value_resolver<preal_tag>::value_collection_type&
unroll_context_value_resolver<preal_tag>::operator ()
	(const unroll_context& c, value_placeholder_type& v) const {
	// no specialization, can't assign to loop vars.  
	const footprint* const f(c.get_target_footprint());
#if USE_INSTANCE_PLACEHOLDERS
	NEVER_NULL(f);
	value_collection_type&
		_vals(IS_A(value_collection_type&, *(*f)[v.get_name()]));
#else
	value_collection_type&
		_vals(f ? IS_A(value_collection_type&, *(*f)[v.get_name()])
			: v);
#endif
	return _vals;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

