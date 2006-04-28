/**
	\file "main/simple_options.tcc"
	$Id: simple_options.tcc,v 1.2.36.1 2006/04/28 03:08:17 fang Exp $
 */

#ifndef	__HAC_MAIN_SIMPLE_OPTIONS_TCC__
#define	__HAC_MAIN_SIMPLE_OPTIONS_TCC__

#include <iostream>
#include "main/simple_options.h"
#include "util/macros.h"
#include "util/getopt_portable.h"
#include "main/main_funcs.h"		// for unknown_option

namespace HAC {
#include "util/using_ostream.h"
//=============================================================================
template <class Opt, class ModMap>
int
parse_simple_command_options(const int argc, char* argv[], Opt& cf, 
		const ModMap& mod_map) {
	static const char optstring[] = "+f:";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
		case 'f': {
			typedef	typename ModMap::mapped_type	mapped_type;
			typedef	typename ModMap::const_iterator	const_iterator;
			const const_iterator mi(mod_map.find(optarg));
			if (mi == mod_map.end() || !mi->second) {
				cerr << "Invalid mode: " << optarg << endl;
				return 1;
			} else {
				const mapped_type& om(mi->second);
				om(cf);
			}
			break;
		}
		case ':':
			cerr << "Expected but missing option-argument." << endl;
			return 1;
		case '?':
			unknown_option(optopt);
			return 1;
		default:
			THROW_EXIT;
	}       // end switch
	}       // end while
	return 0;
}

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_SIMPLE_OPTIONS_TCC__

