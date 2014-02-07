/**
	\file "Object/lang/PRS_dump_context.hh"
	Structures for production rules.
	$Id: PRS_base.hh,v 1.12 2010/07/09 02:14:13 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_PRS_DUMP_CONTEXT_HH__
#define	__HAC_OBJECT_LANG_PRS_DUMP_CONTEXT_HH__

#include "util/memory/excl_ptr.hh"

namespace HAC {
namespace entity {
class scopespace;

/**
	Namespace for PRS objects.  
	There are classes that are stored in process definitions, 
		but not the final result of unroll-creation.  
 */
namespace PRS {
using util::memory::never_ptr;
//=============================================================================
/**
	Dump modifier for PRS rules.  
 */
struct rule_dump_context {
	never_ptr<const scopespace>		parent_scope;

	rule_dump_context() : parent_scope(NULL) { }

	explicit
	rule_dump_context(const scopespace& s) : parent_scope(&s) { }

	explicit
	rule_dump_context(const never_ptr<const scopespace> s) :
		parent_scope(s) { }

	// default copy-constructor

};	// end struct rule_dump_context

//-----------------------------------------------------------------------------
/**
	Helper class for controlling PRS and expression dumps.  
 */
struct expr_dump_context : public rule_dump_context {
	int					expr_stamp;

	expr_dump_context() : rule_dump_context(), expr_stamp(0) { }

	explicit
	expr_dump_context(const never_ptr<const scopespace> s) :
		rule_dump_context(s), expr_stamp(0) { }

	// implicit OK
	expr_dump_context(const rule_dump_context& r) :
		rule_dump_context(r), expr_stamp(0) { }

	// default copy constructor

};	// end struct expr_dump_context

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_PRS_DUMP_CONTEXT_HH__

