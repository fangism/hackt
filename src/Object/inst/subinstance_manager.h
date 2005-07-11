/**
	\file "Object/inst/subinstance_manager.h"
	$Id: subinstance_manager.h,v 1.1.2.1 2005/07/11 20:19:24 fang Exp $
 */

#ifndef	__OBJECT_INST_SUBINSTANCE_MANAGER_H__
#define	__OBJECT_INST_SUBINSTANCE_MANAGER_H__

#include <vector>
#include "util/memory/count_ptr.h"
#include "Object/inst/substructure_alias_fwd.h"

namespace ART {
namespace entity {
class substructure_manager;
class instance_collection_base;
using std::ostream;
using std::istream;
using std::string;
using std::vector;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Contains an array of sub-instances, children collection of aliases.  
	Definitions will cache canonical maps containing already 
	instantiated types.  
	TODO: make them copy-able, reproducing internal connections!
 */
class subinstance_manager {
friend class substructure_manager;
	typedef	subinstance_manager			this_type;
public:
	typedef	count_ptr<instance_collection_base>	entry_value_type;
	typedef	vector<entry_value_type>		array_type;
protected:
	typedef	array_type::const_iterator		const_iterator;
	typedef	array_type::iterator			iterator;
protected:
	array_type					subinstance_array;
public:
	subinstance_manager();

	// explicit custom copy constructor
	explicit
	subinstance_manager(const this_type&);

	~subinstance_manager();

	// for each entry, re-link
	void
	relink_super_instance_alias(const substructure_alias&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class subinstance_manager

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_SUBINSTANCE_MANAGER_H__

