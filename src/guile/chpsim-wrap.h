/**
	\file "guile/chpsim-wrap.h"
	We subdivide functionality into different modules:
	(hackt chpsim-primitives): for general chpsim primitives
	(hackt chpsim-trace-primitives): for trace-file related primitives.
	$Id: chpsim-wrap.h,v 1.3 2009/02/23 09:11:16 fang Exp $
 */

#ifndef	__HAC_GUILE_CHPSIM_WRAP_H__
#define	__HAC_GUILE_CHPSIM_WRAP_H__

#include "util/memory/count_ptr.h"
#include "util/c_decl.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
class State;
}	// end namespace CHPSIM
}	// end namespace SIM
namespace guile_wrap {
using util::memory::count_ptr;

extern
count_ptr<SIM::CHPSIM::State>	chpsim_state;

}	// end namespace guile_wrap
}	// end namespace HAC

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BEGIN_C_DECLS

// don't really need to export this anymore
// these might be need if we ever export as a plug-in (dynamic library)
extern
void
libhackt_chpsim_guile_init(void);

// this loads trace-file interface separately
extern
void
libhackt_chpsim_trace_guile_init(void);

#if 0
extern
void
scm_init_hackt_chpsim_primitives_module(void);

extern
void
scm_init_hackt_chpsim_trace_primitives_module(void);
#endif

END_C_DECLS

#endif	// __HAC_GUILE_CHPSIM_WRAP_H__


