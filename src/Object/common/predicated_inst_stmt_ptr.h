/**
	\file "Object/common/predicated_inst_stmt_ptr.h"
	$Id: predicated_inst_stmt_ptr.h,v 1.4 2006/01/22 18:19:24 fang Exp $
 */

#ifndef	__HAC_OBJECT_COMMON_PREDICATED_INST_STMT_PTR_H__
#define	__HAC_OBJECT_COMMON_PREDICATED_INST_STMT_PTR_H__

#include "util/memory/excl_ptr.h"

namespace HAC {
namespace entity {
class instantiation_statement_base;
using util::memory::never_ptr;
//=============================================================================

// consider templating this by instantation statement sub-type
class predicated_inst_stmt_ptr :
		public never_ptr<const instantiation_statement_base> {
public:
	typedef	never_ptr<const instantiation_statement_base>
					parent_ptr_type;
private:
	bool				_conditional;
public:
	predicated_inst_stmt_ptr() : parent_ptr_type(), _conditional(false) { }
	predicated_inst_stmt_ptr(const parent_ptr_type p, const bool c) :
		parent_ptr_type(p), _conditional(c) { }
	// default copy-constructor
	~predicated_inst_stmt_ptr() { }

	bool
	is_conditional(void) const { return _conditional; }

};	// end class predicated_inst_stmt_ptr

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_COMMON_PREDICATED_INST_STMT_PTR_H__

