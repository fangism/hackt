/**
	\file "art_object_connect.h"
	Declarations for classes related to connection of physical
	entites. 
	$Id: art_object_connect.h,v 1.13 2004/12/07 02:22:07 fang Exp $
 */

#ifndef	__ART_OBJECT_CONNECT_H__
#define	__ART_OBJECT_CONNECT_H__

#include "art_object_base.h"
#include "art_object_instance_management_base.h"
#include "memory/pointer_classes.h"

namespace ART {
namespace entity {

USING_LIST
using std::ostream;
using namespace util::memory;	// for experimental pointer classes

//=============================================================================
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic instance references.  
 */
class instance_reference_connection : public object, 
		public instance_management_base {
protected:
	typedef	list<count_ptr<const instance_reference_base> >	inst_list_type;
protected:
	// items may be singular or collective instances references.  
	inst_list_type						inst_list;
public:
	instance_reference_connection();
virtual	~instance_reference_connection();

// non-virtual
virtual	void	append_instance_reference(
			count_ptr<const instance_reference_base> i);
};	// end class instance_reference_connection

//-----------------------------------------------------------------------------
/**
	Alias-style instance connection, e.g. x = y = z;
	List items are interpreted as connecting to each other, 
	and thus having the same type and size.  
	Should be no need for sub-typing?
 */
class aliases_connection : public instance_reference_connection {
protected:
	// no additional fields
public:
	aliases_connection();
	~aliases_connection() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void	prepend_instance_reference(
			count_ptr<const instance_reference_base> i);

	void unroll(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class aliases_connection

//-----------------------------------------------------------------------------
/**
	Port-style instance connection, e.g. x(y,z,w);
	Which is short-hand for x.first = y, x.second = z, ...;
	List items are interpreted as connecting to the ports
	of the instance.  
	Sub-type into process/data/channel?
 */
class port_connection : public instance_reference_connection {
protected:
	/** should be reference to a simple instance, may be indexed.  */
	count_ptr<const simple_instance_reference>	inst;
private:
	port_connection();
public:
	/** later, accept complex_aggregate_instance_references? */
	port_connection(count_ptr<const simple_instance_reference> i);
	~port_connection();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	void	append_instance_reference(
			count_ptr<const instance_reference_base> i);

	void unroll(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class port_connection

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
}	// end namespace ART

#endif	// __ART_OBJECT_CONNECT_H__

