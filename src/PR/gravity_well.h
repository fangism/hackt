/**
	\file "PR/gravity_well.h"
	$Id: gravity_well.h,v 1.1.2.1 2011/04/27 01:47:39 fang Exp $
 */

#ifndef	__HAC_PR_GRAVITY_WELL_H__
#define	__HAC_PR_GRAVITY_WELL_H__

#include <iosfwd>
#include <set>
#include <map>

#include "PR/numerics.h"

namespace PR {
class placement_engine;
using std::ostream;
using std::istream;

/** 
	A gravity well is a set of coordinates that attract near neighbors.  
	This is useful for legalizing and aligning placement of objects
	on designated grids.  
	A gravity well structure may contain several attractor nodes.  
	A node is really a hyperplane of attraction, not a single point.  
	All nodes (valleys) are consider equipotential.

	Details:
	wells essentially divide up the space into spring regions,
	where the boundaries are determined by the midpoints between
	adjacent well nodes.  
	The pull of the end wells extends out to +/-INF.
	For N well nodes, there are N-1 regions.  

	The strength of pull in each gravity well is passed in externally.
	This structure only provides a map.
 */
class gravity_well {
	friend class placement_engine;
private:
	typedef	std::set<real_type>			node_set_type;
	node_set_type					nodes;	// poles
public:
	gravity_well();
	~gravity_well();

	size_t
	size(void) const { return nodes.size(); }

	void
	add_single_well(const real_type&);

	void
	add_strided_wells(const real_type&, const real_type&, const real_type&);

	void
	remove_well(const real_type&);

	void
	remove_wells(const real_type&, const real_type&);

	ostream&
	dump(ostream&) const;

	void
	save_checkpoint(ostream&) const;

	void
	load_checkpoint(istream&);

};	// end struct gravity_well

}	// end namespace PR

#endif	// __HAC_PR_GRAVITY_WELL_H__
