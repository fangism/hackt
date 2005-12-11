/**
	\file "Object/unroll/aliases_connection_base.h"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: aliases_connection_base.h,v 1.2.32.1 2005/12/11 00:45:54 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_ALIASES_CONNECTION_BASE_H__
#define	__OBJECT_UNROLL_ALIASES_CONNECTION_BASE_H__

#include "Object/unroll/meta_instance_reference_connection.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Alias-style instance connection, e.g. x = y = z;
	List items are interpreted as connecting to each other, 
	and thus having the same type and size.  
	Should be no need for sub-typing?
 */
class aliases_connection_base : public meta_instance_reference_connection {
	typedef	aliases_connection_base			this_type;
protected:
	typedef	meta_instance_reference_connection	parent_type;
	typedef	parent_type::generic_inst_ptr_type	generic_inst_ptr_type;
protected:
	// no additional fields
	aliases_connection_base();

public:
virtual	~aliases_connection_base();

virtual	ostream&
	what(ostream& ) const = 0;

virtual	ostream&
	dump(ostream& ) const = 0;

virtual	void
	reserve(const size_t) = 0;

};	// end class aliases_connection_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_UNROLL_ALIASES_CONNECTION_BASE_H__

