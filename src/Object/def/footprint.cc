/**
	\file "Object/def/footprint.cc"
	Implementation of footprint class. 
	$Id: footprint.cc,v 1.2.2.5 2005/09/09 20:12:31 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "util/hash_specializations.h"
#include "Object/def/footprint.h"
#include "Object/def/port_formals_manager.h"
#include "Object/common/scopespace.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/port_context.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"
#include "util/hash_qmap.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.h"
#include "util/indent.h"
#include "util/wtf.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::auto_indent;

//=============================================================================
// class footprint_base method definitions

template <class Tag>
footprint_base<Tag>::footprint_base() :
		_pool(class_traits<Tag>::instance_pool_chunk_size >> 1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
footprint_base<Tag>::~footprint_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Top-level global state allocation.  
 */
template <class Tag>
good_bool
footprint_base<Tag>::__allocate_global_state(state_manager& sm) const {
	size_t k = 1;
	STACKTRACE_VERBOSE;
	const_iterator i(++_pool.begin());
	const const_iterator e(_pool.end());
	for ( ; i!=e; i++, k++) {
		const size_t j = sm.template allocate<Tag>();
		global_entry<Tag>& g(sm.template get_pool<Tag>()[j]);
		g.parent_tag_value = 0;
		/***
			The parent_tag_value is
		***/
		g.parent_id = 0;
		g.local_offset = k;
		/***
			The local offset corresponds to the relative position
			in the footprint of origin.  
		***/
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Iterate over local footprint of structured entries.  
	\param s the global state allocator.
	\param o the offset from which to start in the global state allocator.
	TODO: combine allocate_subinstance_footprint() and
		assign_footprint_frame() into same function, one step, 
		construct_port_context *first*.
 */
template <class Tag>
good_bool
footprint_base<Tag>::__expand_unique_subinstances(
		const footprint_frame& gframe,
		state_manager& sm, const size_t o) const {
	typedef	typename global_entry_pool<Tag>::pool_type
							global_pool_type;
	STACKTRACE_VERBOSE;
	size_t j = o;
	global_pool_type& gpool(sm.template get_pool<Tag>());
	const_iterator i(++_pool.begin());
	const const_iterator e(_pool.end());
	for ( ; i!=e; i++, j++) {
		global_entry<Tag>& ref(gpool[j]);
		/***
			The footprint frame has not yet been initialized, 
			it is just empty.  allocate_subinstance_footprint()
			will initialize it (resize) for itself automatically.  
		***/
		footprint_frame& frame(ref._frame);
		const instance_alias_info<Tag>&
			formal_alias(*i->get_back_ref());
		// construct port_context
		// util::wtf_is(*i->get_back_ref());
		// frame has not been initialized yet
		// NOTE: the following call does not use the state_manager
		if (!formal_alias.allocate_subinstance_footprint(
				frame, sm).good) {
			return good_bool(false);
		}
#if ENABLE_STACKTRACE
		frame.dump(STACKTRACE_INDENT << "empty frame: ") << endl;
#endif
#if 1
		/***
			Frame is initialized but not asssigned.  
			Now we assign!
			We need the state_manager's global ID info.  
			Construct a port_context, and pass it in.  
			The frame passed in should be a top-level
			master footprint_frame.  
		***/
		port_member_context pmc;
		/***
			TODO: extract partial frame from global context frame.
			Not construct, but project/factor.
		***/
		formal_alias.__construct_port_context(pmc, gframe, sm);
		// formal_alias.construct_port_context(pmc, gframe, sm);
		// WRONG: don't use all of parent's frame
		// need to extract partial frame, projected
#if ENABLE_STACKTRACE
		pmc.dump(STACKTRACE_INDENT << "port-member-context: ") << endl;
		// recursively create private remaining internal state?
		// formal_alias is wrong level of hierarchy
		formal_alias.dump_hierarchical_name(STACKTRACE_INDENT) << endl;
#endif
		formal_alias.assign_footprint_frame(frame, sm, pmc);
#if ENABLE_STACKTRACE
		frame.dump(STACKTRACE_INDENT << "filled frame: ") << endl;
#endif
#endif
		// the allocate private subinstances
	}
	return good_bool(true);
}

//=============================================================================
// class footprint method definitions

footprint::footprint() :
	footprint_base<process_tag>(), 
	footprint_base<channel_tag>(), 
	footprint_base<datastruct_tag>(), 
	footprint_base<enum_tag>(), 
	footprint_base<int_tag>(), 
	footprint_base<bool_tag>(), 
	unrolled(false), created(false),
	instance_collection_map(), 
	// use half-size pool chunks to reduce memory waste for now
	// maybe even quarter-size...
	port_aliases() {
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
	footprint_base<process_tag>::_pool.dump(o);
	footprint_base<channel_tag>::_pool.dump(o);
	footprint_base<datastruct_tag>::_pool.dump(o);
	footprint_base<enum_tag>::_pool.dump(o);
	footprint_base<int_tag>::_pool.dump(o);
	footprint_base<bool_tag>::_pool.dump(o);
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
		port_aliases.dump(o);
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called by the top-level module.  
	This expands unique subinstances in each pool.  
	Expand everything in this footprint at this level first
	before recursing into subinstances' ports.  
 */
good_bool
footprint::expand_unique_subinstances(state_manager& sm) const {
	STACKTRACE_VERBOSE;
	// only processes, channels, and data structures need to be expanded
	// nothing else has substructure.  
	const size_t process_offset = sm.get_pool<process_tag>().size();
	const size_t channel_offset = sm.get_pool<channel_tag>().size();
	const size_t struct_offset = sm.get_pool<datastruct_tag>().size();
	const good_bool a(
		footprint_base<process_tag>::__allocate_global_state(sm).good &&
		footprint_base<channel_tag>::__allocate_global_state(sm).good &&
		footprint_base<datastruct_tag>::__allocate_global_state(sm).good &&
		footprint_base<enum_tag>::__allocate_global_state(sm).good &&
		footprint_base<int_tag>::__allocate_global_state(sm).good &&
		footprint_base<bool_tag>::__allocate_global_state(sm).good);
#if 1
	/***
		Possibly construct footprint_frame(*this);
	***/
	footprint_frame ff(*this);
	ff.init_top_level();
#if ENABLE_STACKTRACE
	this->dump(STACKTRACE_STREAM << "this: ") << endl;
	ff.dump(STACKTRACE_STREAM << "frame: ") << endl;
#endif
#endif
	// this is empty, needs to be assigned before passing down...
	// construct frame using offset?
	if (a.good) {
		const good_bool b(
			footprint_base<process_tag>::
				__expand_unique_subinstances(
					ff, sm, process_offset).good &&
			footprint_base<channel_tag>::
				__expand_unique_subinstances(
					ff, sm, channel_offset).good &&
			footprint_base<datastruct_tag>::
				__expand_unique_subinstances(
					ff, sm, struct_offset).good
		);
		return b;
	} else {
		// error
		return a;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: comment please
 */
void
footprint::assign_footprint_frame(footprint_frame& ff, 
		const state_manager& sm, const port_member_context& pmc) const {
	STACKTRACE_VERBOSE;
	const_instance_map_iterator i(instance_collection_map.begin());
	const const_instance_map_iterator e(instance_collection_map.end());
	for ( ; i!=e; i++) {
		const count_ptr<const physical_instance_collection>
		coll_ptr(i->second.is_a<const physical_instance_collection>());
		if (coll_ptr) {
			// note: port formal is 1-indexed
			// where as member array is 0-indexed
			const size_t pfp = coll_ptr->is_port_formal();
			if (pfp) {
				coll_ptr->assign_footprint_frame(
					ff, sm, pmc.member_array[pfp -1]);
			}
			// else is not port formal, skip
		}
		// else is a param_value_collection, skip
	}
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
	footprint_base<process_tag>::_pool.collect_transient_info_base(m);
	footprint_base<channel_tag>::_pool.collect_transient_info_base(m);
	footprint_base<datastruct_tag>::_pool.collect_transient_info_base(m);
	footprint_base<enum_tag>::_pool.collect_transient_info_base(m);
	footprint_base<int_tag>::_pool.collect_transient_info_base(m);
	footprint_base<bool_tag>::_pool.collect_transient_info_base(m);
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
	footprint_base<process_tag>::_pool.write_object_base(m, o);
	footprint_base<channel_tag>::_pool.write_object_base(m, o);
	footprint_base<datastruct_tag>::_pool.write_object_base(m, o);
	footprint_base<enum_tag>::_pool.write_object_base(m, o);
	footprint_base<int_tag>::_pool.write_object_base(m, o);
	footprint_base<bool_tag>::_pool.write_object_base(m, o);
	port_aliases.write_object_base(m, o);
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
	footprint_base<process_tag>::_pool.load_object_base(m, i);
	footprint_base<channel_tag>::_pool.load_object_base(m, i);
	footprint_base<datastruct_tag>::_pool.load_object_base(m, i);
	footprint_base<enum_tag>::_pool.load_object_base(m, i);
	footprint_base<int_tag>::_pool.load_object_base(m, i);
	footprint_base<bool_tag>::_pool.load_object_base(m, i);
	port_aliases.load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

