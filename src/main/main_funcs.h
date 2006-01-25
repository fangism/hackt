/**
	\file "main/main_funcs.h"
	Prototypes of main-level functions.  
	$Id: main_funcs.h,v 1.4 2006/01/25 20:26:04 fang Exp $
 */

#ifndef	__HAC_MAIN_MAIN_FUNCS_H__
#define	__HAC_MAIN_MAIN_FUNCS_H__

#include "util/boolean_types.h"
#include "Object/module.h"
#include "util/memory/excl_ptr.h"

namespace HAC {
using util::good_bool;
using util::memory::excl_ptr;
using entity::module;

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
excl_ptr<module>
parse_and_check(const char*);

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
excl_ptr<module>
load_module(const char*);

extern
excl_ptr<module>
load_module_debug(const char*);

extern
void
unknown_option(const int);

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_MAIN_FUNCS_H__

