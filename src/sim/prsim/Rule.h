/**
	\file "sim/prsim/Rule.h"
	$Id: Rule.h,v 1.2 2006/04/23 07:37:26 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_RULE_H__
#define	__HAC_SIM_PRSIM_RULE_H__

#include <iosfwd>
#include "sim/time.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::ostream;

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
		RULE_WEAK = 0x02
	}	rule_enum_type;
public:
	time_type			after;
	/**
		Uses rule_enum_type to signal flags.  
	 */
	short				rule_flags;
	/// unused space for now.  
	short				__padding__;
public:
	Rule() : after(delay_policy<time_type>::default_delay), 
		rule_flags(RULE_DEFAULT_FLAGS) { }

	bool
	is_weak(void) const { return this->rule_flags & RULE_WEAK; }

	void
	set_weak(void) { this->rule_flags |= RULE_WEAK; }

	void
	clear_weak(void) { this->rule_flags &= ~RULE_WEAK; }

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
struct RuleState : public Rule<Time> {
};	// end struct RuleState

//-----------------------------------------------------------------------------
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_RULE_H__

