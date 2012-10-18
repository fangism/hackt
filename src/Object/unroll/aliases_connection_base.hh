/**
	\file "Object/unroll/aliases_connection_base.hh"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: aliases_connection_base.hh,v 1.5 2006/01/30 07:42:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_ALIASES_CONNECTION_BASE_H__
#define	__HAC_OBJECT_UNROLL_ALIASES_CONNECTION_BASE_H__

#include "Object/unroll/meta_instance_reference_connection.hh"

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
	dump(ostream&, const expr_dump_context&) const = 0;

virtual	void
	reserve(const size_t) = 0;

};	// end class aliases_connection_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_ALIASES_CONNECTION_BASE_H__

