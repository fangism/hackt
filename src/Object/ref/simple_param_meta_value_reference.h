/**
	\file "Object/ref/simple_param_meta_value_reference.h"
	Base class family for instance references in ART.  
	This file was "Object/art_object_inst_ref_base.h"
		in a previous life.  
	$Id: simple_param_meta_value_reference.h,v 1.1.2.1 2005/07/22 04:54:39 fang Exp $
 */

#ifndef	__OBJECT_REF_SIMPLE_PARAM_META_VALUE_REFERENCE_H__
#define	__OBJECT_REF_SIMPLE_PARAM_META_VALUE_REFERENCE_H__

#include "Object/ref/simple_meta_instance_reference_base.h"
#include "Object/art_object_util_types.h"

namespace ART {
namespace entity {
class param_value_collection;

//=============================================================================
/**
	A reference to a simple instance of parameter.  
	Abstract base class.  
 */
class simple_param_meta_value_reference :
		public simple_meta_instance_reference_base {
protected:
	typedef	simple_meta_instance_reference_base	parent_type;
//	excl_ptr<meta_index_list>		array_indices;	// inherited

protected:
	simple_param_meta_value_reference();
public:
	explicit
	simple_param_meta_value_reference(const instantiation_state& st);

virtual	~simple_param_meta_value_reference() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

virtual	never_ptr<const param_value_collection>
	get_param_inst_base(void) const = 0;

	// consider moving these functions into meta_instance_reference_base
	//	where array_indices are inherited from.  
	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_loop_independent(void) const;

	bool
	is_unconditional(void) const;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

};	// end class simple_param_meta_value_reference

//=============================================================================
// classes pint_meta_instance_reference and pbool_meta_instance_reference
//	are in "art_object_expr_param_ref.*"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_REF_SIMPLE_PARAM_META_VALUE_REFERENCE_H__

