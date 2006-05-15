/**
	\file "Object/lang/CHP_footprint.h"
	Class definitions for unrolled CHP action instances.  
	$Id: CHP_footprint.h,v 1.1.2.1 2006/05/15 03:59:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CHP_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_CHP_FOOTPRINT_H__

#include <iosfwd>
#include <vector>
#include "Object/lang/CHP_footprint_expr.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/expr/types.h"

namespace HAC {
namespace entity {
namespace CHP {
// typedef	size_t			channel_index_type;

//=============================================================================
#if 0
/**
	Footprint send action.  
	The channel is bound statically, meaning that it can only
		be indexed with a meta-parameter, not a non-meta value.  
	Sent value is either bound to an expression or 
	data instance reference. 
 */
class fp_send {
public:
	struct send_source_type {
	};
private:
	channel_index_type		channel_index;
public:
};
#endif

//=============================================================================
/**
	The CHP footprint is created by unrolling a complete type, 
	which results in CHP in expanded form, so all meta-language
	constructs are resolved and expanded.  
 */
class footprint {
public:
	typedef	std::vector<fp_expr_entry>	expr_pool_type;
	typedef	std::vector<int_value_type>	int_const_pool_type;
	typedef	std::vector<fp_Nary_expr_node>	Nary_expr_pool_type;
private:
	expr_pool_type				expr_pool;
	int_const_pool_type			int_const_pool;
	Nary_expr_pool_type			Nary_expr_pool;
	std::vector<fp_subscript_ref<bool_tag> >	bool_ref_pool;
	std::vector<fp_subscript_ref<int_tag> >		int_ref_pool;
	std::vector<fp_subscript_ref<channel_tag> >	chan_ref_pool;
	std::vector<fp_subscript_ref<process_tag> >	proc_ref_pool;
	std::vector<fp_subscript_ref<pbool_tag> >	pbool_ref_pool;
	std::vector<fp_subscript_ref<pint_tag> >	pint_ref_pool;
public:

};	// end class footprint

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CHP_FOOTPRINT_H__

