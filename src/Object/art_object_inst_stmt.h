/**
	\file "Object/art_object_inst_stmt.h"
	Instance statement classes for ART.  
	$Id: art_object_inst_stmt.h,v 1.16.6.1 2005/06/12 21:27:58 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INST_STMT_H__
#define	__OBJECT_ART_OBJECT_INST_STMT_H__

#include "Object/art_object_inst_stmt_base.h"
#include "Object/art_object_classification_fwd.h"
#include "util/memory/count_ptr.h"
#include "util/memory/list_vector_pool_fwd.h"

namespace ART {
namespace entity {
//=============================================================================
USING_LIST
USING_CONSTRUCT

//=============================================================================
#define	INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE			\
template <class Tag>

#define	INSTANTIATION_STATEMENT_CLASS					\
instantiation_statement<Tag>

/**
	Generic instantiation statement template class.  
 */
INSTANTIATION_STATEMENT_TEMPLATE_SIGNATURE
class instantiation_statement :
	public class_traits<Tag>::instantiation_statement_parent_type, 
	public class_traits<Tag>::instantiation_statement_type_ref_base {
private:
	typedef	INSTANTIATION_STATEMENT_CLASS		this_type;
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
private:
	never_ptr<collection_type>			inst_base;
private:
	instantiation_statement();
public:

	instantiation_statement(const type_ref_ptr_type& t, 
		const index_collection_item_ptr_type& i);

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

	void
	unroll(unroll_context& ) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

//	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS
//	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
};	// end class instantiation_statement

//=============================================================================
/**
	Abstract base class for built-in parameter type instantiations.  
 */
class param_instantiation_statement : public instantiation_statement_base {
private:
	typedef	instantiation_statement_base		parent_type;
protected:
	param_instantiation_statement() : parent_type() { }
public:
	explicit
	param_instantiation_statement(const index_collection_item_ptr_type& i);

virtual	~param_instantiation_statement() { }

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

};	// end class param_instantiation_statement

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INST_STMT_H__

