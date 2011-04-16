/**
	\file "PR/channel.h"
	$Id: channel.h,v 1.1.2.3 2011/04/16 01:51:52 fang Exp $
 */
#ifndef	__HAC_PR_CHANNEL_H__
#define	__HAC_PR_CHANNEL_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "PR/numerics.h"

namespace PR {
using std::vector;
using std::ostream;
using std::istream;
using std::string;

/**
	Define to 1 to have channels represented as collection of wires.
	No real need wire-details for the placer.  
 */
#define	PL_CHANNEL_WIRES		0

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PL_CHANNEL_WIRES
struct wire {
	// name of wires?
};	// end struct wire
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For multi-terminal nets.  
 */
struct net_properties {
	// optimizing configurations:
	// some of these choices affect only routing or placement
	enum {
		NET_MIN_SPANNING_TREE,
		NET_MIN_STEINER_TREE,	// rectilinear Steiner tree
		NET_MIN_DEPTH,		// single-source
		NET_MESH		// redundant connections
	};
};	// end struct net_properties

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Defines a coherent group of wires as a channel.
	Channels are by definition point-to-point connections
		with single source, single destination, 
		and are hence modeled as springs.  
	TODO: support for turning, twisting, braiding, to reduce coupling 
		cap. for the router.
 */
struct channel_type {
#if PL_CHANNEL_WIRES
	/**
		Collection of wires in channel, 
		or may be single wire.
	 */
	vector<wire>			wires;
#endif
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
	real_type			spring_coeff;

	channel_type();

	~channel_type();

	bool
	parse_property(const string&);

	bool
	parse_property(const option_value&);

	ostream&
	dump(ostream&) const;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	static
	ostream&
	dump_checkpoint(ostream&, istream&);

};	// end class channel_type

typedef	channel_type			channel_properties;

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
//	const channel_type*		type;
	channel_type			properties;
//	int_type			global_index;	// HAC global ID
	/**
		Terminals are object indices.
	 */
	size_t				source;
	size_t				destination;
#if 0
	/**
		Important, timing-critical channels can be given
		higher spring coefficients to shorten their distances.
	 */
	real_type			spring_coeff;
#endif
	/**
		Force on spring, computed and cached.
	 */
	real_type			tension;

	channel_instance();

	explicit
	channel_instance(const channel_type&);

	~channel_instance();

	ostream&
	dump(ostream&) const;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

	static
	ostream&
	dump_checkpoint(ostream&, istream&);

};	// end struct channel instance

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PR

#endif	// __HAC_PR_CHANNEL_H__
