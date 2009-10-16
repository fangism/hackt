/**
	\file "common/config.h"
	Interface header for version module.  
	$Id: config.h,v 1.4 2009/10/16 20:38:45 fang Exp $
 */

#ifndef	__HAC_COMMON_CONFIG_H__
#define	__HAC_COMMON_CONFIG_H__

#include <iosfwd>

namespace HAC {
using std::ostream;
//=============================================================================
/**
	Configuration and version information.
	TODO: guile configuration
 */
struct config {
	static const char		package_string[];
	static const char		cvstag[];
	static const char		cxx_version[];
	static const char		am_cppflags[];
	static const char		am_cxxflags[];
	static const char		am_ldflags[];
	static const char		config_cppflags[];
	static const char		config_cxxflags[];
	static const char		config_ldflags[];
	static const char		config_libs[];
	static const char		config_params[];
	static const char		lex_version[];
	static const char		yacc_version[];
	static const char		guile_version[];
	static const char		builddate_string[];
	static const char		config_build[];
	static const char		config_host[];

	static
	ostream&
	package(ostream&);

	static
	ostream&
	cvs(ostream&);

	static
	ostream&
	cxx(ostream&);

	static
	ostream&
	lex(ostream&);

	static
	ostream&
	yacc(ostream&);

	static
	ostream&
	builddate(ostream&);

	static
	ostream&
	buildhost(ostream&);

	static
	ostream&
	cxxflags(ostream&);

	static
	ostream&
	configure_params(ostream&);

	static
	ostream&
	readline(ostream&);

	static
	ostream&
	guile(ostream&);

	static
	ostream&
	dump_all(ostream&);

};	// end class config

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_COMMON_CONFIG_H__

