/**
	\file "Object/type/process_type_reference.hh"
	Type-reference classes of the HAC language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: process_type_reference.hh,v 1.14 2011/03/23 00:36:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_PROCESS_TYPE_REFERENCE_H__
#define	__HAC_OBJECT_TYPE_PROCESS_TYPE_REFERENCE_H__

#include "Object/type/fundamental_type_reference.hh"
#include "Object/type/canonical_type_fwd.hh"
#include "Object/devel_switches.hh"
#if PROCESS_CONNECTIVITY_CHECKING
#include "Object/type/channel_direction_enum.hh"
#endif

namespace HAC {
namespace entity {
class process_definition_base;
class footprint;

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
#if PROCESS_CONNECTIVITY_CHECKING
	/**
		Meaning is defined by the direction_type enumeration.  
	 */
	direction_type					direction;
#endif
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

#if PROCESS_CONNECTIVITY_CHECKING
	ostream&
	dump(ostream&) const;

	void
	set_direction(const direction_type c) { direction = c; }

	direction_type
	get_direction(void) const { return direction; }

#if 0
	static
	ostream&
	dump_direction(ostream&, const direction_type);
#endif
#endif

	never_ptr<const definition_base>
	get_base_def(void) const;

	never_ptr<const process_definition_base>
	get_base_proc_def(void) const { return base_proc_def; }

	bool
	is_canonical(void) const;

	bool
	is_accepted_in_datatype(void) const;

	bool
	is_accepted_in_channel(void) const;

	// just resolves template actuals to constants
	count_ptr<const this_type>
	unroll_resolve(const unroll_context&) const;

	/// context-free variant
	count_ptr<const this_type>
	unroll_resolve(void) const;

	good_bool
	unroll_register_complete_type(void) const;

	good_bool
	must_be_valid(void) const;

private:
	struct canonical_compare_result_type;
public:
	TYPE_EQUIVALENT_PROTOS

	count_ptr<const this_type>
	make_canonical_process_type_reference(void) const;

	canonical_process_type
	make_canonical_type(void) const;

	const footprint*
	lookup_footprint(void) const;

private:
	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO;
			
	MAKE_INSTANCE_COLLECTION_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class process_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_PROCESS_TYPE_REFERENCE_H__

