/**
	\file "sim/signal_handler.tcc"
	$Id: signal_handler.tcc,v 1.4 2007/04/15 05:52:29 fang Exp $
 */

#ifndef	__HAC_SIM_SIGNAL_HANDLER_TCC__
#define	__HAC_SIM_SIGNAL_HANDLER_TCC__

#include "sim/signal_handler.hh"
#include "util/NULL.h"

namespace HAC {
namespace SIM {

//=============================================================================
// class State::signal_handler method definitions

/**
	Global static initializer for handler's bound State reference.  
	Must be instantiated.
 */
template <class State>
typename signal_handler<State>::state_type*
signal_handler<State>::_state = NULL;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Preserves the current State* and handler for restoration.  
	Swaps the current signal handler out for this one.  
 */
template <class State>
signal_handler<State>::signal_handler(state_type* s) :
		signal_handler_base(main), 
		_prev(_state) {
	_state = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destructor restores the former referenced state (pointer).  
	Upon destruction of base class, restores the former signal handler.
 */
template <class State>
signal_handler<State>::~signal_handler() {
	_state = this->_prev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class State>
void
signal_handler<State>::main(int) {
	if (_state) {
		_state->stop();
	}
}

//=============================================================================

}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_SIGNAL_HANDLER_TCC__

