/**
	\file "Object/unroll/meta_conditional_base.h"
	$Id: meta_conditional_base.h,v 1.5.74.1 2007/11/25 02:28:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_CONDITIONAL_BASE_H__
#define	__HAC_OBJECT_UNROLL_META_CONDITIONAL_BASE_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/memory/count_ptr.h"

/**
	Define to 1 to support generalized conditional bodies
	in sequential else-if manner.  
	Evaluation should result in an index to the first guard
	that is true, else other signal for none.  
	Goal: 1
 */
#define	GENERALIZED_META_CONDITIONAL		1

#if GENERALIZED_META_CONDITIONAL
#include <vector>
#endif

namespace HAC {
namespace entity {
class pbool_expr;
using std::ostream;
using std::istream;
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Base structure for meta-language conditional construct.  
 */
class meta_conditional_base {
public:
	typedef	count_ptr<const pbool_expr>		guard_ptr_type;
#if GENERALIZED_META_CONDITIONAL
	typedef	std::vector<guard_ptr_type>		guard_list_type;
	typedef	guard_list_type::const_iterator		const_iterator;
	// shared constant to save memory, convenient for else clauses
	// static const guard_ptr_type			_true;
#endif

protected:
#if GENERALIZED_META_CONDITIONAL
	guard_list_type				guards;
#else
	guard_ptr_type				guard;
#endif

	meta_conditional_base();

#if !GENERALIZED_META_CONDITIONAL
	explicit
	meta_conditional_base(const guard_ptr_type&);
#endif

	~meta_conditional_base();

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
};

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_META_CONDITIONAL_BASE_H__

