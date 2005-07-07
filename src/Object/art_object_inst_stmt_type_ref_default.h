/**
	\file "Object/art_object_inst_stmt_type_ref_default.h"
	Contains definition of nested, specialized class_traits types.  
	$Id: art_object_inst_stmt_type_ref_default.h,v 1.1.2.4 2005/07/07 06:02:21 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_TYPE_REF_DEFAULT_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_TYPE_REF_DEFAULT_H__

#include "Object/art_object_classification_details.h"
#include "Object/art_object_type_ref.h"
#include "Object/expr/param_expr_list.h"
#include "util/persistent_object_manager.h"

namespace ART {
namespace entity {
class param_expr_list;
using util::persistent_object_manager;

//=============================================================================
/**
	The default way of handling user-defined types
	in instantiation statements and instance collections.  
	All methods and members are declared protected;
		they're intended for children classes only.  
	\param Tag the meta-type tag.  
 */
template <class Tag>
class instantiation_statement_type_ref_default {
public:
	typedef	typename class_traits<Tag>::type_ref_ptr_type
							type_ref_ptr_type;
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	count_ptr<const param_expr_list>	const_relaxed_args_type;
	// typedef	count_ptr<param_expr_list>	relaxed_args_type;
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

	count_ptr<const fundamental_type_reference>
	get_type(void) const { return type; }

	const_relaxed_args_type
	get_relaxed_actuals(void) const {
		return relaxed_args;
	}

	/**
		TODO: combine type and relaxed_args.
		relaxed_args is still allowed to be NULL.  
		The resulting type is allowed to be relaxed, incomplete. 
	 */
	type_ref_ptr_type
	unroll_type_reference(unroll_context& c) const {
#if 1
		if (relaxed_args) {
			// clumsy but effective, make a temporary deep-copy
			const type_ref_ptr_type
			merged_type(type->merge_relaxed_actuals(relaxed_args));
			return merged_type->unroll_resolve(c);
		} else	return type->unroll_resolve(c);
#else
		// doesn't require relaxed actuals
		return type->unroll_resolve(c);
#endif
	}

	/**
		TODO: make sure scalar has complete type, 
		arrays are allowed to be relaxed.  
	 */
	static
	good_bool
	commit_type_check(instance_collection_generic_type& v,
			const type_ref_ptr_type& t) {
		return v.commit_type(t);
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

