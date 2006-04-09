/**
	\file "Object/inst/substructure_alias_base.tcc"
	$Id: substructure_alias_base.tcc,v 1.6.10.5 2006/04/09 21:24:29 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_TCC__
#define	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_TCC__

#if 0
#include <iostream>
#include "Object/inst/substructure_alias_base.h"
#include "Object/common/cflat_args.h"
#include "Object/common/alias_string_cache.h"
#include "Object/global_entry.h"
#include "Object/def/footprint.h"
#include "Object/traits/class_traits_fwd.h"
#include "Object/inst/alias_printer.h"
#include "main/cflat.h"			// for cflat::print_alias
#include "main/cflat_options.h"
#include "util/sstream.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using std::ostringstream;
//=============================================================================

//=============================================================================
}	// end namespace entity
}	// end namespace HAC
#else
// This file is now obsolete.  All functionality has been relocated to
// visitor functions in "Object/inst/alias_printer.cc" and 
// "Object/inst/alias-matcher.cc"
#endif

#endif	// __HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_TCC__

