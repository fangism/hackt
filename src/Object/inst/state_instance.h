/**
	\file "Object/inst/state_instance.h"
	Class template for instance state.
	$Id: state_instance.h,v 1.13 2010/04/07 00:12:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_STATE_INSTANCE_H__
#define	__HAC_OBJECT_INST_STATE_INSTANCE_H__

#include <iosfwd>
#include "Object/traits/class_traits.h"
#include "util/memory/count_ptr.h"
#include "util/memory/excl_ptr.h"
#include "Object/inst/instance_pool_fwd.h"
#include "Object/global_entry.h"

namespace util {
	class persistent_object_manager;
}

namespace HAC {
namespace entity {
class const_param_expr_list;
using std::istream;
using std::ostream;
using util::memory::never_ptr;
using util::memory::count_ptr;
using util::persistent_object_manager;
struct global_entry_context;
struct global_entry_dumper;
template <class> class instance_alias_info;
template <class> class instance_collection_pool_bundle;

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
class state_instance : public global_entry<Tag> {
	typedef	STATE_INSTANCE_CLASS		this_type;
public:
	typedef	global_entry<Tag>		entry_type;
	typedef	class_traits<Tag>		traits_type;
private:
	typedef	instance_alias_info<Tag>	alias_info_type;
	typedef	instance_collection_pool_bundle<Tag>
					collection_pool_bundle_type;
public:
	typedef	never_ptr<const alias_info_type>	back_ref_type;
	typedef	Tag				tag_type;
private:
	back_ref_type				back_ref;
public:
	state_instance();

	explicit
	state_instance(const alias_info_type&);

	~state_instance();

	back_ref_type
	get_back_ref(void) const { return back_ref; }

	void
	set_back_ref(const back_ref_type b) { back_ref = b; }

	ostream&
	dump(ostream&) const;

	void
	accept(global_entry_context&) const;

#define	STATE_INSTANCE_PERSISTENCE_PROTOS				\
	void								\
	collect_transient_info_base(persistent_object_manager&) const;	\
	void								\
	write_object_base(const collection_pool_bundle_type&, ostream&) const; \
	void								\
	load_object_base(const collection_pool_bundle_type&, istream&);

	STATE_INSTANCE_PERSISTENCE_PROTOS

	void
	write_object(const collection_pool_bundle_type& m, ostream& o) const {
		write_object_base(m, o);
	}
	void
	load_object(const collection_pool_bundle_type& m, istream& i) {
		load_object_base(m, i);
	}

public:
	typedef	instance_pool<this_type>	pool_type;
};	// end class state_instance

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_STATE_INSTANCE_H__

