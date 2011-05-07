/**
	\file "sim/prsim/State-prsim.tcc"
	Relocate some reusable template member functions here.
	$Id: State-prsim.tcc,v 1.1 2011/05/07 21:34:27 fang Exp $
 */

#ifndef	__HAC_SIM_PRSIM_STATE_PRSIM_TCC__
#define	__HAC_SIM_PRSIM_STATE_PRSIM_TCC__

#include "sim/prsim/State-prsim.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
//=============================================================================
/**
	Filter a set of node indices, using a predicate.  
	\param ret set of nodes, filtered set returned here
	\param f is a functor that expects a const node_type&
 */
template <class F>
void
State::filter_nodes(vector<node_index_type>& ret, F f) const {
	vector<node_index_type> tmp;
	tmp.swap(ret);
	vector<node_index_type>::const_iterator i(tmp.begin()), e(tmp.end());
	for ( ; i!=e; ++i) {
		if (f(node_pool[*i])) {
			ret.push_back(*i);
		}
	}
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_STATE_PRSIM_TCC__
