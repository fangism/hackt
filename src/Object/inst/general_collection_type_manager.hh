/**
	\file "Object/inst/general_collection_type_manager.hh"
	Template class for instance_collection's type manager.  
	$Id: general_collection_type_manager.hh,v 1.14 2009/03/09 07:30:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE		0
#endif

#include <iosfwd>
#include "Object/type/canonical_type.hh"
#include "util/persistent_fwd.hh"
#include "util/boolean_types.hh"
#include "Object/devel_switches.hh"

#if	ENABLE_STACKTRACE
#include <iostream>
#include "util/stacktrace.hh"
#endif

namespace HAC {
namespace entity {
using std::istream;
using std::ostream;
using util::good_bool;
using util::bad_bool;
using util::persistent_object_manager;
#if	ENABLE_STACKTRACE
#include "util/using_ostream.hh"
#endif
class const_param_expr_list;
template <class> struct class_traits;

//=============================================================================
/**
	Generic instance collection type manager for classes with
	full type references as type information.  
	Not appropriate for built-in types.  
 */
template <class Tag>
class general_collection_type_manager {
private:
	typedef	general_collection_type_manager<Tag>	this_type;
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename traits_type::type_ref_ptr_type
					type_ref_ptr_type;
	typedef	typename type_ref_ptr_type::element_type
					type_ref_type;
	typedef typename traits_type::resolved_type_ref_type
					resolved_type_ref_type;

	/**
		General type reference pointer for the collection.  
	 */
	instance_collection_parameter_type		type_parameter;

	struct dumper;

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	const instance_collection_parameter_type&
	__get_raw_type(void) const {
#if ENABLE_STACKTRACE
		const instance_collection_parameter_type&
			ret(this->type_parameter);
		ret.dump(STACKTRACE_INDENT << "canonical type: ") << endl;
		return ret;
#else
		return this->type_parameter;
#endif
	}

	resolved_type_ref_type
	get_resolved_canonical_type(void) const {
		return this->type_parameter;
	}

	good_bool
	complete_type_definition_footprint(
		const count_ptr<const const_param_expr_list>& r) const;

	bool
	is_complete_type(void) const;

	bool
	is_relaxed_type(void) const;

	static
	good_bool
	create_definition_footprint(
			const instance_collection_parameter_type& t, 
			const footprint& top) {
		return t.create_definition_footprint(top);
	}

protected:
	/**
		NOTE: called during connection checking.  
	 */
	bool
	must_be_collectibly_type_equivalent(const this_type&) const;

	bad_bool
	check_type(const instance_collection_parameter_type&) const;

	/**
		\param t type must be resolved constant.
		\pre first time called for the collection.  
	 */
	good_bool
	commit_type_first_time(const instance_collection_parameter_type& t);

};	// end struct general_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_GENERAL_COLLECTION_TYPE_MANAGER_H__

