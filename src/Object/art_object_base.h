/**
	\file "Object/art_object_base.h"
	Base classes for semantic objects.  
	$Id: art_object_base.h,v 1.27.2.3 2005/05/13 06:44:38 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_BASE_H__
#define	__OBJECT_ART_OBJECT_BASE_H__

#include <iosfwd>
#include "util/STL/list.h"

#include "Object/art_object_fwd.h"
#include "util/macros.h"

#include "util/memory/pointer_classes.h"
	// need complete definition (never_ptr members)

//=============================================================================
namespace ART {
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
USING_LIST
using std::ostream;
using namespace util::memory;

//=============================================================================
// general non-member functions

//=============================================================================
/**
	The mother-root object type.
	Any class that will persist in the form of an object file
	should ultimately derive from this class.  
 */
class object {
public:
	// only for the purpose of emitting a virtual table, 
	// see notes for class "node" in "art_parser_base.h".  
virtual	void
	bogus(void) const;

virtual ~object() { }

/**
	What eeeeez it man?
	\param o the output stream.
	\return the output stream.
 */
virtual	ostream&
	what(ostream& o) const = 0;

/**
	Detailed hierarchical dump of contents.  
	Later: add detail-level argument.  
	\param o the output stream.
	\return the output stream.
 */
virtual	ostream&
	dump(ostream& o) const = 0;

#if 0
/**
	TODO:
	Prints out what it (may have) looked like in source
	\param o the output stream.
	\return the output stream.
 */
virtual	ostream& source(ostream& o) const = 0;
#endif

/**
	Use this method to automatically dereference object handles.  
 */
virtual const object&
	self(void) const { return *this; }

/**
	Consider an interface to hierarchical errors embedded in the objects.  
	Mmmm... fancy.
**/

};	// end class object

//=============================================================================
/**
	Need handle class of object to add one level of indirection.  
	When handle is destroyed, the dereferenced member is not de-allocated. 
	This is particularly useful for aggregate types whose elements
	are non-const and owned by the container, but you want to 
	add a const (non-owned) member.  
	Using this will ensure that when the handle is deleted the 
	referenced object is left intact.  
	Used in class name_space to manage open namespaces.  
 */
class object_handle : public object {
protected:
	/**
		The actual object referenced.  
		Never delete.  
		Is it ever NULL?
		Should be reference instead of pointer?
	 */
	const object&			obj;
public:
	/**
		Constructor to wrap around an object reference.  
		\param o may not be a handle.  
			Someone else should have responsibility for deleting.  
	 */
	explicit
	object_handle(never_ptr<const object> o);

	/**
		No intention to de-allocate reference object.  
	 */
	~object_handle() { }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	const object&
	self(void) const { return obj; }
};	// end class object_handle

// after note: if we need object_handle (non-const) implement later...

//=============================================================================
/**
	List of objects.
	DEPRECATED: THIS WILL BE PHASED OUT (2005-05-12)

	Will potentially want to make the following sub-typed lists:
	(some cases will have special implicit conversions)
	list<pint_range> for array declarations/instantiations,
	list<index_expr> for array indexing and dimension collapsing, 
	list<param_expr> for param_expression_assignments, 
	list<instance_reference_base> for alias_connections.  

	Should all list methods be available?
 */
class object_list : public object, public list<count_ptr<object> > {
public:
	typedef	count_ptr<object>			value_type;
protected:
	typedef	list<value_type>			parent_type;
public:
	typedef	parent_type::iterator			iterator;
	typedef	parent_type::const_iterator		const_iterator;
public:
	object_list();
	~object_list();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;
// using parent's list interface

	excl_ptr<index_list>
	make_index_list(void) const;

	excl_ptr<dynamic_param_expr_list>
	make_param_expr_list(void) const;

	excl_ptr<const port_connection>
	make_port_connection(
		const count_ptr<const simple_instance_reference>& ir) const;
};	// end class object_list

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_BASE_H__

