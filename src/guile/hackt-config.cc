/**
	\file "guile/hackt-config.cc"
	Guile wrappers that access configure and version information.  
	TODO: currently these are defined as null-functions, 
	would they be better off as permanent constant variables?
	$Id: hackt-config.cc,v 1.2 2007/04/20 18:25:55 fang Exp $
 */

#include "guile/hackt-config.hh"
#include "common/config.hh"
#include <vector>
#include <sstream>
#include "guile/hackt-documentation.hh"
#include "util/guile_STL.hh"
#include "util/for_all.hh"
#include "util/caller.hh"

namespace HAC {
namespace guile_wrap {
//=============================================================================
using util::guile::make_scm;

static
std::vector<scm_init_func_type>		local_registry;

#define HAC_GUILE_DEFINE(FNAME, PRIMNAME, REQ, OPT, VAR, ARGLIST, DOCSTRING) \
HAC_GUILE_DEFINE_PUBLIC(FNAME, PRIMNAME, REQ, OPT,			\
	VAR, ARGLIST, local_registry, DOCSTRING)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "package-string"
HAC_GUILE_DEFINE(wrap_package_string, FUNC_NAME, 0, 0, 0, (void), 
"Returns the configure package-string.") {
	return make_scm<const char*>(config::package_string);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "git-rev"
HAC_GUILE_DEFINE(wrap_gitrev, FUNC_NAME, 0, 0, 0, (void), 
"Returns the git revision of the build.") {
	return make_scm(config::gitrev);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "cxx-version"
HAC_GUILE_DEFINE(wrap_cxx_version, FUNC_NAME, 0, 0, 0, (void), 
"Returns the C++ compiler version used in build.") {
	return make_scm(config::cxx_version);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "am-cppflags"
HAC_GUILE_DEFINE(wrap_am_cppflags, FUNC_NAME, 0, 0, 0, (void), 
"Returns the AM_CPPFLAGS used in build.") {
	return make_scm(config::am_cppflags);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "am-cxxflags"
HAC_GUILE_DEFINE(wrap_am_cxxflags, FUNC_NAME, 0, 0, 0, (void), 
"Returns the AM_CXXFLAGS used in build.") {
	return make_scm(config::am_cxxflags);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "am-ldflags"
HAC_GUILE_DEFINE(wrap_am_ldflags, FUNC_NAME, 0, 0, 0, (void), 
"Returns the AM_LDFLAGS used in build.") {
	return make_scm(config::am_ldflags);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "config-cppflags"
HAC_GUILE_DEFINE(wrap_config_cppflags, FUNC_NAME, 0, 0, 0, (void), 
"Returns the CPPFLAGS used in build.") {
	return make_scm(config::config_cppflags);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "config-cxxflags"
HAC_GUILE_DEFINE(wrap_config_cxxflags, FUNC_NAME, 0, 0, 0, (void), 
"Returns the CXXFLAGS used in build.") {
	return make_scm(config::config_cxxflags);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "config-ldflags"
HAC_GUILE_DEFINE(wrap_config_ldflags, FUNC_NAME, 0, 0, 0, (void), 
"Returns the LDFLAGS used in build.") {
	return make_scm(config::config_ldflags);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "config-libs"
HAC_GUILE_DEFINE(wrap_config_libs, FUNC_NAME, 0, 0, 0, (void), 
"Returns the LIBS used in build.") {
	return make_scm(config::config_libs);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "configure-params"
HAC_GUILE_DEFINE(wrap_configure_params, FUNC_NAME, 0, 0, 0, (void), 
"Returns the configure params used in build.") {
	return make_scm(config::config_params);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "lex-version"
HAC_GUILE_DEFINE(wrap_lex_version, FUNC_NAME, 0, 0, 0, (void), 
"Returns the LEX version used in build.") {
	return make_scm(config::lex_version);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "yacc-version"
HAC_GUILE_DEFINE(wrap_yacc_version, FUNC_NAME, 0, 0, 0, (void), 
"Returns the YACC version used in build.") {
	return make_scm(config::yacc_version);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "build-date"
HAC_GUILE_DEFINE(wrap_build_date, FUNC_NAME, 0, 0, 0, (void), 
"Returns the timestamp of the build.") {
	return make_scm(config::builddate_string);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "config-build"
HAC_GUILE_DEFINE(wrap_config_build, FUNC_NAME, 0, 0, 0, (void), 
"Returns the build-triplet.") {
	return make_scm(config::config_build);
}
#undef	FUNC_NAME

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	FUNC_NAME "config-host"
HAC_GUILE_DEFINE(wrap_config_host, FUNC_NAME, 0, 0, 0, (void), 
"Returns the host-triplet.") {
	return make_scm(config::config_host);
}
#undef	FUNC_NAME

//=============================================================================
}	// end namespace guile_wrap
}	// end namespace HAC

//=============================================================================
BEGIN_C_DECLS
using namespace HAC::guile_wrap;

/**
	Define and export primitives in whatever current module is. 
 */
void
hackt_config_init(void) {
	util::for_all(local_registry, util::caller());
}

END_C_DECLS

