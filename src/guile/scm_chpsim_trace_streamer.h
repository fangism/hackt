/**
	\file "guile/scm_chpsim_trace_streamer.h"
	$Id: scm_chpsim_trace_streamer.h,v 1.1.2.6 2007/04/04 04:31:27 fang Exp $
 */

#ifndef	__HAC_GUILE_SCM_CHPSIM_TRACE_STREAMER_H__
#define	__HAC_GUILE_SCM_CHPSIM_TRACE_STREAMER_H__

#include "util/libguile.h"
#include "sim/chpsim/TraceStreamer.h"

namespace HAC {
namespace guile_wrap {
using HAC::SIM::CHPSIM::TraceManager;

//=============================================================================
/**
	Heap-allocated object that manages a read-only single pass 
	of a trace file.
	These handles provide an interface to the 'events' that occur.  
 */
typedef	TraceManager::entry_streamer	scm_chpsim_trace_stream;
typedef	TraceManager::entry_reverse_streamer
					scm_chpsim_trace_reverse_stream;
typedef	TraceManager::random_accessor	scm_chpsim_trace_random_accessor;

/**
	The trace handle that provides an interface to the stream
	of variable and state value changes.  
 */
typedef	TraceManager::state_change_streamer	scm_chpsim_state_change_stream;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Run-time type identifier set upon guile's SMOB registration.  
 */
extern
const scm_t_bits& raw_chpsim_trace_stream_tag;

extern
const scm_t_bits& raw_chpsim_trace_reverse_stream_tag;

extern
const scm_t_bits& raw_chpsim_trace_random_accessor_tag;

extern
const scm_t_bits& raw_chpsim_state_change_stream_tag;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
scm_chpsim_trace_stream*
scm_smob_to_chpsim_trace_stream_ptr(const SCM&);

extern
scm_chpsim_trace_reverse_stream*
scm_smob_to_chpsim_trace_reverse_stream_ptr(const SCM&);

extern
scm_chpsim_trace_random_accessor*
scm_smob_to_chpsim_trace_random_accessor_ptr(const SCM&);

extern
scm_chpsim_state_change_stream*
scm_smob_to_chpsim_state_change_stream_ptr(const SCM&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern
void
raw_chpsim_trace_stream_smob_init(void);

extern
void
raw_chpsim_trace_reverse_stream_smob_init(void);

extern
void
raw_chpsim_trace_random_accessor_smob_init(void);

extern
void
raw_chpsim_state_change_stream_smob_init(void);

/// All of the above
extern
void
raw_chpsim_trace_modes_smob_init(void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// includes forward-stream, reverse-stream, random-access
extern
void
import_hackt_chpsim_trace_stream_functions(void);

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

#endif	// __HAC_GUILE_SCM_CHPSIM_TRACE_STREAMER_H__

