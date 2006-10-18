/**
	\file "Object/unroll/instantiation_statement_type_ref_default.h"
	Contains definition of nested, specialized class_traits types.  
	This file came from "Object/art_object_inst_stmt_type_ref_default.h"
		in a previous life.  
	$Id: instantiation_statement_type_ref_default.h,v 1.10 2006/10/18 05:33:02 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TYPE_REF_DEFAULT_H__
#define	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TYPE_REF_DEFAULT_H__

#include <iostream>
#include "Object/traits/class_traits.h"
#if ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
#include "Object/expr/dynamic_param_expr_list.h"
#endif
#include "Object/expr/const_param_expr_list.h"
#include "util/persistent_object_manager.h"
#include "Object/def/footprint.h"

namespace HAC {
namespace entity {
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
#if USE_INSTANCE_PLACEHOLDERS
	typedef	typename class_traits<Tag>::instance_placeholder_type
					instance_placeholder_type;
#endif
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	typename class_traits<Tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
#if ALWAYS_USE_DYNAMIC_PARAM_EXPR_LIST
	// TODO: use typedef
	typedef	count_ptr<const dynamic_param_expr_list>
						const_relaxed_args_type;
#else
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
#endif
	// typedef	count_ptr<param_expr_list>	relaxed_args_type;
	typedef	count_ptr<const const_param_expr_list>
					instance_relaxed_actuals_type;
protected:
	/**
		Note: this may be a partial or relaxed type, 
		missing relaxed actuals.  
	 */
	type_ref_ptr_type				type;

	/**
		General user-defined types may have 
		relaxed template actuals.  
		These actuals may be different for each alias in the
		collection.  
		The complete type is the composition of the
		partial type and the relaxed arguments.  
		Invariant: either the 'type' field is strict
			(already includes relaxed actuals)
			or this field contains the relaxed actuals, 
			but NEVER BOTH.
	 */
	const_relaxed_args_type				relaxed_args;

protected:
	instantiation_statement_type_ref_default() : type(NULL) { }

	explicit
	instantiation_statement_type_ref_default(
		const type_ref_ptr_type& t) :
		type(t), relaxed_args(NULL) { }

	instantiation_statement_type_ref_default(
		const type_ref_ptr_type& t, const const_relaxed_args_type& a) :
		type(t), relaxed_args(a) { }

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

	const_relaxed_args_type
	get_relaxed_actuals(void) const {
		return relaxed_args;
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
#if 1
		if (t.is_strict() && !t.unroll_definition_footprint(top).good) {
			return good_bool(false);
		}
#endif
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
#if 1
		if (t.is_strict() && !t.unroll_definition_footprint(top).good) {
			return good_bool(false);
		}
#endif
		return v.check_established_type(t);
	}

	static
	good_bool
	instantiate_indices_with_actuals(instance_collection_generic_type& v, 
			const const_range_list& crl, 
			const unroll_context& c, 
			const instance_relaxed_actuals_type& a) {
		return v.instantiate_indices(crl, a, c);
	}

	void
	collect_transient_info_base(persistent_object_manager& m) const {
		type->collect_transient_info(m);
		if (relaxed_args)
			relaxed_args->collect_transient_info(m);
	}

	void
	write_object_base(const persistent_object_manager& m,
			ostream& o) const {
		m.write_pointer(o, type);
		m.write_pointer(o, relaxed_args);
	}

	void
	load_object_base(const persistent_object_manager& m,
			istream& i) {
		m.read_pointer(i, type);
		m.read_pointer(i, relaxed_args);
	}

};      // end class instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_TYPE_REF_DEFAULT_H__

