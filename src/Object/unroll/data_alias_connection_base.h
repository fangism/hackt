/**
	\file "Object/unroll/data_alias_connection_base.h"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: data_alias_connection_base.h,v 1.1.2.1 2005/07/22 21:34:27 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_DATA_ALIAS_CONNECTION_BASE_H__
#define	__OBJECT_UNROLL_DATA_ALIAS_CONNECTION_BASE_H__

#include "Object/unroll/aliases_connection_base.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	Pointless class, for the sake of classifying data subtype aliases.  
	Just another abstract base class in the hierarchy.  
 */
class data_alias_connection_base : public aliases_connection_base {
	typedef	data_alias_connection_base	this_type;
protected:
	typedef	aliases_connection_base		parent_type;
	typedef	parent_type::generic_inst_ptr_type
						generic_inst_ptr_type;

protected:
	data_alias_connection_base() : parent_type() { }

public:
virtual	~data_alias_connection_base() { }

};	// end class data_aliases_connection_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_UNROLL_DATA_ALIAS_CONNECTION_BASE_H__

