/**
	\file "Object/inst/substructure_alias_base.h"
	$Id: substructure_alias_base.h,v 1.1.2.2 2005/07/14 03:15:41 fang Exp $
 */

#ifndef	__OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__
#define	__OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

#include <iosfwd>
#include "Object/inst/substructure_alias_fwd.h"
#include "Object/inst/subinstance_manager.h"
#include "util/persistent_fwd.h"

namespace ART {
namespace entity {
class physical_instance_collection;
using std::istream;
using std::ostream;
using util::persistent_object_manager;
//=============================================================================
/**
	Parent to all aliases that may contain substructures.
 */
template <>
class substructure_alias_base<true> {
protected:
	/**
		Container of sub-instances.  
	 */
	subinstance_manager			subinstances;
public:
virtual	~substructure_alias_base() { }

	template <class Tag>
	void
	unroll_port_instances(const instance_collection<Tag>& p) {
		subinstances.unroll_port_instances(p);
	}

	/**
		Visits children of the subinstance manager and 
		restores parent-child back-link.  
		Just calls subinstance_manager::relink_super_instance_alias().
	 */
	void
	restore_parent_child_links(void) {
		subinstances.relink_super_instance_alias(*this);
	}

	// call forwarding
	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager& m, ostream& o) const;

	void
	load_object_base(const persistent_object_manager& m, istream& i);

};	// end class substructure_alias_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Completely empty base class.  
 */
template <>
class substructure_alias_base<false> {
protected:
	// has no sub-instances
public:
	/**
		No-op.
	 */
	void
	unroll_port_instances(const physical_instance_collection&) const { }

	/**
		No-op.  
	 */
	void
	restore_parent_child_links(void) { }

	void
	collect_transient_info_base(const persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, 
		const ostream&) const { }

	void
	load_object_base(const persistent_object_manager&, 
		const istream&) const { }

};	// end class substructure_alias_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART


#endif	// __OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_H__

