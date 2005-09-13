/**
	\file "Object/global_entry.tcc"
	$Id: global_entry.tcc,v 1.1.2.3 2005/09/13 01:14:45 fang Exp $
 */

#ifndef	__OBJECT_GLOBAL_ENTRY_TCC__
#define	__OBJECT_GLOBAL_ENTRY_TCC__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

#ifndef	STACKTRACE_PERSISTENTS
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE
#endif

#include <iostream>
#include "util/macros.h"
#include "Object/global_entry.h"
#include "Object/def/footprint.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.tcc"	// for dump_complete_type
#include "Object/inst/instance_collection.h"

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
using util::read_value;
using util::write_value;

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
		// don't do anything yet
		FINISH_ME(Fang);
		// TODO: trace super instances
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
#if 1
	// save away _footprint pointer some how
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	instance_alias_info<Tag>	alias_type;
	INVARIANT(_frame._footprint);
	const pool_type& _pool(topfp.template get_pool<Tag>());
	if (ind >= _pool.size()) {
		FINISH_ME(Fang);
	} else {
		const state_instance<Tag>& _inst(_pool[ind]);
		alias_type::save_canonical_footprint(*_inst.get_back_ref(),
			m, o, _frame._footprint);
	}
#endif
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
#if 1
	typedef	typename state_instance<Tag>::pool_type	pool_type;
	typedef	instance_alias_info<Tag>	alias_type;
	const pool_type& _pool(topfp.template get_pool<Tag>());
	if (ind >= _pool.size()) {
		FINISH_ME(Fang);
	} else {
		const state_instance<Tag>& _inst(_pool[ind]);
		// restores _footprint
		alias_type::restore_canonical_footprint(*_inst.get_back_ref(),
			m, i, _frame._footprint);
	}
#endif
	_frame.load_object_base(m, i);
}

//=============================================================================
// class global_entry method definitions

template <class Tag>
global_entry<Tag>::global_entry() : parent_type(), parent_tag_value(0), 
		parent_id(0), local_offset(0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_entry<Tag>::~global_entry() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		o << "process\t";
		break;
	case CHANNEL:
		o << "channel\t";
		break;
	case STRUCT:
		o << "struct\t";
		break;
	default:
		THROW_EXIT;
	}
	if (parent_tag_value) {
		o << parent_id << '\t';
	}
	o << local_offset << '\t';
	return parent_type::template dump<Tag>(o, ind, topfp, sm);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry<Tag>::write_object_base(const persistent_object_manager& m, 
		ostream& o, const size_t ind, const footprint& f, 
		const state_manager& sm) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	parent_type::template write_object_base<Tag>(m, o, ind, f, sm);
	write_value(o, parent_tag_value);
	write_value(o, parent_id);
	write_value(o, local_offset);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
global_entry<Tag>::load_object_base(const persistent_object_manager& m, 
		istream& i, const size_t ind, const footprint& f, 
		const state_manager& sm) {
	STACKTRACE_PERSISTENT_VERBOSE;
	parent_type::template load_object_base<Tag>(m, i, ind, f, sm);
	read_value(i, parent_tag_value);
	read_value(i, parent_id);
	read_value(i, local_offset);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_GLOBAL_ENTRY_TCC__

