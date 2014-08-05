/**
	\file "sim/prsim/DelayBackAnnotation.cc"
 */


#include "sim/prsim/DelayBackAnnotation.hh"
#include "util/macros.h"

namespace HAC {
namespace SIM {
namespace PRSIM {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// class delay_back_annotation_manager method definitions
void
delay_back_annotation_manager::add_timing_fanin(const node_index_type ni,
		const process_index_type pi) {
	process_timing_fanin_type& ptf(timing_fanin[ni]);
//	INVARIANT(ptf.empty() || pi >= *(--ptf.end()));
	// monotonic by construction
	ptf.insert(pi);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const process_timing_fanin_type*
delay_back_annotation_manager::lookup_process_timing_fanin(
		const node_index_type ni) const {
	const global_timing_fanin_map::const_iterator
		f(timing_fanin.find(ni));
	if (f != timing_fanin.end()) {
		return &f->second;
	} else {
		return NULL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC
