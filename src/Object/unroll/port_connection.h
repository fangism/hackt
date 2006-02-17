/**
	\file "Object/unroll/port_connection.h"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: port_connection.h,v 1.6.12.1 2006/02/17 05:07:52 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_PORT_CONNECTION_H__
#define	__HAC_OBJECT_UNROLL_PORT_CONNECTION_H__

#include "Object/devel_switches.h"

#if SUBTYPE_PORT_CONNECTION
#include "Object/traits/class_traits_fwd.h"
#include "Object/unroll/port_connection_base.h"
#define	PORT_CONNECTION_TEMPLATE_SIGNATURE		template <class Tag>
#define	PORT_CONNECTION_CLASS				port_connection<Tag>
#else
#include <vector>
#include "util/memory/count_ptr.h"
#include "Object/unroll/meta_instance_reference_connection.h"
#define	PORT_CONNECTION_TEMPLATE_SIGNATURE
#define	PORT_CONNECTION_CLASS				port_connection
#endif

namespace HAC {
namespace entity {
#if !SUBTYPE_PORT_CONNECTION
class simple_meta_instance_reference_base;
using std::vector;
class unroll_context;
#endif

//=============================================================================
/**
	Port-style instance connection, e.g. x(y,z,w);
	Which is short-hand for x.first = y, x.second = z, ...;
	List items are interpreted as connecting to the ports
	of the instance.  
	Sub-type into process/data/channel?
 */
PORT_CONNECTION_TEMPLATE_SIGNATURE
class port_connection :
#if SUBTYPE_PORT_CONNECTION
		public port_connection_base
#else
		public meta_instance_reference_connection
#endif
{
	typedef	PORT_CONNECTION_CLASS			this_type;
protected:
#if SUBTYPE_PORT_CONNECTION
	typedef	port_connection_base			parent_type;
	typedef	class_traits<Tag>			traits_type;
#else
	typedef	meta_instance_reference_connection	parent_type;
	typedef	parent_type::generic_inst_ptr_type	generic_inst_ptr_type;
	typedef	std::vector<generic_inst_ptr_type>	inst_list_type;
	inst_list_type					inst_list;
#endif

#if SUBTYPE_PORT_CONNECTION
	typedef	typename traits_type::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	typedef	count_ptr<const simple_meta_instance_reference_type>
							ported_inst_ptr_type;
#else
	/**
		The ported instance referenced must be a a scalar reference.  
	 */
	typedef	count_ptr<const simple_meta_instance_reference_base>
							ported_inst_ptr_type;
#endif
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

#if !SUBTYPE_PORT_CONNECTION
	void
	reserve(const size_t);

	void
	append_meta_instance_reference(const generic_inst_ptr_type& i);
#endif

	good_bool
	unroll(const unroll_context& ) const;

#if !UNIFY_UNROLL_PASS
	UNROLL_META_CONNECT_PROTO;
#endif

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class port_connection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_PORT_CONNECTION_H__

