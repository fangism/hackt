/**
	\file "Object/type/process_type_reference.h"
	Type-reference classes of the ART language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: process_type_reference.h,v 1.2.10.3 2005/08/15 19:58:30 fang Exp $
 */

#ifndef	__OBJECT_TYPE_PROCESS_TYPE_REFERENCE_H__
#define	__OBJECT_TYPE_PROCESS_TYPE_REFERENCE_H__

#include "Object/type/fundamental_type_reference.h"
#include "Object/type/canonical_type_fwd.h"

namespace ART {
namespace entity {
class process_definition_base;

//=============================================================================
/**
	Reference to a process-type definition.  
	Includes optional template parameters.  
 */
class process_type_reference : public fundamental_type_reference {
private:
	typedef	fundamental_type_reference		parent_type;
	typedef	process_type_reference			this_type;
	typedef	process_definition_base			definition_type;
	typedef	never_ptr<const definition_type>	definition_ptr_type;
protected:
	typedef	parent_type::template_args_ptr_type	template_args_ptr_type;
// should be const?  reference to base definition shouldn't change...
	typedef	never_ptr<const definition_type>	base_definition_ptr_type;
	never_ptr<const process_definition_base>	base_proc_def;
private:
	process_type_reference();
public:
	explicit
	process_type_reference(
		const never_ptr<const process_definition_base> td);

	process_type_reference(
		const never_ptr<const process_definition_base> td, 
		const template_actuals&);

	process_type_reference(const canonical_process_type&);

	~process_type_reference();

	ostream&
	what(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	never_ptr<const process_definition_base>
	get_base_proc_def(void) const { return base_proc_def; }

	bool
	is_canonical(void) const;

	// just resolves template actuals to constants
	count_ptr<const this_type>
	unroll_resolve(const unroll_context&) const;

	good_bool
	unroll_register_complete_type(void) const;

	good_bool
	must_be_valid(void) const;

	MERGE_RELAXED_ACTUALS_PROTO;
	UNROLL_PORT_INSTANCES_PROTO;

private:
	struct canonical_compare_result_type;
public:
	TYPE_EQUIVALENT_PROTOS

	count_ptr<const this_type>
	make_canonical_process_type_reference(void) const;

	canonical_type<process_definition>
	make_canonical_type(void) const;

	unroll_context
	make_unroll_context(void) const;
private:
	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO;
			
	MAKE_INSTANCE_COLLECTION_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class process_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_PROCESS_TYPE_REFERENCE_H__

