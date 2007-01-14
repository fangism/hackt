/**
	\file "Object/ref/nonmeta_ref_implementation.tcc"
	Policy-based implementations of some nonmeta reference functions.  
 	$Id: nonmeta_ref_implementation.tcc,v 1.1.2.7 2007/01/14 05:38:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_NONMETA_REF_IMPLEMENTATION_TCC__
#define	__HAC_OBJECT_REF_NONMETA_REF_IMPLEMENTATION_TCC__

#include <iostream>
#include <algorithm>

#include "Object/ref/nonmeta_ref_implementation.h"
// #include "Object/ref/simple_nonmeta_instance_reference.h"
#include "Object/unroll/unroll_context.h"
#include "Object/global_entry_context.tcc"
#include "Object/nonmeta_context.tcc"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/dynamic_meta_index_list.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"

#include "common/ICE.h"

#include "util/compose.h"
#include "util/dereference.h"
#include "util/member_select.h"
#include "util/stacktrace.h"

#if ENABLE_STACKTRACE
#include <iterator>
#endif

//=============================================================================
namespace HAC {
namespace entity {
//=============================================================================
#include "util/using_ostream.h"
using std::transform;
using std::copy;
using ADS::unary_compose;
using util::member_select;
using util::member_select_ref;
using util::dereference;

//=============================================================================
/**
	This collects all references that *may* be referenced
	by an (nonmeta) instance reference, conservatively.
	Private implementation to this module.
	Overloaded and specialized using tag-inheritance.  
	Code ripped from simple_nonmeta_instance_reference::lookup_may_ref...
	\param reference_type may be a simple nonmeta value reference 
		to a data type or a physical simple nonmeta value reference.  
	\param sm top-level global state_manager
	\param fp top-level or local footprint.
	\param ff process footprint frame, or NULL (top-level)
 */
template <class reference_type>
good_bool
__nonmeta_instance_lookup_may_reference_indices_impl(
		const reference_type& r, 
		const global_entry_context& c, 
		vector<size_t>& indices, physical_instance_tag) {
	typedef	typename reference_type::traits_type	traits_type;
	typedef	typename traits_type::tag_type		Tag;
	typedef	typename traits_type::instance_collection_generic_type
				instance_collection_generic_type;
	STACKTRACE_VERBOSE;
	const footprint_frame* const ff = c.get_footprint_frame();
//	if (ff) INVARIANT(ff->_footprint == c.fpf);
	const footprint* local_fp =
		(ff ? ff->_footprint : c.get_top_footprint_ptr());
	const never_ptr<const nonmeta_index_list> r_ind(r.get_indices());
	const count_ptr<dynamic_meta_index_list>
		mil(r_ind ? r_ind->make_meta_index_list() :
			count_ptr<dynamic_meta_index_list>(NULL));
	if (r_ind && !mil) {
		STACKTRACE_INDENT_PRINT("nonmeta indices" << endl);
		// there was at least one non-meta index
		// grab all collection aliases conservatively
		const unroll_context dummy(local_fp, c.get_top_footprint_ptr());
		const never_ptr<instance_collection_generic_type>
			ic(dummy.lookup_instance_collection(
				*r.get_inst_base_subtype())
				.template is_a<instance_collection_generic_type>());
		NEVER_NULL(ic);
		typedef	typename instance_collection_generic_type::const_instance_alias_info_ptr_type
					alias_ptr_type;
		typedef	vector<alias_ptr_type>	alias_list_type;
		typedef	typename traits_type::instance_alias_info_type
						instance_alias_info_type;
		typedef	typename alias_list_type::const_iterator
					const_iterator;
		typedef	typename const_iterator::value_type
						instance_alias_info_ptr_type;
		alias_list_type aliases;
		ic->get_all_aliases(aliases);
		indices.reserve(aliases.size());	// upper bound
		// translate to global_indices
		const const_iterator i(aliases.begin()), e(aliases.end());
#if 0
		STACKTRACE_INDENT_PRINT("local indices = ");
		copy(i, e, std::ostream_iterator<size_t>(cerr, " "));
		cerr << endl;
#endif
		if (ff) {
			STACKTRACE_INDENT_PRINT("footprint-framed" << endl);
			// need to translate local to global
#if 0
			const footprint_frame_map_type&
				ffm(ff->template get_frame_map<Tag>());
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT_PRINT("frame-map = ");
			copy(ffm.begin(), ffm.end(), 
				std::ostream_iterator<size_t>(cerr, " "));
			cerr << endl;
#endif
			for ( ; i!=e; ++i) {
				// NOTE: 1-indexed to 0-indexed
				const size_t j = (*i)->instance_index;
				STACKTRACE_INDENT_PRINT("j = " << j << endl);
				indices.push_back(ffm[j-1]);
			}
#else
			transform(i, e, back_inserter(indices),
				unary_compose(
				footprint_frame_transformer(*ff, Tag()), 
				unary_compose(
					member_select_ref(
						&instance_alias_info_type::instance_index),
					dereference<instance_alias_info_ptr_type>()
				)
				));
#endif
#if ENABLE_STACKTRACE
			STACKTRACE_INDENT_PRINT("global indices = ");
			copy(indices.begin(), indices.end(), 
				std::ostream_iterator<size_t>(cerr, " "));
			cerr << endl;
#endif
		} else {
			STACKTRACE_INDENT_PRINT("top-level" << endl);
			// local indices -1 == global indices
			// copy(i, e, back_inserter(indices));	// WRONG
#if 0
			for ( ; i!=e; ++i)
				indices.push_back((*i)->instance_index);
#else
			transform(i, e, back_inserter(indices), 
				unary_compose(
					member_select_ref(
						&instance_alias_info_type::instance_index),
					dereference<instance_alias_info_ptr_type>()
				)
			);
#endif
		}
		return good_bool(true);
	} else {
		STACKTRACE_INDENT_PRINT("constant indices" << endl);
		// should already be resolved to constants (or NULL)
		// construct an auxiliary meta-instance reference
		// to resolve the reference.  
		typedef	simple_meta_instance_reference<Tag>
				meta_reference_type;
		const meta_reference_type cr(r.get_inst_base_subtype(), mil);
		// the call only results in local indices!
		if (cr.lookup_globally_allocated_indices(
				*c.get_state_manager(), 
				*local_fp, indices).good) {
			if (ff) {
				// apply transformation if not top-level
				transform(indices.begin(), indices.end(), 
					indices.begin(), 
					footprint_frame_transformer(*ff, Tag()));
			}
			return good_bool(true);
		}
		else return good_bool(false);
	}
	return good_bool(false);	// unreachable
}	// end function __nonmeta_instance_lookup_may_reference_indices_impl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Meta-values are not aliasable, value-only semantics.  
	We're done! (no-op).
 */
template <class Tag>
good_bool
__nonmeta_instance_lookup_may_reference_indices_impl(
		const simple_nonmeta_value_reference<Tag>&, 
		const global_entry_context&, 
		vector<size_t>& indices, parameter_value_tag) {
	// no-op!
	return good_bool(true);
}

//-----------------------------------------------------------------------------
/**
	Looks up the exact run-time reference.
	\return globally allocated index, 0 to indicate failure.  
 */
template <class reference_type>
size_t
__nonmeta_instance_global_lookup_impl(
		const reference_type& r,
		const nonmeta_context_base& c, 
		physical_instance_tag) {
	return c.lookup_nonmeta_reference_global_index(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Should never be called, meta-parameters are not instances.  
 */
template <class Tag>
size_t
__nonmeta_instance_global_lookup_impl(
		const simple_nonmeta_value_reference<Tag>&,
		const nonmeta_context_base&, 
		parameter_value_tag) {
	ICE_NEVER_CALL(cerr);
	return 0;
}

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

#endif	// __HAC_OBJECT_REF_NONMETA_REF_IMPLEMENTATION_TCC__

