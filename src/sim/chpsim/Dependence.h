/**
	\file "sim/chpsim/Dependence.h"
	$Id: Dependence.h,v 1.1.2.6 2007/01/20 07:26:13 fang Exp $
 */

#ifndef	__HAC_SIM_CHPSIM_DEPENDENCE_H__
#define	__HAC_SIM_CHPSIM_DEPENDENCE_H__

#include <iosfwd>
#include <valarray>
#include "sim/common.h"
#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {
	class nonmeta_context_base;
	class nonmeta_state_manager;
}
namespace SIM {
namespace CHPSIM {
using std::ostream;
template <class> struct dependence_collector_base;
class DependenceSetCollector;	// from "sim/chpsim/DependenceCollector.h"
using entity::bool_tag;
using entity::int_tag;
using entity::enum_tag;
using entity::channel_tag;
using entity::nonmeta_context_base;
using entity::nonmeta_state_manager;

/**
	To keep this structure as small as possible, we use
	a valarray, but to construct the sets, we will use
	temporary vectors.  
	Invariant: make this set SORT for efficient set union/difference
	algorithms.  
 */
typedef	std::valarray<node_index_type>	instance_set_type;

//=============================================================================
/**
	TODO: move to detail namespace?
 */
template <class Tag>
class dependence_set_base {
	typedef	dependence_set_base<Tag>	this_type;
protected:
	/**
		Aggregated during simulator state allocation and construction.  
		Remains unmodified throughout simulation.  
	 */
	instance_set_type			_set;

	dependence_set_base() : _set() { }
	// default dtor

	this_type&
	operator = (const this_type&);

	void
	__import(const dependence_collector_base<Tag>&);

	void
	__subscribe(const nonmeta_context_base&,
		const event_index_type) const;

	void
	__unsubscribe(const nonmeta_context_base&,
		const event_index_type) const;

	bool
	__subscribed_to_any(const nonmeta_state_manager&,
		const event_index_type) const;

};	// end struct dependence_set_base

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
struct DependenceSet : 
		public dependence_set_base<bool_tag>,
		public dependence_set_base<int_tag>,
		public dependence_set_base<enum_tag>,
		public dependence_set_base<channel_tag> {
private:
	typedef	DependenceSet			this_type;
public:
	DependenceSet();
	~DependenceSet();

	this_type&
	operator = (const this_type&);

	void
	import(const DependenceSetCollector&);

	void
	subscribe(const nonmeta_context_base&, const event_index_type) const;

	void
	unsubscribe(const nonmeta_context_base&, const event_index_type) const;

	ostream&
	dump(ostream&) const;

	ostream&
	dump_subscribed_status(ostream&, const nonmeta_state_manager&, 
		const event_index_type) const;

};	// end class Dependence

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_CHPSIM_DEPENDENCE_H__


