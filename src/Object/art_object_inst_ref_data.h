/**
	\file "art_object_inst_ref_data.h"
	Classes for datatype instance references (built-in and user-defined).
	$Id: art_object_inst_ref_data.h,v 1.4.16.1.12.1 2005/02/19 06:56:49 fang Exp $
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
public:
	typedef	int_instance_collection		instance_collection_type;
	typedef	int_alias_connection		alias_connection_type;
private:
	// excl_ptr<index_list>		array_indices;	// inherited

	const never_ptr<const instance_collection_type>	int_inst_ref;
private:
	int_instance_reference();
public:
	explicit
	int_instance_reference(
		const never_ptr<const instance_collection_type> iic);

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

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_instance_reference

//=============================================================================
/**
	Instance reference to built-in bool (node) type.
 */
class bool_instance_reference : public datatype_instance_reference {
private:
	typedef	datatype_instance_reference		parent_type;
public:
	typedef	bool_instance_collection	instance_collection_type;
	typedef	bool_alias_connection		alias_connection_type;
private:
	// excl_ptr<index_list>		array_indices;	// inherited

	const never_ptr<const instance_collection_type>	bool_inst_ref;

private:
	bool_instance_reference();
public:
	explicit
	bool_instance_reference(
		const never_ptr<const instance_collection_type> bic);

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

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class bool_instance_reference

//=============================================================================
/**
	Instance reference to a user-defined struct.  
 */
class datastruct_instance_reference : public datatype_instance_reference {
private:
	typedef	datatype_instance_reference		parent_type;
public:
	typedef	struct_instance_collection	instance_collection_type;
	typedef	datastruct_alias_connection		alias_connection_type;
private:
	// excl_ptr<index_list>		array_indices;	// inherited

	const never_ptr<const instance_collection_type>
					struct_inst_ref;

private:
	datastruct_instance_reference();
public:
	explicit
	datastruct_instance_reference(
		const never_ptr<const instance_collection_type> sic);

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

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class datastruct_instance_reference

//=============================================================================
/**
	Instance reference to a user-defined enum.  
 */
class enum_instance_reference : public datatype_instance_reference {
private:
	typedef	datatype_instance_reference		parent_type;
public:
	typedef	enum_instance_collection	instance_collection_type;
	typedef	enum_alias_connection		alias_connection_type;
private:
	// excl_ptr<index_list>		array_indices;	// inherited

	const never_ptr<const instance_collection_type>
					enum_inst_ref;

private:
	enum_instance_reference();
public:
	explicit
	enum_instance_reference(
		const never_ptr<const instance_collection_type> eic);

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

private:
	excl_ptr<aliases_connection_base>
	make_aliases_connection_private(void) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class enum_instance_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INST_REF_DATA_H__

