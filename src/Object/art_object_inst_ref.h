/**
	\file "art_object_inst_ref.h"
	Class family for instance references in ART.  
	$Id: art_object_inst_ref.h,v 1.15.16.1.10.4 2005/02/21 19:48:08 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_H__
#define	__ART_OBJECT_INST_REF_H__

#include "art_object_inst_ref_base.h"
#include "art_object_instance_base.h"
#include "memory/pointer_classes.h"
#include "packed_array.h"

namespace ART {
namespace entity {
class unroll_context;
using std::ostream;
using std::istream;
using namespace util::memory;
using util::packed_array_generic;

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
	/// the instance collection base type
	typedef	Collection			instance_collection_type;
	/// the type of alias element contained by instance collections
	typedef	typename instance_collection_type::instance_alias_type
						instance_alias_type;
	/// the type of connections formed by the alias type
	typedef	typename instance_collection_type::alias_connection_type
						alias_connection_type;
	/// pointer type for instance collections
	typedef	never_ptr<const instance_collection_type>
						instance_collection_ptr_type;
	/// type used to unroll collections of instance aliases
	typedef	packed_array_generic<never_ptr<instance_alias_type> >
						alias_collection_type;
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

	// overridden by member_instance_reference
virtual	bool
	unroll_references(unroll_context&, alias_collection_type&) const;

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

