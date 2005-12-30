/**
	\file "Object/state_manager.h"
	Declaration for the creation state management facilities.  
	$Id: state_manager.h,v 1.6.2.2 2005/12/30 17:41:23 fang Exp $
 */

#ifndef	__OBJECT_STATE_MANAGER_H__
#define	__OBJECT_STATE_MANAGER_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "Object/traits/classification_tags.h"
#include "util/list_vector.h"
#include "util/memory/index_pool.h"
#include "util/boolean_types.h"

namespace HAC {
class cflat_options;

namespace entity {
class footprint;
class state_manager;
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
	typedef	typename pool_type::const_iterator	const_iterator;
	typedef	typename pool_type::iterator		iterator;
public:
	global_entry_pool();
	~global_entry_pool();

private:
	/// yes, a private explicit copy-constructor
	explicit
	global_entry_pool(const this_type&);

public:
	using pool_type::size;
	using pool_type::operator[];
	using pool_type::begin;
	using pool_type::end;
	using pool_type::allocate;

protected:
	ostream&
	dump(ostream&, const footprint&) const;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&, 
		const footprint&) const;

	void
	load_object_base(const persistent_object_manager&, istream&, 
		const footprint&);

};	// end class global_entry_pool

//=============================================================================
/**
	The state_manager tracks the unique instances for the entire program.  
	Most simulators will interact with this.  
	Inheriting from base classes protectedly (was private) because of
	explanation at http://gcc.gnu.org/bugzilla/show_bug.cgi?id=12265
 */
class state_manager :
	// public cflat_visitee?
	protected global_entry_pool<process_tag>, 
	protected global_entry_pool<channel_tag>, 
	protected global_entry_pool<datastruct_tag>, 
	protected global_entry_pool<enum_tag>, 
	protected global_entry_pool<int_tag>, 
	protected global_entry_pool<bool_tag> {
	typedef	state_manager				this_type;
	typedef	global_entry_pool<process_tag>		process_pool_type;
	typedef	global_entry_pool<channel_tag>		channel_pool_type;
	typedef	global_entry_pool<datastruct_tag>	struct_pool_type;
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

#if 0
	good_bool
	cflat_prs(ostream&, const footprint&, const cflat_options&) const;
#endif

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&, 
		const footprint&) const;

	void
	load_object_base(const persistent_object_manager&, istream&, 
		const footprint&);

	/// cflat tree walker
	void
	accept(PRS::cflat_visitor&) const;

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
};	// end class state_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_STATE_MANAGER_H__

