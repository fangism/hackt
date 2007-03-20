/**
	\file "guile/scm_chpsim_trace_streamer.h"
	$Id: scm_chpsim_trace_streamer.h,v 1.1.2.1 2007/03/20 23:10:36 fang Exp $
 */

#include "util/libguile.h"
#include "sim/chpsim/TraceStreamer.h"

namespace HAC {
namespace guile_wrap {
//=============================================================================
typedef	HAC::SIM::CHPSIM::TraceManager::entry_streamer	scm_chpsim_trace_stream;

/**
	Run-time type identifier set upon guile's SMOB registration.  
 */
extern
const scm_t_bits& raw_chpsim_trace_stream_tag;

extern
void
raw_chpsim_trace_stream_smob_init(void);

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

