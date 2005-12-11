/**
	\file "Object/unroll/param_instantiation_statement_base.h"
	Parameter instance statement classes for HAC.  
	This file came from "Object/art_object_inst_stmt_param_base.h"
		in a previous life.  
	$Id: param_instantiation_statement_base.h,v 1.2.32.1 2005/12/11 00:45:58 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_BASE_H__
#define	__OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_BASE_H__

#include "Object/unroll/instantiation_statement_base.h"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Abstract base class for built-in parameter type instantiations.  
 */
class param_instantiation_statement_base : public instantiation_statement_base {
private:
	typedef	instantiation_statement_base		parent_type;
protected:
	param_instantiation_statement_base() : parent_type() { }

	explicit
	param_instantiation_statement_base(
		const index_collection_item_ptr_type& i);

public:
virtual	~param_instantiation_statement_base() { }

#if 1
virtual	UNROLL_META_EVALUATE_PROTO = 0;
#endif

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;

};	// end class param_instantiation_statement_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_UNROLL_PARAM_INSTANTIATION_STATEMENT_BASE_H__

