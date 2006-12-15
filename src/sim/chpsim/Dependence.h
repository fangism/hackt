/**
	\file "sim/chpsim/Dependence.h"
	$Id: Dependence.h,v 1.1.2.4 2006/12/15 00:49:43 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_DEPENDENCE_H__
#define	__HAC_SIM_CHPSIM_DEPENDENCE_H__

#include <iosfwd>
#include <valarray>
#include "sim/common.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using std::ostream;
class DependenceSetCollector;	// from "sim/chpsim/DependenceCollector.h"
/**
	To keep this structure as small as possible, we use
	a valarray, but to construct the sets, we will use
	temporary vectors.  
	Invariant: make this set sorted for efficient set union/difference
	algorithms.  
 */
typedef	std::valarray<node_index_type>	instance_set_type;

//=============================================================================
/**
	Each event evaluates a satic set of variables/channels that 
	can cause it to unblock, the wake-up set.  
	The set may be conservative and point to more variables
	than necessary.  This is still correct because the redundant
	wlll cause no expression change and will leave the event still blocked.
	The sets in this class are evaluated once after allocation,
	prior to any execution.  
	When the owning event is blocked, it subscribes itself to
	all the variables in the set.
	When the event is unblocked, it unsubscribes itself from
	all variables in the set accordingly.  
	current object size: 24B
 */
struct DependenceSet {
private:
	typedef	DependenceSet			this_type;
public:
	instance_set_type			bool_set;
	instance_set_type			int_set;
	instance_set_type			channel_set;

	DependenceSet() : bool_set(), int_set(), channel_set() { }
	~DependenceSet() { }

	this_type&
	operator = (const this_type&);

	void
	import(const DependenceSetCollector&);

	ostream&
	dump(ostream&) const;

};	// end class Dependence

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_DEPENDENCE_H__


