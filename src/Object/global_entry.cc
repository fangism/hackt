/**
	\file "Object/global_entry.cc"
	$Id: global_entry.cc,v 1.1.2.7 2005/09/14 13:23:13 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <algorithm>
#include "Object/global_entry.tcc"
#include "Object/def/footprint.h"
#include "Object/port_context.h"
#include "Object/state_manager.h"
#include "util/IO_utils.tcc"
#include "util/stacktrace.h"

namespace ART {
namespace entity {

//=============================================================================
// class footprint_frame_map method definitions

template <class Tag>
footprint_frame_map<Tag>::footprint_frame_map() : id_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The magic constructor, initializes this map using theh
	reference footprint's corresponding pool.  
	\param f the reference footprint. 
 */
template <class Tag>
footprint_frame_map<Tag>::footprint_frame_map(const footprint& f) :
		id_map(f.template get_pool<Tag>().size() -1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
footprint_frame_map<Tag>::~footprint_frame_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In the top-level domain, the frame has offset 1.
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
/**
	See also footprint_base<Tag>::__allocate_global_state.
	TODO: see if we can replace the other call with this
		to reduce code duplication.  
 */
template <class Tag>
void
footprint_frame_map<Tag>::__allocate_remaining_sub(const footprint& fp, 
		state_manager& sm, const parent_tag_enum pt, const size_t pid) {
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	footprint_frame_map_type::iterator	iterator;
	// placeholder pool in the footprint
//	const pool_type& php(fp.template get_pool<Tag>());
	global_entry_pool<Tag>& _pool(sm.template get_pool<Tag>());
	const iterator b(id_map.begin());
	const iterator e(id_map.end());
	iterator i(std::find(b, e, size_t(0)));
	for ( ; i!=e; i = std::find(++i, e, size_t(0))) {
		const size_t ind = std::distance(b, i);
		*i = _pool.allocate();
		global_entry<Tag>& g(_pool[*i]);
		g.parent_tag_value = pt;
		g.parent_id = pid;
		g.local_offset = ind+1;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	const placeholder_pool_type& ppool(fp.template get_pool<Tag>());
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

//=============================================================================
// class footprint_frame method definitions

footprint_frame::footprint_frame() :
		process_map_type(), channel_map_type(), struct_map_type(), 
		enum_map_type(), int_map_type(), bool_map_type(), 
		_footprint(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_frame::footprint_frame(const footprint& f) :
		process_map_type(f), channel_map_type(f), struct_map_type(f), 
		enum_map_type(f), int_map_type(f), bool_map_type(f), 
		_footprint(&f) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint_frame::~footprint_frame() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
footprint_frame_map_type&
footprint_frame::get_frame_map(void) {
	return footprint_frame_map<Tag>::id_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
const footprint_frame_map_type&
footprint_frame::get_frame_map(void) const {
	return footprint_frame_map<Tag>::id_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint_frame::dump_id_map(const footprint_frame_map_type& m, ostream& o, 
		const char* const str) {
	typedef	footprint_frame_map_type::const_iterator	const_iterator;
if (!m.empty()) {
	o << endl << '\t' << str << ": ";
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
/**
	\param o the output stream.
 */
ostream&
footprint_frame::dump_frame(ostream& o) const {
	dump_id_map(footprint_frame_map<process_tag>::id_map, o, 
		class_traits<process_tag>::tag_name);
	dump_id_map(footprint_frame_map<channel_tag>::id_map, o, 
		class_traits<channel_tag>::tag_name);
	dump_id_map(footprint_frame_map<datastruct_tag>::id_map, o, 
		class_traits<datastruct_tag>::tag_name);
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
void
footprint_frame::init_top_level(void) {
	footprint_frame_map<process_tag>::__init_top_level();
	footprint_frame_map<channel_tag>::__init_top_level();
	footprint_frame_map<datastruct_tag>::__init_top_level();
	footprint_frame_map<enum_tag>::__init_top_level();
	footprint_frame_map<int_tag>::__init_top_level();
	footprint_frame_map<bool_tag>::__init_top_level();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	const size_t process_offset = sm.get_pool<process_tag>().size();
	const size_t channel_offset = sm.get_pool<channel_tag>().size();
	const size_t struct_offset = sm.get_pool<datastruct_tag>().size();
	// First just allocate the entries.
	footprint_frame_map<process_tag>::
		__allocate_remaining_sub(fp, sm, pt, pid);
	footprint_frame_map<channel_tag>::
		__allocate_remaining_sub(fp, sm, pt, pid);
	footprint_frame_map<datastruct_tag>::
		__allocate_remaining_sub(fp, sm, pt, pid);
	footprint_frame_map<enum_tag>::
		__allocate_remaining_sub(fp, sm, pt, pid);
	footprint_frame_map<int_tag>::
		__allocate_remaining_sub(fp, sm, pt, pid);
	footprint_frame_map<bool_tag>::
		__allocate_remaining_sub(fp, sm, pt, pid);
	// Now this footprint_frame should be good to pass down to subinstances
	// end expand subinstances...
	const size_t process_end = sm.get_pool<process_tag>().size();
	const size_t channel_end = sm.get_pool<channel_tag>().size();
	const size_t struct_end = sm.get_pool<datastruct_tag>().size();
	footprint_frame_map<process_tag>::
		__expand_subinstances(fp, sm, process_offset, process_end);
	footprint_frame_map<channel_tag>::
		__expand_subinstances(fp, sm, channel_offset, channel_end);
	footprint_frame_map<datastruct_tag>::
		__expand_subinstances(fp, sm, struct_offset, struct_end);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_frame::collect_transient_info_base(
		persistent_object_manager& m) const {
	// footprint pointer is not persistently managed, 
	// and thus needs to be reconstructed by other means
	// need to infer canonical_type and go from there.
	STACKTRACE_PERSISTENT_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_frame::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	// see note in collect_transient_info: footprint pointer reconstruction
	STACKTRACE_PERSISTENT_VERBOSE;
	write_id_map(footprint_frame_map<process_tag>::id_map, o);
	write_id_map(footprint_frame_map<channel_tag>::id_map, o);
	write_id_map(footprint_frame_map<datastruct_tag>::id_map, o);
	write_id_map(footprint_frame_map<enum_tag>::id_map, o);
	write_id_map(footprint_frame_map<int_tag>::id_map, o);
	write_id_map(footprint_frame_map<bool_tag>::id_map, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint_frame::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	// see note in collect_transient_info: footprint pointer reconstruction
	STACKTRACE_PERSISTENT_VERBOSE;
	load_id_map(footprint_frame_map<process_tag>::id_map, i);
	load_id_map(footprint_frame_map<channel_tag>::id_map, i);
	load_id_map(footprint_frame_map<datastruct_tag>::id_map, i);
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
	get_frame_map<datastruct_tag>();
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
	get_frame_map<datastruct_tag>();
	get_frame_map<enum_tag>();
	get_frame_map<int_tag>();
	get_frame_map<bool_tag>();
}

//=============================================================================
// class global_entry_base method definitions

//=============================================================================
}	// end namespace entity
}	// end namespace ART

