/**
	\file "Object/ref/nonmeta_instance_reference_base.hh"
	Most general non-meta instance references.  
	This file was "Object/art_object_nonmeta_inst_ref_base.h"
		in its previous life.  
	$Id: nonmeta_instance_reference_base.hh,v 1.8 2011/02/25 23:19:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_NONMETA_INSTANCE_REFERENCE_BASE_H__

#include "util/persistent.hh"
#if 0
#include "util/memory/excl_ptr.hh"
#include "util/memory/count_ptr.hh"
#endif

namespace HAC {
namespace entity {
struct expr_dump_context;
class definition_base;
#if 0
class meta_instance_reference_base;
class nonmeta_context_base;
#endif
using std::istream;
using std::ostream;
using util::persistent;
#if 0
using util::memory::never_ptr;
using util::memory::count_ptr;
#endif

//=============================================================================
/**
	TODO: should this be fused with simple_nonmeta_instance_reference_base?
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
virtual	~nonmeta_instance_reference_base();

virtual	ostream&
	dump(ostream&, const expr_dump_context&) const = 0;

virtual	size_t
	dimensions(void) const = 0;

#if 0
virtual	never_ptr<const definition_base>
	get_base_def(void) const = 0;

virtual	count_ptr<meta_instance_reference_base>
	resolve_meta_reference(const nonmeta_context_base&) const = 0;
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

