/**
	\file "Object/unroll/meta_loop_base.h"
	$Id: meta_loop_base.h,v 1.4.50.2 2006/09/07 21:34:30 fang Exp $
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
#if USE_INSTANCE_PLACEHOLDERS
template <class> class value_placeholder;
class footprint;
// template <class> class instantiation_statement;
#endif
template <class, size_t> class value_array;
using std::ostream;
using std::istream;
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
#if USE_INSTANCE_PLACEHOLDERS
	typedef	value_placeholder<pint_tag>		pint_placeholder_type;
	typedef	count_ptr<pint_placeholder_type>	ind_var_ptr_type;
protected:
	typedef	value_array<pint_tag, 0>		pint_scalar;
#else
	typedef	value_array<pint_tag, 0>		pint_scalar;
	typedef	count_ptr<pint_scalar>			ind_var_ptr_type;
#endif
public:
	typedef	count_ptr<const meta_range_expr>	range_ptr_type;

protected:
	ind_var_ptr_type			ind_var;
	range_ptr_type				range;

	meta_loop_base();
	meta_loop_base(const ind_var_ptr_type&, const range_ptr_type&);
	~meta_loop_base();

#if USE_INSTANCE_PLACEHOLDERS
	// instantiates an actual variable into the footprint
	count_ptr<pint_scalar>
	initialize_footprint(footprint&) const;
#endif

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

