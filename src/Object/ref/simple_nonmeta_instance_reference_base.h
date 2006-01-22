/**
	\file "Object/ref/simple_nonmeta_instance_reference_base.h"
	Most general non-meta instance references.  
	This file was "Object/art_object_nonmeta_inst_ref_base.h"
		in its previous life.  
	$Id: simple_nonmeta_instance_reference_base.h,v 1.5 2006/01/22 18:20:30 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_BASE_H__

#include "Object/ref/nonmeta_instance_reference_base.h"
#include "util/memory/excl_ptr.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
class scopespace;
class nonmeta_index_list;
class instance_collection_base;
using util::good_bool;
using util::persistent_object_manager;
using util::memory::never_ptr;
using util::memory::excl_ptr;

//=============================================================================
/**
	A simple non-meta instance reference may have nonmeta
	expressions and instance references in the index list.  
 */
class simple_nonmeta_instance_reference_base :
	virtual public nonmeta_instance_reference_base {
	typedef	simple_nonmeta_instance_reference_base	this_type;
	typedef	nonmeta_instance_reference_base		parent_type;
public:
	typedef	nonmeta_index_list			index_list_type;
protected:
	excl_ptr<index_list_type>			array_indices;
	// don't bother tracking instantiation state for non-meta references
protected:
	simple_nonmeta_instance_reference_base();
public:
	explicit
	simple_nonmeta_instance_reference_base(excl_ptr<index_list_type>&);

virtual	~simple_nonmeta_instance_reference_base();

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	size_t
	dimensions(void) const;

virtual	never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

	good_bool
	attach_indices(excl_ptr<index_list_type>&);

	// type equivalence methods...

public:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class simple_nonmeta_instance_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_SIMPLE_NONMETA_INSTANCE_REFERENCE_BASE_H__

