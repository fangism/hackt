// "art_object_connect.h"

#ifndef	__ART_OBJECT_CONNECT_H__
#define	__ART_OBJECT_CONNECT_H__

#include <iosfwd>
#include <string>
#include <list>
#include <deque>

#include "art_macros.h"
#include "art_object_base.h"

#if 0
#include "qmap.h"
#include "hash_qmap.h"
#include "ptrs.h"
#include "count_ptr.h"
#endif

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
// forward declarations
#if 0
	class scopespace;
	class name_space;
	class loop_scope;
	class conditional_scope;

	class definition_base;
	class channel_definition;
	class datatype_definition;
	class process_definition;
	class enum_datatype_def;
	class built_in_datatype_def;
	class built_in_param_def;

	class fundamental_type_reference;
	class collective_type_reference;
	class data_type_reference;
	class channel_type_reference;
	class process_type_reference;
	class param_type_reference;		// redundant

//	class instance_collection_stack_item;

	class instantiation_base;
	class channel_instantiation;
	class datatype_instantiation;
	class process_instantiation;
	class param_instantiation;
	class pint_instantiation;
	class pbool_instantiation;

	class instance_reference_base;
	class simple_instance_reference;
	class datatype_instance_reference;
	class channel_instance_reference;
	class process_instance_reference;
	class param_instance_reference;
//	class pint_instance_reference;		// relocated "art_object_expr"
//	class pbool_instance_reference;		// relocated "art_object_expr"

	class connection_assignment_base;
	class param_expression_assignment;
	class instance_reference_connection;

// declarations from "art_object_expr.h"
	class param_expr;
	class pint_expr;
	class pbool_expr;
	class range_expr;
	class pint_range;
	class const_range;
	class range_expr_list;
	class const_range_list;
	class dynamic_range_list;
	class index_list;			// not ART::parser::index_list

	typedef	count_const_ptr<range_expr_list>
					index_collection_item_ptr_type;
	/** we keep track of the state of instance collections at
		various program points with this container */
	typedef	deque<index_collection_item_ptr_type>
					index_collection_type;

	/** the state of an instance collection, kept track by each 
		instance reference */
	typedef	index_collection_type::const_iterator
					instantiation_state;

typedef	never_const_ptr<param_expr>			param_expr_ptr_type;

/**
	The container type for template parameters.  
	Temporarily allows any entity::object, however, 
	should definitely not contain subclasses
	of scopespace; intended for instantiations of constant
	parameters, (and when things get fancy) other types, 
	(even fancier) other template arguments.  
	These parameter expressions are not owned!  
	(because they are cached?)
 */
typedef	list<param_expr_ptr_type>			template_param_list;
#endif

//=============================================================================
// general non-member functions

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

