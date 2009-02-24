/**
	\file "sim/command_registry.cc"
	$Id: command_registry.cc,v 1.6 2009/02/24 00:35:45 fang Exp $
 */

#include <iostream>
#include <iterator>
#include <algorithm>			// for reverse_copy
#include <set>
#include <list>
#include <string>
#include <cstring>			// for strrchr
#include <vector>
#include "sim/command_registry.h"
#include "sim/command_base.h"
#include "util/qmap.tcc"
#include "util/readline.h"
#include "util/readline_wrap.h"

namespace HAC {
namespace SIM {
using std::set;
using std::ostream_iterator;
#include "util/using_ostream.h"

//=============================================================================
/**
	Make sure name isn't already registered as a category, 
	standard command, or existing alias.  
	Strongly recommend caller checking that aliases don't conflict with
	existing commands first.  
 */
int
command_aliases_base::add_alias(aliases_map_type& aliases, 
		const string& a, const string_list& c) {
	if (aliases.find(a) != aliases.end()) {
		cerr << "\'" << a << "\' is already an alias; "
			"you must unalias it before redefining it." << endl;
		return CommandBase::BADARG;
	} else {
		aliases[a] = c;
		return CommandBase::NORMAL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unregisters an alias.  
	Doesn't check for former existence.  
 */
int
command_aliases_base::unalias(aliases_map_type& aliases, const string& a) {
	aliases.erase(a);
	return CommandBase::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unregisters all aliases.  
 */
int
command_aliases_base::unalias_all(aliases_map_type& aliases) {
	aliases.clear();
	return CommandBase::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
command_aliases_base::list_aliases(const aliases_map_type& aliases,
		ostream& o) {
	o << "Command aliases:" << endl;
	alias_iterator i(aliases.begin()), e(aliases.end());
	for ( ; i!=e; ++i) {
		o << "\t" << i->first << " -> ";
		ostream_iterator<string> osi(o, " ");
		copy(i->second.begin(), i->second.end(), osi);
		o << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands prefix aliases, just like sh.  
	\pre there exist no cyclic aliases.  
	\pre no existing commands are overridden by aliases.  
 */
int
command_aliases_base::expand_aliases(const aliases_map_type& aliases, 
		string_list& c) {
if (c.size()) {
	set<string> seen;
	alias_iterator a(aliases.find(c.front()));
	while (a != aliases.end()) {
		const string_list& x(a->second);
		if (!seen.insert(a->first).second) {
			cerr << "Error: detected cyclic alias during expansion!"
				<< endl;
			return CommandBase::BADARG;
		}
		c.pop_front();
		std::reverse_copy(x.begin(), x.end(), front_inserter(c));
		a = aliases.find(c.front());
	}
	// stops expanding as soon as non-alias is found.  
}
// else nothing to expand
	return CommandBase::NORMAL;
}

//=============================================================================
// member struct auto_file_echo method definitions

command_aliases_base::auto_file_echo::auto_file_echo(
		ostream& o, const bool e, const string& n) :
		os(o), echo(e), name(n) {
	if (echo) {
		os << "## enter: \"" << name << "\"" << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
command_aliases_base::auto_file_echo::~auto_file_echo() {
	if (echo) {
		os << "## leave: \"" << name << "\"" << endl;
	}
}

//=============================================================================
/**
	Custom match-list printer which omits the common
	hierarchical prefix ('.'-delimited) prepended by our 
	completion routine.  
 */
void
display_hierarchical_matches_hook(char** matches, int len, int max) {
#ifdef	USE_READLINE
if (len) {
#if 0
	cout << '{';
	copy(matches, matches+len+1, 
		std::ostream_iterator<const char*>(cout, "|"));
	cout << "}, " << len << ", " << max << endl;
#endif
	// find the last '.', if any
	char* tail = strrchr(matches[0], '.');
	if (tail) {
		const int dist = tail -matches[0] +1;	// past '.'
		std::vector<char*> mod_matches;
		mod_matches.reserve(len+1);
		mod_matches.push_back(matches[0]);
		int i=1;
		for ( ; i<=len; ++i) {
			// remove common prefix
			NEVER_NULL(matches[i]);
//			cout << '(' << matches[i] << ')';
			const size_t ml = strlen(matches[i]);
			INVARIANT(ml > size_t(dist));
			mod_matches.push_back(matches[i] +dist);
		}
		INVARIANT(max > dist);
#if 0
		cout << '{';
		copy(mod_matches.begin(), mod_matches.end(), 
			std::ostream_iterator<const char*>(cout, "|"));
		cout << '}' << endl;
#endif
		rl_display_match_list(&mod_matches[0], len, max -dist);
	} else {
		// use default printer
		rl_display_match_list(matches, len, max);
	}
	rl_forced_update_display();	// refresh prompt, line-buffer
}
#endif
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

