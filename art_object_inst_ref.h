// "art_object_inst_ref.h"

#ifndef	__ART_OBJECT_INST_REF_H__
#define	__ART_OBJECT_INST_REF_H__

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
/**
	PHASE this back into what is currently simple_instance_reference.  
	Base class for anything that *refers* to an instance, 
	or collection thereof.  
	Instance reference should be cacheable?
 */
class instance_reference_base : virtual public object {
public:
	instance_reference_base() : object() { }
virtual	~instance_reference_base() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const = 0;
virtual never_const_ptr<instantiation_base> get_inst_base(void) const = 0;
virtual	string hash_string(void) const = 0;
};	// end class instance_reference_base

//=============================================================================
#if 0
PHASE OUT, or needs a facelift
	EVOLVE INTO: complex_aggregate_instance_reference, muhahahaha!
/// in favor of using generic (simple/complex_aggregate) instance references
//	all have potential indices, forget hierarchy
// scheme has much changed since this idea was proposed...
/**
	Reference to an array (one-level) of instances.  
	Self-reference is acceptable and intended for multidimensional
	array element references.  
 */
class collective_instance_reference : public instance_reference_base {
protected:
	// owned? no belongs to cache, even if multidimensional
	// may also be collective
	never_const_ptr<instance_reference_base>	base_array;
	never_const_ptr<param_expr>			lower_index;
	never_const_ptr<param_expr>			upper_index;
public:
	collective_instance_reference(
		never_const_ptr<instance_reference_base> b, 
		const param_expr* l = NULL, const param_expr* r = NULL);
virtual	~collective_instance_reference();

virtual	ostream& what(ostream& o) const;
virtual	ostream& dump(ostream& o) const;
virtual	string hash_string(void) const;
};	// end class collective_instance_reference
#endif

//=============================================================================
/**
	PHASE THIS back into instance_reference_base.
	OR... call this "simple_instance_reference" instead.  
		and replace collective_instance_reference with
		complex_aggregate_instance_reference...
	Base class for a reference to a particular instance.  
	Where a particular instance, either array or single, is 
	connected or aliased, this object refers to a simple instance
	of a datatype, channel, or process.  
	To check, that the instance references was actually in the 
	dimension range of the array declared.  
	Collection, bundle?
	Sub-classes must contain a const pointer to the appropriate
	subclass of fundamental_type_reference.  
	Should these be hashed into used_id_map?
		Will there be identifier conflicts?
 */
class simple_instance_reference : public instance_reference_base {
protected:
	/**
		JUST USE index_list, defined in "art_object_expr"
		Optional array indices (not ranges).  
		Indices may be symbolic.  
		Should be list of parameter expressions, possibly constants.  
		Expressions are owned by a separate expression cache.  
	typedef	list<never_const_ptr<param_expr> >	array_index_list;
	**/

protected:
	// consider letting collective_instance_reference take care of it...
	excl_ptr<index_list>			array_indices;
	// may have to be count_ptr...
	const instantiation_state		inst_state;

// for subclasses:
//	never_const_ptr<instantiation_base>	inst_ref;

public:
	simple_instance_reference(excl_ptr<index_list> i, 
		const instantiation_state& st);
virtual	~simple_instance_reference();

	size_t dimensions(void) const;
	bool attach_indices(excl_ptr<index_list> i);

virtual	ostream& what(ostream& o) const = 0;
virtual	ostream& dump(ostream& o) const;
virtual never_const_ptr<instantiation_base> get_inst_base(void) const = 0;
virtual	string hash_string(void) const;

private:
	// compute static index coverage
	
};	// end class simple_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of datatype.  
 */
class datatype_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	never_const_ptr<datatype_instantiation>	data_inst_ref;

public:
	datatype_instance_reference(const datatype_instantiation& di, 
		excl_ptr<index_list> i);
	~datatype_instance_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
};	// end class datatype_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of channel.  
 */
class channel_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	never_const_ptr<channel_instantiation>	channel_inst_ref;

public:
	channel_instance_reference(const channel_instantiation& ci, 
		excl_ptr<index_list> i);
	~channel_instance_reference();

	ostream& what(ostream& o) const;
	ostream& dump(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
};	// end class channel_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a simple instance of process.  
 */
class process_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited
	never_const_ptr<process_instantiation>	process_inst_ref;

public:
	process_instance_reference(const process_instantiation& pi, 
		excl_ptr<index_list> i);
	~process_instance_reference();

	ostream& what(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
};	// end class process_instance_reference

//=============================================================================
/**
	A reference to a simple instance of parameter.  
 */
class param_instance_reference : public simple_instance_reference {
protected:
//	excl_ptr<index_list>			array_indices;	// inherited

// virtualized
//	never_ptr<param_instantiation>		param_inst_ref;

public:
	param_instance_reference(excl_ptr<index_list> i, 
		const instantiation_state& st);
virtual	~param_instance_reference() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	never_const_ptr<instantiation_base>
		get_inst_base(void) const = 0;
virtual	never_const_ptr<param_instantiation>
		get_param_inst_base(void) const = 0;

	// consider moving these functions into instance_reference_base
	//	where array_indices are inherited from.  
	bool is_initialized(void) const;
	bool is_static_constant(void) const;
	bool is_loop_independent(void) const;
	bool is_unconditional(void) const;

virtual	bool initialize(count_const_ptr<param_expr> i) = 0;

#if 0
virtual count_ptr<param_expr> make_param_literal(
		count_ptr<param_instance_reference> pr) = 0;
#endif
};	// end class param_instance_reference

//-----------------------------------------------------------------------------
#if 0
// moved to art_object_expr.h
/**
	A reference to a instance of built-in type pbool.  
	Consider multiply deriving from pbool_expr, 
	and replacing pbool_literal.  
 */
class pbool_instance_reference : public param_instance_reference {
protected:
	never_ptr<pbool_instantiation>		pbool_inst_ref;
public:
	pbool_instance_reference(never_ptr<pbool_instantiation> pi, 
		excl_ptr<index_list> i);
	~pbool_instance_reference() { }

	ostream& what(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
	never_const_ptr<param_instantiation>
		get_param_inst_base(void) const;

	bool initialize(count_const_ptr<param_expr> i);
#if 0
	count_ptr<param_expr> make_param_literal(
		count_ptr<param_instance_reference> pr);
#endif
};	// end class pbool_instance_reference

//-----------------------------------------------------------------------------
/**
	A reference to a instance of built-in type pint.  
	Consider multiply deriving from pint_expr, 
	and replacing pint_literal.  
 */
class pint_instance_reference : public param_instance_reference {
protected:
	never_ptr<pint_instantiation>		pint_inst_ref;
public:
	pint_instance_reference(never_ptr<pint_instantiation> pi, 
		excl_ptr<index_list> i);
	~pint_instance_reference() { }

	ostream& what(ostream& o) const;
	never_const_ptr<instantiation_base> get_inst_base(void) const;
	never_const_ptr<param_instantiation>
		get_param_inst_base(void) const;

	bool initialize(count_const_ptr<param_expr> i);
#if 0
	count_ptr<param_expr> make_param_literal(
		count_ptr<param_instance_reference> pr);
#endif
};	// end class pint_instance_reference
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_H__

