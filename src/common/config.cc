/**
	\file "common/config.cc"
	Prints configuration information, everything a maintainer
	would want to know about another's installation configuration.  
	$Id: config.cc,v 1.6 2009/10/16 20:38:44 fang Exp $
 */

#include <iostream>
#include <cstring>
#include "common/config.hh"
// #include "main/main_funcs.hh"
#include "config.h"

// various configure-generated and make-generated headers
#include "gitrev.h"
#include "builddate.h"
#include "cxx_version.h"
#include "cxxflags.h"
#include "am_cxxflags.h"
#include "lexer/lex_version.h"
#include "parser/yacc_version.h"
#include "buildhost.h"
#include "config_params.h"
#include "guile/config_version.h"
#include "util/readline_wrap.hh"

// #include "util/getopt_portable.hh"

namespace HAC {
#include "util/using_ostream.hh"
using util::readline_wrapper;

//=============================================================================
// class version static initializers

//=============================================================================
// static global strings

const char config::package_string[] = PACKAGE_STRING;
const char config::gitrev[] = GITREV;
const char config::cxx_version[] = CXX_VERSION;
const char config::am_cppflags[] = AM_CPPFLAGS;
const char config::am_cxxflags[] = AM_CXXFLAGS;
const char config::am_ldflags[] = AM_LDFLAGS;
const char config::config_cppflags[] = CONFIG_CPPFLAGS;
const char config::config_cxxflags[] = CONFIG_CXXFLAGS;
const char config::config_ldflags[] = CONFIG_LDFLAGS;
const char config::config_libs[] = CONFIG_LIBS;
const char config::config_params[] = CONFIG_PARAMS;
const char config::lex_version[] = LEX_VERSION;
const char config::yacc_version[] = YACC_VERSION;
const char config::guile_version[] = GUILE_CONFIG_VERSION;
const char config::builddate_string[] = BUILDDATE;
const char config::config_build[] = CONFIG_BUILD;
const char config::config_host[] = CONFIG_HOST;

//=============================================================================
ostream&
config::package(ostream& o) {
	return o << "Version: " << package_string;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
config::git(ostream& o) {
	return o << "git revision: " << gitrev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
config::cxx(ostream& o) {
	return o << "c++: " << cxx_version;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Now prints more than just CXXFLAGS, other related configuration
	flags used in the build.
 */
ostream&
config::cxxflags(ostream& o) {
		o << "AM_CPPFLAGS: " << am_cppflags;
	o << endl << "AM_CXXFLAGS: " << am_cxxflags;
	o << endl << "AM_LDFLAGS: " << am_ldflags;
	o << endl << "config-CXXFLAGS: " << config_cxxflags;
	o << endl << "config-CPPFLAGS: " << config_cppflags;
	o << endl << "config-LDFLAGS: " << config_ldflags;
	o << endl << "config-LIBS: " << config_libs;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
config::configure_params(ostream& o) {
	return o << "Configured with: " << config_params;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
config::lex(ostream& o) {
	return o << "lex: " << lex_version;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
config::yacc(ostream& o) {
	return o << "yacc: " << yacc_version;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
config::builddate(ostream& o) {
	return o << "build-date: " << builddate_string;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the host triplet only if it's different from the build triplet.
 */
ostream&
config::buildhost(ostream& o) {
	o << "build-triplet: " << config_build;
	if (strcmp(config_host, config_build))
		o << endl << "host-triplet: " << config_host;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	reports readline version string.
 */
ostream&
config::readline(ostream& o) {
	readline_wrapper::version_string(o << "readline: ");
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
config::guile(ostream& o) {
	return o << "guile: " << guile_version;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
config::dump_all(ostream& o) {
	package(o) << endl;
	git(o) << endl;
	configure_params(o) << endl;
	buildhost(o) << endl;
	cxx(o) << endl;
	cxxflags(o) << endl;
	lex(o) << endl;
	yacc(o) << endl;
	readline(o) << endl;
	guile(o) << endl;
	builddate(o) << endl;
	// influential environment variables
	return o;
}

//=============================================================================
}	// end namespace HAC

