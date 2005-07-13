/**
	\file "Object/art_object_inst_stmt.h"
	Instance statement classes for ART.  
	$Id: art_object_inst_stmt.h,v 1.17.4.4.2.2 2005/07/13 21:56:37 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_H__

#include "Object/art_object_inst_stmt_base.h"
#include "Object/traits/class_traits.h"
#include "util/memory/count_ptr.h"
#include "util/memory/list_vector_pool_fwd.h"

namespace ART {
namespace entity {
//=============================================================================
USING_LIST
USING_CONSTRUCT
struct pint_tag;
struct pbool_tag;

//=============================================================================
#define	INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	INSTANTIATION_STATEMENT_CLASS					\
instantiation_statement<Tag>

/**
	Generic instantiation statement template class.  
	Even though param_instantiation_statement<Tag> derived from this
	class, we NEVER use a pointer to this class directly, 
	so we don't need to make any methods or the destructor virtual.  
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
class instantiation_statement :
	public class_traits<Tag>::instantiation_statement_parent_type, 
	public class_traits<Tag>::instantiation_statement_type_ref_base {
protected:
#if 0
	typedef	INSTANTIATION_STATEMENT_CLASS		this_type;
#else
	/**
		This allows one to override the default with an
		appropriate child class, such is the case with
		pint and pbool, which use param_instantiation_statement.  
	 */
	typedef	typename class_traits<Tag>::instantiation_statement_type
							this_type;
#endif
	typedef	typename
		class_traits<Tag>::instantiation_statement_parent_type
							parent_type;
	/**
		The parent from which to inherit a type reference pointer,
		if applicable.
		If this class is empty, then it will be optimized out
		for space (EBCO).  
	 */
	typedef	typename
		class_traits<Tag>::instantiation_statement_type_ref_base
							type_ref_parent_type;
public:
	typedef	typename class_traits<Tag>::instance_collection_generic_type
							collection_type;
	typedef	never_ptr<collection_type>		collection_ptr_type;
	typedef	typename class_traits<Tag>::type_ref_ptr_type
							type_ref_ptr_type;
#if 0
	typedef	instantiation_statement_base::relaxed_args_type
							relaxed_args_type;
#endif
	typedef	instantiation_statement_base::const_relaxed_args_type
							const_relaxed_args_type;
protected:
	never_ptr<collection_type>			inst_base;
protected:
	instantiation_statement();

	// for param_instantiation_statement and others that don't use
	// relaxed template parameters
	instantiation_statement(const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i);

public:
	instantiation_statement(const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i, 
		const const_relaxed_args_type&);

	~instantiation_statement();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	attach_collection(const never_ptr<instance_collection_base> i);

	never_ptr<instance_collection_base>
	get_inst_base(void);

	never_ptr<const instance_collection_base>
	get_inst_base(void) const;

	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;

	const_relaxed_args_type
	get_relaxed_actuals(void) const;

	good_bool
	unroll(unroll_context& ) const;

	// careful: not declared virtual here, 
	// yet overridden by param_instantiation_statement<>
	UNROLL_META_INSTANTIATE_PROTO;

	INSTANTIATE_PORT_PROTO;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

protected:
	// for childrens' use
	void
	collect_transient_info_base(persistent_object_manager&) const;

//	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
//	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
};	// end class instantiation_statement

//=============================================================================
}	// end namespace entity
}	// end namespace ART

//=============================================================================
namespace util {
namespace memory {
using ART::entity::instantiation_statement;
using ART::entity::pint_tag;
using ART::entity::pbool_tag;

template <class>
class excl_ptr;

/**
	Declaring, but not defining a specialization to guarantee
	that one does not inadvertently use this type virtually.  
 */
template <>
class count_ptr<instantiation_statement<pint_tag> >;

template <>
class count_ptr<instantiation_statement<pbool_tag> >;

template <>
class excl_ptr<instantiation_statement<pint_tag> >;

template <>
class excl_ptr<instantiation_statement<pbool_tag> >;

}	// end namespace memory
}	// end namespace util

#endif	// __OBJECT_ART_OBJECT_INST_STMT_H__

