/**
	\file "art_object_connect.h"
	Declarations for classes related to connection of physical
	entites. 
	$Id: art_object_connect.h,v 1.15.16.1.10.2 2005/02/18 03:25:14 fang Exp $
 */

#ifndef	__ART_OBJECT_CONNECT_H__
#define	__ART_OBJECT_CONNECT_H__

#include "art_object_base.h"
#include "art_object_instance_management_base.h"
#include "memory/pointer_classes.h"

#define	SUBTYPE_ALIASES_CONNECTION			0

namespace ART {
namespace entity {

USING_LIST
using std::ostream;
using namespace util::memory;	// for experimental pointer classes
class unroll_context;

//=============================================================================
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic instance references.  
 */
class instance_reference_connection : 
		public instance_management_base {
protected:
	typedef	instance_reference_base			generic_instance_type;
	typedef	count_ptr<const generic_instance_type>	generic_inst_ptr_type;
#if !SUBTYPE_ALIASES_CONNECTION
protected:
	typedef	list<generic_inst_ptr_type>		inst_list_type;
protected:
	// items may be singular or collective instances references.  
	inst_list_type					inst_list;
#endif
public:
	instance_reference_connection();

virtual	~instance_reference_connection();

	/**
		Temporary: keep the old interface of inserting generic types.
	 */
#if SUBTYPE_ALIASES_CONNECTION
virtual	void
	append_instance_reference(const generic_inst_ptr_type& i) = 0;
#else
virtual	void
	append_instance_reference(const generic_inst_ptr_type& i);
#endif
};	// end class instance_reference_connection

//-----------------------------------------------------------------------------
/**
	Alias-style instance connection, e.g. x = y = z;
	List items are interpreted as connecting to each other, 
	and thus having the same type and size.  
	Should be no need for sub-typing?
 */
class aliases_connection_base : public instance_reference_connection {
	typedef	aliases_connection_base			this_type;
protected:
	typedef	instance_reference_connection		parent_type;
	typedef	parent_type::generic_inst_ptr_type	generic_inst_ptr_type;
protected:
	// no additional fields
public:
	aliases_connection_base();

#if SUBTYPE_ALIASES_CONNECTION
virtual	~aliases_connection_base();
#else
	~aliases_connection_base();
#endif

	ostream&
	what(ostream& o) const;

#if !SUBTYPE_ALIASES_CONNECTION
	ostream&
	dump(ostream& o) const;

	void
	prepend_instance_reference(const generic_inst_ptr_type& i);

	void
	unroll(unroll_context& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
#endif
};	// end class aliases_connection_base

//-----------------------------------------------------------------------------
#if 0
class data_aliases_connection_base : public aliases_connection_base {
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-usable pattern for type-specific alias connection lists, 
	intended for leaf classes because methods are non-virtual.
 */
template <class InstRef, class Parent>
class aliases_connection : public Parent {
public:
	typedef	Parent				parent_type;
	typedef	InstRef				instance_reference_type;
	typedef	count_ptr<const InstRef>	inst_ref_ptr_type;
	typedef	list<inst_ref_ptr_type>		inst_list_type;
protected:
	inst_list_type				inst_list;
public:
	ostream&
	what(ostream& ) const;

	ostream&
	dump(ostream& ) const;

};
#endif

//-----------------------------------------------------------------------------
/**
	Port-style instance connection, e.g. x(y,z,w);
	Which is short-hand for x.first = y, x.second = z, ...;
	List items are interpreted as connecting to the ports
	of the instance.  
	Sub-type into process/data/channel?
 */
class port_connection : public instance_reference_connection {
	typedef	port_connection				this_type;
protected:
	typedef	instance_reference_connection		parent_type;
	typedef	parent_type::generic_inst_ptr_type	generic_inst_ptr_type;
	typedef	list<generic_inst_ptr_type>		inst_list_type;
	typedef	count_ptr<const simple_instance_reference>
							ported_inst_ptr_type;
	/** should be reference to a simple instance, may be indexed.  */
	ported_inst_ptr_type				ported_inst;
#if SUBTYPE_ALIASES_CONNECTION
	inst_list_type					inst_list;
#endif
private:
	port_connection();
public:
	/** later, accept complex_aggregate_instance_references? */
	explicit
	port_connection(const ported_inst_ptr_type& i);

	~port_connection();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	append_instance_reference(const generic_inst_ptr_type& i);

	void
	unroll(unroll_context& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
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

