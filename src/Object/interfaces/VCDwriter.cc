/**
	\file "Object/lang/cflat_printer.cc"
	Implementation of cflattening visitor.
	$Id: cflat_printer.cc,v 1.35 2011/05/02 21:27:18 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
// #include <algorithm>
#include <set>
#include <sstream>
#include "Object/interfaces/VCDwriter.h"
#include "Object/def/footprint.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/inst/state_instance.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/connection_policy.h"
#include "Object/global_entry.h"
#include "Object/global_channel_entry.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/proc_traits.h"
// #include "main/cflat_options.h"
#include "common/ICE.h"
#include "common/TODO.h"
#include "util/stacktrace.h"
#include "util/offset_array.h"
#include "util/member_saver.h"
#include "util/indent.h"

namespace HAC {
namespace VCD {
#include "util/using_ostream.h"
using std::ostringstream;
using std::string;
using entity::footprint;
using entity::instance_alias_info;
using util::auto_indent;

//=============================================================================
// class VCDwriter method definitions

VCDwriter::~VCDwriter() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
VCDwriter::__visit(const footprint& f) {
	visit_ports_and_local<bool_tag>(f);
	visit_recursive(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
VCDwriter::visit(const footprint& f) {
if (scope_stack.size()) {
	os << auto_indent << "$scope module " << scope_stack.back()
		<< " $end" << endl;
{
	INDENT_SECTION(os);
	__visit(f);
}
	os << auto_indent << "$upscope $end" << endl;
} else {
	__visit(f);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print variable map line of VCD file.
 */
void
VCDwriter::visit(const state_instance<bool_tag>& i) {
	// all buses/wires are scalarized
	os << auto_indent << "$var wire 1 <" << id << "> " <<
		scope_stack.back() << 
		// normally bus width spec would go here
			" $end" << endl;
}

//=============================================================================
}	// end namespace VCD
}	// end namespace HAC

