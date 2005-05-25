/**
	\file "Object/art_object_template_formals_manager.h"
	Template formal manager class.  
	$Id: art_object_template_formals_manager.h,v 1.4.2.2 2005/05/25 22:35:43 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_TEMPLATE_FORMALS_MANAGER_H__
#define	__OBJECT_ART_OBJECT_TEMPLATE_FORMALS_MANAGER_H__

#include <iosfwd>
#include <vector>

#include "util/macros.h"
#include "Object/art_object_fwd.h"

#include "util/boolean_types.h"
#include "util/persistent_fwd.h"	// for persistent object interface
#include "util/hash_qmap.h"	// need complete definition
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
//=============================================================================
using std::string;
using std::istream;
using std::ostream;
using std::vector;
using util::bad_bool;
using util::good_bool;
using util::hash_qmap;
using util::persistent;
using util::persistent_object_manager;
using util::memory::excl_ptr;
using util::memory::never_ptr;

//=============================================================================
/**
	Template formal parameter manager class, useful for most definition
	classes.  
 */
class template_formals_manager {
public:
	/**
		Table of template formals.  
		Needs to be ordered for argument checking, 
		and have fast lookup, thus hashlist.  
		Remember: template formals are accessible to the rest 
		of the body and to the port formals as well.  
		For now, the contained type is datatype_instance_collection
			which is generalized to include the paramater types
			pbool and pint, not to be confused with the data 
			types bool and int.  
		In the far future, prepare to extend template formals to 
			include abstract types of processes, channels and 
			data types in template argument list.  
			*shudder*
			It'd be nice to be able to swap instance arguments
			that preserve specified interfaces...
		May need hashqlist, for const-queryable hash structure!!!
	**/
	typedef	never_ptr<const param_instance_collection>
					template_formals_value_type;
	// double-maintenance...
	typedef	hash_qmap<string, template_formals_value_type>
					template_formals_map_type;

	/**
		Using vector instead of list, for constant-time
		position computation, via iterator distance.  
	 */
	typedef	vector<template_formals_value_type>
					template_formals_list_type;
protected:
	/**
		subset of used_id_map, must be coherent with list
	 */
	template_formals_map_type	template_formals_map;

	/**
		subset of used_id_map, contains strictly matched parameters, 
		must be coherent with map
	 */
	template_formals_list_type	strict_template_formals_list;

	/**
		subset of used_id_map, contains relaxed-matched parameters, 
		must be coherent with map
	 */
	template_formals_list_type	relaxed_template_formals_list;

public:
	template_formals_manager();
	~template_formals_manager();

	ostream&
	dump(ostream& o) const;

#if 0
	/**
		OR pair of lists to distinguish between strict and relaxed
		template parameters.  
	 */
	void
	fill_template_actuals_map(template_actuals_map_type&, 
		const param_expr_list& al) const;
#endif

	never_ptr<const param_instance_collection>
	lookup_template_formal(const string& id) const;

	size_t
	lookup_template_formal_position(const string& id) const;

	good_bool
	check_null_template_argument(void) const;

	bool
	equivalent_template_formals(const template_formals_manager&) const;

	good_bool
	certify_template_arguments(
		const never_ptr<dynamic_param_expr_list> ) const;

	excl_ptr<dynamic_param_expr_list>
	make_default_template_arguments(void) const;

	// assumes already checked for conflicts!
	void
	add_strict_template_formal(
		const never_ptr<const param_instance_collection>);

	// assumes already checked for conflicts!
	void
	add_relaxed_template_formal(
		const never_ptr<const param_instance_collection>);

	void
	collect_transient_info_base(persistent_object_manager& m) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	static
	void
	write_object_base_fake(const persistent_object_manager&, ostream&);

	void
	load_object_base(const persistent_object_manager&, istream&);

private:
	static
	ostream&
	dump_formals_list(ostream& o, const template_formals_list_type&);

	static
	good_bool
	partial_check_null_template_argument(const template_formals_list_type&);

	static
	bool
	equivalent_template_formals_lists(const template_formals_list_type&, 
		const template_formals_list_type&, const string&);

	static
	void
	load_template_formals_list(const persistent_object_manager&, 
		template_formals_map_type&, template_formals_list_type&);

};	// end class template_formals_manager

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#if 0
// doesn't actually save space...
namespace std {
// to suppress implicit instantiation of template class

extern template class
vector<ART::entity::template_formals_manager::template_formals_value_type>;
}	// end namespace std
#endif

#endif	// __OBJECT_ART_OBJECT_TEMPLATE_FORMALS_MANAGER_H__

