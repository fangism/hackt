/**
	\file "Object/type/canonical_generic_chan_type.hh"
	$Id: canonical_generic_chan_type.hh,v 1.14 2011/03/23 00:36:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CANONICAL_GENERIC_CHAN_TYPE_HH__
#define	__HAC_OBJECT_TYPE_CANONICAL_GENERIC_CHAN_TYPE_HH__

#include "Object/type/canonical_type_fwd.hh"
#include <vector>
#include "Object/type/canonical_type_base.hh"
#include "Object/type/channel_direction_enum.hh"	// enum type_direction
#include "Object/def/channel_definition_base.hh"
#include "util/memory/excl_ptr.hh"

namespace HAC {
namespace entity {
class unroll_context;
class template_actuals;
class builtin_channel_type_reference;
class subinstance_manager;
class footprint_frame;
using std::vector;
using util::memory::never_ptr;

//=============================================================================

/**
	Specialization hack to accomodate both user-defined and
	built-in channel definitions.
	Keep this until we distinguish the two collection types.  
	For now we're keeping single channel_instance_collection.  
 */
template <>
class canonical_type<channel_definition_base> : public canonical_type_base {
	typedef	canonical_type<channel_definition_base>		this_type;
	typedef	canonical_type_base		base_type;
friend class builtin_channel_type_reference;
public:
	/**
		This must be a non-typedef definition.  
	 */
	typedef	channel_definition_base		canonical_definition_type;
	/**
		Generic fundamental type reference derivative.  
	 */
	typedef	canonical_definition_type::type_reference_type
						type_reference_type;
	typedef	never_ptr<const canonical_definition_type>
						canonical_definition_ptr_type;
	/**
		List of canonical data types, for built-in channels only.  
	 */
	typedef	vector<canonical_generic_datatype>
						datatype_list_type;
private:
	canonical_definition_ptr_type		canonical_definition_ptr;
	/**
		THE HACK:
		We also include a list of canonical data types 
		for the case of built-in channel types.  
		When unused, will be empty.  
		INVARIANT: canonical_definition_ptr and datatype_list
		are mutually exclusive.  
	 */
	datatype_list_type			datatype_list;
public:
	/// only called to signal an error
	canonical_type();

public:
	explicit
	canonical_type(const canonical_definition_ptr_type, 
		const direction_type dir);

	// dummy prototype, not supposed to be used (never relaxed!)
	// called from "Object/inst/general_collection_type_manager.h"
	canonical_type(const this_type&, 
		const const_param_list_ptr_type&);

	canonical_type(const canonical_definition_ptr_type, 
		const param_list_ptr_type&, const direction_type dir);

	canonical_type(const canonical_definition_ptr_type, 
		const template_actuals&, const direction_type dir);

	// default copy-constructor suffices

	~canonical_type();

	// do we need a generic version?
	canonical_definition_ptr_type
	get_base_def(void) const { return canonical_definition_ptr; }

	template_actuals
	get_template_params(void) const;

	const datatype_list_type&
	get_datatype_list(void) const { return datatype_list; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	count_ptr<const type_reference_type>
	make_type_reference(void) const;

	bool
	is_strict(void) const;

	bool
	is_relaxed(void) const { return !is_strict(); }

	operator bool () const;

	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	bool
	must_be_connectibly_type_equivalent(const this_type&) const;

	static
	ostream&
	type_mismatch_error(ostream&, const this_type&, const this_type&);

	good_bool
	unroll_port_instances(const unroll_context&, 
		subinstance_manager&) const;

	good_bool
	create_definition_footprint(const footprint&) const;

	using base_type::combine_relaxed_actuals;
	using base_type::match_relaxed_actuals;

	good_bool
	initialize_footprint_frame(footprint_frame&) const;

public:
// object persistence
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class canonical_type

//=============================================================================
// possilbly specialize built-in data types, but require same interface

// possilbly specialize built-in channel type, but require same interface

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CANONICAL_GENERIC_CHAN_TYPE_HH__

