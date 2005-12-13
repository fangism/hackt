/**
	\file "Object/unroll/meta_instance_reference_connection.h"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: meta_instance_reference_connection.h,v 1.3 2005/12/13 04:15:43 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_H__
#define	__OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_H__

#include "Object/unroll/instance_management_base.h"
#include "util/memory/pointer_classes_fwd.h"

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

virtual	UNROLL_META_CONNECT_PROTO = 0;
};	// end class meta_instance_reference_connection

//-----------------------------------------------------------------------------
#if 0
NOT READY TO UNVEIL
/**
	Wrapper reference to a loop or conditional namespace.  
 */
class dynamic_connection_assignment : public connection_assignment_base {
protected:
	/** the dynamic scope, a loop or conditional */
	never_ptr<const scopespace>			dscope;
	// may be really static if bounds and conditions can be 
	// resolved as static constants...
public:
	dynamic_connection_assignment(never_ptr<const scopespace> s);
	~dynamic_connection_assignment() { }

#if 0
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
#endif
};	// end class dynamic_connection_assignment
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_UNROLL_META_INSTANCE_REFERENCE_CONNECTION_H__

