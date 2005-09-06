/**
	\file "Object/state_manager.h"
	Declaration for the creation state management facilities.  
	$Id: state_manager.h,v 1.2.10.1 2005/09/06 05:56:46 fang Exp $
 */

#ifndef	__OBJECT_STATE_MANAGER_H__
#define	__OBJECT_STATE_MANAGER_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "Object/traits/classification_tags.h"
// #include "Object/global_entry.h"
#include "util/list_vector.h"

namespace ART {
namespace entity {
using std::istream;
using std::ostream;
using util::persistent_object_manager;

template <class Tag> struct global_entry;

//=============================================================================
template <class Tag>
class global_entry_pool : private util::list_vector<global_entry<Tag> > {
public:
	typedef	global_entry<Tag>			entry_type;
private:
	typedef	global_entry_pool<Tag>			this_type;
	typedef	Tag					tag_type;
	typedef	util::list_vector<entry_type>		pool_type;
	typedef	typename pool_type::const_iterator	const_iterator;
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

	size_t
	allocate(void);

	size_t
	allocate(const entry_type&);

};	// end class global_entry_pool

//=============================================================================
/**
	The state_manager tracks the unique instances for the entire program.  
	Most simulators will interact with this.  
 */
class state_manager :
	public global_entry_pool<process_tag>, 
	public global_entry_pool<channel_tag>, 
	public global_entry_pool<datastruct_tag>, 
	public global_entry_pool<enum_tag>, 
	public global_entry_pool<int_tag>, 
	public global_entry_pool<bool_tag> {
	typedef	global_entry_pool<process_tag>		process_pool_type;
	typedef	global_entry_pool<channel_tag>		channel_pool_type;
	typedef	global_entry_pool<datastruct_tag>	struct_pool_type;
	typedef	global_entry_pool<enum_tag>		enum_pool_type;
	typedef	global_entry_pool<int_tag>		int_pool_type;
	typedef	global_entry_pool<bool_tag>		bool_pool_type;

public:
	state_manager();
	~state_manager();

	template <class Tag>
	size_t
	allocate(void);

	template <class Tag>
	size_t
	allocate(const typename global_entry_pool<Tag>::entry_type&);

private:
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
}	// end namespace ART

#endif	// __OBJECT_STATE_MANAGER_H__

