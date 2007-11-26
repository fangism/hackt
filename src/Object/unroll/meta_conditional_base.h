/**
	\file "Object/unroll/meta_conditional_base.h"
	$Id: meta_conditional_base.h,v 1.6 2007/11/26 08:27:46 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_CONDITIONAL_BASE_H__
#define	__HAC_OBJECT_UNROLL_META_CONDITIONAL_BASE_H__

#include <iosfwd>
#include <vector>
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
	typedef	count_ptr<const pbool_expr>		guard_ptr_type;
	typedef	std::vector<guard_ptr_type>		guard_list_type;
	typedef	guard_list_type::const_iterator		const_iterator;
protected:
	guard_list_type				guards;

	meta_conditional_base();

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

