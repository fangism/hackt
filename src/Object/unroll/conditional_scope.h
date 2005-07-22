/**
	\file "Object/unroll/conditional_scope.h"
	This file contains class definitions for control-flow scopes
	of the ART++ language.  
	$Id: conditional_scope.h,v 1.1.2.1 2005/07/22 21:34:27 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_CONDITIONAL_SCOPE_H__
#define	__OBJECT_UNROLL_CONDITIONAL_SCOPE_H__

#include "Object/unroll/instance_management_base.h"
#include "Object/unroll/sequential_scope.h"
#include "util/string_fwd.h"
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
class sequential_scope;
class scopespace;
using std::string;
using util::memory::never_ptr;

//=============================================================================
/**
	Scope of a conditional body.
	Should this be some list?    
 */
class conditional_scope : public instance_management_base, 
		public sequential_scope {
protected:
	// condition expression
	never_ptr<const sequential_scope>		parent;
public:
	conditional_scope(const string& n, const never_ptr<const scopespace>);
		// more args...     
	~conditional_scope();

	// unroll
};      // end class conditional_scope

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	//	__OBJECT_UNROLL_CONDITIONAL_SCOPE_H__

