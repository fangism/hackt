/**
	\file "Object/def/template_formals_manager.h"
	Template formal manager class.  
	This file was "Object/def/template_formals_manager.h"
		in a former life.  
	$Id: template_formals_manager.h,v 1.11.18.1 2008/11/19 05:44:33 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_TEMPLATE_FORMALS_MANAGER_H__
#define	__HAC_OBJECT_DEF_TEMPLATE_FORMALS_MANAGER_H__

#include <iosfwd>
#include <vector>
#include <string>

#include "util/macros.h"
#include "util/boolean_types.h"
#include "util/persistent_fwd.h"	// for persistent object interface
#include "util/STL/hash_map.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
//=============================================================================
class const_param;
class param_value_placeholder;
class param_expr_list;
class dynamic_param_expr_list;
class const_param_expr_list;
class template_actuals;
class unroll_context;
using std::string;
using std::istream;
using std::ostream;
using std::vector;
using util::bad_bool;
using util::good_bool;
using util::persistent;
using util::persistent_object_manager;
using util::memory::excl_ptr;
using util::memory::never_ptr;
using util::memory::count_ptr;

//=============================================================================
/**
	Template formal parameter manager class, useful for most definition
	classes.  
 */
class template_formals_manager {
public:
	typedef	param_value_placeholder		placeholder_type;
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
	typedef	never_ptr<const placeholder_type>
					template_formals_value_type;
	// double-maintenance...
	typedef	HASH_MAP_NAMESPACE::hash_map<string,
			template_formals_value_type>
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

	size_t
	arity(void) const { return template_formals_map.size(); }

	ostream&
	dump(ostream& o) const;

	template_formals_value_type
	lookup_template_formal(const string& id) const;

	bool
	probe_relaxed_template_formal(const string& id) const;

	size_t
	lookup_template_formal_position(const string& id) const;

	good_bool
	check_null_template_argument(void) const;

	size_t
	num_strict_formals(void) const {
		return strict_template_formals_list.size();
	}

	size_t
	num_relaxed_formals(void) const {
		return relaxed_template_formals_list.size();
	}

	bool
	has_relaxed_formals(void) const;

	bool
	equivalent_template_formals(const template_formals_manager&) const;

	good_bool
	certify_template_arguments(template_actuals&) const;

	good_bool
	must_validate_actuals(const template_actuals&) const;

	template_actuals
	make_default_template_arguments(void) const;

	// assumes already checked for conflicts!
	void
	add_strict_template_formal(const template_formals_value_type);

	// assumes already checked for conflicts!
	void
	add_relaxed_template_formal(const template_formals_value_type);

	good_bool
	unroll_formal_parameters(const unroll_context&,
		const template_actuals&) const;
private:
	static
	good_bool
	__unroll_formal_parameters(const unroll_context&,
		const template_formals_list_type&, 
		const count_ptr<const dynamic_param_expr_list>&);
public:
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
}	// end namespace HAC

#if 0
// doesn't actually save space...
namespace std {
// to suppress implicit instantiation of template class

extern template class
vector<HAC::entity::template_formals_manager::template_formals_value_type>;
}	// end namespace std
#endif

#endif	// __HAC_OBJECT_DEF_TEMPLATE_FORMALS_MANAGER_H__

