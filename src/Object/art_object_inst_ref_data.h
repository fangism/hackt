/**
	\file "art_object_inst_ref_data.h"
	Classes for datatype instance references (built-in and user-defined).
	$Id: art_object_inst_ref_data.h,v 1.1 2004/12/10 22:02:17 fang Exp $
 */

#ifndef	__ART_OBJECT_INST_REF_DATA_H__
#define	__ART_OBJECT_INST_REF_DATA_H__

#include "art_object_inst_ref.h"
#include "memory/pointer_classes.h"

namespace ART {
namespace entity {

using std::ostream;
using namespace util::memory;

//=============================================================================
// consider moving datatype_instance_reference here...
// consider moving datatype_member_instance_reference here...

//=============================================================================
/**
	Instance reference to built-in int type.
 */
class int_instance_reference : public datatype_instance_reference {
private:
	typedef	datatype_instance_reference		parent_type;
	typedef	int_instance_collection		instance_collection_type;
private:
	// excl_ptr<index_list>		array_indices;	// inherited

	const never_ptr<const instance_collection_type>	int_inst_ref;
private:
	int_instance_reference();
public:
	int_instance_reference(
		const never_ptr<const instance_collection_type> iic, 
		excl_ptr<index_list> i);

	~int_instance_reference();

	ostream&
	what(ostream& o) const;

#if 0
	// use simple_instance_reference's
	ostream&
	dump(ostream& o) const;
#endif

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class int_instance_reference

//=============================================================================
/**
	Instance reference to built-in bool (node) type.
 */
class bool_instance_reference : public datatype_instance_reference {
private:
	typedef	datatype_instance_reference		parent_type;
	typedef	bool_instance_collection	instance_collection_type;
private:
	// excl_ptr<index_list>		array_indices;	// inherited

	const never_ptr<const instance_collection_type>	bool_inst_ref;

private:
	bool_instance_reference();
public:
	bool_instance_reference(
		never_ptr<const instance_collection_type> bic, 
		excl_ptr<index_list> i);

	~bool_instance_reference();

	ostream&
	what(ostream& o) const;

#if 0
	// use simple_instance_reference's
	ostream&
	dump(ostream& o) const;
#endif

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class bool_instance_reference

//=============================================================================
/**
	Instance reference to a user-defined struct.  
 */
class datastruct_instance_reference : public datatype_instance_reference {
private:
	typedef	datatype_instance_reference		parent_type;
	typedef	struct_instance_collection	instance_collection_type;
private:
	// excl_ptr<index_list>		array_indices;	// inherited

	const never_ptr<const instance_collection_type>
					struct_inst_ref;

private:
	datastruct_instance_reference();
public:
	datastruct_instance_reference(
		never_ptr<const instance_collection_type> sic, 
		excl_ptr<index_list> i);

	~datastruct_instance_reference();

	ostream&
	what(ostream& o) const;

#if 0
	// use simple_instance_reference's
	ostream&
	dump(ostream& o) const;
#endif

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class datastruct_instance_reference

//=============================================================================
/**
	Instance reference to a user-defined enum.  
 */
class enum_instance_reference : public datatype_instance_reference {
private:
	typedef	datatype_instance_reference		parent_type;
	typedef	enum_instance_collection	instance_collection_type;
private:
	// excl_ptr<index_list>		array_indices;	// inherited

	const never_ptr<const instance_collection_type>
					enum_inst_ref;

private:
	enum_instance_reference();
public:
	enum_instance_reference(
		never_ptr<const instance_collection_type> sic, 
		excl_ptr<index_list> i);

	~enum_instance_reference();

	ostream&
	what(ostream& o) const;

#if 0
	// use simple_instance_reference's
	ostream&
	dump(ostream& o) const;
#endif

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

public:
	PERSISTENT_STATIC_MEMBERS_DECL
	PERSISTENT_METHODS
};	// end class enum_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_DATA_H__

