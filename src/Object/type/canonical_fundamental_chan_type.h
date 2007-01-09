/**
	\file "Object/type/canonical_fundamental_chan_type.h"
	$Id: canonical_fundamental_chan_type.h,v 1.1.2.1 2007/01/09 19:30:41 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CANONICAL_FUNDAMENTAL_CHAN_TYPE_H__
#define	__HAC_OBJECT_TYPE_CANONICAL_FUNDAMENTAL_CHAN_TYPE_H__

#include "Object/type/canonical_type_fwd.h"
#include <vector>
#include <set>
// #include "Object/type/canonical_type_base.h"
#include "Object/def/channel_definition_base.h"
// #include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"

namespace HAC {
namespace entity {
class unroll_context;
class template_actuals;
class builtin_channel_type_reference;
class subinstance_manager;
class footprint_frame;
using std::vector;
// using util::memory::never_ptr;
using util::persistent;
using util::memory::count_ptr;

//=============================================================================
/**
	Resolved built-in channel type.  
	For example: chan(int, bool[2])
	Type-list must consist of only fundamental data types for now, 
	i.e. int<>, bool, enum.  
	TODO: add support for arrays in type-list.  
	NOTE: no template parameters (not derived from canonical_type_base)
 */
class canonical_fundamental_chan_type : public persistent {
// friend class builtin_channel_type_reference;
	typedef	canonical_fundamental_chan_type		this_type;
	/**
		TODO: May have to supply custom compare operator, 
		must make sure pointer-address comparison is not used.  
	 */
	typedef	std::set<count_ptr<const this_type> >	global_registry_type;
public:
	/**
		List of canonical data types, for built-in channels only.  
	 */
	typedef	vector<canonical_generic_datatype>
						datatype_list_type;
private:
	/**
		Global set of all instantiated canonical fundamental
		channel types.  
		As each type is instantiated it is registered, or
		checked against the registry for an existing pointer.  
		If there are inter-module static ordering dependencies on this, 
		this will have to be heap-allocated (and ref-counted).  
	 */
	static global_registry_type		global_registry;
	/**
		This data-type list is also used to determine
		set membership.
		TODO: upgrade to type-size pairs to accommodate arrays.  
	 */
	datatype_list_type			datatype_list;
	// TODO: private implementation of cacheable, self-derived data
	// e.g. channel-data footprint sizes
private:
	/// only called to signal an error
	canonical_fundamental_chan_type();

public:
	explicit
	canonical_fundamental_chan_type(const datatype_list_type&);

	// default copy-constructor suffices

	~canonical_fundamental_chan_type();

#if 0
	// do we need a generic version?
	canonical_definition_ptr_type
	get_base_def(void) const { return canonical_definition_ptr; }

	template_actuals
	get_template_params(void) const;
#endif

	const datatype_list_type&
	get_datatype_list(void) const { return datatype_list; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	count_ptr<const this_type>
	register_globally(void) const;

	static
	count_ptr<const this_type>
	register_type(const datatype_list_type&);

#if 0
	/// \param d is '!' or '?' or other
	void
	set_direction(const char d) { direction = d; }

	char
	get_direction(void) const { return direction; }

	count_ptr<const type_reference_type>
	make_type_reference(void) const;
#endif

	bool
	is_strict(void) const { return true; }

	bool
	is_relaxed(void) const { return false; }

#if 0
	operator bool () const;
#endif

#if 0
	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	bool
	must_be_connectibly_type_equivalent(const this_type&) const;
#endif

	static
	ostream&
	type_mismatch_error(ostream&, const this_type&, const this_type&);

	good_bool
	unroll_port_instances(const unroll_context&, 
		subinstance_manager&) const;

#if 0
	// like fundamental_type_reference::unroll_register_complete_type()
	good_bool
	unroll_definition_footprint(const footprint&) const;

	good_bool
	create_definition_footprint(const footprint&) const;

	using base_type::combine_relaxed_actuals;
	using base_type::match_relaxed_actuals;
#endif

	good_bool
	initialize_footprint_frame(footprint_frame&) const;

	bool
	operator < (const this_type&) const;

public:
	FRIEND_PERSISTENT_TRAITS

// object persistence
	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class canonical_type

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CANONICAL_FUNDAMENTAL_CHAN_TYPE_H__

