/**
	\file "Object/ref/meta_value_reference_base.hh"
	$Id: meta_value_reference_base.hh,v 1.6 2010/01/03 01:34:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__

#include <iosfwd>
#include "util/size_t.h"
#include "util/memory/pointer_classes_fwd.hh"
#include "Object/expr/types.hh"		// for pbool_value_type
#include "util/boolean_types.hh"
#include "util/persistent.hh"

namespace HAC {
namespace entity {
using std::ostream;
using util::memory::never_ptr;
using util::good_bool;
using util::persistent_object_manager;
class param_value_placeholder;
struct expr_dump_context;
class unroll_context;

//=============================================================================
/**
	Separate from meta_instance_reference_base.
	This interface represents lvalue referenceable values.  
	(whereas the param_expr hierarchy represents rvalues)
 */
class meta_value_reference_base
	: virtual public util::persistent
{
protected:
	meta_value_reference_base() { }
public:
virtual ~meta_value_reference_base();

virtual	size_t
	dimensions(void) const = 0;

virtual	ostream&
	what(ostream&) const = 0;

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

virtual	never_ptr<const param_value_placeholder>
	get_coll_base(void) const = 0;

virtual	good_bool
	unroll_resolve_defined(const unroll_context&,
		pbool_value_type&) const = 0;

virtual	void
	collect_transient_info(persistent_object_manager&) const = 0;

};	// end class meta_value_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__
