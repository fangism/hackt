/**
	\file "Object/def/footprint.cc"
	Implementation of footprint class. 
	$Id: footprint.cc,v 1.36.10.1 2008/01/17 01:31:47 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include <algorithm>
#include <iterator>
#include "util/hash_specializations.h"
#include "Object/devel_switches.h"
#include "Object/def/footprint.tcc"
#include "Object/common/scopespace.h"
#include "Object/inst/physical_instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/state_manager.tcc"
#include "Object/global_entry.tcc"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#endif
#include "Object/port_context.h"
#include "Object/common/cflat_args.h"
#include "Object/common/alias_string_cache.h"
#include "Object/common/dump_flags.h"
#include "Object/inst/alias_printer.h"
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/inst/instance_collection_pool_bundle_footprint.tcc"
#include "Object/inst/value_collection_pool_bundle.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_scalar.h"
#include "Object/inst/instance_array.h"
#include "Object/inst/port_formal_array.h"
#include "Object/inst/port_actual_collection.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/param_value_collection.h"
#include "Object/inst/value_array.h"
#include "Object/inst/value_scalar.h"
#include "Object/traits/instance_traits.h"
#include "Object/traits/pbool_traits.h"
#include "Object/traits/pint_traits.h"
#include "Object/traits/preal_traits.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/preal_const.h"
#include "Object/def/user_def_datatype.h"
#include "Object/def/process_definition.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/parameterless_collection_type_manager.h"
#include "Object/inst/int_collection_type_manager.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/traits/classification_tags.h"
#include "Object/inst/process_instance.h"
#include "Object/inst/channel_instance.h"
#include "Object/inst/struct_instance.h"
#include "Object/inst/enum_instance.h"
#include "Object/inst/int_instance.h"
#include "Object/inst/bool_instance.h"
#include "Object/inst/pbool_instance.h"
#include "Object/inst/pint_instance.h"
#include "Object/inst/preal_instance.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/CHP.h"
#include "Object/lang/CHP_event_alloc.h"
#include "Object/persistent_type_hash.h"
#if ENABLE_STACKTRACE
#include "Object/expr/expr_dump_context.h"
#endif
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
#include "Object/unroll/unroll_context.h"
#endif
#include "main/cflat_options.h"

#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.h"
#include "util/indent.h"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
        HAC::entity::footprint, GENERIC_FOOTPRINT_TYPE_KEY, 0)
}

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::write_value;
using util::read_value;
using util::auto_indent;
using std::ostream_iterator;
using std::copy;

//=============================================================================
// class footprint_base method definitions

/**
	Top-level global state allocation (not for use with subinstances).  
	Parent tag and id are all zero.  
 */
template <class Tag>
good_bool
footprint_base<Tag>::__allocate_global_state(state_manager& sm) const {
	STACKTRACE_VERBOSE;
	const_iterator i(++_instance_pool->begin());
	const const_iterator e(_instance_pool->end());
	for ( ; i!=e; i++) {
		const size_t j = sm.template allocate<Tag>();
		global_entry<Tag>& g(sm.template get_pool<Tag>()[j]);
		g.parent_tag_value = PARENT_TYPE_NONE;
		/***
			The parent_tag_value is one of the enumerations
			in "Object/traits/type_tag_enum.h"
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
	This is only applicable to process_tag!  
	(nothing else has footprint-frame)
	Each entry is initialized by resizing footprint_frame.  
	\param gframe footprint frame of the top-level global process.  
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
	// remember to skip the NULL entry
	const_iterator i(++_instance_pool->begin());
	const const_iterator e(_instance_pool->end());
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
#if BUILTIN_CHANNEL_FOOTPRINTS
/**
	Establishes the built-in channel footprint.
	Called from top-level only.  
 */
template <class Tag>
good_bool
footprint_base<Tag>::__set_channel_footprints(state_manager& sm) const {
	typedef	global_entry_pool<Tag>		global_pool_type;
	// typedef	typename global_pool_type::iterator	pool_iterator;
	STACKTRACE_VERBOSE;
	global_pool_type& gpool(sm.template get_pool<Tag>());
	size_t j = 1;		// top-level pool, skip 0th entry
{
	// this covers top-level channels only
	const_iterator i(++_instance_pool->begin());	// skip NULL
	const const_iterator e(_instance_pool->end());
	// pool_iterator j(++gpool.begin());	// nested-iterator private
	for ( ; i!=e; ++i, ++j) {
		global_entry<Tag>& ref(gpool[j]);
		// global_entry<Tag>& ref(*j);
		const instance_alias_info<Tag>&
			formal_alias(*i->get_back_ref());
		NEVER_NULL(formal_alias.container);	// container back-ref
		const instance_collection<Tag>&
			col(formal_alias.container->get_canonical_collection());
		// can now access the channel_collection_type_manager base
		ref.channel_type = col.__get_raw_type().get_base_type();
		// possibly re-cache/compute structural summaries
	}
}{
	// this covers internally allocated (private) channels
	// kinda hackish... :(
	const size_t s = gpool.size();
	for ( ; j<s; ++j) {
		typedef	typename state_instance<Tag>::pool_type	pool_type;
		global_entry<Tag>& ref(gpool[j]);
		NEVER_NULL(ref.parent_id);
		INVARIANT(ref.parent_tag_value == PARENT_TYPE_PROCESS);
		// partially ripped from global_entry::__dump_canonical_name
		const global_entry<process_tag>&
			pref(extract_parent_entry<process_tag>(sm, ref));
		const pool_type&
			_lpool(pref._frame._footprint->
				template get_instance_pool<Tag>());
		const state_instance<Tag>& _inst(_lpool[ref.local_offset]);
		const instance_alias_info<Tag>&
			alias(*_inst.get_back_ref());
		const instance_collection<Tag>&
			col(alias.container->get_canonical_collection());
		ref.channel_type = col.__get_raw_type().get_base_type();
	}
}
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// not ready to unveil until simulation engine is ready...
/**
	NOTE: this is only need for simulation, not needed for cflattening.
	Expands each unique process' local production rules, 
	according to its footprint.  
	This is only applicable to process_tag!
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
// class footprint::create_lock method definitions

/**
	Acquires lock for this footprint.
	If already locked, then we've caught self-dependence.  
 */
footprint::create_lock::create_lock(footprint& f) : fp(f) {
	if (fp.lock_state) {
		// already locked!
		cerr << "Error: detected self-dependence of complete type.  "
			"Types cannot depend on themselves recursively." << endl;
		THROW_EXIT;
		// count on parent catching and printing error message
	}
	fp.lock_state = true;
}

//=============================================================================
// class footprint method definitions

footprint::footprint() :
	footprint_base<process_tag>(), 
	footprint_base<channel_tag>(), 
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>(), 
#endif
	footprint_base<enum_tag>(), 
	footprint_base<int_tag>(), 
	footprint_base<bool_tag>(), 
	value_footprint_base<pbool_tag>(), 
	value_footprint_base<pint_tag>(), 
	value_footprint_base<preal_tag>(), 
	unrolled(false), created(false),
	instance_collection_map(), 
	// use half-size pool chunks to reduce memory waste for now
	// maybe even quarter-size...
	scope_aliases(), 
	port_aliases(),
	prs_footprint(new PRS::footprint), 
	chp_footprint(NULL), 	// allocate when we actually need it
	chp_event_footprint(), 
	spec_footprint(new SPEC::footprint), 
	lock_state(false) {
	STACKTRACE_CTOR_VERBOSE;
	NEVER_NULL(prs_footprint);
	NEVER_NULL(spec_footprint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This doesn't actually copy!
	\pre source footprint must be default constructed (empty).
 */
footprint::footprint(const footprint& t) :
	persistent(), 
	footprint_base<process_tag>(), 
	footprint_base<channel_tag>(), 
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>(), 
#endif
	footprint_base<enum_tag>(), 
	footprint_base<int_tag>(), 
	footprint_base<bool_tag>(), 
	value_footprint_base<pbool_tag>(), 
	value_footprint_base<pint_tag>(), 
	value_footprint_base<preal_tag>(), 
	unrolled(false), created(false),
	instance_collection_map(), 
	// use half-size pool chunks to reduce memory waste for now
	// maybe even quarter-size...
	scope_aliases(), 
	port_aliases(),
	prs_footprint(new PRS::footprint), 
	chp_footprint(NULL), 	// allocate when we actually need it
	chp_event_footprint(), 
	spec_footprint(new SPEC::footprint), 
	lock_state(false) {
	NEVER_NULL(prs_footprint);
	NEVER_NULL(spec_footprint);
	STACKTRACE_CTOR_VERBOSE;
	INVARIANT(t.instance_collection_map.empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::~footprint() {
	STACKTRACE_DTOR_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint::what(ostream& o) const {
	return o << "footprint";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint::dump(ostream& o) const {
	// unrolled? created?
	// instance_collection_map ?
	footprint_base<process_tag>::_instance_pool->dump(o);
	footprint_base<channel_tag>::_instance_pool->dump(o);
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>::_instance_pool->dump(o);
#endif
	footprint_base<enum_tag>::_instance_pool->dump(o);
	footprint_base<int_tag>::_instance_pool->dump(o);
	footprint_base<bool_tag>::_instance_pool->dump(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	This prints members in alphabetical order via the collection_map. 
	Dump is not performance critical.  
 */
ostream&
footprint::dump_with_collections(ostream& o, const dump_flags& df, 
		const expr_dump_context& dc) const {
	if (!instance_collection_map.empty()) {
		// NOTE: hash_map is NOT sorted
		const_instance_map_iterator
			i(instance_collection_map.begin());
		const const_instance_map_iterator
			e(instance_collection_map.end());
		for ( ; i!=e; ++i) {
			o << auto_indent << i->first << " = ";
			const instance_collection_ptr_type
				p((*this)[i->second]);
			NEVER_NULL(p);
			p->dump(o, df) << endl;
		}
	if (is_created()) {
		o << auto_indent << "Created state:" << endl;
		dump(o);
		port_aliases.dump(o);
#if ENABLE_STACKTRACE
		// don't bother dumping, unless debugging
		scope_aliases.dump(o);
#endif
		prs_footprint->dump(o, *this);
		if (chp_footprint) {
			chp_footprint->dump(o, *this, dc);
		}
		chp_event_footprint.dump(o, dc);
		spec_footprint->dump(o, *this);
	}	// end if is_created
	}	// end if collection_map is not empty
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up instance collection.  
	\param k the key for the collection, 
		must be local to this definition scope!
	TODO: detect handle global references, 
		probably in simple_meta_instance_reference::unroll
	NOTE: we forbid global references to instances, but allow for
		parameter values (2006-05-11).  
	NOTE: strategy, deferring lookup of global parameters
		to unroll_context_value_resolver specializations.  
	\return pointer to the collection.  
 */
footprint::instance_collection_ptr_type
footprint::operator [] (const string& k) const {
#if ENABLE_STACKTRACE
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("footprint looking up: " << k << endl);
	dump_with_collections(cerr << "we have: " << endl,
		dump_flags::default_value, expr_dump_context::default_value);

#endif
	const const_instance_map_iterator
		e(instance_collection_map.end()),
		f(instance_collection_map.find(k));
	if (f == e) {
		// not found
		return instance_collection_ptr_type(NULL);
	}
	return (*this)[f->second];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
footprint::instance_collection_ptr_type
footprint::operator [] (const collection_map_entry_type& e) const {
	STACKTRACE_VERBOSE;
	// this internal number-type map is documented and maintained in:
	// "Object/traits/type_tag_enum.h"
	switch (e.meta_type) {
	case META_TYPE_PROCESS:
		return footprint_base<meta_type_map<META_TYPE_PROCESS>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
	case META_TYPE_CHANNEL:
		return footprint_base<meta_type_map<META_TYPE_CHANNEL>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
#if ENABLE_DATASTRUCTS
	case META_TYPE_STRUCT:
		return footprint_base<meta_type_map<META_TYPE_STRUCT>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
#endif
	case META_TYPE_BOOL:
		return footprint_base<meta_type_map<META_TYPE_BOOL>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
	case META_TYPE_INT:
		return footprint_base<meta_type_map<META_TYPE_INT>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
	case META_TYPE_ENUM:
		return footprint_base<meta_type_map<META_TYPE_ENUM>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
	case META_TYPE_PBOOL:
		return value_footprint_base<meta_type_map<META_TYPE_PBOOL>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
	case META_TYPE_PINT:
		return value_footprint_base<meta_type_map<META_TYPE_PINT>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
	case META_TYPE_PREAL:
		return value_footprint_base<meta_type_map<META_TYPE_PREAL>::type>::
		collection_pool_bundle->lookup_collection(e.pool_type, e.index);
	default:
		return instance_collection_ptr_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Now allocate the CHP_event_footprint on-demand.
 */
footprint::chp_footprint_type&
footprint::get_chp_footprint(void) {
	if (!chp_footprint) {
		chp_footprint =
			excl_ptr<chp_footprint_type>(new chp_footprint_type);
		NEVER_NULL(chp_footprint);
	}
	return *chp_footprint;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For all instance collections, expand their canonical types.  
	This does not unroll/create PRS footprints, that's done separately.  
	(called by process_definition::create)
	TODO: alternative, iterate over each instance_collection_pool 
		instead of the map, to avoid repeated lookup costs.  
		This applies to both recursive-creation and 
		local-ID-assignment.
		Benefit: eliminates some virtual calls.  
 */
good_bool
footprint::create_dependent_types(const footprint& top) {
	STACKTRACE_VERBOSE;
try {
	create_lock LOCK(*this);	// will catch recursion errors
{
	const good_bool g(
		get_instance_collection_pool_bundle<process_tag>()
			.create_dependent_types(top).good &&
		get_instance_collection_pool_bundle<channel_tag>()
			.create_dependent_types(top).good &&
#if ENABLE_DATASTRUCTS
		get_instance_collection_pool_bundle<datastruct_tag>()
			.create_dependent_types(top).good &&
#endif
		get_instance_collection_pool_bundle<int_tag>()
			.create_dependent_types(top).good &&
		get_instance_collection_pool_bundle<enum_tag>()
			.create_dependent_types(top).good &&
		get_instance_collection_pool_bundle<bool_tag>()
			.create_dependent_types(top).good);
	if (!g.good) {
		return g;
	}
}
#if ENABLE_STACKTRACE
	dump_with_collections(STACKTRACE_STREAM << "footprint:" << endl, 
		dump_flags::default_value, 
		expr_dump_context::default_value) << endl;
#endif
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
	// final pass (backpatching) to instantiate processes
	// with relaxed types, whose types were implicitly bound 
	// *after* aliasing to a complete-typed alias.  
	// this performs the appropriate recursive connections
	const unroll_context c(this, &top);
	// will instantiate into this footprint
	// only processes have relaxed types and substructure
	if (!get_instance_collection_pool_bundle<process_tag>()
			.finalize_substructure_aliases(c).good) {
		return good_bool(false);
	}
	// this call must precede collecting of scope aliases and
	// shorten_canonical_aliases
#endif
{
	const good_bool g(
		get_instance_collection_pool_bundle<process_tag>()
			.allocate_local_instance_ids(*this).good &&
		get_instance_collection_pool_bundle<channel_tag>()
			.allocate_local_instance_ids(*this).good &&
#if ENABLE_DATASTRUCTS
		get_instance_collection_pool_bundle<datastruct_tag>()
			.allocate_local_instance_ids(*this).good &&
#endif
		get_instance_collection_pool_bundle<enum_tag>()
			.allocate_local_instance_ids(*this).good &&
		get_instance_collection_pool_bundle<int_tag>()
			.allocate_local_instance_ids(*this).good &&
		get_instance_collection_pool_bundle<bool_tag>()
			.allocate_local_instance_ids(*this).good);
	if (!g.good) {
		return g;
	}
}
	evaluate_scope_aliases();
	// should this be postponed until connection_diagnostics()?
	mark_created();
	return good_bool(true);
} catch (...) {
	// expect recursion errors to trigger this
	cerr << "Error creating footprint of complete type." << endl;
	return good_bool(false);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects all aliases in this scope and also creates a set
	of port aliases for the sake of replaying internal aliases.  
	\pre the sequential scope was already played for creation.  
	NOTE: this can also called during load-reconstruction.
 */
void
footprint::evaluate_scope_aliases(void) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("got " << instance_collection_map.size()
		<< " entries." << endl);
	get_instance_collection_pool_bundle<process_tag>()
		.collect_scope_aliases(scope_aliases);
	get_instance_collection_pool_bundle<channel_tag>()
		.collect_scope_aliases(scope_aliases);
#if ENABLE_DATASTRUCTS
	get_instance_collection_pool_bundle<datastruct_tag>()
		.collect_scope_aliases(scope_aliases);
#endif
	get_instance_collection_pool_bundle<int_tag>()
		.collect_scope_aliases(scope_aliases);
	get_instance_collection_pool_bundle<enum_tag>()
		.collect_scope_aliases(scope_aliases);
	get_instance_collection_pool_bundle<bool_tag>()
		.collect_scope_aliases(scope_aliases);
	// just copy-filter them over
	port_aliases.import_port_aliases(scope_aliases);
	// don't filter for scope, want to keep around unique entries
	// scope_aliases.filter_uniques();
#if ENABLE_STACKTRACE
	scope_aliases.dump(cerr << "footprint::scope_aliases: " << endl) << endl;
#endif
	scope_aliases.shorten_canonical_aliases(*this);
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
#if !BUILTIN_CHANNEL_FOOTPRINTS
	const size_t channel_offset = sm.get_pool<channel_tag>().size();
#endif
#if ENABLE_DATASTRUCTS
	const size_t struct_offset = sm.get_pool<datastruct_tag>().size();
#endif
	const good_bool a(
		footprint_base<process_tag>::__allocate_global_state(sm).good &&
		footprint_base<channel_tag>::__allocate_global_state(sm).good &&
#if ENABLE_DATASTRUCTS
		footprint_base<datastruct_tag>::__allocate_global_state(sm).good &&
#endif
		footprint_base<enum_tag>::__allocate_global_state(sm).good &&
		footprint_base<int_tag>::__allocate_global_state(sm).good &&
		footprint_base<bool_tag>::__allocate_global_state(sm).good);
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
					ff, sm, process_offset).good
#if !BUILTIN_CHANNEL_FOOTPRINTS
			&& footprint_base<channel_tag>::
				__expand_unique_subinstances(
					ff, sm, channel_offset).good
#endif
#if ENABLE_DATASTRUCTS
			&& footprint_base<datastruct_tag>::
				__expand_unique_subinstances(
					ff, sm, struct_offset).good
#endif
		);
#if BUILTIN_CHANNEL_FOOTPRINTS
		if (!b.good)	return b;
		// assign channel footprints after global allocation is complete
		return footprint_base<channel_tag>::
			__set_channel_footprints(sm);
#else
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
#endif	// BUILTIN_CHANNEL_FOOTPRINTS
	} else {
		// error
		return a;
	}
}	// end method expand_unique_subinstances

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This recursively transforms a port_member_context to local
	frame.  This copies globally assigned indices passed in
	external context into the corresponding (internal) footprint frame.  
	TODO: English, please?
	TODO: iterate over pools.  
 */
void
footprint::assign_footprint_frame(footprint_frame& ff, 
		const port_member_context& pmc) const {
	STACKTRACE_VERBOSE;
	get_instance_collection_pool_bundle<process_tag>()
		.assign_footprint_frame(ff, pmc);
	get_instance_collection_pool_bundle<channel_tag>()
		.assign_footprint_frame(ff, pmc);
#if ENABLE_DATASTRUCTS
	get_instance_collection_pool_bundle<datastruct_tag>()
		.assign_footprint_frame(ff, pmc);
#endif
	get_instance_collection_pool_bundle<int_tag>()
		.assign_footprint_frame(ff, pmc);
	get_instance_collection_pool_bundle<enum_tag>()
		.assign_footprint_frame(ff, pmc);
	get_instance_collection_pool_bundle<bool_tag>()
		.assign_footprint_frame(ff, pmc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Final pass over unique instances to check for connectivity violations, 
	and issue warnings.  
	This was introduced to check channel connectivity.  
	Q: Should we store diagnostic summaries as flags?  
		Could be useful for hierarchy checking.  
	\pre already passed over CHP for channel connectivity.  
	\return true to indicate acceptance.  
 */
good_bool
footprint::connection_diagnostics(void) const {
	return scope_aliases.check_channel_connections();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre chp_footprint has already been created.  
 */
void
footprint::allocate_chp_events(void) {
if (chp_footprint) {
	CHP::local_event_allocator v(chp_event_footprint);
	chp_footprint->accept(v);
	v.compact_and_canonicalize();
}	// else nothing to do
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prefixless wrapper.  
	Called from top-level only, in module::__cflat().
 */
void
footprint::cflat_aliases(ostream& o, const state_manager& sm, 
		const cflat_options& cf) const {
	STACKTRACE_VERBOSE;
	wire_alias_set wires;
	const global_entry_pool<bool_tag>& gbp(sm.get_pool<bool_tag>());
	const size_t s = gbp.size();
	if (cf.wire_mode && !cf.check_prs) {
		// reserve alias slots for all uniquely allocated bools
		wires.resize(s);
	}
	alias_printer v(o, sm, *this, NULL, cf, wires, string());
	accept(v);
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
	Visits all physical instances in instance_collection_map.  
	e.g. this is used to cflat aliases in the instance hierarchy.  
	TODO: iterate over pools.  Save this for last.  
 */
void
footprint::accept(alias_visitor& v) const {
	// NOTE: not polymorphic, doesn't call v.visit(*this);
	STACKTRACE_VERBOSE;
	const_instance_map_iterator i(instance_collection_map.begin());
	const const_instance_map_iterator e(instance_collection_map.end());
	// cerr << instance_collection_map.size() << " collections." << endl;
	for ( ; i!=e; i++) {
		// for now preserve the same order as before
		// even if less efficient... until everything re-verified
		// then we can iterate more efficiently through the pools.
		const never_ptr<const physical_instance_collection>
			coll_ptr((*this)[i->second].
				is_a<const physical_instance_collection>());
		if (coll_ptr) {
			coll_ptr->accept(v);
		}
		// skip parameters
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Counterpart to this is physical_instance_collection::write_pointer.
 */
footprint::instance_collection_ptr_type
footprint::read_pointer(istream& i) const {
	collection_map_entry_type e;
	read_value(i, e.meta_type);
	read_value(i, e.pool_type);
	read_value(i, e.index);
	return (*this)[e];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::collect_transient_info_base(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	// no need to visit def_back_ref
	footprint_base<process_tag>::collect_transient_info_base(m);
	footprint_base<channel_tag>::collect_transient_info_base(m);
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>::collect_transient_info_base(m);
#endif
	footprint_base<enum_tag>::collect_transient_info_base(m);
	footprint_base<int_tag>::collect_transient_info_base(m);
	footprint_base<bool_tag>::collect_transient_info_base(m);
	// value_footprint_bases don't have pointers
	prs_footprint->collect_transient_info_base(m);
	// now we need to register it because locally allocated events
	// may now contain a live pointer to the top-concurrent-actions
	if (chp_footprint) {
		chp_footprint->collect_transient_info(m);
	}
	// alternative is to hack an exception... not worth it
	// this *shouldn't* be necessary, but also performs sanity check
	chp_event_footprint.collect_transient_info_base(m);
	spec_footprint->collect_transient_info_base(m);
	// scope/port alias_sets don't have pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::collect_transient_info(persistent_object_manager& m) const {
	STACKTRACE_PERSISTENT_VERBOSE;
if (!m.register_transient_object(this,
		util::persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: to preserve precise hash_table ordering, we use a copy
	of the hash-table with its internal buckets reversed, 
	so that reconstruction will occur in the desired original order.  
 */
void
footprint::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, unrolled);
	write_value(o, created);
	// reconstruct the map AFTER all collections are loaded
	footprint_base<process_tag>::write_reserve_sizes(o);
	footprint_base<channel_tag>::write_reserve_sizes(o);
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>::write_reserve_sizes(o);
#endif
	footprint_base<enum_tag>::write_reserve_sizes(o);
	footprint_base<int_tag>::write_reserve_sizes(o);
	footprint_base<bool_tag>::write_reserve_sizes(o);

	footprint_base<process_tag>::write_object_base(m, o);
	footprint_base<channel_tag>::write_object_base(m, o);
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>::write_object_base(m, o);
#endif
	footprint_base<enum_tag>::write_object_base(m, o);
	footprint_base<int_tag>::write_object_base(m, o);
	footprint_base<bool_tag>::write_object_base(m, o);

	value_footprint_base<pbool_tag>::write_object_base(m, o);
	value_footprint_base<pint_tag>::write_object_base(m, o);
	value_footprint_base<preal_tag>::write_object_base(m, o);

#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
	port_aliases.write_object_base(*this, o);
	scope_aliases.write_object_base(*this, o);
#endif

	prs_footprint->write_object_base(m, o);
	m.write_pointer(o, chp_footprint);
	// persistent_object_manager will pick up chp_footprint
	chp_event_footprint.write_object_base(m, o);
	// alternative: re-construct event footprint upon loading?
	spec_footprint->write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::write_object(const persistent_object_manager& m, ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	CRITICAL: should allocate all pools first before loading
	collections because subinstance collections (of aliases) reference
	collections by index.  After allocating pools, ordering doesn't matter.
	Is really only critical for instance collections and hierarchies. 
 */
void
footprint::load_object_base(const persistent_object_manager& m, istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, unrolled);
	read_value(i, created);
	// load all collections first, THEN reconstruct the local map
	footprint_base<process_tag>::load_reserve_sizes(i);
	footprint_base<channel_tag>::load_reserve_sizes(i);
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>::load_reserve_sizes(i);
#endif
	footprint_base<enum_tag>::load_reserve_sizes(i);
	footprint_base<int_tag>::load_reserve_sizes(i);
	footprint_base<bool_tag>::load_reserve_sizes(i);

	footprint_base<process_tag>::load_object_base(m, i);
	footprint_base<channel_tag>::load_object_base(m, i);
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>::load_object_base(m, i);
#endif
	footprint_base<enum_tag>::load_object_base(m, i);
	footprint_base<int_tag>::load_object_base(m, i);
	footprint_base<bool_tag>::load_object_base(m, i);

	value_footprint_base<pbool_tag>::load_object_base(m, i);
	value_footprint_base<pint_tag>::load_object_base(m, i);
	value_footprint_base<preal_tag>::load_object_base(m, i);
	// \pre placeholders have aleady been loaded

#if AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
	// instead of writing redundant information, reconstruct it!
	if (created) {
		evaluate_scope_aliases();
	}
#else
	port_aliases.load_object_base(*this, i);
	scope_aliases.load_object_base(*this, i);
#endif

	prs_footprint->load_object_base(m, i);
	m.read_pointer(i, chp_footprint);
	// alternative: re-construct event footprint upon loading?
	chp_event_footprint.load_object_base(m, i);
	spec_footprint->load_object_base(m, i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::load_object(const persistent_object_manager& m, istream& i) {
	load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

