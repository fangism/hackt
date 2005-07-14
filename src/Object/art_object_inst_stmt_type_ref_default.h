/**
	\file "Object/art_object_inst_stmt_type_ref_default.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_type_ref_default.h,v 1.1.2.8.2.3 2005/07/14 03:15:34 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_TYPE_REF_DEFAULT_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_TYPE_REF_DEFAULT_H__

#include <iostream>
#include "Object/traits/class_traits.h"
#include "Object/art_object_type_ref.h"
#include "Object/expr/const_param_expr_list.h"
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
class param_expr_list;
class const_param_expr_list;
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
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
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

	type_ref_ptr_type
	get_resolved_type(void) const {
		const type_ref_ptr_type ret(type->unroll_resolve());
		if (!ret) {
			type->what(cerr << "ERROR: unable to resolve ") <<
				" during unroll." << endl;
		}
#if 0
		else {
			// enable later when things are more stable
			// cache the equivalent resolved type
			const_cast<this_type*>(this)->type = ret;
			// NOTE: caching won't work if the type
			// is loop-dependent!!!
			// need smarter handling...
		}
#endif
		return ret;
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
	unroll_type_reference(void) const {
#if 0
		if (relaxed_args) {
			// clumsy but effective, make a temporary deep-copy
			const type_ref_ptr_type
			merged_type(type->merge_relaxed_actuals(relaxed_args));
			return merged_type->unroll_resolve();
		} else	return type->unroll_resolve();
#else
		return type->unroll_resolve();
#endif
	}

	static
	void
	commit_type_first_time(instance_collection_generic_type& v, 
			const type_ref_ptr_type& t) {
		v.establish_collection_type(t);
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
			const type_ref_ptr_type& t) {
		// note: automatic conversion from bad_bool to good_bool :)
		return v.commit_type(t);
	}

	static
	good_bool
	instantiate_indices_with_actuals(instance_collection_generic_type& v, 
			const const_range_list& crl, 
			const instance_relaxed_actuals_type& a) {
		return v.instantiate_indices(crl, a);
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
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_TYPE_REF_DEFAULT_H__

