/**
	\file "main/main_funcs.hh"
	Prototypes of main-level functions.  
	$Id: main_funcs.hh,v 1.11 2009/10/15 01:05:11 fang Exp $
 */

#ifndef	__HAC_MAIN_MAIN_FUNCS_H__
#define	__HAC_MAIN_MAIN_FUNCS_H__

#include <iosfwd>
#include "util/boolean_types.hh"
#include "Object/module.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
class process_type_reference;
}	// end namespace process_type_reference
using util::good_bool;
using util::memory::count_ptr;
using entity::module;
using entity::process_type_reference;
using std::ostream;
class compile_options;		// defined in "main/compile_options.hh"

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
flatten_source(const char*, const compile_options&);

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

extern
count_ptr<const process_type_reference>
parse_and_create_complete_process_type(const char*, const module&);

extern
int
parse_create_flag(const int, compile_options&);

extern
void
create_usage(const char*, ostream&);

//=============================================================================
}	// end namespace HAC

#endif	// __HAC_MAIN_MAIN_FUNCS_H__

