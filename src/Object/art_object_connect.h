/**
	\file "art_object_connect.h"
	Declarations for classes related to connection of physical entities. 
	$Id: art_object_connect.h,v 1.15.16.1.10.6.2.1 2005/02/24 02:03:45 fang Exp $
 */

#ifndef	__ART_OBJECT_CONNECT_H__
#define	__ART_OBJECT_CONNECT_H__

#include "art_object_fwd.h"
#include "art_object_instance_management_base.h"
#include "memory/pointer_classes.h"

#define	SUBTYPE_ALIASES_CONNECTION			1
#define	USE_CLASSIFICATION_TAGS				1

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
class instance_reference_connection : public instance_management_base {
protected:
	typedef	instance_reference_base			generic_instance_type;
	typedef	count_ptr<const generic_instance_type>	generic_inst_ptr_type;
#if !SUBTYPE_ALIASES_CONNECTION
protected:
	typedef	list<generic_inst_ptr_type>		inst_list_type;
	typedef	inst_list_type::iterator		iterator;
	typedef	inst_list_type::const_iterator		const_iterator;
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

virtual	ostream&
	what(ostream& ) const = 0;

virtual	ostream&
	dump(ostream& ) const = 0;
#else
	~aliases_connection_base();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

#if 0
	void
	prepend_instance_reference(const generic_inst_ptr_type& i);
#endif

	void
	unroll(unroll_context& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
#endif
};	// end class aliases_connection_base

//-----------------------------------------------------------------------------
#if SUBTYPE_ALIASES_CONNECTION
/**
	Pointless class, for the sake of classifying data subtype aliases.  
	Just another abstract base class in the heirarchy.  
 */
class data_alias_connection_base : public aliases_connection_base {
	typedef	data_alias_connection_base	this_type;
protected:
	typedef	aliases_connection_base		parent_type;
	typedef	parent_type::generic_inst_ptr_type
						generic_inst_ptr_type;
public:
	data_alias_connection_base() : parent_type() { }

virtual	~data_alias_connection_base() { }

};	// end class data_aliases_connection_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_CLASSIFICATION_TAGS
#define	ALIAS_CONNECTION_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	ALIAS_CONNECTION_CLASS						\
alias_connection<Tag>
#else
#define	ALIAS_CONNECTION_TEMPLATE_SIGNATURE				\
template <class InstRef, class Parent>

#define	ALIAS_CONNECTION_CLASS						\
alias_connection<InstRef,Parent>
#endif

/**
	Re-usable pattern for type-specific alias connection lists, 
	intended for leaf classes because methods are non-virtual.
 */
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
class alias_connection :
#if USE_CLASSIFICATION_TAGS
	public class_traits<Tag>::alias_connection_parent_type
#else
	public Parent
#endif
{
	typedef	ALIAS_CONNECTION_CLASS		this_type;
public:
#if USE_CLASSIFICATION_TAGS
	/// the base alias connection type, such as aliases_connection_base
	typedef	typename class_traits<Tag>::alias_connection_parent_type
						parent_type;
	/// the instance reference type used by this connection
	typedef	typename class_traits<Tag>::instance_reference_type
						instance_reference_type;
	/// the instance collection type referenced
	typedef	typename class_traits<Tag>::instance_collection_type
						instance_collection_type;
	/// the instance alias type resolved by unrolling
	typedef	typename class_traits<Tag>::instance_alias_type
						instance_alias_type;
#else
	/// the base alias connection type, such as aliases_connection_base
	typedef	Parent				parent_type;
	/// the instance reference type used by this connection
	typedef	InstRef				instance_reference_type;
	/// the instance collection type referenced
	typedef	typename instance_reference_type::instance_collection_type
						instance_collection_type;
	/// the instance alias type resolved by unrolling
	typedef	typename instance_collection_type::instance_alias_type
						instance_alias_type;
#endif

	typedef	typename parent_type::generic_inst_ptr_type
						generic_inst_ptr_type;
	typedef	count_ptr<const instance_reference_type>
						inst_ref_ptr_type;
	typedef	list<inst_ref_ptr_type>		inst_list_type;
	typedef	typename inst_list_type::iterator
						iterator;
	typedef	typename inst_list_type::const_iterator
						const_iterator;
	/// the type of collection for unrolled aliases
	typedef	typename instance_reference_type::alias_collection_type
						alias_collection_type;
protected:
	inst_list_type				inst_list;
public:
	alias_connection();

	~alias_connection();

	ostream&
	what(ostream& ) const;

	ostream&
	dump(ostream& ) const;

	void
	append_instance_reference(const generic_inst_ptr_type& );

	void
	unroll(unroll_context& ) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class alias_connection

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

