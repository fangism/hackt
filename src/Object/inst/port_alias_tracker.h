/**
	\file "Object/inst/port_alias_tracker.h"
	Pair of classes used to keep track of port aliases.  
	Intended as replacement for port_alias_signature.
	$Id: port_alias_tracker.h,v 1.1.2.1 2005/09/04 01:58:13 fang Exp $
 */

#ifndef	__OBJECT_INST_PORT_ALIAS_TRACKER_H__
#define	__OBJECT_INST_PORT_ALIAS_TRACKER_H__

#include <iosfwd>
#include <map>
#include <vector>
#include "util/size_t.h"
#include "util/persistent_fwd.h"
#include "util/memory/excl_ptr.h"
#include "Object/traits/classification_tags.h"

namespace ART {
namespace entity {
using std::istream;
using std::ostream;
using util::persistent_object_manager;
using util::memory::never_ptr;

template <class Tag>
class instance_alias_info;

template <class Tag>
struct port_alias_tracker_getter;

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

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class alias_reference_set

//=============================================================================
/**
	Used to count aliases over strucutres whose connections have been
	played and unique placeholder ID numbers have been assigned.  
	Also keep track of instance references?
 */
class port_alias_tracker {
template <class> friend struct port_alias_tracker_getter;
public:
	/**
		Map used to track number of occurrences of indices.  
	 */
	template <class Tag>
	struct tracker_map_type {
		typedef	std::map<size_t, alias_reference_set<Tag> >	type;
	};

private:
	tracker_map_type<process_tag>::type		process_ids;
	tracker_map_type<channel_tag>::type		channel_ids;
	tracker_map_type<datastruct_tag>::type		struct_ids;
	tracker_map_type<enum_tag>::type		enum_ids;
	tracker_map_type<int_tag>::type			int_ids;
	tracker_map_type<bool_tag>::type		bool_ids;
	bool						has_internal_aliases;
public:
	port_alias_tracker();
	~port_alias_tracker();

	ostream&
	dump(ostream&) const;

private:
	template <class M>
	static
	void
	filter_unique(M&);

	template <class M>
	static
	ostream&
	dump_map(const M&, ostream&);

	template <class M>
	static
	void
	collect_map(const M&, persistent_object_manager&);

	template <class M>
	static
	void
	write_map(const M&, const persistent_object_manager&, ostream&);

	template <class M>
	static
	void
	load_map(M&, const persistent_object_manager&, istream&);

public:
	void
	filter_uniques(void);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end struct port_alias_tracker

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	SPECIALIZE_PORT_ALIAS_TRACKER_GETTER(Tag,Member)		\
template <>								\
struct port_alias_tracker_getter<Tag> {					\
	typedef	port_alias_tracker::tracker_map_type<Tag>::type		\
							map_type;	\
									\
	inline								\
	map_type&							\
	operator () (port_alias_tracker& p) const {			\
		return p.Member;					\
	}								\
									\
	inline								\
	const map_type&							\
	operator () (const port_alias_tracker& p) const {		\
		return p.Member;					\
	}								\
};	// end struct port_alias_tracker_getter
	
SPECIALIZE_PORT_ALIAS_TRACKER_GETTER(process_tag, process_ids)
SPECIALIZE_PORT_ALIAS_TRACKER_GETTER(channel_tag, channel_ids)
SPECIALIZE_PORT_ALIAS_TRACKER_GETTER(datastruct_tag, struct_ids)
SPECIALIZE_PORT_ALIAS_TRACKER_GETTER(enum_tag, enum_ids)
SPECIALIZE_PORT_ALIAS_TRACKER_GETTER(int_tag, int_ids)
SPECIALIZE_PORT_ALIAS_TRACKER_GETTER(bool_tag, bool_ids)

#undef	SPECIALIZE_PORT_ALIAS_TRACKER_GETTER

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

