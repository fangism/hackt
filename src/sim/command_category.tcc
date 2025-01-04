/**
	\file "sim/command_category.tcc"
	$Id: command_category.tcc,v 1.2 2007/01/21 06:00:24 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_CATEGORY_TCC__
#define	__HAC_SIM_COMMAND_CATEGORY_TCC__

#include <iostream>
#include "sim/command_category.hh"
#include "sim/command_registry.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace SIM {
#include "util/using_ostream.hh"

REQUIRES_STACKTRACE_STATIC_INIT

//=============================================================================
// class command_category method definitions

template <class Command>
command_category<Command>::command_category() :
		_name(), _brief(), command_map() {
	STACKTRACE_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Command>
command_category<Command>::command_category(
		const string& _n, const string& _b) :
		_name(_n), _brief(_b), command_map() {
	STACKTRACE_VERBOSE;
	// corresponding command_registry must have already been instantiated!
	typedef	command_registry<command_type>	registry_type;
	STACKTRACE_INDENT_PRINT("Adding category: " << _name << endl);
	registry_type::register_category(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't unregister because assignment is used to set
	a category.  
 */
template <class Command>
command_category<Command>::~command_category() {
	STACKTRACE_VERBOSE;
	// command_category::unregister_category()?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a command with the local category.  
 */
template <class Command>
size_t
command_category<Command>::register_command(const Command& c) {
	STACKTRACE_VERBOSE;
	typedef	typename command_map_type::value_type	value_type;
	typedef	typename command_map_type::iterator	iterator;
	const std::pair<iterator, bool>
		probe(command_map.insert(value_type(c.name(), c)));
	if (!probe.second) {
		cerr << "command \'" << c.name() <<
			"\' has already been registered "
			"in this category (" <<
//			probe.first->second.name() <<
			_name <<
			")." << endl;
		THROW_EXIT;
	}
	return command_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Command>
void
command_category<Command>::list(ostream& o) const {
	o << "Commands available in the \'" << _name << "\' category:" << endl;
	const_iterator i(command_map.begin());
	const const_iterator e(command_map.end());
	for ( ; i!=e; i++) {
		o << '\t' << i->first << " -- " << i->second.brief() << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up a command.  
	Pretty much only used by alias lookups.  
 */
template <class Command>
typename command_category<Command>::const_iterator
command_category<Command>::lookup_command(const string& c) const {
	return command_map.find(c);
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_CATEGORY_TCC__

