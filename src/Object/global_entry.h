/**
	\file "Object/global_entry.h"
	$Id: global_entry.h,v 1.3 2005/10/08 01:39:53 fang Exp $
 */

#ifndef	__OBJECT_GLOBAL_ENTRY_H__
#define	__OBJECT_GLOBAL_ENTRY_H__

#include <iosfwd>
#include <vector>
#include "util/persistent_fwd.h"
// #include <valarray>		// may be more efficient
#include "Object/traits/class_traits.h"
#include "Object/traits/type_tag_enum.h"
#include "util/macros.h"

namespace ART {
class cflat_options;

namespace entity {
using std::ostream;
using std::istream;
using util::persistent_object_manager;

struct bool_tag;
struct dump_flags;
class alias_string_set;
class footprint;
class state_manager;
class port_member_context;

template <class Tag>
struct  global_entry;

template <class Tag>
class global_entry_pool;

typedef	std::vector<size_t>		footprint_frame_map_type;

//=============================================================================
/**
	Yes, I know nothing depends on the template parameter.  
	Rationale becomes clear in the use in class footprint_frame.  
	See the constructor that takes a footprint argument.  
 */
template <class Tag>
struct footprint_frame_map {
	/**
		0-indexed translation table from local to global ID.  
	 */
	footprint_frame_map_type			id_map;
	footprint_frame_map();

	explicit
	footprint_frame_map(const footprint&);

	~footprint_frame_map();

	footprint_frame_map_type::reference
	operator [] (const size_t i) {
		BOUNDS_CHECK(i < id_map.size());
		return id_map[i];
	}

	footprint_frame_map_type::const_reference
	operator [] (const size_t i) const {
		BOUNDS_CHECK(i < id_map.size());
		return id_map[i];
	}

protected:
	void
	__init_top_level(void);

	void
	__allocate_remaining_sub(const footprint&, state_manager&, 
		const parent_tag_enum, const size_t);

	void
	__expand_subinstances(const footprint&, state_manager&,
		const size_t, const size_t);

};	// end struct footprint_frame_mao

//=============================================================================
/**
	Substructure map.  
	Translates local to global offset.  
	Contains a map from local to global IDs.
	Mapping is (injective?) may be many to few, because of 
	external aliases.  

	This is constructed when global aliases are passed down to
	the subinstances.  Each map is as large as the corresponding
	placeholder pool in the reference footprint.  

	TODO: re-use this to pass in external_port_id information
	during global state allocation.  
 */
struct footprint_frame :
	public footprint_frame_map<process_tag>, 
	public footprint_frame_map<channel_tag>, 
	public footprint_frame_map<datastruct_tag>, 
	public footprint_frame_map<enum_tag>, 
	public footprint_frame_map<int_tag>, 
	public footprint_frame_map<bool_tag> {
	typedef	footprint_frame_map<process_tag>	process_map_type;
	typedef	footprint_frame_map<channel_tag>	channel_map_type;
	typedef	footprint_frame_map<datastruct_tag>	struct_map_type;
	typedef	footprint_frame_map<enum_tag>		enum_map_type;
	typedef	footprint_frame_map<int_tag>		int_map_type;
	typedef	footprint_frame_map<bool_tag>		bool_map_type;
	// or use never_ptr<const footprint>
	const footprint*			_footprint;

	footprint_frame();

	explicit
	footprint_frame(const footprint&);

	~footprint_frame();

	template <class Tag>
	footprint_frame_map_type&
	get_frame_map(void);

	template <class Tag>
	const footprint_frame_map_type&
	get_frame_map(void) const;

	void
	init_top_level(void);

	ostream&
	dump_frame(ostream&) const;

	template <class Tag>
	ostream&
	dump_footprint(ostream&, const size_t, const footprint&, 
		const state_manager&) const;

	void
	allocate_remaining_subinstances(const footprint&, state_manager&, 
		const parent_tag_enum, const size_t);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

private:
	static
	ostream&
	dump_id_map(const footprint_frame_map_type&, ostream&, 
		const char* const);

	static
	void
	write_id_map(const footprint_frame_map_type&, ostream&);

	static
	void
	load_id_map(footprint_frame_map_type&, istream&);

	void
	get_frame_map_test(void);

	void
	get_frame_map_test(void) const;
};	// end struct footprint_frame

//=============================================================================
/**
	For meta types without substructure.
	Intentionally empty.  
	\param B whether or not the meta type has substructure.  
 */
template <bool B>
struct global_entry_base {

	template <class Tag>
	ostream&
	dump(ostream& o, const size_t, const footprint&, 
		const state_manager&) const { return o; }

	void
	collect_transient_info_base(persistent_object_manager&) const { }

	template <class Tag>
	void
	write_object_base(const persistent_object_manager&, ostream&, 
		const size_t, const footprint&, const state_manager&) const { }

	template <class Tag>
	void
	load_object_base(const persistent_object_manager&, istream&,
		const size_t, const footprint&, const state_manager&) { }

};	// end struct global_entry_base

//-----------------------------------------------------------------------------
/**
	Specialization for types with substructure.  
 */
template <>
struct global_entry_base<true> {
	footprint_frame			_frame;

	template <class Tag>
	ostream&
	dump(ostream&, const size_t, const footprint&, 
		const state_manager&) const;

	// unused, thus far
	void
	collect_transient_info_base(persistent_object_manager&) const;

	template <class Tag>
	void
	write_object_base(const persistent_object_manager&, ostream&, 
		const size_t, const footprint&, const state_manager&) const;

	template <class Tag>
	void
	load_object_base(const persistent_object_manager&, istream&,
		const size_t, const footprint&, const state_manager&);
};	// end struct global_entry_base

//=============================================================================
struct global_entry_common {
	/**
		Uses parent_tag_enum.
	 */
	char		parent_tag_value;
	/**
		Used as a global index to parent structure.  
	 */
	size_t		parent_id;
	/**
		The placeholder id in the corresponding parent type.  
		This used to lookup the parent's footprint_frame.
		The position in the frame is used to identify
		the canonical entry in the footprint.  
		Invariant: the id obtained from looking up the 
		parent's footprint frame corresponds to the 
		global ID of this entry.  
		Because of aliasing, multiple entries in the footprint
		frame may point to the same global entry.  
	 */
	size_t		local_offset;

	global_entry_common() : parent_tag_value(0), parent_id(0),
			local_offset(0) { }
};

//=============================================================================
/**
	Substructure of processes, which may contain production rules.  
	Need to keep hierarchical substructure to propagate up.  
	TODO: for simulation state allocation, allocate expression
		tree structures with back-links for up propagation.  
 */
class production_rule_substructure {
public:
	// need a pool for rules
	// and a pool for sub-expressions, just like PRS::footprint
	template <class Tag>
	static
	void
	cflat_prs(ostream&, const global_entry<Tag>&, const footprint&,
		const cflat_options&, const state_manager&);

};	// end class production_rule_substructure

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <bool>
struct production_rule_parent_policy {
	/**
		Dummy type, used as an empty base class for meta-types
		without production rule members.  
	 */
	struct type {
	};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct production_rule_parent_policy<true> {
	typedef	production_rule_substructure		type;
};

//=============================================================================
/**
	Globally allocated entry for unique instance.  
 */
template <class Tag>
struct global_entry :
	public global_entry_base<class_traits<Tag>::has_substructure>, 
	public production_rule_parent_policy<
		class_traits<Tag>::has_production_rules>::type, 
	public global_entry_common {
	typedef	global_entry_base<class_traits<Tag>::has_substructure>
						parent_type;
	typedef	typename production_rule_parent_policy<
		class_traits<Tag>::has_production_rules>::type
						prs_parent_type;
public:
	global_entry();
	~global_entry();

	ostream&
	dump(ostream&, const size_t, const footprint&, 
		const state_manager&) const;

	ostream&
	cflat_connect(ostream&, const cflat_options&, 
		const footprint&, const state_manager&) const;

	ostream&
	dump_canonical_name(ostream&,
		const footprint&, const state_manager&) const;

	ostream&
	__dump_canonical_name(ostream&, const dump_flags&,
		const footprint&, const state_manager&) const;

	void
	collect_hierarchical_aliases(alias_string_set&, 
		const footprint&, const state_manager&) const;

	using parent_type::collect_transient_info_base;

	void
	write_object_base(const persistent_object_manager&, ostream&, 
		const size_t, const footprint&, const state_manager&) const;

	void
	load_object_base(const persistent_object_manager&, istream&, 
		const size_t, const footprint&, const state_manager&);

private:
	struct alias_to_string_transformer;

};	// end struct global_entry

//=============================================================================
// Tag must be the tag of a meta-type with subinstances.  

template <class Tag>
const global_entry<Tag>&
extract_parent_entry(const state_manager&, const global_entry_common&);

template <class Tag>
const instance_alias_info<Tag>&
extract_parent_formal_instance_alias(const state_manager&, 
	const global_entry_common&);

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_GLOBAL_ENTRY_H__

