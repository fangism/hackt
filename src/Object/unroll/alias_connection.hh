/**
	\file "Object/unroll/alias_connection.hh"
	Declarations for classes related to connection of physical entities. 
	This file was reincarnated from "Object/art_object_connect.h".
	$Id: alias_connection.hh,v 1.9 2006/10/24 07:27:35 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_ALIAS_CONNECTION_H__
#define	__HAC_OBJECT_UNROLL_ALIAS_CONNECTION_H__

#include <vector>
#include "Object/unroll/aliases_connection_base.hh"
#include "Object/expr/types.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "util/memory/count_ptr.hh"
#include "util/multikey_fwd.hh"

namespace HAC {
namespace entity {
class simple_meta_indexed_reference_base;
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
	typedef	class_traits<Tag>		traits_type;
	/// the base alias connection type, such as aliases_connection_base
	typedef	typename traits_type::alias_connection_parent_type
						parent_type;
	/// the instance reference type used by this connection
	typedef	typename traits_type::simple_meta_instance_reference_type
					simple_meta_instance_reference_type;
	typedef	typename traits_type::meta_instance_reference_parent_type
					meta_instance_reference_type;
	/// the instance collection type referenced
	typedef	typename traits_type::instance_collection_generic_type
					instance_collection_generic_type;
	/// the instance alias type resolved by unrolling
	typedef	typename traits_type::instance_alias_info_type
						instance_alias_info_type;

	typedef	typename parent_type::generic_inst_ptr_type
						generic_inst_ptr_type;
	typedef	count_ptr<const meta_instance_reference_type>
						inst_ref_ptr_type;
	typedef	vector<inst_ref_ptr_type>	inst_list_type;
	typedef	typename inst_list_type::iterator
						iterator;
	typedef	typename inst_list_type::const_iterator
						const_iterator;
	/// the type of collection for unrolled aliases
	typedef	typename traits_type::alias_collection_type
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
	dump(ostream&, const expr_dump_context&) const;

	void
	reserve(const size_t);

	void
	append_meta_instance_reference(const generic_inst_ptr_type& );

	good_bool
	unroll(const unroll_context&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class alias_connection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_ALIAS_CONNECTION_H__

