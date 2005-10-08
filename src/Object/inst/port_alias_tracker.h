/**
	\file "Object/inst/port_alias_tracker.h"
	Pair of classes used to keep track of port aliases.  
	Intended as replacement for port_alias_signature.
	$Id: port_alias_tracker.h,v 1.4 2005/10/08 01:39:58 fang Exp $
 */

#ifndef	__OBJECT_INST_PORT_ALIAS_TRACKER_H__
#define	__OBJECT_INST_PORT_ALIAS_TRACKER_H__

#include <iosfwd>
#include <map>
#include <vector>
#include "util/size_t.h"
#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/boolean_types.h"
#include "Object/traits/classification_tags.h"
#include "Object/inst/substructure_alias_fwd.h"

namespace ART {
namespace entity {
class footprint;
using std::istream;
using std::ostream;
using util::good_bool;
using util::persistent_object_manager;
using util::memory::never_ptr;

template <class Tag>
class instance_alias_info;

template <class Tag>
class state_instance;

template <class T>
class instance_pool;

//=============================================================================
/**
	Keeps track of sets of aliases with same ID.  
 */
template <class Tag>
class alias_reference_set {
public:
	typedef	Tag					tag_type;
	typedef instance_alias_info<Tag>		alias_type;
	typedef never_ptr<const alias_type>		alias_ptr_type;
private:
	typedef	std::vector<alias_ptr_type>		alias_array_type;
	typedef	typename alias_array_type::const_iterator
							const_iterator;

	alias_array_type				alias_array;
public:
	alias_reference_set();
	~alias_reference_set();

	void
	push_back(const alias_ptr_type);

	ostream&
	dump(ostream&) const;

	bool
	is_unique(void) const { return alias_array.size() <= 1; }

	const_iterator
	begin(void) const { return alias_array.begin(); }

	const_iterator
	end(void) const { return alias_array.end(); }

	good_bool
	replay_internal_aliases(substructure_alias&) const;

	alias_ptr_type
	shortest_alias(void) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class alias_reference_set

//=============================================================================
/**
	Meta-class specific base class for tracking collections of aliases.  
	Contains a map from index to instance alias set.  
 */
template <class Tag>
class port_alias_tracker_base {
protected:
	typedef	Tag						tag_type;
	typedef	std::map<size_t, alias_reference_set<Tag> >	map_type;
	typedef	typename map_type::const_iterator		const_iterator;
	typedef	typename map_type::iterator			iterator;

	map_type					_ids;

	port_alias_tracker_base();
	~port_alias_tracker_base();

	void
	filter_unique(void);

	ostream&
	dump_map(ostream&) const;

	good_bool
	__replay_aliases(substructure_alias&) const;

	void
	__shorten_canonical_aliases(
		instance_pool<state_instance<Tag> >&) const;

	void
	collect_map(persistent_object_manager&) const;

	void
	write_map(const persistent_object_manager&, ostream&) const;

	void
	load_map(const persistent_object_manager&, istream&);

};	// end class port_alias_tracker_base

//=============================================================================
/**
	Used to count aliases over structres whose connections have been
	played and unique placeholder ID numbers have been assigned.  
	Note: this can be reused to for tracking both internal aliases
	and port aliases.  
	This is kept to make looking up all aliases in each scope
	instantaneous, basically a cache of alias results.  
	Also keep track of instance references?
 */
class port_alias_tracker :
	private port_alias_tracker_base<process_tag>,
	private port_alias_tracker_base<channel_tag>,
	private port_alias_tracker_base<datastruct_tag>,
	private port_alias_tracker_base<enum_tag>,
	private port_alias_tracker_base<int_tag>,
	private port_alias_tracker_base<bool_tag> {
public:
	/**
		Map used to track number of occurrences of indices.  
	 */
	template <class Tag>
	struct tracker_map_type {
		typedef	std::map<size_t, alias_reference_set<Tag> >	type;
	};

private:
	bool						has_internal_aliases;
public:
	port_alias_tracker();
	~port_alias_tracker();

	ostream&
	dump(ostream&) const;

	template <class Tag>
	typename port_alias_tracker_base<Tag>::map_type&
	get_id_map(void) { return port_alias_tracker_base<Tag>::_ids; }

	template <class Tag>
	const typename port_alias_tracker_base<Tag>::map_type&
	get_id_map(void) const { return port_alias_tracker_base<Tag>::_ids; }

public:
	void
	filter_uniques(void);

	good_bool
	replay_internal_aliases(substructure_alias&) const;

	void
	shorten_canonical_aliases(footprint&);

public:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end struct port_alias_tracker

//=============================================================================
#if 0
/**
	This is the information kept persistent.  
 */
struct port_alias_set {
};	// end struct port_alias_set

//=============================================================================
/**
 */
class port_alias_replayer {
};	// end struct port_alias_replayer
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_PORT_ALIAS_TRACKER_H__

