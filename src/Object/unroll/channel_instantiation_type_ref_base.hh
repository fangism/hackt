/**
	\file "Object/unroll/channel_instantiation_type_ref.hh"
	Contains definition of nested, specialized class_traits types.  
	This file came from "Object/art_object_inst_stmt_type_ref_default.h"
		in a previous life.  
	$Id: channel_instantiation_type_ref_base.hh,v 1.5 2008/11/12 03:00:28 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_CHANNEL_INSTANTIATION_TYPE_REF_H__
#define	__HAC_OBJECT_UNROLL_CHANNEL_INSTANTIATION_TYPE_REF_H__

#include <iostream>
#include "Object/traits/class_traits.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/devel_switches.hh"
#include "util/persistent_object_manager.hh"

namespace HAC {
namespace entity {
class footprint;
class param_expr_list;
using util::persistent_object_manager;
#include "util/using_ostream.hh"

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
class channel_instantiation_type_ref {
	typedef	channel_instantiation_type_ref<Tag>
							this_type;
public:
	typedef	typename class_traits<Tag>::type_ref_type
							type_ref_type;
	typedef	typename class_traits<Tag>::type_ref_ptr_type
							type_ref_ptr_type;
	typedef	typename class_traits<Tag>::instance_placeholder_type
					instance_placeholder_type;
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef	typename class_traits<Tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
protected:
	/**
		Note: this may be a partial or relaxed type, 
		missing relaxed actuals.  
	 */
	type_ref_ptr_type				type;

protected:
	channel_instantiation_type_ref() : type(NULL) { }

	explicit
	channel_instantiation_type_ref(
		const type_ref_ptr_type& t) :
		type(t) { }

	// default destructor
	~channel_instantiation_type_ref() { }

	type_ref_ptr_type
	get_type(void) const { return type; }

	instance_collection_parameter_type
	get_canonical_type(const unroll_context& c) const {
		const type_ref_ptr_type t(type->unroll_resolve(c)
			.template is_a<const type_ref_type>());
		if (!t) {
			type->what(cerr << "ERROR: unable to resolve ") <<
				" during unroll." << endl;
			return instance_collection_parameter_type();
		}
		return t->make_canonical_type();
		// return t->make_canonical_type(c);
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
#if 1
		return type->unroll_resolve_copy(c, type);
#else
		return type;
#endif
	}

	static
	good_bool
	commit_type_first_time(instance_collection_generic_type& v, 
			const instance_collection_parameter_type& t, 
			const footprint& /* top */) {
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
			const footprint& /* top */) {
		// note: automatic conversion from bad_bool to good_bool :)
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

#endif	// __HAC_OBJECT_UNROLL_CHANNEL_INSTANTIATION_TYPE_REF_H__

