/**
	\file "Object/type/builtin_channel_type_reference.h"
	Type-reference classes of the ART language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: builtin_channel_type_reference.h,v 1.3 2005/09/04 21:14:57 fang Exp $
 */

#ifndef	__OBJECT_TYPE_BUILTIN_CHANNEL_TYPE_REFERENCE_H__
#define	__OBJECT_TYPE_BUILTIN_CHANNEL_TYPE_REFERENCE_H__

#include <vector>
#include "Object/type/channel_type_reference_base.h"

namespace ART {
namespace entity {
class data_type_reference;
class builtin_channel_type_reference;
class channel_definition_base;
using std::vector;

//=============================================================================
/**
	Reference to an intrinsic channel type, chan(...).
	There is no built-in channel definition, which is why we need
	this channel type split off.  
 */
class builtin_channel_type_reference : public channel_type_reference_base {
	typedef	builtin_channel_type_reference		this_type;
	typedef	channel_type_reference_base		parent_type;
public:
	typedef	count_ptr<const data_type_reference>	datatype_ptr_type;
	typedef	vector<datatype_ptr_type>		datatype_list_type;
private:
	datatype_list_type				datatype_list;
public:
	builtin_channel_type_reference();
	~builtin_channel_type_reference();

	ostream&
	what(ostream& o) const;

	// overrides grandparent's
	ostream&
	dump(ostream&) const;

	ostream&
	dump_long(ostream&) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	bool
	is_canonical(void) const;

	void
	reserve_datatypes(const size_t);

	void
	add_datatype(const datatype_list_type::value_type&);

	size_t
	num_datatypes(void) const { return datatype_list.size(); }

	// for convenience...
	const datatype_list_type&
	get_datatype_list(void) const { return datatype_list; }

	datatype_ptr_type
	index_datatype(const size_t) const;

private:
	struct datatype_list_comparison;
public:
	TYPE_EQUIVALENT_PROTOS

	bool
	may_be_collectibly_channel_type_equivalent(const this_type&) const;

	bool
	must_be_collectibly_channel_type_equivalent(const this_type&) const;

	bool
	may_be_connectibly_channel_type_equivalent(const this_type&) const;

	bool
	must_be_connectibly_channel_type_equivalent(const this_type&) const;

private:
	// consider using member function template...
	struct datatype_resolver;
	struct datatype_canonicalizer;
	struct yet_another_datatype_canonicalizer;

public:
	count_ptr<const channel_type_reference_base>
	unroll_resolve(const unroll_context&) const;

	never_ptr<const builtin_channel_type_reference>
	resolve_builtin_channel_type(void) const;

	UNROLL_PORT_INSTANCES_PROTO;

	canonical_type<channel_definition_base>
	make_canonical_type(void) const;

private:
	unroll_context
	make_unroll_context(void) const;

	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO;
			
	MAKE_INSTANCE_COLLECTION_PROTO;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class builtin_channel_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_BUILTIN_CHANNEL_TYPE_REFERENCE_H__

