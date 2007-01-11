/**
	\file "Object/global_channel_entry.h"
	$Id: global_channel_entry.h,v 1.1.2.3 2007/01/11 21:03:21 fang Exp $
 */

#ifndef	__HAC_OBJECT_GLOBAL_CHANNEL_ENTRY_H__
#define	__HAC_OBJECT_GLOBAL_CHANNEL_ENTRY_H__

#include "Object/global_entry.h"
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using util::memory::count_ptr;
struct canonical_fundamental_chan_type_base;

//=============================================================================
template <>
struct global_entry_base<channel_tag> :
		public global_entry_substructure_base<false> {
	typedef	global_entry_substructure_base<false>	substructure_policy;
	typedef	canonical_fundamental_chan_type_base	chan_type_impl;

	count_ptr<const chan_type_impl>			channel_type;

	global_entry_base();
	~global_entry_base();

	template <class Tag>
	ostream&
	dump(global_entry_dumper&) const;

	using substructure_policy::collect_subentries;

	void
	collect_transient_info_base(persistent_object_manager&, 
		const size_t, const footprint&, const state_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&, 
		const size_t, const footprint&, const state_manager&) const;

	void
	load_object_base(const persistent_object_manager&, istream&, 
		const size_t, const footprint&, const state_manager&);

};	// end struct global_entry_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// BUILTIN_CHANNEL_FOOTPRINTS
#endif	// __HAC_OBJECT_GLOBAL_CHANNEL_ENTRY_H__

