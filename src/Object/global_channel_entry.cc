/**
	\file "Object/global_channel_entry.cc"
	$Id: global_channel_entry.cc,v 1.4 2010/04/07 00:12:27 fang Exp $
 */

#include "Object/global_channel_entry.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_entry_dumper.h"
// #include "Object/global_entry.tcc"	// for substructure dump
#include "Object/type/canonical_fundamental_chan_type.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"

namespace HAC {
namespace entity {
//=============================================================================
// struct global_entry<channel_tag> method definitions

global_entry_base<channel_tag>::global_entry_base() : 
		channel_type(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
global_entry_base<channel_tag>::~global_entry_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
global_entry_base<channel_tag>::dump(global_entry_dumper& ged) const {
//	substructure_policy::dump(ged);		// no-op
	NEVER_NULL(channel_type);
	return channel_type->dump(ged.os);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_base<channel_tag>::collect_transient_info_base(
		persistent_object_manager& m) const {
	substructure_policy::collect_transient_info_base(m);
	// only the top-level's 0th entry should be NULL
	if (channel_type)
		channel_type->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_base<channel_tag>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	substructure_policy::write_object_base(m, o);
	chan_type_impl::write_pointer(m, o, channel_type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
global_entry_base<channel_tag>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	substructure_policy::load_object_base(m, i);
	channel_type = chan_type_impl::read_pointer(m, i);
}

//=============================================================================
// explicit template instantiation

template
ostream&
global_entry_base<channel_tag>::dump<channel_tag>(global_entry_dumper&) const;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// BUILTIN_CHANNEL_FOOTPRINTS

