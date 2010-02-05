/**
	\file "Object/global_entry.cc"
	$Id: global_entry.cc,v 1.13.24.8 2010/02/05 06:13:20 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include <algorithm>
#include "Object/global_entry.tcc"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif
#include "Object/def/footprint.h"
#include "Object/port_context.h"
#include "Object/state_manager.tcc"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "util/IO_utils.tcc"
#include "util/indent.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using util::auto_indent;

//=============================================================================
// class footprint_frame_map method definitions

template <class Tag>
footprint_frame_map<Tag>::footprint_frame_map() : id_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The magic constructor, initializes this map using the
	reference footprint's corresponding pool.  
	\param f the reference footprint. 
 */
template <class Tag>
footprint_frame_map<Tag>::footprint_frame_map(const footprint& f) :
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
		id_map(f.template get_instance_pool<Tag>().port_entries())
		// bother initializing to 0?
#else
		id_map(f.template get_instance_pool<Tag>().size() -1)
#endif
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Takes parent context and transforms ports to create a local map.
	\param f is the local footprint, form which this is sized/constructed.
	\param context consists of global IDs.  
	\param offset is the global ID offset for local instances.  
 */
template <class Tag>
void
footprint_frame_map<Tag>::__construct_global_context(
		const footprint& f, 
		const footprint_frame_map<Tag>& context, 
		const size_t offset) {
	id_map.resize(f.template get_instance_pool<Tag>().local_entries());
	const size_t s = context.size();	// number of ports
	size_t i = 0;
	// map public ports
	for ( ; i<s; ++i) {
		id_map[i] = context[i];
	}
	// map local IDs for the remainder
	INVARIANT(offset >= s);
	for ( ; i<id_map.size(); ++i) {
		id_map[i] = i +offset -s;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This should only ever be called from top-level expansion.  
	In the top-level domain, the frame has offset 1, because the 0th
	entry is reserved as NULL.  
	(Called from footprint::expand_unique_subinstances().)
 */
template <class Tag>
void
footprint_frame_map<Tag>::__init_top_level(void) {
	const size_t s = id_map.size();
	size_t i = 0;
	for ( ; i<s; i++) {
		id_map[i] = i+1;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Reserved for top-level process footprint frame.
 */
template <class Tag>
void
footprint_frame_map<Tag>::__initialize_top_frame(const footprint& f) {
	id_map.resize(f.template get_instance_pool<Tag>().size());
//	this->__init_top_level();
	const size_t s = id_map.size();
	size_t i = 0;
	for ( ; i<s; ++i) {
		id_map[i] = i;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	After assigning of public reachable instances in the hierarchy
	there may be private instance remaining, which can be identified
	has having index 0 in the id_map.

	For global-allocation (deprecating):
	Allocate a slot for each (private) instance, initialize parent ref.

	For memory-mapped global-allocation, I think this just translates
	to incrementing the private-count by 1, we no longer need to 
	recursively descend into private subinstances.  

	See also footprint_base<Tag>::__allocate_global_state.
	TODO: see if we can replace the other call with this
		to reduce code duplication.  
 */
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
template <class Tag>
void
footprint_frame_map<Tag>::__allocate_remaining_sub(
		state_manager& sm, const parent_tag_enum pt, const size_t pid) {
	STACKTRACE_VERBOSE;
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	footprint_frame_map_type::iterator	iterator;
	// placeholder pool in the footprint
	global_entry_pool<Tag>& _pool(sm.template get_pool<Tag>());
	const iterator b(id_map.begin());
	const iterator e(id_map.end());
#if 1 || MEMORY_MAPPED_GLOBAL_ALLOCATION
	static const size_t uninit = -1;
	size_t last_private = uninit;
#endif
	iterator i(std::find(b, e, size_t(0)));
	// TODO: with new sifted-ports invariant algorithm can just
	// find() the first non-port and assume the rest are non-ports
	for ( ; i!=e; i = std::find(++i, e, size_t(0))) {
		const size_t ind = std::distance(b, i);
#if 1 || MEMORY_MAPPED_GLOBAL_ALLOCATION
	// invariant: private instances all follow public ports
	// i.e. they have higher ID number (here, offset)
	if (last_private != uninit) {
		if (last_private +1 != ind) {
			STACKTRACE_INDENT_PRINT("last_private = " <<
				last_private << ", ind = " << ind << endl);
		}
		INVARIANT(last_private +1 == ind);
	}
		last_private = ind;
#endif
		*i = _pool.allocate();
		global_entry<Tag>& g(_pool[*i]);
		// these are no longer members of global_entry_common
		g.parent_tag_value = pt;
		g.parent_id = pid;
		g.local_offset = ind+1;
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	See also footprint_base<Tag>::__expand_unique_subinstances().
	TODO: see if we can replace the other call with this
		to reduce code duplication.  
 */
template <class Tag>
void
footprint_frame_map<Tag>::__expand_subinstances(const footprint& fp, 
		state_manager& sm, const size_t offset, const size_t end) {
	typedef	global_entry_pool<Tag>	global_pool_type;
	typedef	typename state_instance<Tag>::pool_type	placeholder_pool_type;
	typedef	typename placeholder_pool_type::const_iterator
						const_iterator;
	STACKTRACE_VERBOSE;
	INVARIANT(offset <= end);
	size_t j = offset;
	const footprint_frame& extframe(AS_A(const footprint_frame&, *this));
	global_pool_type& gpool(sm.template get_pool<Tag>());
	const placeholder_pool_type& ppool(fp.template get_instance_pool<Tag>());
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "offset = " << offset << endl;
	fp.dump(cerr << "fp: ");
#endif
	for ( ; j!=end; j++) {
		global_entry<Tag>& ref(gpool[j]);
		footprint_frame& frame(ref._frame);
		const instance_alias_info<Tag>&
			formal_alias(*ppool[ref.local_offset].get_back_ref());
		port_member_context pmc;
		formal_alias.__construct_port_context(pmc, extframe);
		// feel the recursion!
		if (!formal_alias.allocate_assign_subinstance_footprint_frame(
				frame, sm, pmc, j).good) {
			THROW_EXIT;
		}
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
template <class Tag>
void
footprint_frame_map<Tag>::__collect_subentries(entry_collection& e, 
		const state_manager& sm) const {
	footprint_frame_map_type::const_iterator
		mi(id_map.begin()), me(id_map.end());
	for ( ; mi!=me; ++mi) {
		sm.template collect_subentries<Tag>(e, *mi);
	}
}
#endif

//=============================================================================
// class footprint_frame method definitions

footprint_frame::footprint_frame() :
		process_map_type(), channel_map_type(), 
#if ENABLE_DATASTRUCTS
		struct_map_type(), 
#endif
		enum_map_type(), int_map_type(), bool_map_type(), 
		_footprint(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_frame::footprint_frame(const footprint& f) :
		process_map_type(f), channel_map_type(f), 
#if ENABLE_DATASTRUCTS
		struct_map_type(f), 
#endif
		enum_map_type(f), int_map_type(f), bool_map_type(f), 
		_footprint(&f) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_frame::~footprint_frame() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint_frame::dump_type(ostream& o) const {
	if (_footprint) {
		return _footprint->dump_type(o);
	} else	return o << "type?";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reminder: this only contains public ports.
 */
ostream&
footprint_frame::dump_id_map(const footprint_frame_map_type& m, ostream& o, 
		const char* const str) {
	typedef	footprint_frame_map_type::const_iterator	const_iterator;
if (!m.empty()) {
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
	o << endl << auto_indent << str << ": ";
#else
	o << endl << '\t' << str << ": ";
#endif
	const_iterator i(m.begin());
	const const_iterator e(m.end());
	o << *i;
	for (i++; i!=e; i++) {
		o << ',' << *i;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_frame::write_id_map(const footprint_frame_map_type& m, ostream& o) {
	util::write_array(o, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_frame::load_id_map(footprint_frame_map_type& m, istream& i) {
	util::read_sequence_resize(i, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
global_entry_substructure_base<true>::dump_type(ostream& o) const {
	return _frame.dump_type(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Newline is folded into here b/c called by state_instance::dump().
 */
ostream&
global_entry_substructure_base<true>::dump_frame_only(ostream& o) const {
	return _frame.dump_frame(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param o the output stream.
 */
ostream&
footprint_frame::dump_frame(ostream& o) const {
	dump_id_map(footprint_frame_map<process_tag>::id_map, o, 
		class_traits<process_tag>::tag_name);
	dump_id_map(footprint_frame_map<channel_tag>::id_map, o, 
		class_traits<channel_tag>::tag_name);
#if ENABLE_DATASTRUCTS
	dump_id_map(footprint_frame_map<datastruct_tag>::id_map, o, 
		class_traits<datastruct_tag>::tag_name);
#endif
	dump_id_map(footprint_frame_map<enum_tag>::id_map, o, 
		class_traits<enum_tag>::tag_name);
	dump_id_map(footprint_frame_map<int_tag>::id_map, o, 
		class_traits<int_tag>::tag_name);
	dump_id_map(footprint_frame_map<bool_tag>::id_map, o, 
		class_traits<bool_tag>::tag_name);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	TODO: dump the source of the footprint, the canonical type.
	Use topfp to resolve...
	\param o the output stream.
	\param ind the global id number of this entry.  
	\param topfp is the top-level footprint.
	Need info: top-level footprint, this local index (from entry), 
 */
ostream&
footprint_frame::dump_footprint(ostream& o, const size_t ind, 
		const footprint& topfp) const {
	INVARIANT(_footprint);
	// HERE, do stuff
	// check if index is in range of top-level footprint's pool.
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
footprint_frame::count_frame_size(void) const {
	return footprint_frame_map<process_tag>::id_map.size()
		+footprint_frame_map<channel_tag>::id_map.size()
#if ENABLE_DATASTRUCTS
		+footprint_frame_map<datastruct_tag>::id_map.size()
#endif
		+footprint_frame_map<enum_tag>::id_map.size()
		+footprint_frame_map<int_tag>::id_map.size()
		+footprint_frame_map<bool_tag>::id_map.size();
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_frame::init_top_level(void) {
	STACKTRACE_VERBOSE;
	footprint_frame_map<process_tag>::__init_top_level();
	footprint_frame_map<channel_tag>::__init_top_level();
#if ENABLE_DATASTRUCTS
	footprint_frame_map<datastruct_tag>::__init_top_level();
#endif
	footprint_frame_map<enum_tag>::__init_top_level();
	footprint_frame_map<int_tag>::__init_top_level();
	footprint_frame_map<bool_tag>::__init_top_level();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Reserved for process[0], the top-level process.
	Creates identity mapping to allocated indices.
 */
void
footprint_frame::initialize_top_frame(const footprint& f) {
	STACKTRACE_VERBOSE;
	_footprint = &f;
	footprint_frame_map<process_tag>::__initialize_top_frame(f);
	footprint_frame_map<channel_tag>::__initialize_top_frame(f);
#if ENABLE_DATASTRUCTS
	footprint_frame_map<datastruct_tag>::__initialize_top_frame(f);
#endif
	footprint_frame_map<enum_tag>::__initialize_top_frame(f);
	footprint_frame_map<int_tag>::__initialize_top_frame(f);
	footprint_frame_map<bool_tag>::__initialize_top_frame(f);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	See also footprint::expand_unique_subinstances.  
	\param fp the footprint that corresponds to this frame.  
	\param sm the global state allocator.
	\param pt the parent type id enum.
	\param pid the parent's globally assigned id.  
 */
void
footprint_frame::allocate_remaining_subinstances(const footprint& fp, 
		state_manager& sm, const parent_tag_enum pt, const size_t pid) {
	STACKTRACE_VERBOSE;
	const size_t process_offset = sm.get_pool<process_tag>().size();
#if !BUILTIN_CHANNEL_FOOTPRINTS
	const size_t channel_offset = sm.get_pool<channel_tag>().size();
#endif
#if ENABLE_DATASTRUCTS
	const size_t struct_offset = sm.get_pool<datastruct_tag>().size();
#endif
	// First just allocate the entries.
	footprint_frame_map<process_tag>::
		__allocate_remaining_sub(sm, pt, pid);
	footprint_frame_map<channel_tag>::
		__allocate_remaining_sub(sm, pt, pid);
#if ENABLE_DATASTRUCTS
	footprint_frame_map<datastruct_tag>::
		__allocate_remaining_sub(sm, pt, pid);
#endif
	footprint_frame_map<enum_tag>::
		__allocate_remaining_sub(sm, pt, pid);
	footprint_frame_map<int_tag>::
		__allocate_remaining_sub(sm, pt, pid);
	footprint_frame_map<bool_tag>::
		__allocate_remaining_sub(sm, pt, pid);
	// Now this footprint_frame should be good to pass down to subinstances
	// end expand subinstances...
	const size_t process_end = sm.get_pool<process_tag>().size();
#if !BUILTIN_CHANNEL_FOOTPRINTS
	const size_t channel_end = sm.get_pool<channel_tag>().size();
#endif
#if ENABLE_DATASTRUCTS
	const size_t struct_end = sm.get_pool<datastruct_tag>().size();
#endif
	footprint_frame_map<process_tag>::
		__expand_subinstances(fp, sm, process_offset, process_end);
#if !BUILTIN_CHANNEL_FOOTPRINTS
	footprint_frame_map<channel_tag>::
		__expand_subinstances(fp, sm, channel_offset, channel_end);
#endif
#if ENABLE_DATASTRUCTS
	footprint_frame_map<datastruct_tag>::
		__expand_subinstances(fp, sm, struct_offset, struct_end);
#endif
}
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Visits each frame map and collects all reachable subinstances'
	ID numbers into the entry collection.  
 */
void
footprint_frame::collect_subentries(entry_collection& e,
		const state_manager& sm) const {
	footprint_frame_map<process_tag>::__collect_subentries(e, sm);
	footprint_frame_map<channel_tag>::__collect_subentries(e, sm);
#if ENABLE_DATASTRUCTS
	footprint_frame_map<datastruct_tag>::__collect_subentries(e, sm);
#endif
	footprint_frame_map<enum_tag>::__collect_subentries(e, sm);
	footprint_frame_map<int_tag>::__collect_subentries(e, sm);
	footprint_frame_map<bool_tag>::__collect_subentries(e, sm);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: now footprints are heap-allocated and persistently managed, 
	which greatly simplifies footprint pointer serialization and
	reconstruction.  
 */
void
footprint_frame::collect_transient_info_base(
		persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	if (_footprint)
		_footprint->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_frame::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	// see note in collect_transient_info: footprint pointer reconstruction
	STACKTRACE_PERSISTENT_VERBOSE;
	m.write_pointer(o, _footprint);
	write_id_map(footprint_frame_map<process_tag>::id_map, o);
	write_id_map(footprint_frame_map<channel_tag>::id_map, o);
#if ENABLE_DATASTRUCTS
	write_id_map(footprint_frame_map<datastruct_tag>::id_map, o);
#endif
	write_id_map(footprint_frame_map<enum_tag>::id_map, o);
	write_id_map(footprint_frame_map<int_tag>::id_map, o);
	write_id_map(footprint_frame_map<bool_tag>::id_map, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The footprint must be guaranteed to be loaded before 
	loading the id_maps.  
 */
void
footprint_frame::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	// see note in collect_transient_info: footprint pointer reconstruction
	STACKTRACE_PERSISTENT_VERBOSE;
	m.read_pointer(i, _footprint);
	if (_footprint) {
		m.load_object_once(const_cast<footprint*>(_footprint));
	}
	load_id_map(footprint_frame_map<process_tag>::id_map, i);
	load_id_map(footprint_frame_map<channel_tag>::id_map, i);
#if ENABLE_DATASTRUCTS
	load_id_map(footprint_frame_map<datastruct_tag>::id_map, i);
#endif
	load_id_map(footprint_frame_map<enum_tag>::id_map, i);
	load_id_map(footprint_frame_map<int_tag>::id_map, i);
	load_id_map(footprint_frame_map<bool_tag>::id_map, i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Testing instantiation.  
	Non-const versions.  
 */
void
footprint_frame::get_frame_map_test(void) {
	get_frame_map<process_tag>();
	get_frame_map<channel_tag>();
#if ENABLE_DATASTRUCTS
	get_frame_map<datastruct_tag>();
#endif
	get_frame_map<enum_tag>();
	get_frame_map<int_tag>();
	get_frame_map<bool_tag>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Testing instantiation.  
	Const versions.  
 */
void
footprint_frame::get_frame_map_test(void) const {
	get_frame_map<process_tag>();
	get_frame_map<channel_tag>();
#if ENABLE_DATASTRUCTS
	get_frame_map<datastruct_tag>();
#endif
	get_frame_map<enum_tag>();
	get_frame_map<int_tag>();
	get_frame_map<bool_tag>();
}

//=============================================================================
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	Initialized by adding the number of local-private instances
	of the pool.
 */
template <class Tag>
global_offset_base<Tag>::global_offset_base(const this_type& g, 
		const footprint& f) :
		offset(g.offset
#if 1
			+f.template get_instance_pool<Tag>()
			.local_private_entries()
#endif
			) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_offset_base<Tag>&
global_offset_base<Tag>::operator += (const pool_type& p) {
	offset += p.total_private_entries();
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_offset::global_offset(const global_offset& g, const footprint& f) :
		global_offset_base<process_tag>(g, f), 
		global_offset_base<channel_tag>(g, f), 
#if ENABLE_DATASTRUCTS
		global_offset_base<datastruct_tag>(g, f), 
#endif
		global_offset_base<enum_tag>(g, f), 
		global_offset_base<int_tag>(g, f), 
		global_offset_base<bool_tag>(g, f) {
	// always advance process id by 1
//	++global_offset_base<process_tag>::offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_offset&
global_offset::operator += (const footprint& f) {
	global_offset_base<process_tag>::operator += (f.get_instance_pool<process_tag>());
	global_offset_base<channel_tag>::operator += (f.get_instance_pool<channel_tag>());
#if ENABLE_DATASTRUCTS
	global_offset_base<datastruct_tag>::operator += (f.get_instance_pool<datastruct_tag>());
#endif
	global_offset_base<enum_tag>::operator += (f.get_instance_pool<enum_tag>());
	global_offset_base<int_tag>::operator += (f.get_instance_pool<int_tag>());
	global_offset_base<bool_tag>::operator += (f.get_instance_pool<bool_tag>());
	// every process traversed increments offset by at least 1!
//	++(this->global_offset_base<process_tag>::offset);
	return *this;
}
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

//=============================================================================
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects pointers needed for save/restoration of footprint pointers.  
	NOTE: enumerations are defined in "Object/traits/type_tag_enum.h"
 */
void
global_entry_substructure_base<true>::collect_transient_info_base(
		persistent_object_manager& m) const {
	_frame.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use global_entry_dumper!
	TODO: comment: pay attention to ordering, 
		is crucial for reconstruction.
	Q: Is persistent object manager really needed?
	A: yes, some canonical_types contain relaxed template params.
 */
void
global_entry_substructure_base<true>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	_frame.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dependent reconstruction ordering:
	\pre all footprints (top-level and asssociated with complete ypes)
		have been restored prior to calling this.  
		Thus it is safe to reference instance placeholders'
		back-references.  
		See the reconstruction ordering in module::load_object_base().  
 */
void
global_entry_substructure_base<true>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	_frame.load_object_base(m, i);
}

//=============================================================================
// explicit template instantiations
// sometimes needed with aggressive optimizations

template class global_entry<bool_tag>;
template class global_entry<int_tag>;
template class global_entry<enum_tag>;
template class global_entry<channel_tag>;
template class global_entry<process_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

