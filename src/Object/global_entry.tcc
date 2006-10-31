/**
	\file "Object/global_entry.tcc"
	$Id: global_entry.tcc,v 1.14.4.1 2006/10/31 00:28:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_TCC__
#define	__HAC_OBJECT_GLOBAL_ENTRY_TCC__

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
#include "Object/inst/alias_empty.tcc"
#include "Object/inst/alias_actuals.tcc"	// for dump_complete_type
#include "Object/inst/instance_collection.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/common/dump_flags.h"
#include "Object/cflat_context.h"
#include "Object/global_entry_context.h"
#include "Object/lang/cflat_visitor.h"
// #include "Object/lang/cflat_printer.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/def/process_definition.h"
#include "Object/def/user_def_datatype.h"
#include "Object/type/canonical_generic_chan_type.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/IO_utils.h"
#include "common/TODO.h"
#include "common/ICE.h"

namespace HAC {
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
		local_placeholder_pool(pfp.template get_instance_pool<Tag>());
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
footprint_frame::dump_footprint(global_entry_dumper& gec) const {
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	instance_alias_info<Tag>	alias_type;
	INVARIANT(_footprint);
	ostream& o(gec.os);
	const size_t ind(gec.index);
	const footprint& topfp(*gec.fp);
	const state_manager& sm(*gec.sm);
	const pool_type& _pool(topfp.template get_instance_pool<Tag>());
	if (ind >= _pool.size()) {
		// then this isn't top-level
		const global_entry_pool<Tag>&
			gpool(sm.template get_pool<Tag>());
		const global_entry<Tag>& ent(gpool[ind]);
		INVARIANT(ent.parent_tag_value);
		INVARIANT(ent.parent_id);
		switch (ent.parent_tag_value) {
		case TYPE_PROCESS: {
			instance_alias_info<process_tag>::dump_complete_type(
				extract_parent_formal_instance_alias<
					process_tag>(sm, ent),
				o, _footprint);
			break;
		}
		case TYPE_STRUCT: {
			instance_alias_info<datastruct_tag>::dump_complete_type(
				extract_parent_formal_instance_alias<
					datastruct_tag>(sm, ent),
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

template <class Tag>
ostream&
global_entry_base<false>::dump(global_entry_dumper& ged) const {
	return ged.os;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the type with which the footprint is associated, 
	and the brief contents of the footprint frame.  
 */
#if 0
template <class Tag>
ostream&
global_entry_base<true>::dump(ostream& o, const size_t ind, 
		const footprint& topfp, const state_manager& sm) const {
	this->_frame.template dump_footprint<Tag>(o, ind, topfp, sm);
	return this->_frame.dump_frame(o);
}
#else
template <class Tag>
ostream&
global_entry_base<true>::dump(global_entry_dumper& ged) const {
	this->_frame.template dump_footprint<Tag>(ged);
	return this->_frame.dump_frame(ged.os);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects pointers needed for save/restoration of footprint pointers.  
 */
template <class Tag>
void
global_entry_base<true>::collect_transient_info_base(
		persistent_object_manager& m, 
		const size_t ind, const footprint& topfp, 
		const state_manager& sm) const {
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	instance_alias_info<Tag>	alias_type;
	STACKTRACE_PERSISTENT_VERBOSE;
	INVARIANT(_frame._footprint);
	const pool_type& _pool(topfp.template get_instance_pool<Tag>());
	if (ind >= _pool.size()) {
		const global_entry_pool<Tag>&
			gpool(sm.template get_pool<Tag>());
		const global_entry<Tag>& ent(gpool[ind]);
		INVARIANT(ent.parent_tag_value);
		INVARIANT(ent.parent_id);
		switch (ent.parent_tag_value) {
		case TYPE_PROCESS: {
			instance_alias_info<process_tag>::
				collect_canonical_footprint(
				extract_parent_formal_instance_alias<
					process_tag>(sm, ent),
				m);
			break;
		}
		case TYPE_STRUCT: {
			// Q: can datatypes have relaxed parameters?
			instance_alias_info<datastruct_tag>::
				collect_canonical_footprint(
				extract_parent_formal_instance_alias<
					datastruct_tag>(sm, ent),
				m);
			break;
		}
		default:
			// append cases later...
			FINISH_ME_EXIT(Fang);
		}
	} else {
		const state_instance<Tag>& _inst(_pool[ind]);
		alias_type::collect_canonical_footprint(*_inst.get_back_ref(),
			m);
	}
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
template <class Tag>
void
global_entry_base<true>::write_object_base(const persistent_object_manager& m,
		ostream& o, const size_t ind, const footprint& topfp,
		const state_manager& sm) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	// save away _footprint pointer somehow
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	instance_alias_info<Tag>	alias_type;
	INVARIANT(_frame._footprint);
	const pool_type& _pool(topfp.template get_instance_pool<Tag>());
	if (ind >= _pool.size()) {
		const global_entry_pool<Tag>&
			gpool(sm.template get_pool<Tag>());
		const global_entry<Tag>& ent(gpool[ind]);
		INVARIANT(ent.parent_tag_value);
		INVARIANT(ent.parent_id);
		switch (ent.parent_tag_value) {
		case TYPE_PROCESS: {
			instance_alias_info<process_tag>::
				save_canonical_footprint(
				extract_parent_formal_instance_alias<
					process_tag>(sm, ent),
				m, o, _frame._footprint);
			break;
		}
		case TYPE_STRUCT: {
			instance_alias_info<datastruct_tag>::
				save_canonical_footprint(
				extract_parent_formal_instance_alias<
					datastruct_tag>(sm, ent),
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
	const pool_type& _pool(topfp.template get_instance_pool<Tag>());
	if (ind >= _pool.size()) {
		const global_entry_pool<Tag>&
			gpool(sm.template get_pool<Tag>());
		const global_entry<Tag>& ent(gpool[ind]);
		INVARIANT(ent.parent_tag_value);
		INVARIANT(ent.parent_id);
		switch (ent.parent_tag_value) {
		case TYPE_PROCESS: {
			instance_alias_info<process_tag>::
				restore_canonical_footprint(
				extract_parent_formal_instance_alias<
					process_tag>(sm, ent),
				m, i, _frame._footprint);
			break;
		}
		case TYPE_STRUCT: {
			instance_alias_info<datastruct_tag>::
				restore_canonical_footprint(
				extract_parent_formal_instance_alias<
					datastruct_tag>(sm, ent),
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic PRS footprint traversal.  
	Dynamic-cast (cross-cast) is needed because PRS::cflat_visitor
	is not derived from cflat_context... should it be?
 */
template <class Tag>
void
production_rule_substructure::accept(const global_entry<Tag>& _this, 
		PRS::cflat_visitor& v) {
	const footprint* const f(_this._frame._footprint);
	NEVER_NULL(f);
	const PRS::footprint&
		pfp(f->get_prs_footprint());
	const cflat_context::footprint_frame_setter
		tmp(IS_A(cflat_context&, v), _this._frame);
	pfp.accept(v);
	const SPEC::footprint&
		sfp(f->get_spec_footprint());
	sfp.accept(v);
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
	const pool_type& _pool(topfp.template get_instance_pool<Tag>());
	// dump canonical name
	const state_instance<Tag>* _inst;
	switch (parent_tag_value) {
	case TYPE_NONE:
		_inst = &_pool[local_offset];
		break;
	case TYPE_PROCESS: {
		const global_entry<process_tag>&
			p_ent(extract_parent_entry<process_tag>(sm, *this));
		p_ent.__dump_canonical_name(o, df, topfp, sm) << '.';
		// partial, omit formal type parent
		const pool_type&
			_lpool(p_ent._frame._footprint
				->template get_instance_pool<Tag>());
		_inst = &_lpool[local_offset];
		break;
	}
	case TYPE_CHANNEL: {
		const global_entry<channel_tag>&
			p_ent(extract_parent_entry<channel_tag>(sm, *this));
		p_ent.__dump_canonical_name(o, df, topfp, sm) << '.';
		// partial, omit formal type parent
		const pool_type&
			_lpool(p_ent._frame._footprint
				->template get_instance_pool<Tag>());
		_inst = &_lpool[local_offset];
		break;
	}
	case TYPE_STRUCT: {
		const global_entry<datastruct_tag>&
			p_ent(extract_parent_entry<datastruct_tag>(sm, *this));
		p_ent.__dump_canonical_name(o, df, topfp, sm) << '.';
		// partial, omit formal type parent
		const pool_type&
			_lpool(p_ent._frame._footprint
				->template get_instance_pool<Tag>());
		_inst = &_lpool[local_offset];
		break;
	}
	default:
		ICE(cerr, 
			cerr << "Unknown parent tag enumeration: " <<
				parent_tag_value << endl;
		)
	}
	const instance_alias_info<Tag>& _alias(*_inst->get_back_ref());
	return _alias.dump_hierarchical_name(o, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped call that formats properly.  
	\param topfp should be the top-level footprint belonging to the module.
 */
template <class Tag>
ostream&
global_entry<Tag>::dump_canonical_name(ostream& o,
		const footprint& topfp, const state_manager& sm) const {
#if 1
	return __dump_canonical_name(o, dump_flags::no_definition_owner,
		topfp, sm);
#else
	return __dump_canonical_name(o, dump_flags::no_leading_scope,
		topfp, sm);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: currently, only processes are ever super instances.  
 */
template <class Tag>
ostream&
global_entry<Tag>::dump(global_entry_dumper& ged) const {
	ostream& o(ged.os);
	o << ged.index << '\t';
	switch(parent_tag_value) {
	case TYPE_NONE:
		o << "(top)\t-\t";
		break;
	case TYPE_PROCESS:
		o << "process\t" << parent_id << '\t';
		break;
	case TYPE_CHANNEL:
		o << "channel\t" << parent_id << '\t';
		break;
	case TYPE_STRUCT:
		o << "struct\t" << parent_id << '\t';
		break;
	default:
		THROW_EXIT;
	}
	o << local_offset << '\t';
	dump_canonical_name(o, *ged.fp, *ged.sm) << '\t';
	parent_type::template dump<Tag>(ged);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use global_entry_dumper
 */
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
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_TCC__

