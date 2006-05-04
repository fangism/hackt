/**
	\file "sim/prsim/ExprAllocFlags.h"
	$Id: ExprAllocFlags.h,v 1.1.2.2 2006/05/04 23:16:47 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_EXPRALLOCFLAGS_H__
#define	__HAC_SIM_PRSIM_EXPRALLOCFLAGS_H__

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
/**
	Enumerated structure for tracking expression allocation options.  
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

	ExprAllocFlags() : flags(DEFAULT_FLAGS) { }

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

};	// end struct ExprAllocFlags

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_EXPRALLOCFLAGS_H__

