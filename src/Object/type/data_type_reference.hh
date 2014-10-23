/**
	\file "Object/type/data_type_reference.hh"
	Type-reference classes of the HAC language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: data_type_reference.hh,v 1.15 2008/11/12 03:00:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_DATA_TYPE_REFERENCE_HH__
#define	__HAC_OBJECT_TYPE_DATA_TYPE_REFERENCE_HH__

#include "Object/type/fundamental_type_reference.hh"
#include "Object/type/canonical_type_fwd.hh"
#include "Object/expr/types.hh"
#include "Object/unroll/target_context.hh"
#include "Object/type/channel_direction_enum.hh"

namespace HAC {
namespace entity {
class datatype_definition_base;
class footprint;
class unroll_context;
class data_expr;
class data_nonmeta_instance_reference;
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
	/**
		Meaning is defined by the direction_type enumeration.  
	 */
	direction_type					direction;
private:
	data_type_reference();
public:
	explicit
	data_type_reference(const definition_ptr_type td);

	data_type_reference(const definition_ptr_type td, 
		const template_actuals&);

	data_type_reference(const canonical_user_def_data_type&);

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
	is_accepted_in_datatype(void) const;

	bool
	is_accepted_in_channel(void) const;

	bool
	is_canonical(void) const;

	ostream&
	dump(ostream&) const;		// override base class

	void
	set_direction(const direction_type c) { direction = c; }

	direction_type
	get_direction(void) const { return direction; }

	/// unroll-time type-resolution... arguments? return? context?
	// need to be able to lookup parameters... update later...
	count_ptr<const this_type>
	unroll_resolve(const unroll_context&) const;

	count_ptr<const this_type>
	unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const this_type>&) const;

	static
	data_type_reference*
	make_quick_int_type_ref(const pint_value_type);

	static
	canonical_generic_datatype
	make_canonical_int_type_ref(const pint_value_type);

	// sub-typed helper
	count_ptr<const this_type>
	make_canonical_data_type_reference(void) const;

	// should be pure virtual
	canonical_type<definition_type>
	make_canonical_type(void) const;

private:
	struct canonical_compare_result_type;
#if 0
	template <bool (template_actuals::*)(const template_actuals&) const>
	bool
	__type_comparator(const this_type&) const;

	canonical_compare_result_type
	canonical_definition_base_equivalent(const this_type&) const;
#endif
public:
	TYPE_EQUIVALENT_PROTOS

	bool
	may_be_assignably_type_equivalent(const this_type&) const;

	bool
	may_be_binop_type_equivalent(const this_type&) const;

	static
	void
	unroll_port_instances(const never_ptr<const definition_type>, 
//		const template_actuals&, 
		const count_ptr<const const_param_expr_list>&, 
		target_context&, subinstance_manager&);

//	UNROLL_PORT_INSTANCES_PROTO;

private:
	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO;
			
	MAKE_INSTANCE_COLLECTION_PROTO;

friend struct canonical_definition_load_policy<definition_type>;

	/// TODO: eliminate this once we properly sub-type data types.  
	static
	void
	intercept_builtin_definition_hack(
		const persistent_object_manager&, definition_ptr_type&);

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class data_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_DATA_TYPE_REFERENCE_HH__

