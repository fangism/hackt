/**
	\file "Object/type/data_type_reference.h"
	Type-reference classes of the ART language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: data_type_reference.h,v 1.1.2.1 2005/07/21 19:48:24 fang Exp $
 */

#ifndef	__OBJECT_TYPE_DATA_TYPE_REFERENCE_H__
#define	__OBJECT_TYPE_DATA_TYPE_REFERENCE_H__

#include "Object/type/fundamental_type_reference.h"
#include "Object/expr/types.h"

namespace ART {
namespace entity {
class datatype_definition_base;
class unroll_context;
using std::ostream;
using parser::token_identifier;

//=============================================================================
/**
	Reference to a data-type definition.  
	Includes optional template parameters.  
	TODO: consider sub-typing, and also keeping a generic type like this.
 */
class data_type_reference : public fundamental_type_reference {
private:
	typedef	fundamental_type_reference		parent_type;
	typedef	data_type_reference			this_type;
	typedef	datatype_definition_base		definition_type;
	typedef	never_ptr<const definition_type>	definition_ptr_type;
protected:
	typedef	parent_type::template_args_ptr_type	template_args_ptr_type;
	/**
		Reference to data type definition, which may be a 
		built-in type, enumeration, struct, or another typedef.  
	 */
	definition_ptr_type				base_type_def;

private:
	data_type_reference();
public:
	explicit
	data_type_reference(const definition_ptr_type td);

	data_type_reference(const definition_ptr_type td, 
		const template_actuals&);

	// virtualize if something derives from this
	~data_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	never_ptr<const datatype_definition_base>
	get_base_datatype_def(void) const;

	good_bool
	must_be_valid(void) const;

	bool
	is_canonical(void) const;

	/// unroll-time type-resolution... arguments? return? context?
	// need to be able to lookup parameters... update later...
	count_ptr<const this_type>
	unroll_resolve(const unroll_context&) const;

	static
	data_type_reference*
	make_quick_int_type_ref(const pint_value_type);

	// sub-typed helper
	count_ptr<const this_type>
	make_canonical_data_type_reference(void) const;

	MAKE_CANONICAL_TYPE_REFERENCE_PROTO;

	MERGE_RELAXED_ACTUALS_PROTO;

	UNROLL_PORT_INSTANCES_PROTO;

	unroll_context
	make_unroll_context(void) const;
private:
	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO;
			
	MAKE_INSTANCE_COLLECTION_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class data_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_DATA_TYPE_REFERENCE_H__

