/**
	\file "Object/def/footprint.cc"
	Implementation of footprint class. 
	$Id: footprint.cc,v 1.1.2.2 2005/08/11 00:20:17 fang Exp $
 */

#include "util/hash_specializations.h"
#include "Object/def/footprint.h"
#include "util/persistent_object_manager.tcc"
#include "util/hash_qmap.tcc"
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
	process_pool(class_traits<process_tag>::instance_pool_chunk_size),
	channel_pool(class_traits<channel_tag>::instance_pool_chunk_size),
	struct_pool(class_traits<datastruct_tag>::instance_pool_chunk_size),
	enum_pool(class_traits<enum_tag>::instance_pool_chunk_size),
	int_pool(class_traits<int_tag>::instance_pool_chunk_size),
	bool_pool(class_traits<bool_tag>::instance_pool_chunk_size) {
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
	// instance_collection_map
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
	// instance_collection_map
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
	// instance_collection_map
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

