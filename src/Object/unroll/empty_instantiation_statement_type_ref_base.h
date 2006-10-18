/**
	\file "Object/unroll/empty_instantiation_statement_type_ref_base.h"
	$Id: empty_instantiation_statement_type_ref_base.h,v 1.4 2006/10/18 08:52:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_EMPTY_INSTANTIATION_STATEMENT_TYPE_REF_BASE_H__
#define	__HAC_OBJECT_UNROLL_EMPTY_INSTANTIATION_STATEMENT_TYPE_REF_BASE_H__

#include <iosfwd>
#include "Object/expr/const_param_expr_list.h"
#include "Object/unroll/null_parameter_type.h"
#include "util/memory/count_ptr.h"

namespace util {
	class persistent_object_manager;
}

namespace HAC {
namespace entity {
using std::istream;
using std::ostream;
using util::persistent_object_manager;
using util::memory::count_ptr;

//=============================================================================
/**
	Handy empty class that does nothing but meet a convenient interface
	for persistent object management of type references.  
	For example this is used for meta parameter types.  
 */
class empty_instantiation_statement_type_ref_base {
public:
	// TODO: use typedef
	typedef	count_ptr<const dynamic_param_expr_list>
						const_relaxed_args_type;
	typedef	count_ptr<const const_param_expr_list>
						instance_relaxed_actuals_type;
	typedef	null_parameter_type	instance_collection_parameter_type;
public:

	instance_collection_parameter_type
	get_canonical_type(const unroll_context&) const {
		return instance_collection_parameter_type();
	}

	/**
		Relaxed parameters to not apply to built-in parameter types.  
	 */
	const_relaxed_args_type
	get_relaxed_actuals(void) const {
		return const_relaxed_args_type(NULL);
	}

	void
	collect_transient_info_base(persistent_object_manager&) const {
		// nothing to collect
	}

	void
	write_object_base(const persistent_object_manager&, ostream&) const {
		// nothing to write
	}

	void
	load_object_base(const persistent_object_manager&, istream&) {
		// nothing to load
	}

};      // end class empty_instantiation_statement_type_ref_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_UNROLL_EMPTY_INSTANTIATION_STATEMENT_TYPE_REF_BASE_H__

