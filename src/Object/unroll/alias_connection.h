/**
	\file "Object/unroll/alias_connection.h"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: alias_connection.h,v 1.3 2005/09/04 21:15:00 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_ALIAS_CONNECTION_H__
#define	__OBJECT_UNROLL_ALIAS_CONNECTION_H__

#include <vector>
#include "Object/unroll/aliases_connection_base.h"
#include "Object/expr/types.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/multikey_fwd.h"

namespace ART {
namespace entity {
class simple_meta_instance_reference_base;
using std::vector;
using util::persistent_object_manager;

//=============================================================================
#define	ALIAS_CONNECTION_TEMPLATE_SIGNATURE				\
template <class Tag>

#define	ALIAS_CONNECTION_CLASS						\
alias_connection<Tag>

/**
	Re-usable pattern for type-specific alias connection lists, 
	intended for leaf classes because methods are non-virtual.
 */
ALIAS_CONNECTION_TEMPLATE_SIGNATURE
class alias_connection :
	public class_traits<Tag>::alias_connection_parent_type {
	typedef	ALIAS_CONNECTION_CLASS		this_type;
public:
	/// the base alias connection type, such as aliases_connection_base
	typedef	typename class_traits<Tag>::alias_connection_parent_type
						parent_type;
	/// the instance reference type used by this connection
	typedef	typename class_traits<Tag>::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	/// the instance collection type referenced
	typedef	typename class_traits<Tag>::instance_collection_generic_type
					instance_collection_generic_type;
	/// the instance alias type resolved by unrolling
	typedef	typename class_traits<Tag>::instance_alias_base_type
						instance_alias_base_type;

	typedef	typename parent_type::generic_inst_ptr_type
						generic_inst_ptr_type;
	typedef	count_ptr<const simple_meta_instance_reference_type>
						inst_ref_ptr_type;
	typedef	vector<inst_ref_ptr_type>	inst_list_type;
	typedef	typename inst_list_type::iterator
						iterator;
	typedef	typename inst_list_type::const_iterator
						const_iterator;
	/// the type of collection for unrolled aliases
	typedef	typename simple_meta_instance_reference_type::alias_collection_type
						alias_collection_type;
private:
	typedef	util::multikey_generator_generic<pint_value_type>
						key_generator_type;
private:
	inst_list_type				inst_list;
public:
	alias_connection();

	~alias_connection();

	ostream&
	what(ostream& ) const;

	ostream&
	dump(ostream& ) const;

	void
	reserve(const size_t);

	void
	append_meta_instance_reference(const generic_inst_ptr_type& );

	good_bool
	unroll(const unroll_context&) const;

	UNROLL_META_CONNECT_PROTO;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class alias_connection

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_UNROLL_ALIAS_CONNECTION_H__

