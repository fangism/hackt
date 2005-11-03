/**
	\file "Object/def/footprint.cc"
	Implementation of footprint class. 
	$Id: footprint.cc,v 1.5.2.1 2005/11/03 07:31:16 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			0 && ENABLE_STACKTRACE

#include <algorithm>
#include <iterator>
#include "util/hash_specializations.h"
#include "Object/def/footprint.h"
#include "Object/common/scopespace.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/state_manager.h"
#include "Object/global_entry.h"
#include "Object/port_context.h"
#include "Object/common/cflat_args.h"
#include "Object/common/alias_string_cache.h"
#include "main/cflat_options.h"
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
using std::ostream_iterator;
using std::copy;

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
	Parent tag and id are all zero.  
 */
template <class Tag>
good_bool
footprint_base<Tag>::__allocate_global_state(state_manager& sm) const {
	STACKTRACE_VERBOSE;
	const_iterator i(++_pool.begin());
	const const_iterator e(_pool.end());
	for ( ; i!=e; i++) {
		const size_t j = sm.template allocate<Tag>();
		global_entry<Tag>& g(sm.template get_pool<Tag>()[j]);
		g.parent_tag_value = 0;
		/***
			The parent_tag_value is
		***/
		g.parent_id = 0;
		g.local_offset = j;
		/***
			The local offset corresponds to the relative position
			in the footprint of origin.  
			Remember, the footprint is 1-indexed, 
			while the frame is 0-indexed.  
		***/
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Iterate over local footprint of structured entries.  
	\param s the global state allocator.
	\param o the offset from which to start in the global state allocator.
 */
template <class Tag>
good_bool
footprint_base<Tag>::__expand_unique_subinstances(
		const footprint_frame& gframe,
		state_manager& sm, const size_t o) const {
	typedef	global_entry_pool<Tag>		global_pool_type;
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
			__construct_port_context projects the context
			global actual IDs into this unique instance's
			ports.  (Passing top-down).
			NOTE: this shouldn't require the state_manager.
		***/
		formal_alias.__construct_port_context(pmc, gframe);
#if ENABLE_STACKTRACE
		formal_alias.dump_hierarchical_name(STACKTRACE_INDENT) << endl;
		pmc.dump(STACKTRACE_INDENT << "port-member-context: ") << endl;
		// formal_alias is wrong level of hierarchy
#endif
		// initialize (allocate) frame and assign at the same time.  
		// recursively create private remaining internal state
		if (!formal_alias.allocate_assign_subinstance_footprint_frame(
				frame, sm, pmc, j).good) {
			return good_bool(false);
		}
#if ENABLE_STACKTRACE
		frame.dump_frame(STACKTRACE_INDENT << "filled frame: ") << endl;
#endif
		// the allocate private subinstances
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// not ready to unveil until simulation engine is ready...
/**
	NOTE: this is only need for simulation, not needed for cflattening.
	Expands each unique process' local production rules, 
	according to its footprint.  
	This should only be instantiated with Tag = process_tag.
	Is footprint_frame needed?
 */
template <class Tag>
good_bool
footprint_base<Tag>::__expand_production_rules(const footprint_frame& ff, 
		state_manager& sm) const {
	typedef	typename global_entry_pool<Tag>::entry_type	entry_type;
	const global_entry_pool<bool_tag>&
		bpool(sm.get_pool<bool_tag>());
	global_entry_pool<Tag>&
		proc_pool(sm.get_pool<Tag>());
	size_t i = 1;
	const size_t s = proc_pool.size();
	for ( ; i<=s; i++) {
		entry_type& pe(proc_pool[i]);
		// inherited from production_rule_substructure
		pe.allocate_prs();
	}
	return good_bool(true);
}
#endif

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
	scope_aliases(), 
	port_aliases(),
	prs_footprint() {
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
#if ENABLE_STACKTRACE
		// don't bother dumping, unless debugging
		scope_aliases.dump(o);
#endif
		prs_footprint.dump(o, *this);
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
	NOTE: This is entended for importing from definitions' scopespaces
		because templated types need to work with their own copies
		of instance collections; the definitions' instance
		collections are merely placeholders.  
		We make a deep copy of the collections (while they're empty).  
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
	The difference is that the key used for the collection
	map is a hierarchical (qualified) name to avoid collisions
	as a result of collating over all namespaces.  
	Called by top-level module only.
	TODO: Arg -- code duplication -- clean later.  
 */
void
footprint::import_hierarchical_scopespace(const scopespace& s) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "at: " << this << endl;
#endif
if (instance_collection_map.empty()) {
	typedef	scopespace::const_map_iterator	const_map_iterator;
	const_map_iterator si(s.id_map_begin());
	const const_map_iterator se(s.id_map_end());
	for ( ; si!=se; si++) {
		const never_ptr<const physical_instance_collection>
		pc(si->second.is_a<const physical_instance_collection>());
		if (pc) {
		/***
			DIRTY HACK ALERT:
			collection wants count_ptr, but shallow pointer copy
			comes from never_ptr!  can't mix oil and wanter!
			The hack, initialize the count_ptr with count 1
			instead of 0.  The last and only weak reference
			will never delete.  
			const_cast is safe: we promise to never modify
			the contents from the top-level module.  

			ACTUALLY, this won't work, persistent object manager
			will restore and detect inconsistent pointer
			ownership (owned and shared), resulting in 
			fatal error.  FOILED by my own protective measures!
		***/
			static size_t one = 1;
			instance_collection_map[pc->get_qualified_name()] =
				count_ptr<instance_collection_base>(
				&const_cast<physical_instance_collection&>(*pc), &one);
		}
		// else is not instance collection, we don't care
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Why would you ever want to clear the collection map?
	See import_hierarchical_scopespace comments about hack.  
 */
void
footprint::clear_instance_collection_map(void) {
	instance_collection_map.clear();
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
	Collects all aliases in this scope.  
	\pre the sequential scope was already played for creation.  
 */
void
footprint::evaluate_scope_aliases(void) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT << "got " << instance_collection_map.size()
		<< " entries." << endl;
#endif
	const_instance_map_iterator i(instance_collection_map.begin());
	const const_instance_map_iterator e(instance_collection_map.end());
	for ( ; i!=e; i++) {
		const count_ptr<const physical_instance_collection>
			pic(i->second.is_a<const physical_instance_collection>());
		if (pic) {
#if ENABLE_STACKTRACE
			pic->dump(STACKTRACE_INDENT << "collecting: ") << endl;
#endif
			// method is called collect_port,
			// but it collects everything in scope
			// good re-use of function!
			pic->collect_port_aliases(scope_aliases);
			if (pic->is_port_formal())
				pic->collect_port_aliases(port_aliases);
		}
	}
	// don't filter for scope, want to keep around unique entries
	// scope_aliases.filter_uniques();
#if ENABLE_STACKTRACE
	scope_aliases.dump(cerr << "footprint::scope_aliases: " << endl) << endl;
#endif
	// NOTE: don't filter uniques for scope_aliases, needed for aliases
	port_aliases.filter_uniques();
	// TODO: pick better canonical names
#if 1
	scope_aliases.shorten_canonical_aliases(*this);
#endif
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
	/***
		Possibly construct footprint_frame(*this);
	***/
	footprint_frame ff(*this);
	ff.init_top_level();
#if ENABLE_STACKTRACE
	this->dump(STACKTRACE_STREAM << "this: ") << endl;
	ff.dump_frame(STACKTRACE_STREAM << "frame: ") << endl;
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
#if 0
		if (!b.good)
			return b;
		// now expand the processes' production rules
		// creating a map per process_entry
		// only needed for simulation, not cflattening
		return footprint_base<process_tag>::
			__expand_production_rules(ff, sm);
#else
		return b;
#endif
	} else {
		// error
		return a;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This recursively transforms a port_member_context to local
	frame.  This copies globally assigned indices passed in
	external context into the corresponding (internal) footprint frame.  
	TODO: English, please?
 */
void
footprint::assign_footprint_frame(footprint_frame& ff, 
		const port_member_context& pmc) const {
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
					ff, pmc.member_array[pfp -1]);
			}
			// else is not port formal, skip
		}
		// else is a param_value_collection, skip
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prefixless wrapper.  
	Called from top-level only.  
 */
void
footprint::cflat_aliases(ostream& o, const state_manager& sm, 
		const cflat_options& cf) const {
	wire_alias_set wires;
	const global_entry_pool<bool_tag>& gbp(sm.get_pool<bool_tag>());
	const size_t s = gbp.size();
	if (cf.wire_mode && !cf.check_prs) {
		// reserve alias slots for all uniquely allocated bools
		wires.resize(s);
	}
	cflat_aliases(cflat_aliases_arg_type(
		o, sm, *this, NULL, cf, wires, string()));
	if (cf.wire_mode && cf.connect_style && !cf.check_prs) {
		// style need not be CONNECT_STYLE_WIRE, just not NONE
		// aliases were suppressed while accumulating
		// now print them in wire-accumulated form
		size_t j = 1;
		for ( ; j<s; j++) {
		const alias_string_cache& ac(wires[j]);
		if (!ac.strings.empty()) {
			const global_entry<bool_tag>& b(gbp[j]);
			o << "wire (";
			// NOTE: thus far, wire-style names are never quoted
			// currently, this does not respect cf.enquote_names.
			ostream_iterator<alias_string_cache::value_type>
				osi(o, ",");
			copy(ac.strings.begin(), ac.strings.end(), osi);
			b.dump_canonical_name(o, *this, sm) << ");" << endl;
		}
		// else is loner, has no aliases
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints all cflat aliases in the instance hierarchy,
	starting from the top-level with this.  
 */
void
footprint::cflat_aliases(const cflat_aliases_arg_type& c) const {
	STACKTRACE_VERBOSE;
	const_instance_map_iterator i(instance_collection_map.begin());
	const const_instance_map_iterator e(instance_collection_map.end());
	// cerr << instance_collection_map.size() << " collections." << endl;
	for ( ; i!=e; i++) {
		const count_ptr<const physical_instance_collection>
		coll_ptr(i->second.is_a<const physical_instance_collection>());
		if (coll_ptr) {
			coll_ptr->cflat_aliases(c);
		}
		// skip parameters
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
	scope_aliases.write_object_base(m, o);
	prs_footprint.write_object_base(o);
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
	scope_aliases.load_object_base(m, i);
	prs_footprint.load_object_base(i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

