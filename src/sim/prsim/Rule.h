/**
	\file "sim/prsim/Rule.h"
	$Id: Rule.h,v 1.6.2.4 2008/11/03 07:58:29 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_RULE_H__
#define	__HAC_SIM_PRSIM_RULE_H__

#include <iosfwd>
#include "sim/time.h"
#include "util/attributes.h"
#include "sim/prsim/devel_switches.h"

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
struct Rule {
	typedef	Time			time_type;
	typedef	enum {
		RULE_DEFAULT_FLAGS = 0x00,
		RULE_UNSTAB = 0x01,
		RULE_WEAK = 0x02,
		RULE_ALWAYS_RANDOM = 0x04
	}	rule_enum_type;
public:
#if PRSIM_WEAK_RULES
	/**
		Since each rule is maintained separately, 
		there is no need to different delays in this structure
		for weak vs. strong rules.
		By design, every rule/direction gets its own delay.
	 */
#endif
	time_type			after;
	/**
		Uses rule_enum_type to signal flags.  
		TODO: move direction bit here from Expr,
			add strength vector/bitfield here
	 */
	short				rule_flags;
public:
	Rule() : after(delay_policy<time_type>::default_delay), 
		rule_flags(RULE_DEFAULT_FLAGS) { }

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

