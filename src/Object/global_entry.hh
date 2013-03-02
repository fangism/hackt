/**
	\file "Object/global_entry.hh"
	$Id: global_entry.hh,v 1.22 2010/08/07 00:00:01 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_H__
#define	__HAC_OBJECT_GLOBAL_ENTRY_H__

#include <iosfwd>
#include "util/persistent_fwd.hh"
// #include <valarray>		// may be more efficient, slice/maskable
#include "Object/traits/type_tag_enum.hh"
#include "Object/common/frame_map.hh"
#include "Object/devel_switches.hh"
#include "util/macros.h"

namespace HAC {
struct cflat_options;

namespace entity {
using std::ostream;
using std::istream;
using util::persistent_object_manager;

namespace PRS {
	class cflat_visitor;
}

struct dump_flags;
struct global_entry_context_base;
struct global_entry_dumper;
class alias_string_set;
class footprint;
class state_manager;
struct entry_collection;		// defined in "Object/entry_collection.h"
class footprint_frame;
struct global_offset;
template <class Tag>
struct global_offset_base;
template <class Tag>
class  state_instance;
template <class T>
class  instance_pool;

template <class Tag>
struct  global_entry;

template <class Tag>
class instance_alias_info;

// TODO: use valarray for memory efficiency
typedef	std::vector<size_t>		footprint_frame_map_type;

/**
	Define to 1 to extend footprint frame with a range to
	represent local private subinstances of structures.
	The non-extended footprint-frame only passes in ports, 
	this would show the range of IDs local to subprocesses.
	This will take a little more memory...
 */
#define	EXTENDED_FOOTPRINT_FRAME	1

//=============================================================================
/**
	Yes, I know nothing depends on the template parameter.  
	Rationale becomes clear in the use in class footprint_frame.  
	See the constructor that takes a footprint argument.  
 */
template <class Tag>
class footprint_frame_map {
	typedef	footprint_frame_map<Tag>		this_type;
public:
	/**
		0-indexed translation table from local to global ID.  
	 */
	footprint_frame_map_type			id_map;
	footprint_frame_map();

	explicit
	footprint_frame_map(const footprint&);
protected:
	footprint_frame_map(const this_type&, const this_type&);

public:

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
	__swap(footprint_frame&);

	void
	__construct_top_global_context(const footprint&, 
		const global_offset_base<Tag>&);

	void
	__construct_global_context(const footprint&, 
		const footprint_frame_map<Tag>&,
		const global_offset_base<Tag>&);

};	// end class footprint_frame_map

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
class footprint_frame :
	public footprint_frame_map<process_tag>, 
	public footprint_frame_map<channel_tag>, 
#if ENABLE_DATASTRUCTS
	public footprint_frame_map<datastruct_tag>, 
#endif
	public footprint_frame_map<enum_tag>, 
	public footprint_frame_map<int_tag>, 
	public footprint_frame_map<bool_tag> {
public:
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

	footprint_frame(const footprint_frame&, const footprint_frame&);

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

	void
	swap(footprint_frame&);

	ostream&
	dump_type(ostream&) const;

	template <class Tag>
	ostream&
	__dump_frame(ostream&) const;

	ostream&
	dump_frame(ostream&) const;

private:
	template <class Tag>
	ostream&
	__dump_extended_frame(ostream&, const global_offset&, 
		const global_offset&, const global_offset&) const;

public:
	ostream&
	dump_extended_frame(ostream&, const global_offset&, 
		const global_offset&, const global_offset&) const;

#if EXTENDED_FOOTPRINT_FRAME
private:
	template <class Tag>
	void
	__extend_frame(const global_offset&, const global_offset&);

public:
	void
	extend_frame(const global_offset&, const global_offset&);
#endif

	template <class Tag>
	ostream&
	dump_footprint(global_entry_dumper&) const;

	void
	construct_top_global_context(const footprint&, const global_offset&);

	void
	construct_global_context(const footprint&, 
		const footprint_frame&, const global_offset&);

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
	ostream&
	dump_extended_id_map(const footprint_frame_map_type&,
		const size_t, const size_t, const size_t, 
		ostream&, const char* const);

#if EXTENDED_FOOTPRINT_FRAME
	static
	void
	extend_id_map(footprint_frame_map_type&, const size_t, const size_t);
#endif

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
};	// end class footprint_frame

//=============================================================================
/**
	Functor for transforming indices via footprint frame lookup.  
 */
class footprint_frame_transformer {
public:
	// public unary_function<...>
	typedef	size_t				argument_type;
	typedef	size_t				result_type;
private:
	const footprint_frame_map_type&			ff;

public:
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
	
};	// end class footprint_frame_transformer

//=============================================================================
struct add_local_private_tag {};
struct add_total_private_tag {};
struct add_all_local_tag {};

template <class Tag>
struct global_offset_base {
	typedef	state_instance<Tag>	instance_type;
	typedef	instance_pool<instance_type>	pool_type;
	typedef	global_offset_base<Tag>		this_type;

	size_t				offset;

	global_offset_base() : offset(0) { }
	global_offset_base(const this_type&, const footprint&, 
		const add_local_private_tag);
	global_offset_base(const this_type&, const footprint&, 
		const add_all_local_tag);
	global_offset_base(const this_type&, const footprint&, 
		const add_total_private_tag);

	this_type&
	operator += (const pool_type&);

	this_type&
	operator += (const this_type&);

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
	global_offset(const global_offset&, const footprint&, 
		const add_local_private_tag);
	global_offset(const global_offset&, const footprint&, 
		const add_all_local_tag);
	global_offset(const global_offset&, const footprint&, 
		const add_total_private_tag);

	template <class Tag>
	const size_t&
	get_offset(void) const {
		return global_offset_base<Tag>::offset;
	}

	global_offset&
	operator += (const footprint&);

	global_offset&
	operator += (const global_offset&);

};	// end struct global_offset

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream&, const global_offset&);

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

	// try to avoid run-time checks in favor of compile-time...
	const footprint_frame*
	get_frame(void) const { return NULL; }

	ostream&
	dump_type(ostream& o) const {
		return o;
	}

	ostream&
	dump_frame_only(ostream& o) const {
		return o;
	}

	template <class Tag>
	static
	const size_t&
	count_frame_size(const size_t& s, const global_entry<Tag>&) {
		return s;
	}

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

	// try to avoid run-time checks in favor of compile-time...
	const footprint_frame*
	get_frame(void) const { return &_frame; }

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
	Default implementation.  
 */
template <class Tag>
struct global_entry_base :
	public global_entry_substructure_base<false> {
	typedef	global_entry_substructure_base<false>	substructure_policy;
	using substructure_policy::dump;
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
	public global_entry_base<Tag> {
	// no need to derive from substructure policy classes, really
	typedef	global_entry_base<Tag>		parent_type;
public:
	global_entry();
	~global_entry();

	ostream&
	dump_base(global_entry_dumper&) const;

	ostream&
	dump(global_entry_dumper&) const;

	// use footprint::get_instance<>().get_back_ref()->dump_attributes()

	// use footprint::dump_canonical_name() now

	// use footprint::get_instance()

	using parent_type::collect_transient_info_base;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end struct global_entry

//=============================================================================
/**
	Uses global_entry_context to construct process context.
	Frame and offset go together.
 */
struct global_process_context {
	footprint_frame				frame;
	global_offset				offset;

	global_process_context() : frame(), offset() { }

	explicit
	global_process_context(const footprint& top) :
		frame(footprint_frame(top)), offset() { }

	explicit
	global_process_context(const footprint_frame& tf) :
		frame(tf), offset() { }

	// for a specific process instance
	global_process_context(const footprint&, const size_t);

	void
	construct_top_global_context(void) {
		frame.construct_top_global_context(*frame._footprint, offset);
	}

	const footprint_frame&
	get_frame(void) const { return frame; }

	const global_offset&
	get_offset(void) const { return offset; }

	void
	descend_frame(const global_process_context&, 
		const size_t lpid, const bool);

	/// modifies self, overwriting with child frame/offset
	void
	descend_frame(const size_t lpid, const bool is_top) {
		descend_frame(*this, lpid, is_top);
	}

	void
	descend_port(const global_process_context&, const size_t lpid);

	void
	descend_port(const size_t lpid) {
		descend_port(*this, lpid);
	}

	ostream&
	dump_frame(ostream& o) const {
		return frame.dump_frame(o);
	}

	ostream&
	dump_offset(ostream& o) const {
		return o << offset;
	}
};	// end struct global_process_context

//=============================================================================
#if	!FOOTPRINT_OWNS_CONTEXT_CACHE
/**
	Same as above, but with extra global process id field.
	This is useful as a return type from instance-reference lookups.
 */
struct global_process_context_id : public global_process_context {
	typedef	global_process_context		parent_type;
	size_t					gpid;

	// usually default ctor
	global_process_context_id() : global_process_context(), gpid(0) { }

	void
	descend_frame(const global_process_context& gpc, 
			const size_t lpid, const bool is_top) {
		parent_type::descend_frame(gpc, lpid, is_top);
		gpid = lpid;
	}

	void
	descend_frame(const size_t lpid, const bool is_top) {
		parent_type::descend_frame(*this, lpid, is_top);
		gpid = lpid;
	}

	void
	descend_port(const global_process_context& gpc, 
			const size_t lpid) {
		parent_type::descend_port(gpc, lpid);
		gpid = lpid;
	}

	void
	descend_port(const size_t lpid) {
		parent_type::descend_port(*this, lpid);
		gpid = lpid;
	}

};	// end struct global_process_context_id
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_H__

