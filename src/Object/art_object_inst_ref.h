/**
	\file "art_object_inst_ref.h"
	Class family for instance references in ART.  
	$Id: art_object_inst_ref.h,v 1.15.16.1.10.2.2.2 2005/02/20 20:55:05 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_H__
#define	__ART_OBJECT_INST_REF_H__

#include "art_object_inst_ref_base.h"
#include "art_object_instance_base.h"
#include "memory/pointer_classes.h"

namespace ART {
namespace entity {
using std::ostream;
using std::istream;
using namespace util::memory;

//=============================================================================
#define	INSTANCE_REFERENCE_TEMPLATE_SIGNATURE				\
template <class Collection, class Parent>

#define	INSTANCE_REFERENCE_CLASS					\
instance_reference<Collection,Parent>

/**
	Class template for physical instance references.
	Needs to be virtual so that member_instance_reference may safely
	derive from this class.  
	\param Collection the instance collection type.
	\param Parent the type from which this is derived, 
		probably simple_instance_reference or descendant.  
 */
INSTANCE_REFERENCE_TEMPLATE_SIGNATURE
class instance_reference : public Parent {
	typedef	instance_reference<Collection,Parent>	this_type;
protected:
	typedef	Parent					parent_type;
public:
	typedef	Collection			instance_collection_type;
	typedef	typename instance_collection_type::instance_alias_type
						instance_alias_type;
	typedef	typename instance_collection_type::alias_connection_type
						alias_connection_type;
	typedef	never_ptr<const instance_collection_type>
						instance_collection_ptr_type;
private:
	const instance_collection_ptr_type	inst_collection_ref;
protected:
	instance_reference();
public:
	explicit
	instance_reference(const instance_collection_ptr_type);

virtual	~instance_reference();

	ostream&
	what(ostream&) const;

#if 0
	ostream&
	dump(ostream&) const;
#else
	using parent_type::dump;
#endif

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

protected:
	void
	collect_transient_info_base(persistent_object_manager& ) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

public:
	VIRTUAL_PERSISTENT_METHODS_DECLARATIONS

};	// end class instance_reference

//=============================================================================
// consider relocating to "art_object_inst_ref_data.h"
/**
	A reference to a simple instance of datatype.  
	Consider sub-typing into user-defined and built-in, 
	making this an abstract base.
 */
class datatype_instance_reference : public simple_instance_reference {
private:
	typedef	simple_instance_reference		parent_type;
protected:
//	excl_ptr<index_list>			array_indices;	// inherited

protected:
	datatype_instance_reference();

	explicit
	datatype_instance_reference(const instantiation_state& s);

public:
virtual	~datatype_instance_reference();

virtual	ostream&
	what(ostream& o) const = 0;

//	ostream& dump(ostream& o) const;

virtual	never_ptr<const instance_collection_base>
	get_inst_base(void) const = 0;

private:
virtual	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_instance_reference

//=============================================================================
// classes pint_instance_reference and pbool_instance_reference
//	are in "art_object_expr_param_ref.*"

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_H__

