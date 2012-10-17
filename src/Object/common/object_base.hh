/**
	\file "Object/common/object_base.hh"
	Base classes for semantic objects.  
	This file was reincarnated from "Object/common/object_base.h".
	$Id: object_base.hh,v 1.5 2006/01/22 18:19:23 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_OBJECT_BASE_H__
#define	__HAC_OBJECT_COMMON_OBJECT_BASE_H__

#include <iosfwd>
#include "util/macros.h"
#include "util/memory/excl_ptr.hh"
	// need complete definition (never_ptr members)

//=============================================================================
namespace HAC {
/**
	The namespace of objects that will be returned by the type-checker, 
	and includes the various hierarchical symbol tables in their 
	respective scopes.  
 */
namespace entity {
//=============================================================================
using std::ostream;
using util::memory::never_ptr;

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
	object_handle(const never_ptr<const object> o);

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
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_OBJECT_BASE_H__

