/**
	\file "Object/art_object_inst_ref_data.h"
	Classes for datatype instance references (built-in and user-defined).
	$Id: art_object_inst_ref_data.h,v 1.8.4.1 2005/07/05 07:59:42 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_REF_DATA_H__
#define	__OBJECT_ART_OBJECT_INST_REF_DATA_H__

#include "Object/art_object_inst_ref_base.h"
// #include "Object/art_object_inst_ref.h"
// might as well include this here because data_reference<Tag>::interface_type
// is going to be int_expr or bool_expr from the base class header file.  
// #include "Object/art_object_data_expr_base.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	A reference to a simple instance of datatype.  
	Consider sub-typing into user-defined and built-in, 
	making this an abstract base.
 */
class simple_datatype_meta_instance_reference_base :
		public simple_meta_instance_reference_base {
private:
	typedef simple_meta_instance_reference_base               parent_type;
protected:
//      excl_ptr<meta_index_list>                   array_indices;  // inherited

protected:
	simple_datatype_meta_instance_reference_base();

	explicit
	simple_datatype_meta_instance_reference_base(const instantiation_state& s);

public:
virtual ~simple_datatype_meta_instance_reference_base();

virtual ostream&
	what(ostream& o) const = 0;

//      ostream& dump(ostream& o) const;

virtual never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

private:
virtual excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};      // end class simple_datatype_meta_instance_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_REF_DATA_H__

