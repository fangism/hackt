// "art_parser.h"
// parser classes for ART only!

#ifndef __ART_PARSER_H__
#define __ART_PARSER_H__

// probably stick to lists, no hash_map's until the build() phase

#include <iostream>
#include <string>

#ifdef	__APPLE__		// different location with gcc-3.3 on OS X
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
#endif

#include "art_switches.h"
#include "list_of_ptr.h"	// includes <list>

using namespace std;

namespace ART {
namespace parser {

};
};

#endif	// __ART_PARSER_H__

