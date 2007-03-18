/**
	\file "guile/chpsim-wrap.h"
	$Id: chpsim-wrap.h,v 1.1 2007/03/18 00:24:57 fang Exp $
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

extern
void
libhacktsim_guile_init(void);

END_C_DECLS

#endif	// __HAC_GUILE_CHPSIM_WRAP_H__


