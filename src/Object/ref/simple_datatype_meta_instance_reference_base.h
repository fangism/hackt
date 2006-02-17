/**
	\file "Object/ref/simple_datatype_meta_instance_reference_base.h"
	Classes for datatype instance references (built-in and user-defined).
	This file was reincarnated from "Object/art_object_inst_ref_data.h".
	$Id: simple_datatype_meta_instance_reference_base.h,v 1.4.18.1 2006/02/17 05:07:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_DATATYPE_META_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_SIMPLE_DATATYPE_META_INSTANCE_REFERENCE_BASE_H__

#include "Object/devel_switches.h"
#include "Object/ref/simple_meta_instance_reference_base.h"

namespace HAC {
namespace entity {
#if DECOUPLE_INSTANCE_REFERENCE_HIERARCHY
class instance_collection_base;
class aliases_connection_base;
#endif
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

#if ENABLE_STATIC_COMPILE_CHECKS
	explicit
	simple_datatype_meta_instance_reference_base(
		const instantiation_state& s);
#endif

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
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_DATATYPE_META_INSTANCE_REFERENCE_BASE_H__

