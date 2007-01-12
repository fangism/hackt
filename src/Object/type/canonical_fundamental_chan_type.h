/**
	\file "Object/type/canonical_fundamental_chan_type.h"
	$Id: canonical_fundamental_chan_type.h,v 1.1.4.2 2007/01/12 03:11:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CANONICAL_FUNDAMENTAL_CHAN_TYPE_H__
#define	__HAC_OBJECT_TYPE_CANONICAL_FUNDAMENTAL_CHAN_TYPE_H__

#include "Object/type/canonical_type_fwd.h"
#include <vector>
#include <set>
#include "Object/def/channel_definition_base.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"
#include "util/memory/chunk_map_pool_fwd.h"

namespace HAC {
namespace entity {
class unroll_context;
class template_actuals;
class builtin_channel_type_reference;
class subinstance_manager;
class fundamental_channel_footprint;
	// defined in "Object/def/fundamental_channel_footprint.h"
using std::vector;
using util::persistent;
using util::memory::excl_ptr;
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
class canonical_fundamental_chan_type_base : public persistent {
// friend class builtin_channel_type_reference;
	typedef	canonical_fundamental_chan_type_base	this_type;
	/**
		TODO: May have to supply custom compare operator, 
		must make sure pointer-address comparison is not used.  
	 */
	typedef	std::set<count_ptr<const this_type> >	global_registry_type;
	struct channel_footprint_refresher;
public:
	/**
		List of canonical data types, for built-in channels only.  
	 */
	typedef	vector<canonical_generic_datatype>
						datatype_list_type;
protected:
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
	/**
		private implementation of cacheable, self-derived data
		e.g. channel-data footprint sizes
		We don't maintain this persistently because the 
		cache can be regenerated or re-evaluated.  
		This may change without affecting the effective
		value of this structure, w.r.t. set ordering.  
	 */
	mutable excl_ptr<fundamental_channel_footprint>
						footprint_cache;
#if 0
private:
#else
public:
#endif
	/// only called to signal an error or create a temporary
	canonical_fundamental_chan_type_base();

public:
	explicit
	canonical_fundamental_chan_type_base(const datatype_list_type&);

	// default copy-constructor suffices

	~canonical_fundamental_chan_type_base();

	const datatype_list_type&
	get_datatype_list(void) const { return datatype_list; }

	datatype_list_type&
	get_datatype_list(void) { return datatype_list; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&) const;

	ostream&
	dump(ostream&, const char) const;

	static
	count_ptr<const this_type>
	register_globally(const count_ptr<const this_type>&);

	static
	count_ptr<const this_type>
	register_type(const datatype_list_type&);

	static
	ostream&
	dump_global_registry(ostream&);

	bool
	is_strict(void) const { return true; }

	bool
	is_relaxed(void) const { return false; }

	static
	ostream&
	type_mismatch_error(ostream&, const this_type&, const this_type&);

#if 0
	good_bool
	unroll_port_instances(const unroll_context&, 
		subinstance_manager&) const;

	good_bool
	initialize_footprint_frame(footprint_frame&) const;
#endif

	bool
	operator < (const this_type&) const;

	bool
	operator == (const this_type&) const;

	/**
		\pre footprint already cached
	 */
	const fundamental_channel_footprint&
	footprint_size(void) const {
		NEVER_NULL(footprint_cache);
		return *footprint_cache;
	}

	void
	refresh_footprint(void) const;

	static
	void
	refresh_all_footprints(void);

public:
	FRIEND_PERSISTENT_TRAITS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(16)

public:
// object persistence
	void
	collect_transient_info(persistent_object_manager&) const;

	static
	void
	write_pointer(const persistent_object_manager&, ostream&, 
		const count_ptr<const this_type>&);

	static
	count_ptr<const this_type>
	read_pointer(const persistent_object_manager&, istream&);

private:
	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class canonical_fundamental_chan_type_base

//=============================================================================
/**
	Fundamental channel type with direction.  
	We choose to wrap around a reference-counted pointer
	(containership, vs. inheritance) because we want to share the 
	canonical base type as much as possible.  
 */
class canonical_fundamental_chan_type {
	typedef	canonical_fundamental_chan_type		this_type;
	typedef	canonical_fundamental_chan_type_base	base_type;
	typedef	base_type::datatype_list_type		datatype_list_type;
protected:
	typedef	count_ptr<const base_type>	base_chan_ptr_type;
	base_chan_ptr_type	base_chan_type;
	char			direction;
public:
	canonical_fundamental_chan_type();
	// implicit
	canonical_fundamental_chan_type(const base_chan_ptr_type&);
	~canonical_fundamental_chan_type();

	const base_chan_ptr_type&
	get_base_type(void) const { return base_chan_type; }

	/// \param d is '!' or '?' or other
	void
	set_direction(const char d) { direction = d; }

	char
	get_direction(void) const { return direction; }

	ostream&
	dump(ostream&) const;

// other functions are forwarded explicitly
	const datatype_list_type&
	get_datatype_list(void) const {
		return base_chan_type->get_datatype_list();
	}

	operator bool () const { return base_chan_type; }

	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	bool
	must_be_connectibly_type_equivalent(const this_type&) const;

	static
	ostream&
	type_mismatch_error(ostream&, const this_type&, const this_type&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class canonical_fundamental_chan_type

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CANONICAL_FUNDAMENTAL_CHAN_TYPE_H__

