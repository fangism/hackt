/**
	\file "art_object_base.h"
	Base classes for semantic objects.  
	$Id: art_object_base.h,v 1.22 2004/12/06 07:11:18 fang Exp $
 */

#ifndef	__ART_OBJECT_BASE_H__
#define	__ART_OBJECT_BASE_H__

#include "STL/list.h"

#include "art_object_fwd.h"
#include "macros.h"
#include "persistent.h"		// for persistent object interface
	// includes <iosfwd> <string>

#include "qmap.h"		// need complete definition
#include "hash_qmap.h"		// need complete definition
#include "memory/pointer_classes.h"
				// need complete definition (never_ptr members)

// for convenience
#include "art_object_type_hash.h"

//=============================================================================
// macros

//=============================================================================
// temporary switches

//=============================================================================
// forward declarations

namespace ART {
//=============================================================================
// forward declarations from outside namespaces
namespace parser {
	// note: methods may specify string as formal types, 
	// but you can still pass token_identifiers and token_strings
	// because they are derived from string.
	class token_string;
	class token_identifier;
	class qualified_id_slice;
	class qualified_id;
	class context;
}
using namespace parser;

//=============================================================================
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
USING_LIST
USING_DEQUE
using namespace util;
using namespace util::memory;
using namespace QMAP_NAMESPACE;
using namespace HASH_QMAP_NAMESPACE;

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
virtual ~object() { }

/**
	What eeeeez it man?
 */
virtual	ostream& what(ostream& o) const = 0;

/**
	Detailed hierarchical dump of contents.  
	Later: add detail-level argument.  
 */
virtual	ostream& dump(ostream& o) const = 0;

#if 0
/**	prints out what it (may have) looked like in source */
virtual	ostream& source(ostream& o) const = 0;
#endif

/**
	Use this method to automatically dereference object handles.  
 */
virtual const object& self(void) const { return *this; }

/**
	Consider an interface to hierarchical errors embedded in the objects.  
	Mmmm... fancy.
**/

#if 0
FACTORED OUT INTO class persistent;
/** walks object hierarchy and registers reachable pointers with manager */
virtual	void collect_transient_info(persistent_object_manager& m) const;
/** Writes the object out to a managed buffer */
virtual	void write_object(const persistent_object_manager& m) const;
/** Loads the object from a managed buffer */
virtual	void load_object(persistent_object_manager& m);

public:
	static bool			warn_unimplemented;
#endif
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
	object_handle(never_ptr<const object> o);

	/**
		No intention to de-allocate reference object.  
	 */
	~object_handle() { }

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	const object& self(void) const { return obj; }
};	// end class object_handle

// after note: if we need object_handle (non-const) implement later...

//=============================================================================
/**
	List of objects.
	Will potentially want to make the following sub-typed lists:
	(some cases will have special implicit conversions)
	list<pint_range> for array declarations/instantiations,
	list<index_expr> for array indexing and dimension collapsing, 
	list<param_expr> for param_expression_assignments, 
	list<instance_reference_base> for alias_connections.  
 */
class object_list : public object, public list<count_ptr<object> > {
public:
	typedef	count_ptr<object>			value_type;
protected:
	typedef	list<value_type>			parent;
public:
	object_list();
	~object_list();
	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
// using parent's list interface

	// could use excl_ptr, but then would need to release to count_ptr...
	count_ptr<range_expr_list>
		make_formal_dense_range_list(void) const;
	count_ptr<range_expr_list>
		make_sparse_range_list(void) const;
	excl_ptr<index_list>
		make_index_list(void) const;
	excl_ptr<dynamic_param_expr_list>
		make_param_expr_list(void) const;
	// not const because may modify self
	excl_ptr<param_expression_assignment>
		make_param_assignment(void);
	excl_ptr<const aliases_connection>
		make_alias_connection(void) const;
	excl_ptr<const port_connection>
		make_port_connection(
			count_ptr<const simple_instance_reference> ir) const;
};	// end class object_list

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_BASE_H__

