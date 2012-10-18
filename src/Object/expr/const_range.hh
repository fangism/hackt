/**
	\file "Object/expr/const_range.hh"
	Classes related to constant expressions.
	NOTE: this file was spanwed from "Object/art_object_expr_const.h"
		for revision history tracking purposes.  
	$Id: const_range.hh,v 1.18 2011/02/08 22:32:47 fang Exp $
 */

#ifndef __HAC_OBJECT_EXPR_CONST_RANGE_H__
#define __HAC_OBJECT_EXPR_CONST_RANGE_H__

#include <utility>
#include "util/macros.h"
#include "util/memory/list_vector_pool_fwd.hh"
#include "Object/expr/meta_range_expr.hh"
#include "Object/expr/const_index.hh"

//=============================================================================
// switches

/**
	Since the issue of negative ranges has not yet been resolved, 
	we leave this as a compile time switch.  
	(Added 20051021)
 */
#define	ALLOW_NEGATIVE_RANGES			1

//=============================================================================
namespace HAC {
namespace entity {
class pint_const;
USING_CONSTRUCT
using std::pair;
// using util::persistent_object_manager;	// forward declared

//=============================================================================
/**
	Constant version of range expression.  
	Deriving from pair to inherit its interface with first and second.  
 */
class const_range : public meta_range_expr, public const_index,
		public pair<pint_value_type, pint_value_type> {
friend class const_range_list;
private:
	typedef	const_range				this_type;
	typedef	pair<pint_value_type,pint_value_type>	parent_type;
	// typedef for interval_type (needs discrete_interval_set)
	// relocated to source file
public:
	// dispense with pint_const objects here
	/**
		Default constructor initializes with invalid range.  
	 */
	const_range() :
		meta_range_expr(), const_index(), parent_type(0, -1) { }

	/** explicit conversion from x[N] to x[0..N-1] */
	explicit
	const_range(const pint_value_type n);

	explicit
	const_range(const pint_const& n);

	explicit
	const_range(const parent_type& p);

	const_range(const pint_value_type l, const pint_value_type u);

#if 0
	const_range(const const_range& r);
#endif

private:
	const_range(const pint_value_type l, const pint_value_type u,
		const bool);
public:
	~const_range() { }

	/** use this to query whether or not range is valid */
	bool
	empty(void) const { return first > second; }

	bool
	negative(void) const;

	pint_value_type
	lower(void) const {
#if !ALLOW_NEGATIVE_RANGES
		INVARIANT(!empty());
#endif
		return first;
	}

	pint_value_type
	upper(void) const {
#if !ALLOW_NEGATIVE_RANGES
		INVARIANT(!empty());
#endif
		return second;
	}

	/**
		\return The size spanned by this range.
		The size includes the bounds of the range, 
		so size(N..N) == 1.
	 */
	pint_value_type
	size(void) const {
#if !ALLOW_NEGATIVE_RANGES
		INVARIANT(!empty());
#endif
		return second -first +1;
	}

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o, const expr_dump_context&) const;

	ostream&
	dump_force(ostream& o) const;

	const_range
	static_overlap(const const_range& r) const;

	bool
	operator == (const const_range& c) const;

	bool
	is_sane(void) const;

	bool
	is_static_constant(void) const { return !empty(); }

	bool
	is_relaxed_formal_dependent(void) const {
#if !ALLOW_NEGATIVE_RANGES
		INVARIANT(!empty());
#endif
		return false;
	}

	bool
	range_size_equivalent(const const_index& i) const;

	pint_value_type
	lower_bound(void) const;

	pint_value_type
	upper_bound(void) const;

	good_bool
	unroll_resolve_range(const unroll_context&, const_range& r) const;

	count_ptr<const_index>
	unroll_resolve_index(const unroll_context&) const;

	UNROLL_RESOLVE_COPY_META_INDEX_PROTO;
	NONMETA_RESOLVE_COPY_INDEX_PROTO;
	SUBSTITUTE_DEFAULT_PARAMETERS_META_INDEX_PROTO;

	void
	accept(nonmeta_expr_visitor&) const;

protected:
	using meta_index_expr::unroll_resolve_copy;
#if OVERLOAD_VIRTUAL_USING
	using nonmeta_index_expr_base::unroll_resolve_copy;
	using const_index::unroll_resolve_copy;
#endif
public:
	bool
	must_be_formal_size_equivalent(const meta_range_expr& ) const;

	static
	ostream&
	diagnose_bad_range(ostream&, const const_range&);

public:
	PERSISTENT_METHODS_DECLARATIONS
//	LIST_VECTOR_POOL_ESSENTIAL_FRIENDS	// not needed, ctors are public
	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
	// don't need robust declarations, unless dynamically allocating
	// during global static initialization.
};	// end class const_range

//=============================================================================
}	// end namespace HAC
}	// end namespace entity

#endif	// __HAC_OBJECT_EXPR_CONST_RANGE_H__

