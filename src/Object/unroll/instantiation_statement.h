/**
	\file "Object/unroll/instantiation_statement.h"
	Instance statement classes for HAC.  
	This file used to be "Object/art_object_inst_stmt.h"
		in a previous life.  
	$Id: instantiation_statement.h,v 1.11.12.1 2006/04/26 19:50:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_H__
#define	__HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_H__

#include "Object/unroll/instantiation_statement_base.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/memory/list_vector_pool_fwd.h"

namespace HAC {
namespace entity {
//=============================================================================
using std::list;
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
	TODO: use template technique to enforce never-delete on this type.  
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
class instantiation_statement :
	public class_traits<Tag>::instantiation_statement_parent_type, 
	public class_traits<Tag>::instantiation_statement_type_ref_base {
public:
	typedef	class_traits<Tag>			traits_type;
protected:
#if 0
	typedef	INSTANTIATION_STATEMENT_CLASS		this_type;
#else
	/**
		This allows one to override the default with an
		appropriate child class, such is the case with
		pint and pbool, which use param_instantiation_statement.  
	 */
	typedef	typename traits_type::instantiation_statement_type
							this_type;
#endif
	typedef	typename traits_type::instantiation_statement_parent_type
							parent_type;
	/**
		The parent from which to inherit a type reference pointer,
		if applicable.
		If this class is empty, then it will be optimized out
		for space (EBCO).  
	 */
	typedef	typename
		traits_type::instantiation_statement_type_ref_base
							type_ref_parent_type;
public:
	typedef	typename traits_type::instance_collection_generic_type
							collection_type;
	typedef	never_ptr<collection_type>		collection_ptr_type;
	typedef	typename traits_type::type_ref_ptr_type
							type_ref_ptr_type;
	typedef	typename traits_type::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef	instantiation_statement_base::const_relaxed_args_type
							const_relaxed_args_type;
protected:
	never_ptr<collection_type>			inst_base;
protected:
	instantiation_statement();

	// for param_instantiation_statement and others that don't use
	// relaxed template parameters
public:
	instantiation_statement(const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i);

	instantiation_statement(const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i, 
		const const_relaxed_args_type&);

	~instantiation_statement();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

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
	unroll(const unroll_context&) const;

	// careful: not declared virtual here, 
	// yet overridden by param_instantiation_statement<>

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
}	// end namespace HAC

//=============================================================================
#if 0
namespace util {
namespace memory {
using HAC::entity::instantiation_statement;
using HAC::entity::pint_tag;
using HAC::entity::pbool_tag;

// excl_ptr already forward-declared

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
#endif

#endif	// __HAC_OBJECT_UNROLL_INSTANTIATION_STATEMENT_H__

