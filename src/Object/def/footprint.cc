/**
	\file "Object/def/footprint.cc"
	Implementation of footprint class. 
	$Id: footprint.cc,v 1.1.2.4 2005/08/20 21:03:46 fang Exp $
 */

#include "util/hash_specializations.h"
#include "Object/def/footprint.h"
#include "Object/inst/instance_collection_base.h"
#include "util/persistent_object_manager.tcc"
#include "util/hash_qmap.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.h"

namespace ART {
namespace entity {
using util::write_value;
using util::read_value;
//=============================================================================
// class footprint method definitions

footprint::footprint() :
	unrolled(false), created(false),
	instance_collection_map(), 
	// use half-size pool chunks to reduce memory waste for now
	// maybe even quarter-size...
	process_pool(class_traits<process_tag>::instance_pool_chunk_size >> 1),
	channel_pool(class_traits<channel_tag>::instance_pool_chunk_size >> 1),
	struct_pool(class_traits<datastruct_tag>::instance_pool_chunk_size >> 1),
	enum_pool(class_traits<enum_tag>::instance_pool_chunk_size >> 1),
	int_pool(class_traits<int_tag>::instance_pool_chunk_size >> 1),
	bool_pool(class_traits<bool_tag>::instance_pool_chunk_size >> 1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::~footprint() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint::dump(ostream& o) const {
	// unrolled? created?
	// instance_collection_map ?
	process_pool.dump(o);
	channel_pool.dump(o);
	struct_pool.dump(o);
	enum_pool.dump(o);
	int_pool.dump(o);
	bool_pool.dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
{
	// instance_collection_map
	const_instance_map_iterator i(instance_collection_map.begin());
	const const_instance_map_iterator e(instance_collection_map.end());
	for ( ; i!=e; i++) {
		const instance_collection_map_type::mapped_type&
			coll_ptr(i->second);
		NEVER_NULL(coll_ptr);
		coll_ptr->collect_transient_info(m);
	}
}
	process_pool.collect_transient_info_base(m);
	channel_pool.collect_transient_info_base(m);
	struct_pool.collect_transient_info_base(m);
	enum_pool.collect_transient_info_base(m);
	int_pool.collect_transient_info_base(m);
	bool_pool.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	write_value(o, unrolled);
	write_value(o, created);
{
	// instance_collection_map
	write_value(o, instance_collection_map.size());
	const_instance_map_iterator i(instance_collection_map.begin());
	const const_instance_map_iterator e(instance_collection_map.end());
	for ( ; i!=e; i++) {
		// remember, the keys are stored in the instance_collections
		const instance_collection_map_type::mapped_type&
			coll_ptr(i->second);
		m.write_pointer(o, coll_ptr);
	}
}
	process_pool.write_object_base(m, o);
	channel_pool.write_object_base(m, o);
	struct_pool.write_object_base(m, o);
	enum_pool.write_object_base(m, o);
	int_pool.write_object_base(m, o);
	bool_pool.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::load_object_base(const persistent_object_manager& m, istream& i) {
	read_value(i, unrolled);
	read_value(i, created);
{
	// instance_collection_map
	size_t coll_map_size;
	read_value(i, coll_map_size);
	size_t j = 0;
	for ( ; j<coll_map_size; j++) {
		instance_collection_map_type::mapped_type coll_ptr;
		m.read_pointer(i, coll_ptr);
		NEVER_NULL(coll_ptr);
		// need to load the collection to get its key.  
		m.load_object_once(coll_ptr);
		instance_collection_map[coll_ptr->get_name()] = coll_ptr;
	}
}
	process_pool.load_object_base(m, i);
	channel_pool.load_object_base(m, i);
	struct_pool.load_object_base(m, i);
	enum_pool.load_object_base(m, i);
	int_pool.load_object_base(m, i);
	bool_pool.load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

