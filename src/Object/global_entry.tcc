/**
	\file "Object/global_entry.tcc"
	$Id: global_entry.tcc,v 1.24 2010/04/07 00:12:28 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_ENTRY_TCC__
#define	__HAC_OBJECT_GLOBAL_ENTRY_TCC__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

#ifndef	STACKTRACE_PERSISTENTS
#define	STACKTRACE_PERSISTENTS		(0 && ENABLE_STACKTRACE)
#endif

#include <string>
#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include "util/macros.h"
#include "util/sstream.h"
#include "Object/global_entry.h"
#include "Object/def/footprint.h"
#include "Object/traits/proc_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.tcc"
#include "Object/inst/alias_actuals.tcc"	// for dump_complete_type
#include "Object/inst/instance_collection.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/state_instance.h"
#include "Object/traits/type_tag_enum.h"
#include "Object/common/dump_flags.h"
#include "Object/cflat_context.h"
#include "Object/global_entry_context.h"
#include "Object/global_entry_dumper.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/lang/CHP.h"		// for concurrent_actions
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/def/process_definition.h"
#include "Object/def/user_def_datatype.h"
#include "Object/type/canonical_generic_chan_type.h"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/IO_utils.h"
#include "util/memory/index_pool.h"
#include "util/indent.h"
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
// class footprint_frame_map method definitions

template <class Tag>
footprint_frame_map<Tag>::~footprint_frame_map() { }

//=============================================================================
// class footprint_frame method definitions

//=============================================================================
// class global_entry_substructure_base method definitions

template <class Tag>
ostream&
global_entry_substructure_base<false>::dump(global_entry_dumper& ged) const {
	return ged.os;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the type with which the footprint is associated, 
	and the brief contents of the footprint frame.  
 */
template <class Tag>
ostream&
global_entry_substructure_base<true>::dump(global_entry_dumper& ged) const {
	this->_frame.template dump_footprint<Tag>(ged);
	const util::indent __tab__(ged.os, "\t");
	return this->_frame.dump_frame(ged.os);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
size_t
global_entry_substructure_base<true>::count_frame_size(const size_t s, 
		const this_type& t) {
	return s +t._frame.count_frame_size();
}

//=============================================================================
// class global_entry method definitions

template <class Tag>
global_entry<Tag>::global_entry() : parent_type() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
global_entry<Tag>::~global_entry() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <class Tag>
footprint_frame_map_type&
footprint_frame::get_frame_map(void) {
	return footprint_frame_map<Tag>::id_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
const footprint_frame_map_type&
footprint_frame::get_frame_map(void) const {
	return footprint_frame_map<Tag>::id_map;
}
#endif

/**
	TODO: use global_entry_dumper
 */
template <class Tag>
void
global_entry<Tag>::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	STACKTRACE_PERSISTENT_VERBOSE;
	parent_type::write_object_base(m, o);
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
		istream& i) {
	STACKTRACE_PERSISTENT_VERBOSE;
	parent_type::load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_GLOBAL_ENTRY_TCC__

