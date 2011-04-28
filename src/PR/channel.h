/**
	\file "PR/channel.h"
	$Id: channel.h,v 1.1.2.8 2011/04/28 21:44:19 fang Exp $
 */
#ifndef	__HAC_PR_CHANNEL_H__
#define	__HAC_PR_CHANNEL_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "PR/numerics.h"
#include "PR/channel_type.h"

namespace PR {
using std::vector;
using std::ostream;
using std::istream;
using std::string;
struct placer_options;

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
	Stateful information for every channel.
 */
struct channel_state {
	/**
		A cached-only value computed separately.
	 */
	real_type			potential_energy;

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

};	// end struct channel_state

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Instance of spring-model point-to-point channel.
	TODO: alignment: favor vertical or horizontal direction.
 */
struct channel_instance : public channel_state {
//	const channel_type*		type;
	channel_type			properties;
//	int_type			global_index;	// HAC global ID
	/**
		Terminals are object indices.
		Don't use pointers because of possible need to 
		swap sets of object states/positions.
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

	channel_instance();

	channel_instance(const size_t, const size_t);

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

	ostream&
	emit_dot(ostream&, const placer_options&) const;

};	// end struct channel instance

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PR

#endif	// __HAC_PR_CHANNEL_H__
