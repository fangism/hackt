/**
	\file "Object/unroll/port_connection.h"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: port_connection.h,v 1.1.2.1 2005/07/22 21:34:31 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_PORT_CONNECTION_H__
#define	__OBJECT_UNROLL_PORT_CONNECTION_H__

#include <vector>
#include "Object/unroll/meta_instance_reference_connection.h"
#include "util/memory/count_ptr.h"

namespace ART {
namespace entity {
class simple_meta_instance_reference_base;
using std::vector;
class unroll_context;

//=============================================================================
/**
	Port-style instance connection, e.g. x(y,z,w);
	Which is short-hand for x.first = y, x.second = z, ...;
	List items are interpreted as connecting to the ports
	of the instance.  
	Sub-type into process/data/channel?
 */
class port_connection : public meta_instance_reference_connection {
	typedef	port_connection				this_type;
protected:
	typedef	meta_instance_reference_connection	parent_type;
	typedef	parent_type::generic_inst_ptr_type	generic_inst_ptr_type;
	typedef	vector<generic_inst_ptr_type>		inst_list_type;
	/**
		The ported instance referenced must be a a scalar reference.  
	 */
	typedef	count_ptr<const simple_meta_instance_reference_base>
							ported_inst_ptr_type;
	/** should be reference to a simple instance, may be indexed.  */
	ported_inst_ptr_type				ported_inst;
	inst_list_type					inst_list;
private:
	port_connection();
public:
	/** later, accept complex_aggregate_meta_instance_references? */
	explicit
	port_connection(const ported_inst_ptr_type& i);

	~port_connection();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	reserve(const size_t);

	void
	append_meta_instance_reference(const generic_inst_ptr_type& i);

	good_bool
	unroll(unroll_context& ) const;

	UNROLL_META_CONNECT_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class port_connection

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_UNROLL_PORT_CONNECTION_H__

