/**
	\file "Object/art_object_inst_ref_data.h"
	Classes for datatype instance references (built-in and user-defined).
	$Id: art_object_inst_ref_data.h,v 1.7.4.2 2005/06/04 04:47:58 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_REF_DATA_H__
#define	__OBJECT_ART_OBJECT_INST_REF_DATA_H__

#include "Object/art_object_inst_ref.h"
// might as well include this here because data_reference<Tag>::interface_type
// is going to be int_expr or bool_expr from the base class header file.  
#include "Object/art_object_data_expr_base.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	A reference to a simple instance of datatype.  
	Consider sub-typing into user-defined and built-in, 
	making this an abstract base.
 */
class datatype_meta_instance_reference_base : public simple_meta_instance_reference {
private:
	typedef simple_meta_instance_reference               parent_type;
protected:
//      excl_ptr<index_list>                    array_indices;  // inherited

protected:
	datatype_meta_instance_reference_base();

	explicit
	datatype_meta_instance_reference_base(const instantiation_state& s);

public:
virtual ~datatype_meta_instance_reference_base();

virtual ostream&
	what(ostream& o) const = 0;

//      ostream& dump(ostream& o) const;

virtual never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

private:
virtual excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};      // end class datatype_meta_instance_reference_base

//=============================================================================
#define	DATA_REFERENCE_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	DATA_REFERENCE_CLASS					\
data_reference<Tag>

/**
	Data instance reference for built-in types need to be accepted
	as data-expressions in CHP.  
	Note: member_inst_reference derives from this, 
		so we need at least a virtual dtor.  
 */
DATA_REFERENCE_TEMPLATE_SIGNATURE
class data_reference :
		public meta_instance_reference<Tag>,
		public class_traits<Tag>::data_expr_base_type {
	typedef	DATA_REFERENCE_CLASS				this_type;
	typedef	meta_instance_reference<Tag>				parent_type;
public:
	typedef	typename class_traits<Tag>::data_expr_base_type	interface_type;
	typedef	typename class_traits<Tag>::data_value_type	data_value_type;
	typedef	typename parent_type::instance_collection_ptr_type
						instance_collection_ptr_type;
protected:
	data_reference();
public:
	explicit
	data_reference(const instance_collection_ptr_type);

virtual	~data_reference();	// non-virtual

	ostream&
	what(ostream&) const;

#if 0
	using parent_type::dump;
	using parent_type::dimensions;
#else
	ostream&
	dump(ostream&) const;

	size_t
	dimensions(void) const;
#endif

protected:
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class data reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_REF_DATA_H__

