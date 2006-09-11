/**
	\file "Object/ref/meta_value_reference_base.h"
	$Id: meta_value_reference_base.h,v 1.2.32.1.4.1 2006/09/11 02:39:25 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__

#include <iosfwd>
#include "util/size_t.h"
#include "util/memory/pointer_classes_fwd.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
using std::ostream;
using util::memory::never_ptr;
#if USE_INSTANCE_PLACEHOLDERS
class param_value_placeholder;
#else
class param_value_collection;
#endif
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

#if USE_INSTANCE_PLACEHOLDERS
virtual	never_ptr<const param_value_placeholder>
#else
virtual	never_ptr<const param_value_collection>
#endif
	get_coll_base(void) const = 0;

#if ENABLE_STATIC_ANALYSIS
virtual	bool
	must_be_initialized(void) const = 0;
#endif

};	// end class meta_value_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC


#endif	// __HAC_OBJECT_REF_META_VALUE_REFERENCE_BASE_H__
