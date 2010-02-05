/**
	\file "Object/global_entry.h"
	$Id: global_entry.h,v 1.18.20.10 2010/02/05 09:17:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
// #include <valarray>		// may be more efficient
#include "Object/traits/type_tag_enum.h"
#include "Object/common/frame_map.h"
#include "Object/devel_switches.h"
#include "util/macros.h"

namespace HAC {
class cflat_options;

namespace entity {
using std::ostream;
using std::istream;
using util::persistent_object_manager;

namespace PRS {
	class cflat_visitor;
}

struct dump_flags;
class global_entry_context_base;
struct global_entry_dumper;
class alias_string_set;
class footprint;
class state_manager;
class entry_collection;		// defined in "Object/entry_collection.h"
class footprint_frame;
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
struct global_offset;
template <class Tag>
struct global_offset_base;
template <class Tag>
struct  state_instance;
template <class T>
struct  instance_pool;
#endif

template <class Tag>
struct  global_entry;

template <class Tag>
class global_entry_pool;

template <class Tag>
class instance_alias_info;

// TODO: use valarray for memory efficiency
typedef	std::vector<size_t>		footprint_frame_map_type;

//=============================================================================
/**
	Yes, I know nothing depends on the template parameter.  
	Rationale becomes clear in the use in class footprint_frame.  
	See the constructor that takes a footprint argument.  
 */
template <class Tag>
struct footprint_frame_map {
	typedef	footprint_frame_map<Tag>		this_type;
	/**
		0-indexed translation table from local to global ID.  
	 */
	footprint_frame_map_type			id_map;
	footprint_frame_map();

	explicit
	footprint_frame_map(const footprint&);
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
protected:
	footprint_frame_map(const this_type&, const this_type&);

public:
#endif

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

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	void
	__construct_global_context(const footprint&, 
		const footprint_frame_map<Tag>&,
		const global_offset_base<Tag>&);
#else
	void
	__initialize_top_frame(const footprint&);

	void
	__allocate_remaining_sub(state_manager&, 
		const parent_tag_enum, const size_t);

	void
	__expand_subinstances(const footprint&, state_manager&,
		const size_t, const size_t);

	void
	__collect_subentries(entry_collection&, const state_manager&) const;
#endif

};	// end struct footprint_frame_map

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
#if ENABLE_DATASTRUCTS
	public footprint_frame_map<datastruct_tag>, 
#endif
	public footprint_frame_map<enum_tag>, 
	public footprint_frame_map<int_tag>, 
	public footprint_frame_map<bool_tag> {
	typedef	footprint_frame_map<process_tag>	process_map_type;
	typedef	footprint_frame_map<channel_tag>	channel_map_type;
#if ENABLE_DATASTRUCTS
	typedef	footprint_frame_map<datastruct_tag>	struct_map_type;
#endif
	typedef	footprint_frame_map<enum_tag>		enum_map_type;
	typedef	footprint_frame_map<int_tag>		int_map_type;
	typedef	footprint_frame_map<bool_tag>		bool_map_type;
	/**
		The footprint definition upon which this frame is based.  
		(or use never_ptr<const footprint>?)
	 */
	const footprint*			_footprint;

	footprint_frame();

	explicit
	footprint_frame(const footprint&);

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	footprint_frame(const footprint_frame&, const footprint_frame&);
#endif

	~footprint_frame();

	template <class Tag>
	footprint_frame_map_type&
	get_frame_map(void) {
		return footprint_frame_map<Tag>::id_map;
	}

	template <class Tag>
	const footprint_frame_map_type&
	get_frame_map(void) const {
		return footprint_frame_map<Tag>::id_map;
	}

	size_t
	count_frame_size(void) const;

	void
	init_top_level(void);

	void
	initialize_top_frame(const footprint&);

	ostream&
	dump_type(ostream&) const;

	ostream&
	dump_frame(ostream&) const;

	template <class Tag>
	ostream&
	dump_footprint(global_entry_dumper&) const;

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	void
	construct_global_context(const footprint&, 
		const footprint_frame&, const global_offset&);
#else
	void
	allocate_remaining_subinstances(const footprint&, state_manager&, 
		const parent_tag_enum, const size_t);

	void
	collect_subentries(entry_collection&, const state_manager&) const;
#endif

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
	Functor for transforming indices via footprint frame lookup.  
 */
struct footprint_frame_transformer {
	typedef	size_t				argument_type;
	typedef	size_t				result_type;

	const footprint_frame_map_type&			ff;

	explicit
	footprint_frame_transformer(const footprint_frame_map_type& f) :
		ff(f) { }

	/**
		Less error-prone constructor, forcing tag specification.  
	 */
	template <class Tag>
	footprint_frame_transformer(const footprint_frame& f, const Tag) :
		ff(f.template get_frame_map<Tag>()) { }

	/**
		Translate a local offset/index to a global index.
		Automatically does 1-base to 0-base conversion in index.
		\param i footprint-local index (must be > 0).
		\return global index from allocation.  
	 */
	size_t
	operator () (const size_t i) const {
		return ff[i -1];
	}
	
};	// end struct footprint_frame_transformer

//=============================================================================
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
template <class Tag>
struct global_offset_base {
	typedef	state_instance<Tag>	instance_type;
	typedef	instance_pool<instance_type>	pool_type;
	typedef	global_offset_base<Tag>		this_type;

	size_t				offset;

	global_offset_base() : offset(0) { }
	global_offset_base(const this_type&, const footprint&);

	this_type&
	operator += (const pool_type&);

};	// end struct global_offset_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Set offsets used for global ID calculation.  
 */
struct global_offset :
	public global_offset_base<process_tag>, 
	public global_offset_base<channel_tag>, 
#if ENABLE_DATASTRUCTS
	public global_offset_base<datastruct_tag>, 
#endif
	public global_offset_base<enum_tag>, 
	public global_offset_base<int_tag>, 
	public global_offset_base<bool_tag> {

	// default ctor
	global_offset() {}
	global_offset(const global_offset&, const footprint&);

	global_offset&
	operator += (const footprint&);

};	// end struct global_offset

#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//=============================================================================
template <bool B>
struct global_entry_substructure_base;

//=============================================================================
/**
	For meta types without substructure.
	Intentionally empty.  
	\param B whether or not the meta type has substructure.  
 */
template <>
struct global_entry_substructure_base<false> {

	template <class Tag>
	ostream&
	dump(global_entry_dumper&) const;

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	ostream&
	dump_type(ostream& o) const {
		return o;
	}

	ostream&
	dump_frame_only(ostream& o) const {
		return o;
	}
#endif

	template <class Tag>
	static
	const size_t&
	count_frame_size(const size_t& s, const global_entry<Tag>&) {
		return s;
	}

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	void
	collect_subentries(entry_collection&, const state_manager&) const { }
#endif

	void
	collect_transient_info_base(const persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, const ostream&) const { }

	void
	load_object_base(const persistent_object_manager&, const istream&) { }

};	// end struct global_entry_substructure_base

//-----------------------------------------------------------------------------
/**
	Specialization for types with substructure.  
 */
template <>
struct global_entry_substructure_base<true> {
private:
	typedef	global_entry_substructure_base		this_type;
public:
	footprint_frame			_frame;

	template <class Tag>
	ostream&
	dump(global_entry_dumper&) const;

	ostream&
	dump_type(ostream& o) const;

	ostream&
	dump_frame_only(ostream& o) const;

	template <class Tag>
	static
	size_t
	count_frame_size(const size_t s, const this_type&);

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	void
	initialize_top_frame(const footprint& f) {
		_frame.initialize_top_frame(f);
	}

	void
	collect_subentries(entry_collection& e, const state_manager& sm) const {
		_frame.collect_subentries(e, sm);
	}
#endif

	// some footprint (in frame) may contain relaxed template arguments.  
	void
	collect_transient_info_base(persistent_object_manager&) const;

	// dumper could be reused for write_object_base!
	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	/**
		Consider bundling arguments together...
	 */
	void
	load_object_base(const persistent_object_manager&, istream&);
};	// end struct global_entry_substructure_base

//=============================================================================
/**
	Data common to all global entries (uniquely allocated objects).  
	TODO: is this still useful with MEMORY_MAPPED_GLOBAL_ALLOCATION?
 */
struct global_entry_common {
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
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

	global_entry_common() : parent_tag_value(META_TYPE_NONE), parent_id(0),
			local_offset(0) { }
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION
};	// end struct global_entry_common

//=============================================================================
/**
	Default implementation.  
 */
template <class Tag>
struct global_entry_base :
	public global_entry_substructure_base<false> {
	typedef	global_entry_substructure_base<false>	substructure_policy;
	using substructure_policy::dump;
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	using substructure_policy::collect_subentries;
#endif
	using substructure_policy::collect_transient_info_base;
	using substructure_policy::write_object_base;
	using substructure_policy::load_object_base;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization for processes.  
 */
template <>
struct global_entry_base<process_tag> :
	public global_entry_substructure_base<true> {
	typedef	global_entry_substructure_base<true>	substructure_policy;

	using substructure_policy::dump;
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	using substructure_policy::collect_subentries;
#endif
	using substructure_policy::collect_transient_info_base;
	using substructure_policy::write_object_base;
	using substructure_policy::load_object_base;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if BUILTIN_CHANNEL_FOOTPRINTS
/**
	Specialization for globally allocated channel info.  
	TODO: channel footprint definition.  
 */
template <>
struct global_entry_base<channel_tag>;
#endif

//=============================================================================
/**
	Globally allocated entry for unique instance.  
 */
template <class Tag>
struct global_entry :
	public global_entry_base<Tag>, 
	// no need to derive from substructure policy classes, really
	public global_entry_common {
	typedef	global_entry_base<Tag>		parent_type;
public:
	global_entry();
	~global_entry();

	ostream&
	dump(global_entry_dumper&) const;

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	// use footprint::get_instance<>().get_back_ref()->dump_attributes()
#else
	ostream&
	dump_attributes(global_entry_dumper&) const;
#endif

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	// use footprint::dump_canonical_name() now
#else
	ostream&
	dump_canonical_name(ostream&,
		const footprint&
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		, const state_manager&
#endif
		) const;

	ostream&
	__dump_canonical_name(ostream&, const dump_flags&,
		const footprint&
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		, const state_manager&
#endif
		) const;
#endif

#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	// use footprint::get_instance()
#else
	const state_instance<Tag>&
	get_canonical_instance(const global_entry_context_base&) const;
#endif

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	// relocated to state_instance<Tag>::accept
	void
	accept(PRS::cflat_visitor&) const;
#endif

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	using parent_type::collect_subentries;
#endif
	using parent_type::collect_transient_info_base;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

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
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_H__

