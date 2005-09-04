/**
	\file "Object/inst/state_instance.h"
	Class template for instance state.
	$Id: state_instance.h,v 1.3 2005/09/04 21:14:53 fang Exp $
 */

#ifndef	__OBJECT_INST_STATE_INSTANCE_H__
#define	__OBJECT_INST_STATE_INSTANCE_H__

#include <iosfwd>
#include "Object/traits/class_traits.h"
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"
#include "Object/inst/instance_pool_fwd.h"

namespace util {
	class persistent_object_manager;
}

namespace ART {
namespace entity {
class const_param_expr_list;
using std::istream;
using std::ostream;
using util::memory::never_ptr;
using util::memory::count_ptr;

#define	STATE_INSTANCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	STATE_INSTANCE_CLASS			state_instance<Tag>

typedef	count_ptr<const const_param_expr_list>
					state_instance_actuals_ptr_type;

//=============================================================================
/**
	This class is just an indexed placeholder for state allocation, 
	it doesn't actually contain any state information.  
	Created during the create-unique compiler phase.  
	\param Tag the meta-class tag.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
class state_instance {
	typedef	STATE_INSTANCE_CLASS		this_type;
	typedef	typename class_traits<Tag>::instance_alias_info_type
						alias_info_type;
public:
	typedef	Tag				tag_type;
private:
	never_ptr<const alias_info_type>	back_ref;
public:
	state_instance();

	explicit
	state_instance(const alias_info_type&);

	~state_instance();

	never_ptr<const alias_info_type>
	get_back_ref(void) const {
		return back_ref;
	}

	ostream&
	dump(ostream&) const;

#if 0
#define	STATE_INSTANCE_GET_ACTUALS_PROTO				\
	state_instance_actuals_ptr_type					\
	get_actuals(void) const

#define	STATE_INSTANCE_SET_ACTUALS_PROTO				\
	void								\
	set_actuals(const state_instance_actuals_ptr_type& arg) const
#endif

#define	STATE_INSTANCE_PERSISTENCE_PROTOS				\
	void								\
	collect_transient_info_base(persistent_object_manager&) const;	\
	void								\
	write_object_base(const persistent_object_manager&, ostream&) const; \
	void								\
	load_object_base(const persistent_object_manager&, istream&);

#if 0
#define	STATE_INSTANCE_PERSISTENCE_EMPTY_DEFS				\
	void								\
	collect_transient_info_base(persistent_object_manager&) const { } \
	void								\
	write_object_base(const persistent_object_manager&,		\
		ostream&) const { }					\
	void								\
	load_object_base(const persistent_object_manager&, istream&) { }
#endif

	STATE_INSTANCE_PERSISTENCE_PROTOS

public:
	typedef	instance_pool<this_type>	pool_type;
};	// end class state_instance

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_STATE_INSTANCE_H__

