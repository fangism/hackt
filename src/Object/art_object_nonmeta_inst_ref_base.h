/**
	\file "Object/art_object_nonmeta_inst_ref_base.h"
	Most general non-meta instance references.  
	$Id: art_object_nonmeta_inst_ref_base.h,v 1.1.4.3.2.1 2005/06/12 19:01:27 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__
#define	__OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__

#include <iosfwd>
#include "util/persistent.h"
#include "util/memory/excl_ptr.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
class nonmeta_index_list;
class instance_collection_base;
using std::istream;
using std::ostream;
using util::good_bool;
using util::persistent;
using util::persistent_object_manager;
using util::memory::never_ptr;
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
	dump_brief(ostream&) const;

	ostream&
	dump(ostream&) const;

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
/**
	A reference to a nonmeta data type.  
	Abstract base class.
	NOTE: both true datatypes and nonmeta-dependent 
		meta parameter references fall into this category.  
	This will be the base class interface for all
		simple_nonmeta_value_references.  
 */
class simple_datatype_nonmeta_value_reference :
	public simple_nonmeta_instance_reference_base {
protected:
	typedef	simple_nonmeta_instance_reference_base	parent_type;
protected:
	simple_datatype_nonmeta_value_reference() : parent_type() { }
public:
virtual	~simple_datatype_nonmeta_value_reference() { }

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

};	// end class simple_datatype_nonmeta_value_reference

//=============================================================================
// NOTE: probably not going to support
//	complex_aggregate_nonmeta_instance_reference
//	is too complicated, not particularly useful.  
//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_NONMETA_INST_REF_BASE_H__

