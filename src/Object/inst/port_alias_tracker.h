/**
	\file "Object/inst/port_alias_tracker.h"
	Pair of classes used to keep track of port aliases.  
	Intended as replacement for port_alias_signature.
	$Id: port_alias_tracker.h,v 1.11 2006/11/27 08:29:16 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_ALIAS_TRACKER_H__
#define	__HAC_OBJECT_INST_PORT_ALIAS_TRACKER_H__

#include <iosfwd>
#include <map>
#include <vector>
#include "util/size_t.h"
#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/boolean_types.h"
#include "Object/traits/classification_tags.h"
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/devel_switches.h"

/**
	This was introduced temporarily on branch, but is currently not used.
	There may be a time in the future when this will come in handy, 
	so we keep it around commented out.  
	Appeared on branch: HACXX-00-01-04-main-00-62-cflat-02
 */
#define	USE_ALIAS_STRING_CACHE			0

#if USE_ALIAS_STRING_CACHE
#include "Object/common/alias_string_cache.h"
#endif

namespace HAC {
namespace entity {
class footprint;
#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
using std::istream;
#endif
using std::ostream;
using util::good_bool;
#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
using util::persistent_object_manager;
#endif
using util::memory::never_ptr;

template <class>
struct class_traits;

template <class Tag>
class instance_alias_info;

template <class Tag>
class state_instance;

template <class T>
class instance_pool;

template <class>
class instance_collection_pool_bundle;

//=============================================================================
/**
	Keeps track of sets of aliases with same ID.  
 */
template <class Tag>
class alias_reference_set {
	typedef	alias_reference_set<Tag>		this_type;
public:
	typedef	Tag					tag_type;
	typedef	class_traits<Tag>			traits_type;
	typedef typename traits_type::instance_alias_info_type	alias_type;
	typedef never_ptr<const alias_type>		const_alias_ptr_type;
	typedef never_ptr<alias_type>			alias_ptr_type;
#if USE_ALIAS_STRING_CACHE
	/**
		Blatantly copied from class global_entry<Tag>.
		TODO: move this to "inst/instance_alias_info.h"
	 */
	struct alias_to_string_transformer;
#endif
private:
	typedef	std::vector<alias_ptr_type>		alias_array_type;
	typedef	typename alias_array_type::iterator	iterator;
	typedef	instance_collection_pool_bundle<Tag>
						collection_pool_bundle_type;
public:
	typedef	typename alias_array_type::const_iterator
							const_iterator;
private:
	alias_array_type				alias_array;
#if USE_ALIAS_STRING_CACHE
	/**
		String cache is not kept persistent, it is reconstructed
		on-demand, for cflat purpose.  
		All methods that modify the alias array should
		invalidate this cache.  
		Declared mutable because it is allowed to be updated
		during const methods.  
	 */
	mutable alias_string_cache			cache;
#endif
public:
	alias_reference_set();
	~alias_reference_set();

	size_t
	size(void) const { return this->alias_array.size(); }

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

	typename alias_array_type::const_reference
	front(void) const { return alias_array.front(); }

	good_bool
	replay_internal_aliases(substructure_alias&) const;

	const_alias_ptr_type
	shortest_alias(void);

#if USE_ALIAS_STRING_CACHE
	void
	refresh_string_cache(void) const;

	const alias_string_cache&
	get_string_cache(void) const { return cache; }
#endif

#if COPY_IF_PORT_ALIASES
	void
	__import_port_aliases(const this_type&);

	struct port_alias_predicate;
#endif

#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
#if 0
	void
	collect_transient_info_base(persistent_object_manager&) const;
#endif

	void
	write_object_base(const collection_pool_bundle_type&, ostream&) const;

	void
	load_object_base(const collection_pool_bundle_type&, istream&);
#endif

};	// end class alias_reference_set

//=============================================================================
/**
	Meta-class specific base class for tracking collections of aliases.  
	Contains a map from index to instance alias set.  
 */
template <class Tag>
class port_alias_tracker_base {
	typedef	port_alias_tracker_base<Tag>			this_type;
protected:
	typedef	Tag						tag_type;
	typedef	std::map<size_t, alias_reference_set<Tag> >	map_type;
	typedef	typename map_type::const_iterator		const_iterator;
	typedef	typename map_type::iterator			iterator;
	typedef	typename map_type::value_type			value_type;
	typedef	instance_collection_pool_bundle<Tag>
						collection_pool_bundle_type;

	map_type					_ids;

	port_alias_tracker_base();
	~port_alias_tracker_base();

#if !COPY_IF_PORT_ALIASES
	void
	filter_unique(void);
#endif

	ostream&
	dump_map(ostream&) const;

	good_bool
	__replay_aliases(substructure_alias&) const;

	void
	__shorten_canonical_aliases(
		instance_pool<state_instance<Tag> >&);

	// only applicable to channels, really
	good_bool
	check_connections(void) const;

#if COPY_IF_PORT_ALIASES
	void
	__import_port_aliases(const this_type&);

	struct port_alias_importer;
#endif

#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
#if 0
	void
	collect_map(persistent_object_manager&) const;
#endif

	void
	write_map(const footprint&, ostream&) const;

	void
	load_map(const footprint&, istream&);
#endif

};	// end class port_alias_tracker_base

//=============================================================================
/**
	This really should be renamed alias_tracker, as its use is not
	just restricted to ports.  

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
	typedef	port_alias_tracker			this_type;
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
#if !COPY_IF_PORT_ALIASES
	void
	filter_uniques(void);
#endif

	good_bool
	replay_internal_aliases(substructure_alias&) const;

	void
	shorten_canonical_aliases(footprint&);

	good_bool
	check_channel_connections(void) const;

#if COPY_IF_PORT_ALIASES
	void
	import_port_aliases(const this_type&);
#endif

public:

#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
#if 0
	void
	collect_transient_info_base(persistent_object_manager&) const;
#endif

	void
	write_object_base(const footprint&, ostream&) const;

	void
	load_object_base(const footprint&, istream&);
#endif

};	// end struct port_alias_tracker

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_ALIAS_TRACKER_H__

