/**
	\file "Object/global_entry.tcc"
	$Id: global_entry.tcc,v 1.4.2.3 2005/11/02 06:17:53 fang Exp $
 */

#ifndef	__OBJECT_GLOBAL_ENTRY_TCC__
#define	__OBJECT_GLOBAL_ENTRY_TCC__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

#ifndef	STACKTRACE_PERSISTENTS
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE
#endif

#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include "util/macros.h"
#include "util/sstream.h"
#include "Object/global_entry.h"
#include "Object/state_manager.h"
#include "Object/def/footprint.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.tcc"	// for dump_complete_type
#include "Object/inst/instance_collection.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/common/dump_flags.h"
#if USE_CFLAT_CONNECT
#include "Object/common/alias_string_set.h"
#endif

#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/def/process_definition.h"
#include "Object/def/user_def_datatype.h"
#include "Object/type/canonical_generic_chan_type.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/IO_utils.h"
#include "common/TODO.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using std::ostringstream;
using std::transform;
using std::back_inserter;
using util::read_value;
using util::write_value;

//=============================================================================
/**
	\param sm the global state allocator.  
	\param gec the referencing entry with parent_id, and local_offset.
	\return reference to the parent super-instance global entry.  
 */
template <class Tag>
const global_entry<Tag>&
extract_parent_entry(const state_manager& sm, 
		const global_entry_common& gec) {
	const global_entry_pool<Tag>&
		gproc_pool(sm.template get_pool<Tag>());
	return gproc_pool[gec.parent_id];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resolves a global parent instance reference.
	\param Tag must be the tag of a meta type with substructure, 
		such as process_tag, channel_tag, datastruct_tag.  
	\param sm the global state allocator.  
	\param gec the referencing entry with parent_id, and local_offset.
		The parent_id member is the global id of the parent instance.
		The local_offset member the position of the corresponding
		formal instance alias in the parent's footprint.  
 */
template <class Tag>
const instance_alias_info<Tag>&
extract_parent_formal_instance_alias(const state_manager& sm, 
		const global_entry_common& gec) {
	const global_entry<Tag>&
		p_ent(extract_parent_entry<Tag>(sm, gec));
	const footprint& pfp(*p_ent._frame._footprint);
	const typename state_instance<Tag>::pool_type&
		local_placeholder_pool(pfp.template get_pool<Tag>());
	const state_instance<Tag>&
		_inst(local_placeholder_pool[gec.local_offset]);
	return *_inst.get_back_ref();
}

//=============================================================================
// class footprint_frame method definitions

/**
	Prints the canonical type associated with this footprint_frame's
	reference footprint.  
	\param ind the globally assigned index (top-level) for state.  
 */
template <class Tag>
ostream&
footprint_frame::dump_footprint(ostream& o, const size_t ind, 
		const footprint& topfp, const state_manager& sm) const {
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	instance_alias_info<Tag>	alias_type;
	INVARIANT(_footprint);
	const pool_type& _pool(topfp.template get_pool<Tag>());
	if (ind >= _pool.size()) {
		// then this isn't top-level
		const global_entry_pool<Tag>&
			gpool(sm.template get_pool<Tag>());
		const global_entry<Tag>& ent(gpool[ind]);
		INVARIANT(ent.parent_tag_value);
		INVARIANT(ent.parent_id);
		switch (ent.parent_tag_value) {
		case PROCESS: {
			instance_alias_info<process_tag>::dump_complete_type(
				extract_parent_formal_instance_alias<
					process_tag>(sm, ent),
				o, _footprint);
			break;
		}
		default:
			// for now, the only thing that can be parent
			// is process, append cases later...
			FINISH_ME_EXIT(Fang);
		}
	} else {
		// then it is top-level, can index the top-level footprint
		const state_instance<Tag>& _inst(_pool[ind]);
		alias_type::dump_complete_type(*_inst.get_back_ref(),
			o, _footprint);
	}
	return o;
}

//=============================================================================
// class global_entry_base method definitions

/**
	Prints the type with which the footprint is associated, 
	and the brief contents of the footprint frame.  
 */
template <class Tag>
ostream&
global_entry_base<true>::dump(ostream& o, const size_t ind, 
		const footprint& topfp, const state_manager& sm) const {
	this->_frame.template dump_footprint<Tag>(o, ind, topfp, sm);
	return this->_frame.dump_frame(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0 && USE_GLOBAL_ENTRY_PARENT_REFS
/**
	Visit each footprint_frame to mark each public subinstance
	with a back-reference to the parent.  
	\param sm the entier state manager (referenced).
	\param pi the parent process index.  
 */
template <class Tag>
void
global_entry_base<true>::cache_process_parent_refs(
		const state_manager& sm, const size_t pi) const {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: comment: pay attention to ordering, 
		is crucial for reconstruction.
	Q: Is persistent object manager really needed?
 */
template <class Tag>
void
global_entry_base<true>::write_object_base(const persistent_object_manager& m,
		ostream& o, const size_t ind, const footprint& topfp,
		const state_manager& sm) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	// save away _footprint pointer some how
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	instance_alias_info<Tag>	alias_type;
	INVARIANT(_frame._footprint);
	const pool_type& _pool(topfp.template get_pool<Tag>());
	if (ind >= _pool.size()) {
		const global_entry_pool<Tag>&
			gpool(sm.template get_pool<Tag>());
		const global_entry<Tag>& ent(gpool[ind]);
		INVARIANT(ent.parent_tag_value);
		INVARIANT(ent.parent_id);
		switch (ent.parent_tag_value) {
		case PROCESS: {
			instance_alias_info<process_tag>::
				save_canonical_footprint(
				extract_parent_formal_instance_alias<
					process_tag>(sm, ent),
				m, o, _frame._footprint);
			break;
		}
		default:
			// for now, the only thing that can be parent
			// is process, append cases later...
			FINISH_ME_EXIT(Fang);
		}
	} else {
		const state_instance<Tag>& _inst(_pool[ind]);
		alias_type::save_canonical_footprint(*_inst.get_back_ref(),
			m, o, _frame._footprint);
	}
	_frame.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dependent reconstruction ordering:
	\pre all footprints (top-level and asssociated with complete ypes)
		have been restored prior to callilng this.  
		Thus it is safe to reference instance placeholders'
		back-references.  
		See the reconstruction ordering in module::load_object_base().  
 */
template <class Tag>
void
global_entry_base<true>::load_object_base(const persistent_object_manager& m,
		istream& i, const size_t ind, const footprint& topfp,
		const state_manager& sm) {
	STACKTRACE_PERSISTENT_VERBOSE;
	// restore _footprint pointer some how
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	instance_alias_info<Tag>	alias_type;
	const pool_type& _pool(topfp.template get_pool<Tag>());
	if (ind >= _pool.size()) {
		const global_entry_pool<Tag>&
			gpool(sm.template get_pool<Tag>());
		const global_entry<Tag>& ent(gpool[ind]);
		INVARIANT(ent.parent_tag_value);
		INVARIANT(ent.parent_id);
		switch (ent.parent_tag_value) {
		case PROCESS: {
			instance_alias_info<process_tag>::
				restore_canonical_footprint(
				extract_parent_formal_instance_alias<
					process_tag>(sm, ent),
				m, i, _frame._footprint);
			break;
		}
		default:
			// for now, the only thing that can be parent
			// is process, append cases later...
			FINISH_ME_EXIT(Fang);
		}
	} else {
		const state_instance<Tag>& _inst(_pool[ind]);
		// restores _footprint
		alias_type::restore_canonical_footprint(*_inst.get_back_ref(),
			m, i, _frame._footprint);
	}
	_frame.load_object_base(m, i);
}

//=============================================================================
// class production_rule_substructure method definitions

/**
	Only ever instantiated for processes.  
	might need state_manager...
 */
template <class Tag>
void
production_rule_substructure::cflat_prs(ostream& o,
		const global_entry<Tag>& _this, const footprint& topfp,
		const cflat_options& cf, const state_manager& sm) {
	// the footprint_frame will translate from local
	// to global
	const PRS::footprint&
		pfp(_this._frame._footprint->get_prs_footprint());
	const footprint_frame_map<bool_tag>& bfm(_this._frame);
	pfp.cflat_prs(o, bfm, topfp, cf, sm);
}

//=============================================================================
// class global_entry method definitions

template <class Tag>
global_entry<Tag>::global_entry() : parent_type(), prs_parent_type(),
		global_entry_common() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_entry<Tag>::~global_entry() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively prints canonical name.  
 */
template <class Tag>
ostream&
global_entry<Tag>::__dump_canonical_name(ostream& o, const dump_flags& df, 
		const footprint& topfp, const state_manager& sm) const {
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	const pool_type& _pool(topfp.template get_pool<Tag>());
	// dump canonical name
	const state_instance<Tag>* _inst;
	if (parent_tag_value) {
		INVARIANT(parent_tag_value == PROCESS);
		const global_entry<process_tag>&
			p_ent(extract_parent_entry<process_tag>(sm, *this));
		p_ent.__dump_canonical_name(o, df, topfp, sm) << '.';
		// partial, omit formal type parent
		const pool_type&
			_lpool(p_ent._frame._footprint
				->template get_pool<Tag>());
		_inst = &_lpool[local_offset];
	} else {
		_inst = &_pool[local_offset];
	}
	const instance_alias_info<Tag>& _alias(*_inst->get_back_ref());
	return _alias.dump_hierarchical_name(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_CFLAT_CONNECT
/**
	Helper functor for translating an alias to a string.  
 */
template <class Tag>
struct global_entry<Tag>::alias_to_string_transformer : 
	public std::unary_function<
		never_ptr<const instance_alias_info<Tag> >,
		string> {
	typedef	std::unary_function<
		never_ptr<const instance_alias_info<Tag> >, string>
				parent_type;
	typename parent_type::result_type
	operator () (const typename parent_type::argument_type a) const {
		INVARIANT(a);
		ostringstream o;
		a->dump_hierarchical_name(o, dump_flags::no_owner);
		return o.str();
	}
};	// end struct alias_to_string_transformer
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_CFLAT_CONNECT
/**
	Accumulates aliases at each level of the instance hierarchy
	in the alias_string_set for alias generation.  
	TODO: May need to collect aliases at multiple levels of hierarchy!
	Algorithm:
	for all immediate aliases at this level {
	}
	Walkthrough example:
	suppose at the top level, we have aliases a.b.c = c.d.e.
	(members may all be private or public.)
	We examine: a.b.c:
		a.b.? (checks for aliases to private members of b)
		a.? (only if c is public member of b, otherwise...)
 */
template <class Tag>
void
global_entry<Tag>::collect_hierarchical_aliases(alias_string_set& al,
		const footprint& topfp, const state_manager& sm) const {
	STACKTRACE_VERBOSE;
	const port_alias_tracker* _alias_tracker;
	if (parent_tag_value) {
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "have parent_tag_value = "
			<< parent_tag_value << endl;
#endif
		INVARIANT(parent_tag_value == PROCESS);
		const global_entry<process_tag>&
			p_ent(extract_parent_entry<process_tag>(sm, *this));
		// recurse: outermost levels of hierarchy first
		p_ent.collect_hierarchical_aliases(al, topfp, sm);
		_alias_tracker = &p_ent._frame._footprint
				->get_scope_alias_tracker();
	} else {
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "have parent_tag_value = 0" << endl;
#endif
		_alias_tracker = &topfp.get_scope_alias_tracker();
	}
	typedef	typename port_alias_tracker::tracker_map_type<Tag>::type
						tracker_map_type;
	typedef	typename tracker_map_type::const_iterator
						const_map_iterator;
	const tracker_map_type&	// a map<size_t, alias_reference_set<Tag> >
		tm(_alias_tracker->template get_id_map<Tag>());
#if 0
	_alias_tracker->dump(cerr << "***" << endl) << endl << "***" << endl;
#endif
	const const_map_iterator a(tm.find(local_offset));
	al.push_back();		// empty entry, new list
	/***
		Since we keep track of singletons in the scope_alias_tracker, 
		all lookups should be valid.  
	***/
	INVARIANT(a != tm.end());
	// a->second is an alias_reference_set
#if USE_ALIAS_STRING_CACHE
	// temporary
	a->second.refresh_string_cache();	// faster with subseqent calls
	copy(a->second.get_string_cache().strings.begin(), 
		a->second.get_string_cache().strings.end(), 
		back_inserter(al.back()));
#else
	transform(a->second.begin(), a->second.end(),
		back_inserter(al.back()), 
		alias_to_string_transformer()
	);
		// transform, back_inserter... aliases to strings
#endif
}	// end method collect_hierarchical_aliases
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_GLOBAL_ENTRY_PARENT_REFS && 0
/**
	\pre process_parent_refs is cached.  
	Algorithm:
	if is top-level
		get the top-footprint alias-tracker
		note: can still have multiple parent back-refs
	else if not top-level

 */
template <class Tag>
void
global_entry<Tag>::cflat_hierarchical_aliases(ostream& o, 
		const footprint& topfp, const state_manager& sm) const {
	STACKTRACE_VERBOSE;
#if 1 && 0
	o << '{';
{
	INDENT_SECTION(o);
	// aliases in other levels of instance hierarchy
	typedef	parent_ref_cache_type::const_iterator	parent_ref_iterator;
	const global_entry_pool<process_tag>&
		proc_entry_pool(sm.get_pool<process_tag>());
	parent_ref_iterator i(process_parent_refs.begin());
	const parent_ref_iterator e(process_parent_refs.end());
	const footprint* const
		primary_parent_fp = 
			extract_parent_entry<process_tag>(sm, *this)
				._frame._footprint;
	for ( ; i!=e; i++) {
		const global_entry<process_tag>& p(proc_entry_pool[*i]);
		// These process aliases do NOT all have the same type/footprint
		// because public members are linked back to parents
		// as long as they are publicly visible to parents.  
		// Thus, we need to distinguish between cases.  
		const footprint* const pfp = p._frame._footprint;
		if (primary_parent_fp == pfp) {
			// can just use parent's alias name
			o << endl << auto_indent;
			p.cflat_hierarchical_aliases(o, topfp, sm);
			// print local aliases of this corresponding entry
			// pfp->cflat_member(...);
		} else {
			// TODO:
			o << "(parent alias of non-primary type)" << endl;
		}
	}
	// aliases at the top-level

}
	o << endl << auto_indent << '}';
#else
if (parent_tag_value) {
	// then is not publicly visible from top-level
	FINISH_ME(Fang);
} else {
	// then is publicly visible from top-level
	const port_alias_tracker*
		_alias_tracker = &topfp.get_scope_alias_tracker();
	typedef	typename port_alias_tracker::tracker_map_type<Tag>::type
						tracker_map_type;
	typedef	typename tracker_map_type::const_iterator
						const_map_iterator;
	const tracker_map_type&	// a map<size_t, alias_reference_set<Tag> >
		tm(_alias_tracker->template get_id_map<Tag>());
	const const_map_iterator a(tm.find(local_offset));
	// a->second.refresh_string_cache();	// faster with subseqent calls
	typedef	typename alias_reference_set<Tag>::const_iterator
						const_alias_iterator;
	const_alias_iterator i(a->second.begin());
	const const_alias_iterator e(a->second.end());
	for ( ; i!=e; i++) {
		// have each alias print all parental hierarchical alias names
		(*i)->cflat_aliases(o);
	}
}
#endif
}
#endif	// USE_GLOBAL_ENTRY_PARENT_REFS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call that formats properly.  
	\param topfp should be the top-level footprint belonging to the module.
 */
template <class Tag>
ostream&
global_entry<Tag>::dump_canonical_name(ostream& o,
		const footprint& topfp, const state_manager& sm) const {
	return __dump_canonical_name(o, dump_flags::no_owner,
		topfp, sm);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: currently, only processes are ever super instances.  
 */
template <class Tag>
ostream&
global_entry<Tag>::dump(ostream& o, const size_t ind,
		const footprint& topfp, const state_manager& sm) const {
	o << ind << '\t';
	switch(parent_tag_value) {
	case NONE:
		o << "(top)\t-\t";
		break;
	case PROCESS:
		o << "process\t" << parent_id << '\t';
		break;
	case CHANNEL:
		o << "channel\t" << parent_id << '\t';
		break;
	case STRUCT:
		o << "struct\t" << parent_id << '\t';
		break;
	default:
		THROW_EXIT;
	}
	o << local_offset << '\t';
	dump_canonical_name(o, topfp, sm) << '\t';
	parent_type::template dump<Tag>(o, ind, topfp, sm);
#if USE_GLOBAL_ENTRY_PARENT_REFS
	if (!process_parent_refs.empty()) {
		o << endl << '\t' << "parent-procs: ";
		typedef parent_ref_cache_type::const_iterator	const_iterator;
		const_iterator i(process_parent_refs.begin());
		const const_iterator e(process_parent_refs.end());
		o << *i;
		for (i++; i!=e; i++) {
			o << ',' << *i;
		}
	}
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_CFLAT_CONNECT
/**
	Print out all equivalent aliases in the hierarchy.  
	TODO: cache the results of visiting the hierarchy to avoid
		repetitious string evaluation.  
		Perhaps cache in parallel structure in footprint?
		Shadowing the scope_alias_tracker.  
 */
template <class Tag>
ostream&
global_entry<Tag>::cflat_connect(ostream& o, const cflat_options& cf,
		const footprint& topfp, const state_manager& sm) const {
	ostringstream canonical;
	dump_canonical_name(canonical, topfp, sm);
#if 1
	// collect all aliases at all levels of hierarchy...
	alias_string_set _aliases;
	collect_hierarchical_aliases(_aliases, topfp, sm);
	// compact representation for debugging
	// _aliases.dump(o) << endl;	
	_aliases.dump_aliases(o, canonical.str(), cf);
#else
	// using alias traversal instead
	// alias_string_set _aliases;
	// temporary: dump hiearchical name fragments
	cflat_hierarchical_aliases(o, topfp, sm);
#endif
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry<Tag>::write_object_base(const persistent_object_manager& m, 
		ostream& o, const size_t ind, const footprint& f, 
		const state_manager& sm) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	write_value(o, parent_tag_value);
	write_value(o, parent_id);
	write_value(o, local_offset);
	parent_type::template write_object_base<Tag>(m, o, ind, f, sm);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ordering matters because parent_type::load_object_base
		depends on parent_id and parent_tag_value
		in reconstructing the footprint pointer.  
 */
template <class Tag>
void
global_entry<Tag>::load_object_base(const persistent_object_manager& m, 
		istream& i, const size_t ind, const footprint& f, 
		const state_manager& sm) {
	STACKTRACE_PERSISTENT_VERBOSE;
	read_value(i, parent_tag_value);
	read_value(i, parent_id);
	read_value(i, local_offset);
	parent_type::template load_object_base<Tag>(m, i, ind, f, sm);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_GLOBAL_ENTRY_TCC__

