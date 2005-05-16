/**
	\file "Object/art_object_PRS_base.h"
	Structures for production rules.
	$Id: art_object_PRS_base.h,v 1.1.2.1 2005/05/16 03:52:20 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_PRS_BASE_H__
#define	__OBJECT_ART_OBJECT_PRS_BASE_H__

#include <list>
#include "Object/art_object_fwd.h"
#include "util/memory/excl_ptr.h"
#include "util/memory/count_ptr.h"
#include "util/persistent.h"

namespace ART {
namespace entity {
/**
	Namespace for PRS objects.  
	There are classes that are stored in process definitions, 
		but not the final result of unroll-creation.  
 */
namespace PRS {
using std::list;
using util::memory::excl_ptr;
using util::memory::count_ptr;
using util::persistent;
//=============================================================================

class rule;
class prs_expr;
typedef	sticky_ptr<prs_expr>			guard_type;
typedef	excl_ptr<prs_expr>			guard_arg_type;

//=============================================================================
/**
	A collection or production rules.  
 */
class rule_set : public list<excl_ptr<rule> > {
	typedef	rule_set			this_type;
protected:
	typedef	list<excl_ptr<rule> >		parent_type;
public:
	rule_set();
	~rule_set();

};	// end class rule_set

//=============================================================================
/**
	Abstract base class for a production rule.  
 */
class rule : public persistent {
public:
	rule() { }
virtual	~rule() { }

// virtual	ostream& dump(ostream&) const;
};	// end class rule

//=============================================================================
/**
	Abstract class of production rule expressions.  
	These expressions are not unrolled.  
 */
class prs_expr : public persistent {
public:
	/**
		Worry about implementation efficiency later...
		(Vector of raw pointers or excl_ptr with copy-constructor.)
	 */
	typedef	list<guard_type>	expr_sequence_type;
public:
	prs_expr() { }
virtual	~prs_expr() { }

};	// end class prs_expr

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_PRS_BASE_H__

