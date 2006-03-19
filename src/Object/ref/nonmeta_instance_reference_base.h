/**
	\file "Object/ref/nonmeta_instance_reference_base.h"
	Most general non-meta instance references.  
	This file was "Object/art_object_nonmeta_inst_ref_base.h"
		in its previous life.  
	$Id: nonmeta_instance_reference_base.h,v 1.5.24.2 2006/03/19 06:14:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_BASE_H__

#include "util/persistent.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
struct expr_dump_context;
using std::istream;
using std::ostream;
using util::persistent;

//=============================================================================
/**
	The most general instance reference.  
	Non-meta means that reference may depend on non-meta (non parameter)
	values, such as datatypes, in the CHP context.  
	These are run-time values.  
	NOTE however that run-time constants (meta-params) meet the criteria
	of nonmeta references, so meta_instance_reference_base
	is derived from nonmeta_instance_reference_base.  
 */
class nonmeta_instance_reference_base : virtual public persistent {
	typedef	nonmeta_instance_reference_base		this_type;
protected:
	nonmeta_instance_reference_base() : persistent() { }
public:
virtual	~nonmeta_instance_reference_base() { }

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

#if NONMETA_TYPE_EQUIVALENCE
virtual	bool
	may_be_type_equivalent(const this_type&) const = 0;

virtual	bool
	must_be_type_equivalent(const this_type&) const = 0;
#endif

};	// end class nonmeta_instance_reference_base

//=============================================================================
// NOTE: probably not going to support
//	complex_aggregate_nonmeta_instance_reference
//	is too complicated, not particularly useful.  
//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_BASE_H__

