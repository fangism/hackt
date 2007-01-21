/**
	\file "sim/command.tcc"
	$Id: command.tcc,v 1.2 2007/01/21 06:00:19 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_TCC__
#define	__HAC_SIM_COMMAND_TCC__

#include <iostream>
#include "sim/command.h"

namespace HAC {
namespace SIM {
#include "util/using_ostream.h"

//=============================================================================
// class Command method definitions

template <class State>
Command<State>::Command() : CommandBase(), _category(NULL), _main(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class State>
Command<State>::Command(const string& _n, const string& _b, 
		const category_ptr_type _c,
		const main_ptr_type _m, const usage_ptr_type _u) :
		CommandBase(_n, _b, _u), 
		_category(_c), _main(_m) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 0 on success.  
 */
template <class State>
int
Command<State>::main(state_type& s, const string_list& args) const {
	if (_main) {
		return (*_main)(s, args);
	} else {
		cerr << "command is undefined." << endl;
		return CommandBase::UNKNOWN;
	}
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_TCC__

