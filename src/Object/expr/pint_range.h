/**
	\file "Object/expr/pint_range.h"
	Boolean relations between integer parameters.  
	NOTE: this file was spawned from the old
		"Object/art_object_expr.h" for revision history tracking.  
	$Id: pint_range.h,v 1.8.8.1.2.1 2006/09/11 02:39:11 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_PINT_RANGE_H__
#define __HAC_OBJECT_EXPR_PINT_RANGE_H__

#include "Object/expr/meta_range_expr.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
class pint_expr;
using std::ostream;
using util::memory::count_ptr;

//=============================================================================
/**
	Range expression object, not necessarily constant.  
	Must contain pint_expr's.
	Derive from object or param_expr?
 */
class pint_range : public meta_range_expr {
	typedef	meta_range_expr				parent_type;
	typedef	pint_range				this_type;
protected:
	// need to be const, or modifiable?
	count_ptr<const pint_expr>	lower;
	count_ptr<const pint_expr>	upper;
private:
	pint_range();
public:
	/** implicit conversion from x[N] to x[0..N-1] */
	explicit
	pint_range(const count_ptr<const pint_expr>& n);

	pint_range(const count_ptr<const pint_expr>& l,
		const count_ptr<const pint_expr>& u);

#if 0
	pint_range(const pint_range& pr);
#endif

	~pint_range();

	// first, second? pair interface?

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

#if ENABLE_STATIC_ANALYSIS
	bool
	may_be_initialized(void) const;

	bool
	must_be_initialized(void) const;
#endif

	bool
	is_sane(void) const;

	bool
	is_static_constant(void) const;

	bool
	is_relaxed_formal_dependent(void) const;

	const_range
	static_constant_range(void) const;

#if !USE_INSTANCE_PLACEHOLDERS
	good_bool
	resolve_range(const_range& r) const;
#endif

	good_bool
	unroll_resolve_range(const unroll_context&, const_range& r) const;

	bool
	must_be_formal_size_equivalent(const meta_range_expr& ) const;

	UNROLL_RESOLVE_COPY_META_INDEX_PROTO;
protected:
	using parent_type::unroll_resolve_copy;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class pint_range

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_PINT_RANGE_H__

