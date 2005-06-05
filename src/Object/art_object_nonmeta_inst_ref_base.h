/**
	\file "Object/art_object_nonmeta_inst_ref_base.h"
	Most general non-meta instance references.  
	$Id: art_object_nonmeta_inst_ref_base.h,v 1.1.2.3 2005/06/05 22:06:14 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__
#define	__OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__

#include <iosfwd>
#include "util/persistent.h"
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
class nonmeta_index_list;
using std::istream;
using std::ostream;
using util::persistent;
using util::persistent_object_manager;
using util::memory::excl_ptr;
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
#if 1
/**
	A simple non-meta instance reference may have nonmeta
	expressions and instance references in the index list.  
 */
class simple_nonmeta_instance_reference :
	virtual public nonmeta_instance_reference_base {
	typedef	simple_nonmeta_instance_reference	this_type;
public:
	typedef	nonmeta_index_list			index_list_type;
protected:
	excl_ptr<index_list_type>			array_indices;
	// don't bother tracking instantiation state for non-meta references
protected:
	simple_nonmeta_instance_reference();
public:
	explicit
	simple_nonmeta_instance_reference(excl_ptr<index_list_type>&);

virtual	~simple_nonmeta_instance_reference();

	ostream&
	dump(ostream&) const;

	// type equivalence methods...

public:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class simple_nonmeta_instance_reference
#endif

//=============================================================================
// NOTE: probably not going to support
//	complex_aggregate_nonmeta_instance_reference
//	is too complicated, not particularly useful.  
//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__

