// "art_object_connect.h"

#ifndef	__ART_OBJECT_CONNECT_H__
#define	__ART_OBJECT_CONNECT_H__

#include "art_macros.h"
#include "art_object_base.h"
#include "count_ptr.h"

/*********** note on use of data structures ***************
Lists are needed for sets that need to maintain sequence, such as
formal declarations in definitions.  Type-checking is done in order
of elements, comparing actuals against formals one-by-one.  
For some lists, however, we'd like constant time access to 
elements in the sequence by hashing indices.  Hashlist provides
this added functionality by associating a key to each element in the 
list.  

Maps...

********************** end note **************************/

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
	using namespace std;
	using namespace fang;		// for experimental pointer classes

//=============================================================================
/**
	Base class for connections, and expression assignments.  
	These will all be kept in a list to be expanded by each scope
	when unrolled.  
	List keeps things in program order.  
	Need to consider how re-packed, constructed arrays, 
	will fit into picture.  
 */
class connection_assignment_base {
protected:
	// don't need parent back-reference
public:
	connection_assignment_base() { }
virtual	~connection_assignment_base() { }

// interface functions
// need to specify argument as something containing template arguments
// virtual	void unroll_build() const = 0;
// virtual	void static_check() const = 0;
};	// end class connection_assignment_base

//-----------------------------------------------------------------------------
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic expressions.  
 */
class param_expression_assignment : public connection_assignment_base {
protected:
	// really should be exclusive pointers
	list<count_const_ptr<param_expr> >		ex_list;
	// param_expr may contain references to parameter instances, ok
public:
	param_expression_assignment();
	~param_expression_assignment();

	void	append_param_expression(count_const_ptr<param_expr> e);
	void	prepend_param_expression(count_const_ptr<param_expr> e);

};	// end class param_expression_assignment

//-----------------------------------------------------------------------------
/**
	Class for saving and managing expression assignments.  
	Includes both static and dynamic instance references.  
 */
class instance_reference_connection : public connection_assignment_base {
protected:
	// items may be singular or collective instances references.  
	list<count_const_ptr<instance_reference_base> >		inst_list;
public:
	instance_reference_connection();
virtual	~instance_reference_connection() { }

// non-virtual
virtual	void	append_instance_reference(
			count_const_ptr<instance_reference_base> i);
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

	void	prepend_instance_reference(
			count_const_ptr<instance_reference_base> i);

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
	count_const_ptr<simple_instance_reference>	inst;
public:
	/** later, accept complex_aggregate_instance_references? */
	port_connection(count_const_ptr<simple_instance_reference> i);
	~port_connection() { }

	void	append_instance_reference(
			count_const_ptr<instance_reference_base> i);

};	// end class port_connection

//-----------------------------------------------------------------------------
/**
	Wrapper reference to a loop or conditional namespace.  
 */
class dynamic_connection_assignment : public connection_assignment_base {
protected:
	/** the dynamic scope, a loop or conditional */
	never_const_ptr<scopespace>			dscope;
	// may be really static if bounds and conditions can be 
	// resolved as static constants...
public:
	dynamic_connection_assignment(never_const_ptr<scopespace> s);
	~dynamic_connection_assignment() { }

};	// end class dynamic_connection_assignment

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_CONNECT_H__

