/**
	\file "Object/unroll/port_connection_base.h"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: port_connection_base.h,v 1.1.4.2 2006/02/19 03:53:18 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_PORT_CONNECTION_BASE_H__
#define	__HAC_OBJECT_UNROLL_PORT_CONNECTION_BASE_H__

#include <vector>
#include "Object/unroll/meta_instance_reference_connection.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class unroll_context;
using std::istream;
using util::persistent_object_manager;

//=============================================================================
/**
	Common base class for type-specific port connections.  
	All members are protected because never supposed to 
	manipulate this type directly.  
 */
class port_connection_base : public meta_instance_reference_connection {
	typedef	port_connection_base			this_type;
protected:
	typedef	meta_instance_reference_connection	parent_type;
	typedef	parent_type::generic_inst_ptr_type	generic_inst_ptr_type;
	typedef	std::vector<generic_inst_ptr_type>	inst_list_type;
	inst_list_type					inst_list;
protected:
	port_connection_base();

public:
virtual	~port_connection_base();

#if 0
	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;
#endif
protected:
	ostream&
	dump_ports(ostream&, const expr_dump_context&) const;

public:
	void
	reserve(const size_t);

	void
	append_meta_instance_reference(const generic_inst_ptr_type&);

#if 0
	good_bool
	unroll(const unroll_context& ) const;

	UNROLL_META_CONNECT_PROTO;
#endif

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class port_connection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_PORT_CONNECTION_BASE_H__

