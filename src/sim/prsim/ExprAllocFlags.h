/**
	\file "sim/prsim/ExprAllocFlags.h"
	$Id: ExprAllocFlags.h,v 1.3 2010/08/30 23:51:50 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPRALLOCFLAGS_H__
#define	__HAC_SIM_PRSIM_EXPRALLOCFLAGS_H__

#include <iosfwd>
#include "sim/prsim/devel_switches.h"		// for PRSIM_WEAK_RULES

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;

//=============================================================================
/**
	Enumerated structure for tracking expression allocation options.  
	Member functions defined in ExprAlloc.cc.
 */
struct ExprAllocFlags {
	enum {
		/// default options
		DEFAULT_FLAGS		= 0x00,
		/// whether or not to fold leaf nodes
		FOLD_LITERALS		= 0x01,
		/// whether or not to pull up negations (DeMorgan's)
		DENORMALIZE_NEGATIONS	= 0x02,
		/// crank up the optimizations
		OPTIMIZE_FLAGS		=
			FOLD_LITERALS | DENORMALIZE_NEGATIONS
	};

	unsigned char			flags;
#if PRSIM_WEAK_RULES
	/**
		Set to true to interpret iskeeper rules as 
		weak and fast (after=0).
		Otherwise, by default, they are simply dropped and ignored.  
	 */
	bool				fast_weak_keepers;
#endif
	/**
		Set to true to automatically infer 
		precharge invariant expressions.
		Default: false
	 */
	bool				auto_precharge_invariants;

	ExprAllocFlags() : flags(DEFAULT_FLAGS)
#if PRSIM_WEAK_RULES
		, fast_weak_keepers(false)
#endif
		, auto_precharge_invariants(false)
		{ }

	bool
	assert_equal(const ExprAllocFlags&) const;

	bool
	is_fold_literals(void) const { return flags & FOLD_LITERALS; }

	void
	fold_literals(void) { flags |= FOLD_LITERALS; }

	void
	no_fold_literals(void) { flags &= ~FOLD_LITERALS; }

	bool
	is_denormalize_negations(void) const {
		return flags & DENORMALIZE_NEGATIONS;
	}

	void
	denormalize_negations(void) { flags |= DENORMALIZE_NEGATIONS; }

	void
	no_denormalize_negations(void) { flags &= ~DENORMALIZE_NEGATIONS; }

	/**
		Only two levels: 0, 1.
	 */
	void
	optimize(const int level) {
		if (level)
			flags |= OPTIMIZE_FLAGS;
		else	flags &= ~OPTIMIZE_FLAGS;
	}

	bool
	any_optimize(void) const {
		return flags | OPTIMIZE_FLAGS;
	}

	void
	write_object(ostream&) const;

	void
	load_object(istream&);

};	// end struct ExprAllocFlags

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPRALLOCFLAGS_H__

