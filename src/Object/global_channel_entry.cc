/**
	\file "Object/global_channel_entry.cc"
	$Id: global_channel_entry.cc,v 1.1.2.2 2007/01/11 08:04:45 fang Exp $
 */

#include "Object/global_channel_entry.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/type/canonical_fundamental_chan_type.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"

namespace HAC {
namespace entity {
//=============================================================================
// class global_entry<channel_tag> method definitions

global_entry_base<channel_tag>::global_entry_base() : 
		channel_type(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_base<channel_tag>::~global_entry_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_base<channel_tag>::collect_transient_info_base(
		persistent_object_manager& m, const size_t ind, 
		const footprint& f, const state_manager& sm) const {
	substructure_policy::collect_transient_info_base(m, ind, f, sm);
	NEVER_NULL(channel_type);
	channel_type->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_base<channel_tag>::write_object_base(
		const persistent_object_manager& m, ostream& o, 
		const size_t ind,
		const footprint& f, const state_manager& sm) const {
	substructure_policy::write_object_base(m, o, ind, f, sm);
	canonical_fundamental_chan_type_base::write_pointer(m, o, channel_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_base<channel_tag>::load_object_base(
		const persistent_object_manager& m, istream& i, 
		const size_t ind,
		const footprint& f, const state_manager& sm) {
	substructure_policy::load_object_base(m, i, ind, f, sm);
	channel_type = canonical_fundamental_chan_type_base::read_pointer(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// BUILTIN_CHANNEL_FOOTPRINTS

