/**
	\file "PR/channel_type.hh"
	$Id: 
 */
#ifndef	__HAC_PR_CHANNEL_TYPE_H__
#define	__HAC_PR_CHANNEL_TYPE_H__

/**
	Define to 1 to have channels represented as collection of wires.
	No real need wire-details for the placer.  
 */
#define	PL_CHANNEL_WIRES		0

#include <iosfwd>
#include <string>
#if PL_CHANNEL_WIRES
#include <vector>
#endif
#include "PR/numerics.hh"

namespace HAC {
namespace PR {
#if PL_CHANNEL_WIRES
using std::vector;
#endif
using std::ostream;
using std::istream;
using std::string;

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
	/**
		Sum of two radii of the end-objects, 
		the ideal 0-energy distance, computed automatically.
		This is calculated, not set.
	 */
	real_type			equilibrium_distance;

//	real_type			repulsion_coeff;

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
}	// end namespace PR
}	// end namespace HAC

#endif	// __HAC_PR_CHANNEL_TYPE_H__
