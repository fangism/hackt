/**
	\file "Object/inst/subinstance_manager.h"
	$Id: subinstance_manager.h,v 1.1.2.2 2005/07/13 21:56:43 fang Exp $
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
	NOTE: this only applies to public ports, 
		not any of the private internals!
	This will be tied closely to the port_formals_manager class.  
	Also interesting to note is that this level does NOT even depend
		on the strict template formals, as it only depends on
		the list of names in the port formals list, not their sizes.
		The types/sizes of the port actual entries will, however, 
		depend on the template parameters.  
	TODO: rename this to port_actuals_manager.
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

	bool
	empty(void) const { return subinstance_array.empty(); }

	void
	reserve(const size_t s) { subinstance_array.reserve(s); }

	void
	push_back(const entry_value_type& v) {
		subinstance_array.push_back(v);
	}

	// want to recursively expand ports when this is instantiated
	void
	unroll_port_instances(const instance_collection_base&);

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

