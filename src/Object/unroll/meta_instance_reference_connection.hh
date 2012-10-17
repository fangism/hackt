/**
	\file "Object/unroll/meta_instance_reference_connection.hh"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: meta_instance_reference_connection.hh,v 1.5 2006/02/21 04:48:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_H__
#define	__HAC_OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_H__

#include "Object/unroll/instance_management_base.hh"
#include "util/memory/pointer_classes_fwd.hh"

namespace HAC {
namespace entity {
class meta_instance_reference_base;
using util::memory::count_ptr;

//=============================================================================
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic instance references.  
 */
class meta_instance_reference_connection : public instance_management_base {
protected:
	typedef	meta_instance_reference_base		generic_instance_type;
	typedef	count_ptr<const generic_instance_type>	generic_inst_ptr_type;

protected:
	meta_instance_reference_connection();

public:
virtual	~meta_instance_reference_connection();

	/**
		Temporary: keep the old interface of inserting generic types.
	 */
virtual	void
	append_meta_instance_reference(const generic_inst_ptr_type& i) = 0;

};	// end class meta_instance_reference_connection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_H__

