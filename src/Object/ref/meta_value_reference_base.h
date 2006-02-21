/**
	\file "Object/ref/meta_value_reference_base.h"
	$Id: meta_value_reference_base.h,v 1.1.4.3 2006/02/21 00:30:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__

#include <iosfwd>
#include "util/size_t.h"
#include "util/memory/pointer_classes_fwd.h"

namespace HAC {
namespace entity {
using std::ostream;
using util::memory::never_ptr;
class param_value_collection;
struct expr_dump_context;

//=============================================================================
/**
	Separate from meta_instance_reference_base.
	This interface represents lvalue referenceable values.  
	(whereas the param_expr hierarchy represents rvalues)
 */
class meta_value_reference_base {
protected:
	meta_value_reference_base() { }
public:
virtual ~meta_value_reference_base() { }

virtual	size_t
	dimensions(void) const = 0;

virtual	ostream&
	what(ostream&) const = 0;

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

virtual	never_ptr<const param_value_collection>
	get_coll_base(void) const = 0;

virtual	bool
	must_be_initialized(void) const = 0;

};	// end class meta_value_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__
