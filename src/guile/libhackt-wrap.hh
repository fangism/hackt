/**
	\file "main/libhackt-wrap.hh"
	$Id: libhackt-wrap.hh,v 1.3 2009/02/23 09:11:16 fang Exp $
	Earliest history, file was "main/libhackt-wrap.h"
 */

#ifndef	__HAC_GUILE_LIBACKT_WRAP_H__
#define	__HAC_GUILE_LIBACKT_WRAP_H__

#include "util/c_decl.h"
#include "util/memory/count_ptr.hh"
#include "guile/devel_switches.hh"
#include "util/libguile.hh"
#include <map>		// really only need forward declaration...
#include <string>

//=============================================================================
// global data to be made accessible to guile interpreter

namespace HAC {
namespace entity {
class module;
}	// end namespace entity

/// namespace for guile-wrappers and interfaces
namespace guile_wrap {
using util::memory::count_ptr;
using entity::module;
/**
	Top-level object module to be loaded before passing control
	over to guile/scheme interpreter.
	Shouldn't be const, because we may need to allocate.
	This is reference-counted so other plug-ins may safely copy
	the reference and ensure proper lifetime.  
 */
extern	count_ptr<module>	obj_module;

/**
	Unique SCM symbols 'bool, 'int...
	To be initialized...
 */
extern
SCM
scm_type_symbols[];

typedef	std::map<std::string, size_t>	scm_symbol_to_enum_map_type;

/**
	This map performs reverse lookup, using the pointer
	to a permanent symbol as the key.  
	This is populated during procedure-loading initialization, below.  
 */
extern
scm_symbol_to_enum_map_type		scm_symbol_to_enum_map;

}	// end namespace guile
}	// end namespace HAC


//=============================================================================
BEGIN_C_DECLS

extern
void
libhackt_guile_init(void);

#if 0
extern
void
scm_init_hackt_libhackt_primitives_module(void);
#endif

END_C_DECLS

#endif	// __HAC_GUILE_LIBACKT_WRAP_H__

