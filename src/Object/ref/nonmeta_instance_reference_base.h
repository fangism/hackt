/**
	\file "Object/ref/nonmeta_instance_reference_base.h"
	Most general non-meta instance references.  
	This file was "Object/art_object_nonmeta_inst_ref_base.h"
		in its previous life.  
	$Id: nonmeta_instance_reference_base.h,v 1.2 2005/07/23 06:52:48 fang Exp $
 */

#ifndef	__OBJECT_REF_NONMETA_INSTANCE_REFERENCE_BASE_H__
#define	__OBJECT_REF_NONMETA_INSTANCE_REFERENCE_BASE_H__

#include "util/persistent.h"
// includes <iosfwd> already

namespace ART {
namespace entity {
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
protected:
	nonmeta_instance_reference_base() : persistent() { }
public:
virtual	~nonmeta_instance_reference_base() { }

virtual	ostream&
	dump(ostream&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

};	// end class nonmeta_instance_reference_base

//=============================================================================
// NOTE: probably not going to support
//	complex_aggregate_nonmeta_instance_reference
//	is too complicated, not particularly useful.  
//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_REF_NONMETA_INSTANCE_REFERENCE_BASE_H__

