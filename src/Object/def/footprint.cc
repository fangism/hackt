/**
	\file "Object/def/footprint.cc"
	Implementation of footprint class. 
	$Id: footprint.cc,v 1.66 2011/03/23 00:36:09 fang Exp $
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
#include "Object/global_entry.tcc"
#include "Object/global_entry_dumper.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#endif
#include "Object/common/cflat_args.h"
#include "Object/common/alias_string_cache.h"
#include "Object/common/dump_flags.h"
#include "Object/inst/alias_visitor.h"
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
#include "Object/traits/value_traits.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/pstring_const.h"
#include "Object/def/user_def_datatype.h"
#include "Object/def/process_definition.h"
#if IMPLICIT_SUPPLY_PORTS
#include "Object/inst/bool_instance_collection.h"	// for bool_scalar
#include "Object/type/process_type_reference.h"
#endif
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
#include "Object/inst/pstring_instance.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/CHP.h"
#include "Object/lang/CHP_event_alloc.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/persistent_type_hash.h"
#if ENABLE_STACKTRACE
#include "Object/expr/expr_dump_context.h"
#endif
#include "Object/unroll/unroll_context.h"
#include "Object/interfaces/VCDwriter.h"	// should belong elsewhere
#include "common/TODO.h"
#include "main/cflat_options.h"

#include "util/compose.h"
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
USING_UTIL_COMPOSE

//=============================================================================
// class footprint_base method definitions

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
footprint_base<Tag>::__expand_unique_subinstances(void) {
	STACKTRACE_VERBOSE;
	typedef	typename instance_pool_type::iterator	iterator;
	// pool already sized in partition_local...
	iterator i(_instance_pool->begin());	// first entry valid, 0-based
	const iterator e(_instance_pool->end());
	for ( ; i!=e; ++i) {
		state_instance<Tag>& ref(*i);
		footprint_frame& target_frame(ref._frame);
		// done by canonical_type
		const instance_alias_info<Tag>&
			actual_alias(*ref.get_back_ref());
		// INVARIANT(actual_alias has complete type)
		if (!actual_alias.allocate_assign_subinstance_footprint_frame(
				target_frame).good) {
			ICE_EXIT(cerr);
		}
		const footprint* formal_fp(target_frame._footprint);
		NEVER_NULL(formal_fp);
		// for each meta-type
		const port_alias_tracker&
			formal_ports(formal_fp->get_scope_alias_tracker());
		formal_ports.assign_alias_frame(actual_alias, target_frame);
#if ENABLE_STACKTRACE
		const footprint_frame& frame(target_frame);
		frame.dump_type(STACKTRACE_INDENT << "frame type: ") << endl;
		frame.dump_frame(STACKTRACE_INDENT << "filled frame: ") << endl;
#endif
		// the allocate private subinstances
	}	// end for
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If f has non-zero private subinstances, then append an 
	entry to the map.
	\param i index of the current sub-process.  1-indexed.
 */
template <class Tag>
void
footprint_base<Tag>::__append_private_map_entry(const this_type& f,
		const size_t i) {
	const size_t p = f._instance_pool->total_private_entries();
	typedef	typename instance_pool_type::private_entry_map_type
				private_entry_map_type;
	private_entry_map_type& pm(this->_instance_pool->private_entry_map);
	INVARIANT(pm.size());	// always starts with sentinel entry
	if (p) {
		// set the index of the *previous* entry
		typename instance_pool_type::private_map_entry_type&
			l(pm.back());
		INVARIANT(!l.first);
		// sanity check: previous index (invalid) is 0
		l.first = i;
		// the next entry will start at l.second +p (cumulative)
		pm.push_back(std::make_pair(0, l.second +p));
		// reminder: reference to l no longer valid after push_back
	}
	// otherwise, don't bother appending an entry
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if BUILTIN_CHANNEL_FOOTPRINTS
/**
	Establishes the built-in channel footprint.
	Called from top-level only.  
 */
template <class Tag>
good_bool
footprint_base<Tag>::__set_channel_footprints(void) {
	typedef	global_entry_pool<Tag>		global_pool_type;
	// typedef	typename global_pool_type::iterator	pool_iterator;
	STACKTRACE_VERBOSE;
{
	// this covers top-level channels only
	typedef	typename instance_pool_type::iterator	iterator;
	iterator i(_instance_pool->begin());		// skip NULL?
	const iterator e(_instance_pool->end());
	for ( ; i!=e; ++i) {
		global_entry<Tag>& ref(*i);
		const instance_alias_info<Tag>&
			formal_alias(*i->get_back_ref());
		NEVER_NULL(formal_alias.container);	// container back-ref
		const instance_collection<Tag>&
			col(formal_alias.container->get_canonical_collection());
		// can now access the channel_collection_type_manager base
		ref.channel_type = col.__get_raw_type().get_base_type();
		// possibly re-cache/compute structural summaries
	}
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

// for dummy construction
const temp_footprint_tag_type
temp_footprint_tag = temp_footprint_tag_type();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
	value_footprint_base<pstring_tag>(), 
	prs_footprint(new PRS::footprint), 
	spec_footprint(new SPEC::footprint),
	warning_count(0), 
	lock_state(false) { }
// the other members, don't care, just placeholder ctor before loading object

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partial constructor, ONLY INTENDED for the footprint_manager to use.
	To finish constructing, use __reconstruct().
 */
footprint::footprint(const const_param_expr_list& p,
	const util::uninitialized_tag_type& x) :
	footprint_base<process_tag>(x), 
	footprint_base<channel_tag>(x), 
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>(x), 
#endif
	footprint_base<enum_tag>(x), 
	footprint_base<int_tag>(x), 
	footprint_base<bool_tag>(x), 
	value_footprint_base<pbool_tag>(x), 
	value_footprint_base<pint_tag>(x), 
	value_footprint_base<preal_tag>(x), 
	value_footprint_base<pstring_tag>(x), 
	param_key(p) {
	// still leaves two calls to port_alias_tracker's default ctor
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ewww... partial placement construction and destruction.  
	This should only ever be called after a partial construct.
 */
void
footprint::__reconstruct(const const_param_expr_list& p, 
	const definition_base& d) {
	param_key.~const_param_expr_list();
	new (this) this_type(p, d);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The actual initializating constructor.
	Called in-place?
 */
footprint::footprint(const const_param_expr_list& p,
	const definition_base& d) :
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
	value_footprint_base<pstring_tag>(), 
	param_key(p), 
	owner_def(&d),
	created(false),
	instance_collection_map(), 
	// use half-size pool chunks to reduce memory waste for now
	// maybe even quarter-size...
	scope_aliases(), 
	port_aliases(),
	prs_footprint(new PRS::footprint), 
	chp_footprint(NULL), 	// allocate when we actually need it
	chp_event_footprint(), 
	spec_footprint(new SPEC::footprint), 
	warning_count(0),
	lock_state(false) {
	STACKTRACE_CTOR_VERBOSE;
	NEVER_NULL(prs_footprint);
	NEVER_NULL(spec_footprint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// default construct every member! we don't care...
// except that we need them for dumping... arg!
// WISH: delegating ctors (C++0x)
footprint::footprint(const temp_footprint_tag_type&) :
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
	value_footprint_base<pstring_tag>(), 
	created(false),
	instance_collection_map(), 
	scope_aliases(), 
	port_aliases(),
	prs_footprint(new PRS::footprint), 
	chp_footprint(NULL), 	// allocate when we actually need it
	chp_event_footprint(), 
	spec_footprint(new SPEC::footprint),
	warning_count(0),
	lock_state(false) {
	STACKTRACE_CTOR_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This doesn't actually copy!
	Is just another default ctor.
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
	value_footprint_base<pstring_tag>(), 
	param_key(t.param_key), 
	owner_def(t.owner_def),
	created(false),
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
meta_type_tag_enum
footprint::get_meta_type(void) const {
	return owner_def->get_meta_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint::what(ostream& o) const {
	return o << "footprint";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: distinguish strict from relaxed template parameters
 */
ostream&
footprint::dump_type(ostream& o) const {	// dump_flags
	// cast as a scopespace? cross-cast definition_base to name_space?
	// be careful if owner-def is a namespace, key will be file-name!
	owner_def->dump_qualified_name(o, dump_flags::default_value);
//	o << owner_def->get_key();
	param_key.dump(o << '<', expr_dump_context::default_value) << '>';
		// context doesn't matter, they are constants anyhow
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
footprint::dump(ostream& o) const {
	// created?
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
	// TODO: process may have no instances, but still have CHP events!
	if (!instance_collection_map.empty()) {
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
		// kind of want this for top-level footprint printing
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

ostream&
footprint::dump_with_collections(ostream& o) const {
	return dump_with_collections(o, dump_flags::default_value, 
		expr_dump_context::default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Suitable for listing members that were actually instantiated, 
	as opposed to those names appearing in the scopespace.
 */
ostream&
footprint::dump_member_list(ostream& o) const {
	typedef	vector<never_ptr<const physical_instance_collection> >
		instance_list_type;
	typedef	vector<never_ptr<const param_value_collection> >
		value_list_type;
	value_list_type value_list;
	instance_list_type instance_list;
	const_instance_map_iterator
		i(instance_collection_map.begin()),
		e(instance_collection_map.end());
	// sift: want partition-like algorithm (not in-place)
	for ( ; i!=e; ++i) {
		const instance_collection_ptr_type p((*this)[i->second]);
		NEVER_NULL(p);
		const never_ptr<const physical_instance_collection>
			pi(p.is_a<const physical_instance_collection>());
		if (pi) {
			instance_list.push_back(pi);
		} else {
			const never_ptr<const param_value_collection>
				pv(p.is_a<const param_value_collection>());
			NEVER_NULL(pv);
			value_list.push_back(pv);
		}
	}
if (value_list.size()) {
	o << auto_indent << "Parameters:" << endl;
	value_list_type::const_iterator
		pi(value_list.begin()), pe(value_list.end());
	INDENT_SECTION(o);
	for ( ; pi != pe; ++pi) {
		o << auto_indent << (*pi)->get_footprint_key() << " = ";
		(*pi)->type_dump(o);
		const size_t d = (*pi)->get_dimensions();
		if (d) o << '^' << d;
		o << endl;
	}
}
if (instance_list.size()) {
	o << auto_indent << "Instances:" << endl;
	instance_list_type::const_iterator
		pi(instance_list.begin()), pe(instance_list.end());
	INDENT_SECTION(o);
	for ( ; pi != pe; ++pi) {
		o << auto_indent << (*pi)->get_footprint_key() << " = ";
		(*pi)->type_dump(o);
		const size_t d = (*pi)->get_dimensions();
		if (d) o << '^' << d;
		o << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param r the rule which must belong to the member PRS footprint.
 */
ostream&
footprint::dump_rule(ostream& o, const PRS::footprint_rule& r) const {
	NEVER_NULL(prs_footprint);
	return prs_footprint->dump_rule(r, o, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just collect list of local names of instantiated things, 
	should be weak-subset of instance names in corresponding scopespace.
 */
void
footprint::export_instance_names(vector<string>& v) const {
	const_instance_map_iterator
		i(instance_collection_map.begin()),
		e(instance_collection_map.end());
	for ( ; i!=e; ++i) {
		const instance_collection_ptr_type p((*this)[i->second]);
		NEVER_NULL(p);
		const never_ptr<const physical_instance_collection>
			pi(p.is_a<const physical_instance_collection>());
		if (pi) {
			v.push_back(i->first);
		}
	}
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
#if 0
	STACKTRACE_INDENT_PRINT("footprint looking up: " << k << endl);
	dump_with_collections(cerr << "we have: " << endl,
		dump_flags::default_value, expr_dump_context::default_value);

#endif
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
//	STACKTRACE_VERBOSE;
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
	case META_TYPE_PSTRING:
		return value_footprint_base<meta_type_map<META_TYPE_PSTRING>::type>::
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
	\param top top-level footprint for looking up global parameters?
 */
good_bool
footprint::create_dependent_types(const unroll_context& c) {
	STACKTRACE_VERBOSE;
	const footprint& top(*c.get_top_footprint());
#if ENABLE_STACKTRACE
	dump_type(STACKTRACE_STREAM << "*** CREATING TYPE: ") << endl;
#endif
try {
//	create_lock LOCK(*this);	// will catch recursion errors
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
	// final pass (backpatching) to instantiate processes
	// with relaxed types, whose types were implicitly bound 
	// *after* aliasing to a complete-typed alias.  
	// this performs the appropriate recursive connections
	// will instantiate into this footprint
	// only processes have relaxed types and substructure
	if (!get_instance_collection_pool_bundle<process_tag>()
			.finalize_substructure_aliases(c).good) {
		return good_bool(false);
	}
	// this call must precede collecting of scope aliases and
	// shorten_canonical_aliases
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

	evaluate_scope_aliases(true);

	// should this be postponed until connection_diagnostics()?
	// after evaluate_scope_aliases, indices have been adjusted so
	//	all publicly reachable indices are before private ones,
	//	now we can set the number of private entries
	partition_local_instance_pool();
	expand_unique_subinstances();
	construct_private_entry_map();

	// for all structures with private subinstances (processes)
	//	publicly reachable local processes that are aliased to a port
	mark_created();
#if ENABLE_STACKTRACE
	dump_type(STACKTRACE_STREAM << "*** DONE CREATING: ") << endl;
#endif
#if 0
	// check inside process_definition::create_complete_type instead
	if (warning_count) {
		dump_type(cerr << "Warnings found (" << warning_count <<
			") while creating complete type ") << "." << endl;
	}
#endif
	return good_bool(true);
} catch (...) {
	// expect recursion errors to trigger this
	dump_type(cerr << "Error creating footprint of complete type ")
		<< "." << endl;
	return good_bool(false);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print canonical name.
	\param r the type-index pair, where index is 1-based.
 */
ostream&
footprint::dump_canonical_name(ostream& o,
		const global_indexed_reference& r, 
		const dump_flags& df) const {
	INVARIANT(r.second);
	const size_t i = r.second -1;	// adjust to 0-based
switch (r.first) {
	case META_TYPE_PROCESS:
		return dump_canonical_name<process_tag>(o, i, df);
	case META_TYPE_CHANNEL:
		return dump_canonical_name<channel_tag>(o, i, df);
#if ENABLE_DATASTRUCTS
	case META_TYPE_STRUCT:
		return dump_canonical_name<datastruct_tag>(o, i, df);
#endif
	case META_TYPE_BOOL:
		return dump_canonical_name<bool_tag>(o, i, df);
	case META_TYPE_INT:
		return dump_canonical_name<int_tag>(o, i, df);
	case META_TYPE_ENUM:
		return dump_canonical_name<enum_tag>(o, i, df);
	default:	o << "<Unhandled-TAG>";
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects all aliases, deeply-recursively.
	\param r the type-index pair, where index is 1-based.
 */
void
footprint::collect_aliases_recursive(const global_indexed_reference& r, 
		const dump_flags& df, 
		set<string>& aliases) const {
	INVARIANT(r.second);
	const size_t i = r.second -1;	// adjust to 0-based
switch (r.first) {
	case META_TYPE_PROCESS:
		collect_aliases_recursive<process_tag>(i, df, aliases); break;
	case META_TYPE_CHANNEL:
		collect_aliases_recursive<channel_tag>(i, df, aliases); break;
#if ENABLE_DATASTRUCTS
	case META_TYPE_STRUCT:
		collect_aliases_recursive<datastruct_tag>(i, df, aliases); break;
#endif
	case META_TYPE_BOOL:
		collect_aliases_recursive<bool_tag>(i, df, aliases); break;
	case META_TYPE_INT:
		collect_aliases_recursive<int_tag>(i, df, aliases); break;
	case META_TYPE_ENUM:
		collect_aliases_recursive<enum_tag>(i, df, aliases); break;
	default:	cerr << "<Unhandled-TAG>";
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not used, written for the sake of instantiating
	the template functions.
 */
void
footprint::__dummy_get_instance(void) const {
	ICE_NEVER_CALL(cerr);
	get_instance<process_tag>(0);
	get_instance<channel_tag>(0);
#if ENABLE_DATASTRUCTS
	get_instance<datastruct_tag>(0);
#endif
	get_instance<enum_tag>(0);
	get_instance<int_tag>(0);
	get_instance<bool_tag>(0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
footprint_base<Tag>::__partition_local_instance_pool(
		const port_alias_tracker& st) {
	this->_instance_pool->resize(st.template local_pool_size<Tag>());
	this->_instance_pool->_port_entries =
		st.template port_frame_size<Tag>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the public/private boundary index in the meta-type pools.
	Also resizes the local pools, based on number of unique aliases.
 */
void
footprint::partition_local_instance_pool(void) {
	STACKTRACE_VERBOSE;
	footprint_base<process_tag>::__partition_local_instance_pool(scope_aliases);
	footprint_base<channel_tag>::__partition_local_instance_pool(scope_aliases);
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>::__partition_local_instance_pool(scope_aliases);
#endif
	footprint_base<enum_tag>::__partition_local_instance_pool(scope_aliases);
	footprint_base<int_tag>::__partition_local_instance_pool(scope_aliases);
	footprint_base<bool_tag>::__partition_local_instance_pool(scope_aliases);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct map that maps indices to their local
	parent instance structures.
	Indices that exceed the local public maximum index
	will look up these maps, by first subtracting the public offset,
	and the remainder is used to identify the process to which
	it belongs.  Only applies to processes as they are the only
	meta-type that can have private (non-port) subinstances.  
 */
void
footprint::construct_private_entry_map(void) {
	STACKTRACE_VERBOSE;
	// traverse all *private-local* process instances, skip process-ports
	// add entry of pairs to map for every non-empty process
	// needs to be accumulated to set _private_entries
	typedef	footprint_base<process_tag>::instance_pool_type
					process_pool_type;
	typedef	process_pool_type::const_iterator	const_iterator;
	process_pool_type& pp(get_instance_pool<process_tag>());
	size_t index = pp.port_entries() +1;	// index map key is 1-based
	const_iterator i(pp.local_private_begin()), e(pp.end());
	for ( ; i!=e; ++i, ++index) {
		// CAUTION: do not re-visit subordinate processes ports
		// as they would be double-counted when visiting
		// their parents!  but what about aliases???
		// tried set of covered pid's, but failed...
		const footprint* spf = i->_frame._footprint;
		INVARIANT(spf);
		append_private_map_entry(*spf, index);
	}
	// last entry in each pool points to "past the end"
	// need to back-patch the index to non-zero to maintain monotonicity
	// last_entry.second is the cumulative number of private entries
	finish_instance_pools(index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::finish_instance_pools(const size_t index) {
	STACKTRACE_VERBOSE;
	get_instance_pool<process_tag>().private_entry_map.back().first = index;
	get_instance_pool<channel_tag>().private_entry_map.back().first = index;
#if ENABLE_DATASTRUCTS
	get_instance_pool<datastruct_tag>().private_entry_map.back().first = index;
#endif
	get_instance_pool<enum_tag>().private_entry_map.back().first = index;
	get_instance_pool<int_tag>().private_entry_map.back().first = index;
	get_instance_pool<bool_tag>().private_entry_map.back().first = index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::append_private_map_entry(const footprint& sf, const size_t i) {
	STACKTRACE_VERBOSE;
	footprint_base<process_tag>::__append_private_map_entry(sf, i);
	footprint_base<channel_tag>::__append_private_map_entry(sf, i);
#if ENABLE_DATASTRUCTS
	footprint_base<datastruct_tag>::__append_private_map_entry(sf, i);
#endif
	footprint_base<enum_tag>::__append_private_map_entry(sf, i);
	footprint_base<int_tag>::__append_private_map_entry(sf, i);
	footprint_base<bool_tag>::__append_private_map_entry(sf, i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This forcibly zeros out the count of top-level ports, 
	which should eliminated the need for any special cases
	at the top-level.  
	Only the top-level module should ever call this.
 */
void
footprint::zero_top_level_ports(void) {
	get_instance_pool<process_tag>()._port_entries = 0;
	get_instance_pool<channel_tag>()._port_entries = 0;
#if ENABLE_DATASTRUCTS
	get_instance_pool<datastruct_tag>()._port_entries = 0;
#endif
	get_instance_pool<enum_tag>()._port_entries = 0;
	get_instance_pool<int_tag>()._port_entries = 0;
	get_instance_pool<bool_tag>()._port_entries = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes all instances that share the same key as collections
	found in the referenced scopespace.  
	Purpose is to fake process-type creation while preserving
	top-level parameters that are referenced out-of-scope.  
	This works because top-level parameters have already been resolved
	to constants.  
	It should be sufficient to just dissociate the map?
	This means that new parameter collection 
 */
void
footprint::remove_shadowed_collections(const scopespace& s) {
	scopespace::const_map_iterator
		i(s.id_map_begin()), e(s.id_map_end());
for ( ; i!=e; ++i) {
	const string& k(i->first);
	const instance_collection_map_type::iterator
		mf(instance_collection_map.find(k));
	if (mf != instance_collection_map.end()) {
		const collection_map_entry_type& x(mf->second);
	switch (x.meta_type) {
	case META_TYPE_PBOOL:
	case META_TYPE_PINT:
	case META_TYPE_PREAL:
	case META_TYPE_PSTRING:
		// dissociate, rather than destroy
		instance_collection_map.erase(mf);
		break;
	default: break;
	// ignore all other collections, they pose no threat
	}	// end switch
	}	// end if
}	// end for
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if IMPLICIT_SUPPLY_PORTS
/**
	Define to 1 to check for unique canonical visits to 
	process port aliases. 
	Goal: 0 because of they way automatic connections are determined.
	This doesn't work anymore because of the way we check 
	bool aliases before automatically connecting to !GND/!Vdd.
	But now we need to beware that not all aliases in the same union-find
	set will have complete type in the case of relaxed templates.  
 */
#define	VISIT_UNIQUE_PROCESS_ALIAS			0

// temporary debugging flag
#define	DEBUG_IMPLICIT_SUPPLY				0

/**
	Collect all top-level process instances in this scope.
	Intend to automatically connect implicit supply ports. 
	Note: this does NOT yet support internal node types!
 */
struct implicit_supply_connector : public alias_visitor {
	typedef	instance_alias_info<process_tag>	alias_type;
	typedef	instance_alias_info<bool_tag>		node_type;
	const unroll_context&				c;
#if VISIT_UNIQUE_PROCESS_ALIAS
	std::set<const alias_type*>			visited;
#endif
	/**
		
	 */
	node_type&					GND;
	node_type&					Vdd;
	bool						err;
	// default GND
	// default Vdd
	implicit_supply_connector(const unroll_context& _c, 
		node_type& g, node_type& v) : 
		c(_c), 
#if VISIT_UNIQUE_PROCESS_ALIAS
		visited(), 
#endif
		GND(g), Vdd(v), err(false) { }

VISIT_INSTANCE_ALIAS_INFO_PROTOS(CPP_EMPTY)

private:
	static
	bool
	__auto_connect_port(const alias_type&, const unroll_context&, 
		const physical_instance_placeholder&, node_type&);
};	// end struct implicit_supply_connector

#define	DEFINE_SCOPE_PROCESS_VISIT_NULL(Tag)				\
void									\
implicit_supply_connector::visit(const instance_alias_info<Tag>&) { }

DEFINE_SCOPE_PROCESS_VISIT_NULL(bool_tag)
DEFINE_SCOPE_PROCESS_VISIT_NULL(enum_tag)
DEFINE_SCOPE_PROCESS_VISIT_NULL(int_tag)
DEFINE_SCOPE_PROCESS_VISIT_NULL(channel_tag)

#undef	DEFINE_SCOPE_PROCESS_VISIT_NULL

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Not recursive, just meant for top-level processes 
	instance collections, local to this scope.  
	In case of process aliases, just visit the canonical process once.
	TODO: visit process port members of processes (recursive!)
 */
void
implicit_supply_connector::visit(const instance_alias_info<process_tag>& _p) {
	STACKTRACE_VERBOSE;
#if VISIT_UNIQUE_PROCESS_ALIAS
#if 1
	const alias_type::pseudo_const_iterator cpi(_p.find());
	if (!cpi->has_complete_type()) {
		return;
	}
#else
	// problem: breaks union-find connectivity if is not canonical
	const alias_type::pseudo_const_iterator
		cpi(_p.find_complete_type_alias());
#endif
	const alias_type& cp(*cpi);
#else
	// visit all processes, even if not canonical!
	// but may not necessarily have complete type, 
	// but should we still be able to connect its ports?
	const alias_type& cp(_p);
#endif
#if VISIT_UNIQUE_PROCESS_ALIAS
	const std::set<const alias_type*>::const_iterator
		f(visited.find(&cp));	// canonical
if (f == visited.end()) {
	// not found, first time
	visited.insert(&cp);
#endif
#if DEBUG_IMPLICIT_SUPPLY
	cp.dump_hierarchical_name(cerr << "implicit ports: ") << endl;
#endif
#if !VISIT_UNIQUE_PROCESS_ALIAS
if (cp.has_complete_type()) {
#if DEBUG_IMPLICIT_SUPPLY
	cerr << "has complete type." << endl;
#endif
	// recurse into process ports of processes
	cp.subinstance_manager::accept(*this);
#endif
	// yes, we intend to modify!
	// lookup Vdd port and GND port
	// need type information of _p to get port placeholders
	const process_definition&
		pd(cp.container->get_canonical_collection()
			.get_placeholder()->get_unresolved_type_ref()
			.is_a<const process_type_reference>()
			// ->make_canonical_process_type_reference()
			->get_base_proc_def()->get_canonical_proc_def());
	// must canonicalize to take care of typedefs
	const port_formals_manager& pfm(pd.get_port_formals());
	const size_t imp = pfm.implicit_ports();
	if (imp) {
		INVARIANT(imp == 2);		// Vdd, GND for now
		port_formals_manager::const_list_iterator fi(pfm.begin());
		// NOTE: keep this consistent with order in
		// "AST/globals.cc" and "net/netgraph.cc" and "AST/instance.cc"
		// first, GND
		if (__auto_connect_port(cp, c, **fi, GND))
			err = true;
		++fi;
		if (__auto_connect_port(cp, c, **fi, Vdd))
			err = true;
		// then, Vdd
	}
#if !VISIT_UNIQUE_PROCESS_ALIAS
} else {
	// else skip aliases with incomplete type
#if DEBUG_IMPLICIT_SUPPLY
	cerr << "has incomplete type." << endl;
#endif
#if 0
	INVARIANT(cp.peek() != &cp);
	// assert: this is not canonical
#endif
}
#endif
// else is already connected to a non-default supply node (override)
#if VISIT_UNIQUE_PROCESS_ALIAS
}	// else already visited this process alias
#endif
}	// end implicit_supply_connector::visit

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true on error.
 */
bool
implicit_supply_connector::__auto_connect_port(const alias_type& cp,
		const unroll_context& c, 
		const physical_instance_placeholder& a, node_type& n) {
	typedef	port_actual_collection<bool_tag>	bool_port;
	STACKTRACE_VERBOSE;
	// problem: if alias is not canonical, 
	// not guaranteed to have complete type
	const subinstance_manager::entry_value_type
		bc(cp.lookup_port_instance(a));
	NEVER_NULL(bc);
	const never_ptr<bool_port> bs(bc.is_a<bool_port>());
	NEVER_NULL(bs);
	INVARIANT(bs->collection_size() == 1);	// is scalar
#if 0
	node_type& lg(*bs->begin()->find());	// must link from canonical
	// FIXME: peek-check will ALWAYS be true for canonical nodes!
#else
	node_type& lg(*bs->begin());	// don't follow canonical yet!
#endif
#if DEBUG_IMPLICIT_SUPPLY
	lg.dump_hierarchical_name(cerr) << " -> ";
	lg.peek()->dump_hierarchical_name(cerr) << endl;
#endif
	if (lg.peek() == &lg) {
#if DEBUG_IMPLICIT_SUPPLY
		lg.dump_hierarchical_name(cerr << "auto-connecting: ") << endl;
#endif
		if (!bool_instance_alias_info::checked_connect_alias(
			lg, n, c).good) {
			return true;
			// should never happen
		}
		INVARIANT(lg.peek() == &n);
	}	// else is already connected to something else
#if DEBUG_IMPLICIT_SUPPLY
	else lg.dump_hierarchical_name(cerr << "not connecting: ") << endl;
#endif
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Will crash-fail.
 */
template <class Tag>
instance_alias_info<Tag>&
footprint::__lookup_scalar_port_alias(const string& s) const {
	const const_instance_map_iterator
		f(instance_collection_map.find(s)),
		e(instance_collection_map.end());
	INVARIANT(f != e);
	return ((*this)[f->second].template is_a<instance_array<Tag, 0> >()
		->get_the_instance());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Intended for use with implicit supply ports.  
	\param s must pass in a known-good port name!
 */
size_t
footprint::lookup_implicit_bool_port(const string& s) const {
	return __lookup_scalar_port_alias<bool_tag>(s).instance_index;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For all top-level process instances in this scope, 
	automatically connect implicit supply ports Vdd and GND, 
	if they are not already connected.  
	Only applicable to footprints of processes. 
 */
good_bool
footprint::connect_implicit_ports(const unroll_context& c) {
	STACKTRACE_VERBOSE;
	// find local Vdd and GND ports first
	const_instance_map_iterator
		mi(instance_collection_map.begin()), 
		me(instance_collection_map.end());
	implicit_supply_connector::node_type&
		gp(__lookup_scalar_port_alias<bool_tag>("!GND"));
	implicit_supply_connector::node_type&
		vp(__lookup_scalar_port_alias<bool_tag>("!Vdd"));
	implicit_supply_connector spc(c, gp, vp);
	// lookup the lone bool
for ( ; mi!=me; ++mi) {
	const never_ptr<instance_collection_base> b((*this)[mi->second]);
	const never_ptr<process_instance_collection>
		p(b.is_a<process_instance_collection>());
	// includes dense formals and sparse local collections
if (p) {
	// can be scalar or array, need to collect them all
	p->accept(spc);
	// p->get_all_aliases(vector&);
}
}	// end for process collections
	return good_bool(true);
}
#undef	VISIT_UNIQUE_PROCESS_ALIAS
#endif	// IMPLICIT_SUPPLY_PORTS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects all aliases in this scope and also creates a set
	of port aliases for the sake of replaying internal aliases.  
	\pre the sequential scope was already played for creation.  
	NOTE: this can also called during load-reconstruction.
	\param sift if true, then re-enumerate the local instance IDs
		such that publicly reachable ports aliases have lower ID
		than local non-port aliases.  This makes a clean
		partitioning for memory mapping.  
		Sifting only needs to be done once, can be skipped
		during persistent object reconstruction, because
		already ordered.
 */
void
footprint::evaluate_scope_aliases(const bool sift) {
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
if (sift) {
	// sift: make sure all public ports reachable instances have 
	// *lower* ID than private non-port instances
	// for now, implement as a check, and if needed, actually swap IDs
	// this must happen *before* calling evaluate_scope_aliases
	scope_aliases.sift_ports();
}
	// TODO:
	// at this point, indices are stable/frozen
	// iterate over unique substructures to populate footprint frames

	// just copy-filter them over
	port_aliases.import_port_aliases(scope_aliases);
	// don't filter for scope, want to keep around unique entries
	// scope_aliases.filter_uniques();
#if ENABLE_STACKTRACE
	scope_aliases.dump(cerr << "footprint::scope_aliases (before): " << endl) << endl;
#endif
	scope_aliases.shorten_canonical_aliases(*this);
//	port_aliases.shorten_canonical_aliases(*this);
	// NOTE: at this point, prs has not yet been processed
	// for connectivity diagnostics.
#if ENABLE_STACKTRACE
	scope_aliases.dump(cerr << "footprint::scope_aliases (after): " << endl) << endl;
	scope_aliases.dump(cerr << "footprint::port_aliases (after): " << endl) << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Perform local allocation of unique resources.
	\pre instance indices and pool sizes (from scope_aliases) frozen.
	This expands unique subinstances in each pool, 
	called for each footprint.  
	Expand everything in this footprint at this level first
	before recursing into public subinstances' ports.  
 */
good_bool
footprint::expand_unique_subinstances(void) {
	STACKTRACE_VERBOSE;
	// only processes, channels, and data structures need to be expanded
	// nothing else has substructure.  
	// no need for offsets, as allocation is done locally
	// no need to __allocate_local_state?
	// this is empty, needs to be assigned before passing down...
	// construct frame using offset?
#if 0
	footprint_frame ff(*this);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("FRAME ctor:" << endl);
	ff.dump_frame(cerr) << endl;
#endif
	ff.init_top_level();		// not sure if this is correct
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("FRAME: init" << endl);
	ff.dump_frame(cerr) << endl;
#endif
#endif
	// is this needed? does this just create the identity map?
	const good_bool b(
		footprint_base<process_tag>::
			__expand_unique_subinstances(
				// scope_aliases, ff
				).good
#if !BUILTIN_CHANNEL_FOOTPRINTS
		&& footprint_base<channel_tag>::
			__expand_unique_subinstances(
				// scope_aliases, ff
				).good
#endif
#if ENABLE_DATASTRUCTS
		&& footprint_base<datastruct_tag>::
			__expand_unique_subinstances(
				// scope_aliases, ff
				).good
#endif
	);
#if BUILTIN_CHANNEL_FOOTPRINTS
	if (!b.good)	return b;
	// assign channel footprints after global allocation is complete
	return footprint_base<channel_tag>::__set_channel_footprints();
#else
	return b;
#endif	// BUILTIN_CHANNEL_FOOTPRINTS
}	// end method expand_unique_subinstances

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Final pass over unique instances to check for connectivity violations, 
	and issue warnings.  
	This was introduced to check channel connectivity.  
	Q: Should we store diagnostic summaries as flags?  
		Could be useful for hierarchy checking.  
	\pre already passed over CHP for channel connectivity.  
	\param top is true if this is top-level, skip bool-PRS checking
		for the top-level footprint.
	\return true to indicate acceptance.  
 */
error_count
footprint::connection_diagnostics(const bool top) const {
	error_count ret(scope_aliases.check_channel_connections());
	if (!top) {
#if BOOL_PRS_CONNECTIVITY_CHECKING
		ret += scope_aliases.check_bool_connections();
#endif
#if PROCESS_CONNECTIVITY_CHECKING
		ret += scope_aliases.check_process_connections();
#endif
	}
	warning_count += ret.warnings;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre chp_footprint has already been created.  
 */
void
footprint::allocate_chp_events(void) {
	STACKTRACE_VERBOSE;
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
footprint::cflat_aliases(ostream& o,
		const cflat_options& cf) const {
	STACKTRACE_VERBOSE;
	wire_alias_set wires;
	const size_t s = get_instance_pool<bool_tag>().total_entries() +1;
	if (cf.wire_mode && !cf.check_prs) {
		// reserve alias slots for all uniquely allocated bools
		wires.resize(s);
		// WARNING: could be HUGE
	}
	const footprint_frame ff(*this);
	global_offset g;
if (cf.connect_style == cflat_options::CONNECT_STYLE_HIERARCHICAL) {
if (cf.primary_tool == cflat_options::TOOL_VCD) {
	VCD::VCDwriter v(ff, g, o);
	accept(v);
} else {
	hierarchical_alias_visitor v(ff, g);	// is quiet, but traverses
	accept(v);
}
} else {
	alias_printer v(o, ff, g, cf, wires, string());
	accept(AS_A(global_entry_context&, v));
	if (cf.wire_mode && cf.connect_style && !cf.check_prs) {
		// style need not be CONNECT_STYLE_WIRE, just not NONE
		// aliases were suppressed while accumulating
		// now print them in wire-accumulated form
		size_t j = 1;
		for ( ; j<s; j++) {
		const alias_string_cache& ac(wires[j]);
		if (!ac.strings.empty()) {
			o << "wire (";
			// NOTE: thus far, wire-style names are never quoted
			// currently, this does not respect cf.enquote_names.
			ostream_iterator<alias_string_cache::value_type>
				osi(o, ",");
			copy(ac.strings.begin(), ac.strings.end(), osi);
			dump_canonical_name<bool_tag>(o, j-1, 
				cf.__dump_flags) << ");" << endl;
		}
		// else is loner, has no aliases
		}
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i 1-based local process index.
 */
template <class Tag>
void
footprint::__set_global_offset_by_process(global_offset& g,
		const size_t i) const {
	g.global_offset_base<Tag>::offset =
		get_instance_pool<Tag>().locate_cumulative_entry(i).second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Construct relative offset.
	\param i is 1-based index into local process pool.
 */
void
footprint::set_global_offset_by_process(global_offset& g,
		const size_t i) const {
	STACKTRACE_VERBOSE;
	const state_instance<process_tag>::pool_type&
		p(get_instance_pool<process_tag>());
	INVARIANT(i <= p.local_entries());
	__set_global_offset_by_process<process_tag>(g, i);
	__set_global_offset_by_process<channel_tag>(g, i);
#if ENABLE_DATASTRUCTS
	__set_global_offset_by_process<datastruct_tag>(g, i);
#endif
	__set_global_offset_by_process<enum_tag>(g, i);
	__set_global_offset_by_process<int_tag>(g, i);
	__set_global_offset_by_process<bool_tag>(g, i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
inline
bool
bool_has_sub_fanin(const state_instance<bool_tag>& b) {
	return b.get_back_ref()->has_any_fanin();
}

static
inline
bool
bool_has_not_sub_fanin(const state_instance<bool_tag>& b) {
	return !bool_has_sub_fanin(b);
}

/**
        \return true for every node that is driven *locally*, 
                does not account for fanin from subprocesses.  
 */
void
footprint::has_sub_fanin_map(vector<bool>& ret) const {
	const state_instance<bool_tag>::pool_type&
		bp(get_instance_pool<bool_tag>());
        ret.reserve(bp.local_entries());
        transform(bp.begin(), bp.end(),
                back_inserter(ret),
	// g++/libstdc++ 4.4.4 c++0x mode chokes on back_inserter [PR 44963]
                std::ptr_fun(&bool_has_sub_fanin));
}

/**
        \return true for every node that is not driven *locally*, 
                does not account for fanin from subprocesses.  
 */
void
footprint::has_not_sub_fanin_map(vector<bool>& ret) const {
	const state_instance<bool_tag>::pool_type&
		bp(get_instance_pool<bool_tag>());
        ret.reserve(bp.local_entries());
        transform(bp.begin(), bp.end(),
                back_inserter(ret),
	// g++/libstdc++ 4.4.4 c++0x mode chokes on back_inserter [PR 44963]
		std::ptr_fun(&bool_has_not_sub_fanin));
#if 0
// debug only
	state_instance<bool_tag>::pool_type::const_iterator
		i(bp.begin()), e(bp.end());
	for ( ; i!=e; ++i) {
		i->dump(cerr);
		i->get_back_ref()->dump_raw_attributes(cerr << " ") << endl;
//		i->get_back_ref()->find()->dump_raw_attributes(cerr << " ") << endl;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For meta-type Tag, print dot nodes for all instances, 
	using canonical names.  
	Stolen from former state_manager::dump_dot_nodes().
 */
template <class Tag>
ostream&
footprint::__dump_instances_dot_nodes(ostream& o) const {
        size_t j = 0;
	const size_t e = get_instance_pool<Tag>().total_entries();
        for ( ; j<e; ++j) {
                o << class_traits<Tag>::tag_name << '_' << j+1 <<
                        "\t[style=bold,label=\"";
                // other styles: dashed, dotted, bold, invis
                dump_canonical_name<Tag>(o, j, 
			dump_flags::no_owners);	// 0-based index
                o << "\"];" << endl;
        }
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For graphviz/dot dumps.
	Prints all non-process instances as graph nodes.
 */
ostream&
footprint::dump_non_process_dot_nodes(ostream& o) const {
//	__dump_instances_dot_nodes<process_tag>(o);
	__dump_instances_dot_nodes<channel_tag>(o);
#if ENABLE_DATASTRUCTS
	__dump_instances_dot_nodes<datastruct_tag>(o);
#endif
	__dump_instances_dot_nodes<enum_tag>(o);
	__dump_instances_dot_nodes<int_tag>(o);
	__dump_instances_dot_nodes<bool_tag>(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	In order of global ID, print global frame information about
	every instance in the hierarchy, using computed offsets
	where applicable.
 */
template <class Tag>
ostream&
// footprint_base?
footprint::__dump_allocation_map(ostream& o) const {
	// traverse local pools first (instance_pool)
	// traverse private subinstances, by ID and offset
	//	by traversing private entry map, passing context
	// recall: local pools are populated by ports and non-ports

	// the following traversal is inefficient, but who cares?
	const typename state_instance<Tag>::pool_type&
		_pool(get_instance_pool<Tag>());
if (_pool.total_entries()) {
	o << "[global " << class_traits<Tag>::tag_name << " entries]" << endl;
	// empty top-level footprint frame, has no ports to pass in!
	const footprint_frame ff(*this);
	global_offset g;	// 0s
	// TODO: why not just have global_offset initialize to 1s for 1-based?
	// instead of adding 1 everywhere else?
	global_allocation_dumper<Tag> d(o, ff, g);
	this->accept(d);
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Takes place of state_manager::dump().
 */
ostream&
footprint::dump_allocation_map(ostream& o) const {
	o << global_entry_dumper::table_header << endl;
	__dump_allocation_map<process_tag>(o);
	__dump_allocation_map<channel_tag>(o);
#if ENABLE_DATASTRUCTS
	__dump_allocation_map<datastruct_tag>(o);
#endif
	__dump_allocation_map<enum_tag>(o);
	__dump_allocation_map<int_tag>(o);
	__dump_allocation_map<bool_tag>(o);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Visits all physical instances in instance_collection_map.  
	e.g. this is used to cflat aliases in the instance hierarchy.  
	NOTE: this only traverses from immediate subinstances.
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
void
footprint::accept(global_entry_context& v) const {
try {
	v.visit(*this);
} catch (...) {
	// print stack trace of instantiations
	v.report_instantiation_error(cerr);
	throw;
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
	param_key.collect_transient_info_base(m);
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
	// don't bother writing owner_def because it will be reconstructed
	// yes, write it 2x, preserve object binary format
	write_value(o, created);
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
	value_footprint_base<pstring_tag>::write_object_base(m, o);

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
void
footprint::write_param_key(const persistent_object_manager& m,
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	param_key.write_object(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Partially load first: param key is critical to parent map.
	Also load the owner definition reference while we're at it.
 */
void
footprint::load_param_key(const persistent_object_manager& m, istream& i,
		const definition_base& d) {
	STACKTRACE_PERSISTENT_VERBOSE;
	const_cast<const_param_expr_list&>(param_key).load_object(m, i);
	const_cast<never_ptr<const definition_base>&>(owner_def) =
		never_ptr<const definition_base>(&d);
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
	// don't bother reading owner_def because it will be reconstructed
	// yes, read it 2x, preserve object binary format
	read_value(i, created);
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
	value_footprint_base<pstring_tag>::load_object_base(m, i);
	// \pre placeholders have aleady been loaded

#if AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
	// instead of writing redundant information, reconstruct it!
	if (created) {
		evaluate_scope_aliases(false);
		partition_local_instance_pool();
		// top-level process should call zero_top_level_ports
		// after this reconstruction
		expand_unique_subinstances();
		construct_private_entry_map();
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
	lock_state = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
footprint::load_object(const persistent_object_manager& m, istream& i) {
	load_object_base(m, i);
}

//=============================================================================
// explicit template instantiations

template
const state_instance<channel_tag>&
footprint::get_instance<channel_tag>(const size_t) const;
template
const state_instance<process_tag>&
footprint::get_instance<process_tag>(const size_t) const;
template
const state_instance<bool_tag>&
footprint::get_instance<bool_tag>(const size_t) const;
template
const state_instance<int_tag>&
footprint::get_instance<int_tag>(const size_t) const;
template
const state_instance<enum_tag>&
footprint::get_instance<enum_tag>(const size_t) const;

template
ostream&
footprint::dump_canonical_name<process_tag>(ostream&, const size_t, 
	const dump_flags&, const bool) const;
template
ostream&
footprint::dump_canonical_name<channel_tag>(ostream&, const size_t, 
	const dump_flags&, const bool) const;
template
ostream&
footprint::dump_canonical_name<bool_tag>(ostream&, const size_t, 
	const dump_flags&, const bool) const;
template
ostream&
footprint::dump_canonical_name<int_tag>(ostream&, const size_t, 
	const dump_flags&, const bool) const;
template
ostream&
footprint::dump_canonical_name<enum_tag>(ostream&, const size_t, 
	const dump_flags&, const bool) const;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

