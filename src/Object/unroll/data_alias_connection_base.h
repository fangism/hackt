/**
	\file "Object/unroll/data_alias_connection_base.h"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: data_alias_connection_base.h,v 1.4 2006/01/22 18:20:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_DATA_ALIAS_CONNECTION_BASE_H__
#define	__HAC_OBJECT_UNROLL_DATA_ALIAS_CONNECTION_BASE_H__

#include "Object/unroll/aliases_connection_base.h"

namespace HAC {
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_DATA_ALIAS_CONNECTION_BASE_H__

