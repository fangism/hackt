/**
	\file "Object/inst/int_collection_type_manager.hh"
	Template class for instance_collection's type manager.  
	$Id: int_collection_type_manager.hh,v 1.13 2007/04/15 05:52:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_H__
#define	__HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_H__

#include <iostream>
#include "Object/type/canonical_type_fwd.hh"	// for conditional
#include "util/persistent_fwd.hh"
#include "util/boolean_types.hh"
#include "Object/expr/types.hh"		// for pint_value_type
#include "util/memory/pointer_classes_fwd.hh"

namespace HAC {
namespace entity {
class footprint;
class const_param_expr_list;
using std::istream;
using std::ostream;
#include "util/using_ostream.hh"
using util::good_bool;
using util::bad_bool;
using util::persistent_object_manager;
using util::memory::count_ptr;
template <class> struct class_traits;

//=============================================================================
/**
	Generic instance collection type manager for classes with
	full type references as type information.  
	Not appropriate for built-in types.  
 */
template <class Tag>
class int_collection_type_manager {
private:
	typedef	int_collection_type_manager<Tag>	this_type;
	typedef	class_traits<Tag>			traits_type;
protected:
	typedef typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	typedef typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef typename traits_type::instance_placeholder_type
					instance_placeholder_type;
	typedef typename traits_type::type_ref_ptr_type
					type_ref_ptr_type;
	typedef typename traits_type::resolved_type_ref_type
					resolved_type_ref_type;

	/**
		Type is a constant integer.  
	 */
	instance_collection_parameter_type		type_parameter;

	/**
		Width is zero before it is determined at unroll-time.
		Is this the correct thing to do?
		TODO: fix me for good.
	 */
	int_collection_type_manager() : type_parameter(0) { }

	struct dumper;

	void
	collect_transient_info_base(persistent_object_manager&) const { }

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	const instance_collection_parameter_type&
	__get_raw_type(void) const { return type_parameter; }

	resolved_type_ref_type
	get_resolved_canonical_type(void) const;

	good_bool
	complete_type_definition_footprint(
			const count_ptr<const const_param_expr_list>&) const {
		return good_bool(true);
	}

	bool
	is_complete_type(void) const { return true; }

	bool
	is_relaxed_type(void) const { return false; }

	/**
		ints don't have footprints.
	 */
	static
	good_bool
	create_definition_footprint(
			const instance_collection_parameter_type&, 
			const footprint& /* top */) {
		return good_bool(true);
	}

protected:
	/**
		NOTE: called during connection checking.  
	 */
	bool
	must_be_collectibly_type_equivalent(const this_type& t) const {
		if (type_parameter == t.type_parameter) {
			return true;
		} else {
			cerr << "ERROR: integer widths do not match!  "
				"have: " << type_parameter << " vs. " <<
				t.type_parameter << endl;
			return false;
		}
	}


	bad_bool
	check_type(const instance_collection_parameter_type&) const;

	/**
		\param t type must be resolved constant.
		\pre first time called for the collection.  
	 */
	good_bool
	commit_type_first_time(const instance_collection_parameter_type& t);

private:
	static
	pint_value_type
	get_int_width(const type_ref_ptr_type&);

};	// end struct int_collection_type_manager

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INT_COLLECTION_TYPE_MANAGER_H__

