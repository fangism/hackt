/**
	\file "guile/hackt-documentation.h"
	Snarfing and documentation macros, specific to this project.
	Inspired by lilypond's "lily/include/lily-guile-macros.hh"!
	$Id: hackt-documentation.h,v 1.1.2.1 2007/03/27 06:20:38 fang Exp $
 */

#ifndef	__HAC_GUILE_HACKT_DOCUMENTATION_H__
#define	__HAC_GUILE_HACKT_DOCUMENTATION_H__

#include "util/libguile.h"
#include "util/cppcat.h"

namespace HAC {
namespace guile_wrap {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Function registration type.
	Declare a vector of these for a function registry.  
 */
typedef	void	(*scm_init_func_type)(void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// call this to automatically register documentation
extern
void
add_function_documentation(SCM, const char*, const char*, const char*);

extern
void
init_documentation(void);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	ALERT: local global initialization ordering dependence...
	\param func is the function name of type void (*)(void).
	\param local_registry is a local container of function pointers
		to call upon registration.
 */
#define ADD_SCM_INIT_FUNC(func, local_registry)			\
static const size_t UNIQUIFY(func_receipt) =			\
	(local_registry.push_back(func), local_registry.size());

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Internal macro for defining the function.  
	Also defining an init function to register it.  
	Usually INITPREFIX is the same as FNAME.
	\param PRIMNAME the primitive name as seen by guile.
	\param REQ number of required arguments.
	\param OPT number of optional arguments.
	\param VAR variable arguments(?).
	\param ARGLIST prototype parameter list.
	\param REGISTRY container with which to register function.
	\param DOCSTRING documentation string for (help procedure).
 */
#define HAC_GUILE_DEFINE_PUBLIC_WITHOUT_DECL(				\
		INITPREFIX, FNAME, PRIMNAME, REQ, OPT, VAR, 		\
		ARGLIST, REGISTRY, DOCSTRING)				\
  static SCM FNAME ## _proc;						\
  static void								\
  INITPREFIX ## _init (void) {						\
    FNAME ## _proc = scm_c_define_gsubr (PRIMNAME, REQ, OPT, VAR,	\
		reinterpret_cast<util::guile::scm_gsubr_type>(FNAME));	\
    add_function_documentation (FNAME ## _proc, PRIMNAME, #ARGLIST,	\
				DOCSTRING);				\
    scm_c_export (PRIMNAME, NULL);					\
  }									\
  ADD_SCM_INIT_FUNC(INITPREFIX ## _init, REGISTRY);			\
  SCM									\
  FNAME ARGLIST

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prepends a prototype declaration (with linkage) 
	before the definition.  
 */
#define HAC_GUILE_DEFINE_PUBLIC_LINKAGE(FNAME, PRIMNAME, LINKAGE,	\
		REQ, OPT, VAR, ARGLIST, REGISTRY, DOCSTRING)		\
  LINKAGE SCM FNAME ARGLIST;						\
  HAC_GUILE_DEFINE_PUBLIC_WITHOUT_DECL(FNAME, FNAME, PRIMNAME,		\
		REQ, OPT, VAR, ARGLIST, REGISTRY, DOCSTRING)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We mostly use this macro.
	By default, our functions are defined with static linkage.
 */
#define HAC_GUILE_DEFINE_PUBLIC(FNAME, PRIMNAME,			\
		REQ, OPT, VAR, ARGLIST, REGISTRY, DOCSTRING)		\
HAC_GUILE_DEFINE_PUBLIC_LINKAGE(FNAME, PRIMNAME, static,		\
		REQ, OPT, VAR, ARGLIST, REGISTRY, DOCSTRING)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace guile_wrap
}	// end namespace HAC

#endif	// __HAC_GUILE_HACKT_DOCUMENTATION_H__

