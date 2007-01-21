/**
	\file "sim/signal_handler.h"
	$Id: signal_handler.h,v 1.2 2007/01/21 06:00:33 fang Exp $
 */

#ifndef	__HAC_SIM_SIGNAL_HANDLER_H__
#define	__HAC_SIM_SIGNAL_HANDLER_H__

#include "util/attributes.h"

namespace HAC {
namespace SIM {

//=============================================================================
/**
	Base class for signal handler.  
	Hard-coded for SIGINT (interrupt).  
 */
class signal_handler_base {
protected:
	void (*_main)(int);
public:
	explicit
	signal_handler_base(void (*)(int));

	~signal_handler_base();

private:
	/// private, undefined copy-constructor
	explicit
	signal_handler_base(const signal_handler_base&);

	/// private undefined assignment operator
	signal_handler_base&
	operator = (const signal_handler_base&);

};	// end class signal_handler_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Class that maintain a temporary setting of a signal event handler.
	\param State the type of class (simulater state) affected 
		by the signal handler routine.  The state type is required
		to have a member function called stop().  
 */
template <class State>
class signal_handler : protected signal_handler_base {
public:
	typedef	State		state_type;
private:
	static state_type*		_state;
	static void main(int);
private:
	state_type*			_prev;
public:
	explicit
	signal_handler(state_type*);

	~signal_handler();

} __ATTRIBUTE_UNUSED__ ;	// end class signal_handler

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_SIGNAL_HANDLER_H__

