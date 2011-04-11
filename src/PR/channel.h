/**
	\file "PR/channel.h"
	$Id: channel.h,v 1.1.2.1 2011/04/11 18:38:34 fang Exp $
 */
#ifndef	__HAC_PR_CHANNEL_H__
#define	__HAC_PR_CHANNEL_H__

#include <iosfwd>
#include <vector>
#include "PR/numerics.h"

namespace PR {
using std::vector;
using std::ostream;

struct wire {
	// name of wires?
};	// end struct wire

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Defines a coherent group of wires as a channel.
	Channels are by definition point-to-point connections
		with single source, single destination, 
		and are hence modeled as springs.  
	TODO: support for turning, twisting, braiding, to reduce coupling cap.
 */
struct channel_type {
	/**
		Collection of wires in channel, 
		or may be single wire.
	 */
	vector<wire>			wires;
#if 0
	/**
		If true, channel/wire must have exactly one source and 
		one destination.
		If false, wire may be multi-terminal, have global fanout.
	 */
	bool				point_to_point;
#endif
	/**
		The higher the value, the tighter the spring.
		This is just a default value, actual channel instances
		may be overridden.  
	 */
	real_type			default_spring_coeff;

};	// end class channel_type

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instance of multiterminal net.
	Uses different heuristic for routing:
		like minimum-spanning-tree, minimum-depth-tree.
 */
struct net_instance {
	const channel_type*		type;
	vector<int_type>		terminals;

};	// end struct net_instance

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instance of spring-model point-to-point channel.
	TODO: alignment: favor vertical or horizontal direction.
 */
struct channel_instance {
	const channel_type*		type;
	int_type			global_index;	// HAC global ID
	int_type			source;
	int_type			destination;
	/**
		Important, timing-critical channels can be given
		higher spring coefficients to shorten their distances.
	 */
	real_type			spring_coeff;
	/**
		Force on spring.
	 */
	real_type			tension;
};	// end struct channel instance

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PR

#endif	// __HAC_PR_CHANNEL_H__
