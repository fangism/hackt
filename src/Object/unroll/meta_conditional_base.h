/**
	\file "Object/unroll/meta_conditional_base.h"
	$Id: meta_conditional_base.h,v 1.2.10.2 2005/12/13 02:58:12 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_META_CONDITIONAL_BASE_H__
#define	__OBJECT_UNROLL_META_CONDITIONAL_BASE_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/memory/count_ptr.h"

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
	typedef	count_ptr<pbool_expr>		guard_ptr_type;

protected:
	guard_ptr_type				guard;

	meta_conditional_base();

	explicit
	meta_conditional_base(const guard_ptr_type&);

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

#endif	// __OBJECT_UNROLL_META_CONDITIONAL_BASE_H__

