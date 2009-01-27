/**
	\file "sim/prsim/Rule.h"
	$Id: Rule.h,v 1.12.2.1 2009/01/27 00:18:55 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_RULE_H__
#define	__HAC_SIM_PRSIM_RULE_H__

#include <iosfwd>
#include "sim/time.h"
#include "util/attributes.h"
#include "sim/prsim/devel_switches.h"

/**
	Define to 1 to support bounded delay random timing simulation.
 */
#define	PRSIM_AFTER_RANGE		1

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;
using std::istream;

//-----------------------------------------------------------------------------
/**
	Structural information per production rule.  
	This is where rule-attributes are loaded during the ExprAlloc phase.  
 */
template <typename Time>
class Rule {
	typedef	enum {
		RULE_DEFAULT_FLAGS = 0x00,
#if PRSIM_RULE_DIRECTION
		RULE_DIR = 0x01,	///< whether rule pulls up or down
#endif
		RULE_WEAK = 0x02,
		RULE_ALWAYS_RANDOM = 0x04,
#if PRSIM_INVARIANT_RULES
		RULE_INVARIANT = 0x08,
#endif
		RULE_UNSTAB = 0x10,
		RULE_BIT_MASK = 0xFFFF	/// unsigned short flags
	}	rule_enum_type;
public:
	typedef	Time			time_type;
	/**
		Mutable value for default delay. 
		To take effect, this value must be changed before 
		state allocation.  
	 */
	static	time_type		default_unspecified_delay;
#if PRSIM_WEAK_RULES
	/**
		Since each rule is maintained separately, 
		there is no need to different delays in this structure
		for weak vs. strong rules.
		By design, every rule/direction gets its own delay.
	 */
#endif
	time_type			after;
#if PRSIM_AFTER_RANGE
	time_type			after_min;
	time_type			after_max;
#endif
	/**
		Uses rule_enum_type to signal flags.  
		TODO: move direction bit here from Expr,
			add strength vector/bitfield here
	 */
	short				rule_flags;
public:
	Rule() : after(default_unspecified_delay), 
#if PRSIM_AFTER_RANGE
		after_min(delay_policy<time_type>::zero), 
		after_max(delay_policy<time_type>::zero), 
#endif
		rule_flags(RULE_DEFAULT_FLAGS) { }

#if PRSIM_RULE_DIRECTION
	void
	set_direction(const bool d) {
		if (d)	rule_flags |= RULE_DIR;
		else	rule_flags &= ~RULE_DIR;
	}

	bool
	direction(void) const { return this->rule_flags & RULE_DIR; }
#endif

        bool
        is_always_random(void) const { return this->rule_flags & RULE_ALWAYS_RANDOM; }

        void
        set_always_random(void) { this->rule_flags |= RULE_ALWAYS_RANDOM; }

        void
        clear_always_random(void) { this->rule_flags &= ~RULE_ALWAYS_RANDOM; }

	bool
	is_weak(void) const { return this->rule_flags & RULE_WEAK; }

	void
	set_weak(void) { this->rule_flags |= RULE_WEAK; }

#if PRSIM_INVARIANT_RULES
	bool
	is_invariant(void) const { return this->rule_flags & RULE_INVARIANT; }

	void
	set_invariant(void) { this->rule_flags |= RULE_INVARIANT; }
#else
	bool
	is_invariant(void) const { return false; }
#endif

	void
	clear_weak(void) { this->rule_flags &= ~RULE_WEAK; }

	bool
	is_unstable(void) const { return this->rule_flags & RULE_UNSTAB; }

	void
	set_unstable(void) { this->rule_flags |= RULE_UNSTAB; }

	void
	clear_unstable(void) { this->rule_flags &= ~RULE_UNSTAB; }

	void
	set_delay(const time_type t) { this->after = t; }

	ostream&
	dump(ostream&) const;

};	// end struct Rule

//-----------------------------------------------------------------------------
/**
	Per-rule stateful information.  
	Nothing new yet.  
	TODO: one bit coverage flag (ever fired)
	Note: decided not to track critical trace index here because
		of memory cost per rule_state.
 */
template <typename Time>
struct RuleState
#if !PRSIM_INDIRECT_EXPRESSION_MAP
	: public Rule<Time>
#else
	// don't really need Time template parameter!
#endif
{

	void
	save_state(ostream&) const { }

	void
	load_state(istream&) { }

	static
	ostream&
	dump_checkpoint_state(ostream& o, istream&) { return o; }

}
#if PRSIM_INDIRECT_EXPRESSION_MAP
__ATTRIBUTE_PACKED__
#else
__ATTRIBUTE_ALIGNED__
#endif
;	// end struct RuleState

//-----------------------------------------------------------------------------
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_RULE_H__

