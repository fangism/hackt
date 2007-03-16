/**
	\file "main/main_funcs.h"
	Prototypes of main-level functions.  
	$Id: main_funcs.h,v 1.8 2007/03/16 07:07:24 fang Exp $
 */

#ifndef	__HAC_MAIN_MAIN_FUNCS_H__
#define	__HAC_MAIN_MAIN_FUNCS_H__

#include "util/boolean_types.h"
#include "Object/module.h"
#include "util/memory/count_ptr.h"

namespace HAC {
using util::good_bool;
using util::memory::count_ptr;
using entity::module;
class compile_options;		// defined in "main/compile_options.h"

//=============================================================================
extern
FILE*
open_source_file(const char*);

extern
good_bool
check_object_loadable(const char*);

extern
good_bool
check_file_writeable(const char*);

extern
good_bool
flatten_source(const char*);

extern
count_ptr<module>
parse_and_check(const char*, const compile_options&);

extern
good_bool
self_test_module(const module&);

extern
void
save_module(const module&, const char*);

extern
void
save_module_debug(const module&, const char*, const bool = false);

extern
count_ptr<module>
load_module(const char*);

extern
count_ptr<module>
load_module_debug(const char*);

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_MAIN_FUNCS_H__

