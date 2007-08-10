/**
	\file "Object/inst/instance_fwd.h"
	Forward declarations for all instance and alias classes.  
	$Id: instance_fwd.h,v 1.3.38.1 2007/08/10 06:49:48 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INSTANCE_FWD_H__
#define	__HAC_OBJECT_INST_INSTANCE_FWD_H__

#include "util/size_t.h"
#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {

template <class>
class state_instance;

class pint_instance;	// should be value
class pbool_instance;	// should be value
class preal_instance;	// should be value
typedef state_instance<int_tag>		int_instance;
typedef state_instance<bool_tag>	bool_instance;
typedef state_instance<enum_tag>	enum_instance;
typedef state_instance<real_tag>	real_instance;
typedef state_instance<datastruct_tag>	struct_instance;
typedef state_instance<channel_tag>	channel_instance;
typedef state_instance<process_tag>	process_instance;

template <class>		class instance_alias_info;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INSTANCE_FWD_H__

