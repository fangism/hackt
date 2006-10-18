/**
	\file "Object/inst/datatype_instance_placeholder.h"
	Instance placeholder classes for HAC.  
	This file came from "Object/art_object_instance.h" in a previous life. 
	$Id: datatype_instance_placeholder.h,v 1.3 2006/10/18 05:32:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_DATATYPE_INSTANCE_PLACEHOLDER_H__
#define	__HAC_OBJECT_INST_DATATYPE_INSTANCE_PLACEHOLDER_H__

#include "Object/type/canonical_type_fwd.h"		// for conditional
#include "Object/inst/physical_instance_placeholder.h"
#include "Object/traits/data_traits.h"

namespace HAC {
namespace entity {
class data_type_reference;
class datatype_instance_collection;
class footprint;

//=============================================================================
/**
	Base class for instantiation of a data type, 
	either inside or outside definition.  
 */
class datatype_instance_placeholder : public physical_instance_placeholder {
private:
	typedef	physical_instance_placeholder	parent_type;
	typedef	datatype_instance_placeholder	this_type;
	typedef	class_traits<datatype_tag>	traits_type;
protected:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	count_ptr<const data_type_reference>	type_ref_ptr_type;
protected:
	datatype_instance_placeholder() : parent_type() { }

	explicit
	datatype_instance_placeholder(const size_t d) : parent_type(d) { }

	datatype_instance_placeholder(const scopespace& o, const string& n, 
		const size_t d);

public:

virtual	~datatype_instance_placeholder();

	datatype_instance_collection*
	make_collection(void) const;

virtual	ostream&
	what(ostream& o) const = 0;

virtual	void
	attach_initial_instantiation_statement(
		const count_ptr<const instantiation_statement_base>&) = 0;

virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;

protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_instance_placeholder

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_DATATYPE_INSTANCE_PLACEHOLDER_H__

