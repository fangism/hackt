/**
	\file "Object/unroll/port_connection.hh"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: port_connection.hh,v 1.7 2006/02/21 04:48:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_PORT_CONNECTION_H__
#define	__HAC_OBJECT_UNROLL_PORT_CONNECTION_H__

#include "Object/traits/class_traits_fwd.hh"
#include "Object/unroll/port_connection_base.hh"

#define	PORT_CONNECTION_TEMPLATE_SIGNATURE		template <class Tag>
#define	PORT_CONNECTION_CLASS				port_connection<Tag>

namespace HAC {
namespace entity {

//=============================================================================
/**
	Port-style instance connection, e.g. x(y,z,w);
	Which is short-hand for x.first = y, x.second = z, ...;
	List items are interpreted as connecting to the ports
	of the instance.  
	Sub-type into process/data/channel?
 */
PORT_CONNECTION_TEMPLATE_SIGNATURE
class port_connection : public port_connection_base {
	typedef	PORT_CONNECTION_CLASS			this_type;
public:
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef	port_connection_base			parent_type;

	typedef	typename traits_type::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	/**
		The ported instance referenced must be a a scalar reference.  
	 */
	typedef	count_ptr<const simple_meta_instance_reference_type>
							ported_inst_ptr_type;
	/** should be reference to a simple instance, may be indexed.  */
	ported_inst_ptr_type				ported_inst;
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
	dump(ostream&, const expr_dump_context&) const;

	good_bool
	unroll(const unroll_context& ) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class port_connection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_PORT_CONNECTION_H__

