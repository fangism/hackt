/**
	\file "Object/inst/port_alias_tracker.hh"
	Pair of classes used to keep track of port aliases.  
	Intended as replacement for port_alias_signature.
	$Id: port_alias_tracker.hh,v 1.27 2011/03/23 00:36:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PORT_ALIAS_TRACKER_HH__
#define	__HAC_OBJECT_INST_PORT_ALIAS_TRACKER_HH__

#include <iosfwd>
#include <map>
#include <set>
#include <vector>
#include "util/size_t.h"
#include "util/persistent_fwd.hh"
#include "util/memory/excl_ptr.hh"
#include "util/boolean_types.hh"
#include "common/status.hh"
#include "Object/traits/classification_tags.hh"
#include "Object/inst/substructure_alias_fwd.hh"
#include "Object/devel_switches.hh"

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
struct dump_flags;
class footprint;
class footprint_frame;
using std::ostream;
using std::vector;
using std::set;
using std::string;
using util::good_bool;
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
struct instance_collection_pool_bundle;

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
		Blatantly copied from struct global_entry<Tag>.
		TODO: move this to "inst/instance_alias_info.h"
	 */
	struct alias_to_string_transformer;
#endif
private:
	typedef	vector<alias_ptr_type>			alias_array_type;
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
	dump(ostream&, const dump_flags&) const;

	bool
	is_unique(void) const { return alias_array.size() <= 1; }

	const_iterator
	begin(void) const { return alias_array.begin(); }

	const_iterator
	end(void) const { return alias_array.end(); }

	typename alias_array_type::const_reference
	front(void) const { return alias_array.front(); }

	alias_ptr_type
	is_aliased_to_port(void) const;

	void
	swap(this_type&);

	void
	bare_swap(this_type&);

	void
	override_id(const size_t);

	good_bool
	replay_internal_aliases(substructure_alias&) const;

	void
	export_alias_properties(substructure_alias&) const;

	const_iterator
	find_any_port_alias(void) const;

	const_iterator
	find_shortest_alias(const bool) const;

	const_alias_ptr_type
	shortest_alias(void);

	void
	synchronize_flags(void);

	void
	export_alias_strings(const dump_flags&,	set<string>&) const;

#if USE_ALIAS_STRING_CACHE
	void
	refresh_string_cache(void) const;

	const alias_string_cache&
	get_string_cache(void) const { return cache; }
#endif

	void
	__import_port_aliases(const this_type&);

	struct port_alias_predicate;

};	// end class alias_reference_set

//=============================================================================
/**
	Meta-class specific base class for tracking collections of aliases.  
	Contains a map from index to instance alias set.  
	TODO: now that port aliases are sifted, their indices will always
		be contiguous starting from 1, so we can replace the
		sparse map structure with a vector!
 */
template <class Tag>
class port_alias_tracker_base {
	typedef	port_alias_tracker_base<Tag>			this_type;
protected:
	typedef	Tag						tag_type;
public:
	// public for workaround in "Object/inst/instance_collection.tcc"
	typedef	alias_reference_set<Tag>			alias_set_type;
	typedef	std::map<size_t, alias_set_type>		map_type;
protected:
	typedef	typename map_type::const_iterator		const_iterator;
	typedef	typename map_type::iterator			iterator;
	typedef	typename map_type::value_type			value_type;
	typedef	instance_collection_pool_bundle<Tag>
						collection_pool_bundle_type;

	map_type					_ids;

	port_alias_tracker_base();
	~port_alias_tracker_base();

	ostream&
	dump_map(ostream&, const dump_flags&) const;

	good_bool
	__replay_aliases(substructure_alias&) const;

	void
	__export_alias_properties(substructure_alias&) const;

	void
	__sift_ports(void);

	struct port_alias_predicate;

	void
	__shorten_canonical_aliases(
		instance_pool<state_instance<Tag> >&);

	// only applicable to channels, really
	error_count
	check_connections(void) const;

	void
	__import_port_aliases(const this_type&);

	struct port_alias_importer;

	void
	__synchronize_flags(void);

	size_t
	__port_offset(void) const;

	void
	__assign_frame(const substructure_alias&, footprint_frame&) const;

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
#if ENABLE_DATASTRUCTS
	private port_alias_tracker_base<datastruct_tag>,
#endif
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
	/**
		Cached flag that indicates whether or not any 
		internal aliases were found.  
		Beats having to recheck more than once.  
	 */
	bool						has_internal_aliases;
public:
	port_alias_tracker();
	~port_alias_tracker();

	ostream&
	dump(ostream&) const;

	ostream&
	dump_local_bool_aliases(ostream&) const;

	template <class Tag>
	typename port_alias_tracker_base<Tag>::map_type&
	get_id_map(void) { return port_alias_tracker_base<Tag>::_ids; }

	template <class Tag>
	const typename port_alias_tracker_base<Tag>::map_type&
	get_id_map(void) const { return port_alias_tracker_base<Tag>::_ids; }

	// should only be called from footprint::port_aliases
	template <class Tag>
	size_t
	port_frame_size(void) const {
		return port_alias_tracker_base<Tag>::__port_offset();
	}

	// should only be called from footprint::scope_aliases
	template <class Tag>
	size_t
	local_pool_size(void) const {
		return get_id_map<Tag>().size();
	}

	// only port_aliases as this, with Tag=process_tag
	void
	assign_alias_frame(const substructure_alias&, footprint_frame&) const;

public:

	good_bool
	replay_internal_aliases(substructure_alias&) const;

	void
	sift_ports(void);

	void
	export_alias_properties(substructure_alias&) const;

	void
	shorten_canonical_aliases(footprint&);

	void
	synchronize_flags(void);

	error_count
	check_bool_connections(void) const;

	error_count
	check_process_connections(void) const;

	error_count
	check_channel_connections(void) const;

	void
	import_port_aliases(const this_type&);

};	// end struct port_alias_tracker

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PORT_ALIAS_TRACKER_HH__

