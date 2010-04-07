/**
	\file "Object/unroll/instantiation_statement_type_ref_default.h"
	Contains definition of nested, specialized class_traits types.  
	This file came from "Object/art_object_inst_stmt_type_ref_default.h"
		in a previous life.  
	$Id: instantiation_statement_type_ref_default.h,v 1.17 2010/04/07 00:12:58 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TYPE_REF_DEFAULT_H__
#define	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TYPE_REF_DEFAULT_H__

#include <iostream>
#include "Object/traits/class_traits.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/persistent_object_manager.h"

namespace HAC {
namespace entity {
class footprint;
class param_expr_list;
using util::persistent_object_manager;
#include "util/using_ostream.h"

//=============================================================================
/**
	The default way of handling user-defined types
	in instantiation statements and instance collections.  
	All methods and members are declared protected;
		they're intended for children classes only.  
	\param Tag the meta-type tag.  
	TODO: cache the partial results of type resolution, 
		and re-use them in subsequent resolutions.  
 */
template <class Tag>
class instantiation_statement_type_ref_default {
	typedef	instantiation_statement_type_ref_default<Tag>
							this_type;
public:
	typedef	typename class_traits<Tag>::type_ref_ptr_type
							type_ref_ptr_type;
	typedef	typename class_traits<Tag>::instance_placeholder_type
					instance_placeholder_type;
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	typename class_traits<Tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
// relaxed template parameters are now managed by
// "Object/unroll/template_type_completion.h"
	// TODO: use typedef outside of class for consistency
	typedef	count_ptr<const dynamic_param_expr_list>
						const_relaxed_args_type;
	// typedef	count_ptr<param_expr_list>	relaxed_args_type;
	typedef	count_ptr<const const_param_expr_list>
					instance_relaxed_actuals_type;
protected:
	/**
		Note: this may be a partial or relaxed type, 
		missing relaxed actuals.  
	 */
	type_ref_ptr_type				type;

protected:
	instantiation_statement_type_ref_default() : type(NULL) { }

	explicit
	instantiation_statement_type_ref_default(
		const type_ref_ptr_type& t) :
		type(t) { }

// shouldn't need this definition at all in sibling policies, 
// after moving relaxed template paramters to template_type_completion class.
	instantiation_statement_type_ref_default(
		const type_ref_ptr_type& t, const const_relaxed_args_type& a) :
		type(t) { }

	// default destructor
	~instantiation_statement_type_ref_default() { }

	type_ref_ptr_type
	get_type(void) const { return type; }

	instance_collection_parameter_type
	get_canonical_type(const unroll_context& c) const {
		const type_ref_ptr_type t(type->unroll_resolve(c));
		if (!t) {
			type->what(cerr << "ERROR: unable to resolve ") <<
				" during unroll." << endl;
			return instance_collection_parameter_type();
		}
		return t->make_canonical_type();
	}

	/**
		2005-07-09: changed mind, NOT FUSING
		TODO: combine type and relaxed_args.
		relaxed_args is still allowed to be NULL.  
		The resulting type is allowed to be relaxed, incomplete. 
		TODO: rename to unroll_fused_type_reference
	 */
	type_ref_ptr_type
	unroll_type_reference(const unroll_context& c) const {
#if 0
		if (relaxed_args) {
			// clumsy but effective, make a temporary deep-copy
			const type_ref_ptr_type
			merged_type(type->merge_relaxed_actuals(relaxed_args));
			return merged_type->unroll_resolve();
		} else	return type->unroll_resolve();
#else
		return type->unroll_resolve(c);
#endif
	}

	static
	good_bool
	commit_type_first_time(instance_collection_generic_type& v, 
			const instance_collection_parameter_type& t, 
			const footprint& top) {
		if (t.is_strict() &&
			!t.create_definition_footprint(top).good) {
			return good_bool(false);
		}
		v.establish_collection_type(t);
		return good_bool(true);
	}

	/**
		TODO: make sure scalar has complete type, 
		arrays are allowed to be relaxed.  
		TODO: Rename this check_type (collectible)
			and make it const.  
	 */
	static
	good_bool
	commit_type_check(instance_collection_generic_type& v,
			const instance_collection_parameter_type& t, 
			const footprint& top) {
		// note: automatic conversion from bad_bool to good_bool :)
		if (t.is_strict() &&
			!t.create_definition_footprint(top).good) {
			return good_bool(false);
		}
		return v.check_established_type(t);
	}

	static
	good_bool
	instantiate_indices_with_actuals(instance_collection_generic_type& v, 
			const const_range_list& crl, 
			const unroll_context& c) {
		return v.instantiate_indices(crl, c);
	}

	void
	collect_transient_info_base(persistent_object_manager& m) const {
		type->collect_transient_info(m);
	}

	void
	write_object_base(const persistent_object_manager& m,
			ostream& o) const {
		m.write_pointer(o, type);
	}

	void
	load_object_base(const persistent_object_manager& m,
			istream& i) {
		m.read_pointer(i, type);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TYPE_REF_DEFAULT_H__

