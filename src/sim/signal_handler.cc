/**
	\file "sim/signal_hander.cc"
	$Id: signal_handler.cc,v 1.1.2.1 2006/12/08 07:51:24 fang Exp $
 */

#include "sim/signal_handler.h"
#include "util/signal.h"

namespace HAC {
namespace SIM {
//=============================================================================

/**
	For the life of this object, replace the signal handler, 
	while saving away the former handler.
 */
signal_handler_base::signal_handler_base(void (*m)(int)) :
		_main(signal(SIGINT, m)) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Upon destruction, end-of-line, restore the previous event-handler.  
 */
signal_handler_base::~signal_handler_base() {
	signal(SIGINT, _main);
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

