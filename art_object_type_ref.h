// "art_object_type_ref.h"

#ifndef	__ART_OBJECT_TYPE_REF_H__
#define	__ART_OBJECT_TYPE_REF_H__

#include <iosfwd>
#include <string>
#include <list>
#include <deque>

#include "art_macros.h"

#include "art_object_base.h"

#include "qmap.h"
#include "hash_qmap.h"
#include "ptrs.h"
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
// class type_reference_base declared in "art_object_base.h"
// class fundamental_type_reference declared in "art_object_base.h"

//=============================================================================
#if 0
MAY BE OBSOLETE
/**
	Class for reference to a collection or array of fundamental types.
	Or should we allow collective_types to contain collective_types?
	Depends on whether or not gramma allows both styles of arrays:
	x[i][j] vs. x[i,j], and whether or not they are equivalent.  
 */
class collective_type_reference : public type_reference_base {
protected:
	// don't own these members
	never_const_ptr<type_reference_base>	base;
	never_const_ptr<array_index_list>		dim;
public:
	collective_type_reference(const type_reference_base& b, 
		never_const_ptr<array_index_list> d);
	~collective_type_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
};	// end class collective_type_reference
#endif

//-----------------------------------------------------------------------------
/**
	Reference to a data-type definition.  
	Includes optional template parameters.  
 */
class data_type_reference : public fundamental_type_reference {
protected:
//	excl_ptr<template_param_list>	template_params;	// inherited
	never_const_ptr<datatype_definition>	base_type_def;
public:
	data_type_reference(
		never_const_ptr<datatype_definition> td);
	data_type_reference(
		never_const_ptr<datatype_definition> td, 
		excl_ptr<template_param_list> pl);
		// not gcc-2.95.3 friendly default argument = NULL
virtual	~data_type_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
	excl_ptr<instantiation_base>
		make_instantiation(never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			index_collection_item_ptr_type d) const;
};	// end class data_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a channel-type definition.  
	Includes optional template parameters.  
 */
class channel_type_reference : public fundamental_type_reference {
protected:
//	excl_ptr<template_param_list>	template_params;	// inherited
	never_const_ptr<channel_definition>	base_chan_def;
public:
	channel_type_reference(
		never_const_ptr<channel_definition> td);
	channel_type_reference(
		never_const_ptr<channel_definition> td, 
		excl_ptr<template_param_list> pl);
virtual	~channel_type_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
	excl_ptr<instantiation_base>
		make_instantiation(never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			index_collection_item_ptr_type d) const;
};	// end class channel_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a process-type definition.  
	Includes optional template parameters.  
 */
class process_type_reference : public fundamental_type_reference {
protected:
//	excl_ptr<template_param_list>	template_params;	// inherited
	never_const_ptr<process_definition>	base_proc_def;
public:
	process_type_reference(
		never_const_ptr<process_definition> td);
	process_type_reference(
		never_const_ptr<process_definition> td, 
		excl_ptr<template_param_list> pl);
		// not gcc-2.95.3 friendly default argument = NULL
virtual	~process_type_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
	excl_ptr<instantiation_base>
		make_instantiation(never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			index_collection_item_ptr_type d) const;
};	// end class process_type_reference

//-----------------------------------------------------------------------------
/**
	Reference to a param definition.  
	This class really does nothing, but exists merely for consistency's
	sake.  Parameters are non-templatable, thus it suffices for 
	parameter instances to refer directly to the built-in definitions, 
	but we rather than make a special-case exception, we use this class.  
	The template_params member is inherited but not used.  
	Built-in implementation: only will ever be two instance of this class. 
 */
class param_type_reference : public fundamental_type_reference {
protected:
//	excl_ptr<template_param_list>	template_params;	// inherited, unused
	never_const_ptr<built_in_param_def>	base_param_def;
public:
	param_type_reference(never_const_ptr<built_in_param_def> td);
virtual	~param_type_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<definition_base> get_base_def(void) const;
	excl_ptr<instantiation_base>
		make_instantiation(never_const_ptr<scopespace> s, 
			const token_identifier& id, 
			index_collection_item_ptr_type d) const;
};	// end class param_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_TYPE_REF_H__

