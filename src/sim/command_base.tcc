/**
	\file "sim/command_base.tcc"
	$Id: command_base.tcc,v 1.2 2009/02/18 00:22:39 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_BASE_TCC__
#define	__HAC_SIM_COMMAND_BASE_TCC__

#include <iostream>
#include "sim/command_base.hh"

namespace HAC {
namespace SIM {
#include "util/using_ostream.hh"

//=============================================================================
// class Command method definitions

template <class State>
Command<State>::Command() : CommandBase(), _category(NULL), _main(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class State>
Command<State>::Command(const string& _n, const string& _b, 
		const category_ptr_type _c,
		const main_ptr_type _m, const usage_ptr_type _u, 
		const command_completer _k) :
		CommandBase(_n, _b, _u, _k), 
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

#endif	// __HAC_SIM_COMMAND_BASE_TCC__

