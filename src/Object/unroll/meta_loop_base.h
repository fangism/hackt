/**
	\file "Object/unroll/meta_loop_base.h"
	$Id: meta_loop_base.h,v 1.6.4.1 2006/11/01 07:52:45 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_LOOP_BASE_H__
#define	__HAC_OBJECT_UNROLL_META_LOOP_BASE_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/memory/count_ptr.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {
class meta_range_expr;
struct pint_tag;
template <class> class value_placeholder;
class footprint;
template <class, size_t> class value_array;
using std::ostream;
using std::istream;
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
using util::memory::never_ptr;
#endif
using util::memory::count_ptr;
using util::persistent_object_manager;

//=============================================================================
/**
	Base structure for meta-language loop construct.  
	May need to derive from scopespace after introducing placeholders
		to distinguish from collections and make footprint lookup 
		consistent in the unroll_context.  
 */
class meta_loop_base {
public:
	typedef	value_placeholder<pint_tag>		pint_placeholder_type;
	typedef	count_ptr<pint_placeholder_type>	ind_var_ptr_type;
protected:
	typedef	value_array<pint_tag, 0>		pint_scalar;
public:
	typedef	count_ptr<const meta_range_expr>	range_ptr_type;

protected:
	ind_var_ptr_type			ind_var;
	range_ptr_type				range;

	meta_loop_base();
	meta_loop_base(const ind_var_ptr_type&, const range_ptr_type&);
	~meta_loop_base();

	// instantiates an actual variable into the footprint
#if POOL_ALLOCATE_ALL_COLLECTIONS_PER_FOOTPRINT
	never_ptr<pint_scalar>
#else
	count_ptr<pint_scalar>
#endif
	initialize_footprint(footprint&) const;

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

#endif	// __HAC_OBJECT_UNROLL_META_LOOP_BASE_H__

