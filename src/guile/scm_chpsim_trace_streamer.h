/**
	\file "guile/scm_chpsim_trace_streamer.h"
	$Id: scm_chpsim_trace_streamer.h,v 1.1.2.2 2007/03/23 23:16:22 fang Exp $
 */

#ifndef	__HAC_GUILE_SCM_CHPSIM_TRACE_STREAMER_H__
#define	__HAC_GUILE_SCM_CHPSIM_TRACE_STREAMER_H__

#include "util/libguile.h"
#include "sim/chpsim/TraceStreamer.h"

namespace HAC {
namespace guile_wrap {
//=============================================================================
/**
	Heap-allocated object that manages a read-only single pass 
	of a trace file.
 */
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

#endif	// __HAC_GUILE_SCM_CHPSIM_TRACE_STREAMER_H__

