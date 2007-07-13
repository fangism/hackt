/**
	\file "Object/unroll/template_type_completion.h"
	For relaxed template instantiations, attach the appropriate
	relaxed template formals.  
	Also recursively instantiate ports upon completion.  
	$Id: template_type_completion.h,v 1.1.2.2 2007/07/13 22:56:38 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_H__
#define	__HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_H__

#include <iosfwd>
#include "Object/unroll/instance_management_base.h"
#include "Object/ref/references_fwd.h"
#include "Object/traits/class_traits_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class dynamic_param_expr_list;	// from "Object/expr/dynamic_param_expr_list.h"
class const_param_expr_list;
using util::memory::count_ptr;
using util::persistent_object_manager;
using std::istream;

//=============================================================================
/**
	Statement that attaches relaxed template parameters to 
	an existing instance reference.  
	Only applicable to meta-types that support relaxed parameters.  
 */
template <class Tag>
class template_type_completion : public instance_management_base {
	typedef	template_type_completion<Tag>		this_type;
public:
	typedef	class_traits<Tag>			traits_type;
	/**
		Should aggregate references be allowed? member references?
		Member references are useful for completing the types of
		relax-typed public ports... so allow member refs.  
		Aggregates should be permissible too because aggregate
		references satisfy collectible type-equivalence.  
	 */
	typedef	meta_instance_reference<Tag>	reference_type;
//	typedef	simple_meta_instance_reference<Tag>	reference_type;
	typedef	count_ptr<const reference_type>	reference_ptr_type;
	typedef	count_ptr<const dynamic_param_expr_list>
						const_relaxed_args_type;
	typedef	count_ptr<const const_param_expr_list>
						instance_relaxed_actuals_type;
private:
	/**
		The instance(s) to which relaxed parameters should be 
		attached.  
	 */
	reference_ptr_type				ref;
	/**
		The relaxed template parameters to associate with 
		each referenced instance.  
	 */
	const_relaxed_args_type				relaxed_args;
public:
	template_type_completion();
	template_type_completion(const reference_ptr_type&, 
		const const_relaxed_args_type&);
	// default copy-ctor
	~template_type_completion();

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	good_bool
	unroll(const unroll_context&) const;

	void
	collect_transient_info(persistent_object_manager&) const;

	void
	write_object(const persistent_object_manager&, ostream&) const;

	void
	load_object(const persistent_object_manager&, istream&);

};	// end class template_type_completion

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_TEMPLATE_TYPE_COMPLETION_H__

