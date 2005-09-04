/**
	\file "Object/def/footprint.cc"
	Implementation of footprint class. 
	$Id: footprint.cc,v 1.1.2.12 2005/09/04 01:58:08 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "util/hash_specializations.h"
#include "Object/def/footprint.h"
#include "Object/def/port_formals_manager.h"
#include "Object/common/scopespace.h"
#include "Object/inst/physical_instance_collection.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"
#include "util/hash_qmap.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.h"
#include "util/indent.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::auto_indent;

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
	bool_pool(class_traits<bool_tag>::instance_pool_chunk_size >> 1)
#if USE_PORT_ALIAS_TRACKER
	, port_aliases()
#endif
	{
	STACKTRACE_CTOR_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::~footprint() {
	STACKTRACE_DTOR_VERBOSE;
}

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
/** 
	TODO: sort map entries or use qmap.
 */
ostream&
footprint::dump_with_collections(ostream& o) const {
	if (!instance_collection_map.empty()) {
		// NOTE: hash_map is NOT sorted
		const_instance_map_iterator
			i(instance_collection_map.begin());
		const const_instance_map_iterator
			e(instance_collection_map.end());
		for ( ; i!=e; i++) {
			i->second->dump(o << auto_indent) << endl;
		}
		dump(o);
#if USE_PORT_ALIAS_TRACKER
		port_aliases.dump(o);
#endif
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up instance collection.  
	\param k the key for the collection, 
		must be local to this definition scope!
	TODO: detect handle global references, 
		probably in simple_meta_instance_reference::unroll
	\return pointer to the collection.  
 */
footprint::instance_collection_ptr_type
footprint::operator [] (const string& k) const {
	return instance_collection_map[k];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This copies instance_collections (physical and parameter)
	into its own map.  
	If this scopespace has already been popluated, then it won't actually
	reload the map, will just exit.  
 */
void
footprint::import_scopespace(const scopespace& s) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "at: " << this << endl;
#endif
if (instance_collection_map.empty()) {
	typedef	scopespace::const_map_iterator	const_map_iterator;
	const_map_iterator si(s.id_map_begin());
	const const_map_iterator se(s.id_map_end());
	for ( ; si!=se; si++) {
		const never_ptr<const instance_collection_base>
			icb(si->second.is_a<const instance_collection_base>());
		if (icb) {
			// then we need to make a deep copy of it 
			// in our own footprint's instance collection map
			const count_ptr<instance_collection_base>
			fc(icb->make_instance_collection_footprint_copy(*this));
			NEVER_NULL(fc);
			instance_collection_map[fc->get_name()] = fc;
		}
		// else is not instance collection, we don't care
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For all instance collections, expand their canonical types.  
 */
good_bool
footprint::create_dependent_types(void) const {
	STACKTRACE_VERBOSE;
	const_instance_map_iterator i(instance_collection_map.begin());
	const const_instance_map_iterator e(instance_collection_map.end());
	for ( ; i!=e; i++) {
		const count_ptr<const physical_instance_collection>
			pic(i->second.is_a<const physical_instance_collection>());
		if (pic && !pic->create_dependent_types().good)
			return good_bool(false);
	}
#if ENABLE_STACKTRACE
	dump_with_collections(STACKTRACE_STREAM << "footprint:" << endl)
		<< endl;
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre the sequential scope was already played for creation.  
 */
void
footprint::evaluate_port_aliases(const port_formals_manager& pfm) {
	STACKTRACE_VERBOSE;
#if USE_PORT_ALIAS_TRACKER
	// find port aliases
	// work out const-cast-ness
	typedef port_formals_manager::const_list_iterator
						const_list_iterator;
	const_list_iterator i(pfm.begin());
	const const_list_iterator e(pfm.end());
	for ( ; i!=e; i++) {
		// includes assertions
		instance_collection_map[(*i)->get_name()]
			.is_a<const physical_instance_collection>()
			->collect_port_aliases(port_aliases);
	}
	port_aliases.filter_uniques();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	// no need to visit def_back_ref
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
#if USE_PORT_ALIAS_TRACKER
	port_aliases.collect_transient_info_base(m);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
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
#if USE_PORT_ALIAS_TRACKER
	port_aliases.write_object_base(m, o);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::load_object_base(const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
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
#if USE_PORT_ALIAS_TRACKER
	port_aliases.load_object_base(m, i);
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

