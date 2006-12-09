/**
	\file "sim/command_category.tcc"
	$Id: command_category.tcc,v 1.1.2.2 2006/12/09 07:52:08 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_CATEGORY_TCC__
#define	__HAC_SIM_COMMAND_CATEGORY_TCC__

#include <iostream>
#include "sim/command_category.h"
#include "sim/command_registry.h"
#include "util/qmap.tcc"

namespace HAC {
namespace SIM {
#include "util/using_ostream.h"

//=============================================================================
// class command_category method definitions

template <class Command>
command_category<Command>::command_category() :
		_name(), _brief(), command_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Command>
command_category<Command>::command_category(
		const string& _n, const string& _b) :
		_name(_n), _brief(_b), command_map() {
	typedef	command_registry<command_type>	registry_type;
	registry_type::register_category(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't unregister because assignment is used to set
	a category.  
 */
template <class Command>
command_category<Command>::~command_category() {
	// command_category::unregister_category()?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a command with the local category.  
 */
template <class Command>
size_t
command_category<Command>::register_command(const Command& c) {
	typedef	typename command_map_type::mapped_type	mapped_type;
	mapped_type& probe(command_map[c.name()]);
	if (probe) {
		cerr << "command \'" << c.name() <<
			"\' has already been registered "
			"in this category (" << probe.name() << ")." << endl;
		THROW_EXIT;
	} else {
		probe = c;
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

