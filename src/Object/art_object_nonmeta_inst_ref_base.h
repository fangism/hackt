/**
	\file "Object/art_object_nonmeta_inst_ref_base.h"
	Most general non-meta instance references.  
	$Id: art_object_nonmeta_inst_ref_base.h,v 1.1.2.1 2005/06/04 04:48:03 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__
#define	__OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__

#include <iosfwd>
#include "util/persistent.h"

namespace ART {
namespace entity {
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
/**
	A simple non-meta instance reference may have nonmeta
	expressions and instance references in the index list.  
 */
class simple_nonmeta_instance_reference :
	public nonmeta_instance_reference_base {
};	// end class simple_nonmeta_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__

