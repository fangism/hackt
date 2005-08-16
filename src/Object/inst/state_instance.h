/**
	\file "Object/inst/state_instance.h"
	Class template for instance state.
	$Id: state_instance.h,v 1.2.4.3 2005/08/16 21:10:47 fang Exp $
 */

#ifndef	__OBJECT_INST_STATE_INSTANCE_H__
#define	__OBJECT_INST_STATE_INSTANCE_H__

#include <iosfwd>
#include "Object/traits/class_traits.h"
#include "util/memory/excl_ptr.h"
#include "Object/inst/instance_pool_fwd.h"

namespace util {
	class persistent_object_manager;
}

namespace ART {
namespace entity {
using std::istream;
using std::ostream;
using util::memory::never_ptr;

#define	STATE_INSTANCE_TEMPLATE_SIGNATURE	template <class Tag>
#define	STATE_INSTANCE_CLASS			state_instance<Tag>

//=============================================================================
/**
	Template for uniquely allocated state information.  
	Created during the create-unique compiler phase.  
	\param Tag the meta-class tag.  
 */
STATE_INSTANCE_TEMPLATE_SIGNATURE
class state_instance : public class_traits<Tag>::state_instance_base {
	typedef	STATE_INSTANCE_CLASS		this_type;
	typedef	typename class_traits<Tag>::instance_alias_info_type
						alias_info_type;
	typedef	typename class_traits<Tag>::state_instance_base
						state_instance_base;
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

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	typedef	instance_pool<this_type>	pool_type;
};	// end class state_instance

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_STATE_INSTANCE_H__

