/**
	\file "Object/state_manager.h"
	Declaration for the creation state management facilities.  
	$Id: state_manager.h,v 1.18.2.1 2010/01/09 03:30:01 fang Exp $
 */

#ifndef	__HAC_OBJECT_STATE_MANAGER_H__
#define	__HAC_OBJECT_STATE_MANAGER_H__

#include "Object/devel_switches.h"
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
// #error OBSOLETE!!!
#else
#include <iosfwd>
#include "util/persistent_fwd.h"
#include "Object/traits/classification_tags.h"
#include "Object/common/frame_map.h"
#include "util/list_vector.h"
#include "util/memory/index_pool.h"
#include "util/boolean_types.h"

namespace HAC {
class cflat_options;

namespace entity {
class footprint;
class state_manager;
class entry_collection;	// defined in "Object/entry_collection.h"
struct expr_dump_context;
using std::istream;
using std::ostream;
using util::good_bool;
using util::persistent_object_manager;
using util::list_vector;
using util::memory::index_pool;

namespace PRS {
	class cflat_visitor;
}

template <class Tag> struct global_entry;

//=============================================================================
/**
	Global state allocation pool.  
	Is 1-indexed, because first entry is null in the pool.  
	TODO: consider a flatten operation that will turn the list vector
		into a monolithic vector for constant-time access.
 */
template <class Tag>
class global_entry_pool :
	protected index_pool<list_vector<global_entry<Tag> > > {
public:
	typedef	global_entry<Tag>			entry_type;
	typedef	index_pool<list_vector<entry_type> >	pool_type;
private:
	typedef	global_entry_pool<Tag>			this_type;
	typedef	Tag					tag_type;
	typedef	typename pool_type::iterator		iterator;
public:
	typedef	typename pool_type::const_iterator	const_iterator;
	global_entry_pool();
	~global_entry_pool();

private:
	/// yes, a private explicit copy-constructor
	explicit
	global_entry_pool(const this_type&);

	using pool_type::set_chunk_size;
public:
	using pool_type::size;
	using pool_type::operator[];
	using pool_type::begin;
	using pool_type::end;
	using pool_type::allocate;
	using pool_type::clear;

protected:
	ostream&
	dump(ostream&, const footprint&) const;

	ostream&
	dump_dot_nodes(ostream&, const footprint&) const;

	void
	accept(PRS::cflat_visitor&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	ostream&
	__dump_memory_usage(ostream&) const;

	using pool_type::flatten;
};	// end class global_entry_pool

//=============================================================================
/**
	The state_manager tracks the unique instances for the entire program.  
	Most simulators will interact with this.  
	Inheriting from base classes protectedly (was private) because of
	explanation at http://gcc.gnu.org/bugzilla/show_bug.cgi?id=12265
	As of g++-4.4 anything but public inheritance is rejected
	in Object/state_manager.cc during static_cast... need to think why...
	4.3 and earlier accepted this, perhaps wrongly?
 */
class state_manager :
	// public cflat_visitee?
	public global_entry_pool<process_tag>, 
	public global_entry_pool<channel_tag>, 
#if ENABLE_DATASTRUCTS
	public global_entry_pool<datastruct_tag>, 
#endif
	public global_entry_pool<enum_tag>, 
	public global_entry_pool<int_tag>, 
	public global_entry_pool<bool_tag> {
	typedef	state_manager				this_type;
	typedef	global_entry_pool<process_tag>		process_pool_type;
	typedef	global_entry_pool<channel_tag>		channel_pool_type;
#if ENABLE_DATASTRUCTS
	typedef	global_entry_pool<datastruct_tag>	struct_pool_type;
#endif
	typedef	global_entry_pool<enum_tag>		enum_pool_type;
	typedef	global_entry_pool<int_tag>		int_pool_type;
	typedef	global_entry_pool<bool_tag>		bool_pool_type;

public:
	state_manager();
	~state_manager();

	/**
		Just static cast to one of the base types.  
	 */
	template <class Tag>
	global_entry_pool<Tag>&
	get_pool(void) { return *this; }

	/**
		Just static cast to one of the base types (const).  
	 */
	template <class Tag>
	const global_entry_pool<Tag>&
	get_pool(void) const { return *this; }

	template <class Tag>
	size_t
	allocate(void);

	template <class Tag>
	size_t
	allocate(const typename global_entry_pool<Tag>::entry_type&);

	ostream&
	dump(ostream&, const footprint&) const;

	ostream&
	dump_dot_instances(ostream&, const footprint&) const;

	void
	collect_transient_info_base(persistent_object_manager&, 
		const footprint&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&, 
		const footprint&) const;

	void
	load_object_base(const persistent_object_manager&, istream&, 
		const footprint&);

	/// cflat tree walker
	void
	accept(PRS::cflat_visitor&) const;

	template <class Tag>
	void
	__accept(PRS::cflat_visitor&) const;

	template <class Tag>
	void
	collect_subentries(entry_collection&, const size_t) const;

	// TODO: templatize to other metatypes (with structure)
	expr_dump_context
	make_process_dump_context(const footprint&, const size_t) const;

	/**
		Frequenty used function...
		Lookups up global process index, and returns the corresponding
		bool frame map.  
	 */
	const footprint_frame_map_type&
	get_bool_frame_map(const size_t pid) const;

	ostream&
	dump_memory_usage(ostream&) const;

	void
	optimize_pools(void);

	void
	clear(void);

private:
	explicit
	state_manager(const this_type&);

	/// never actually called
	template <class Tag>
	void
	__allocate_test(void);

	/// never actually called
	void
	allocate_test(void);

	void
	__collect_subentries_test(void) const;

};	// end class state_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC
#endif	// MEMORY_MAPPED_GLOBAL_ALLOCATION

#endif	// __HAC_OBJECT_STATE_MANAGER_H__

