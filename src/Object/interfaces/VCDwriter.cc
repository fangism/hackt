/**
	\file "Object/lang/cflat_printer.cc"
	Implementation of cflattening visitor.
	$Id: cflat_printer.cc,v 1.35 2011/05/02 21:27:18 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <sstream>
#include <vector>
#include "Object/interfaces/VCDwriter.hh"
#include "Object/def/footprint.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/inst/state_instance.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/connection_policy.hh"
#include "Object/global_entry.hh"
#include "Object/global_channel_entry.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/traits/proc_traits.hh"
#include "main/cflat_options.hh"
#include "common/ICE.hh"
#include "common/TODO.hh"
#include "util/stacktrace.hh"
#include "util/offset_array.hh"
#include "util/member_saver.hh"
#include "util/indent.hh"

namespace HAC {
namespace VCD {
#include "util/using_ostream.hh"
using std::ostringstream;
using std::string;
using std::vector;
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
		<< " $end\n";
{
	INDENT_SECTION(os);
	__visit(f);
}
	os << auto_indent << "$upscope $end\n";
} else {
	__visit(f);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print variable map line of VCD file.
	TODO: port direction? in, out, vs. wire?
 */
void
VCDwriter::visit(const state_instance<bool_tag>& i) {
	// all buses/wires are scalarized
	os << auto_indent << "$var wire 1 ";
	print_id(os, id, opt.mangled_vcd_ids);
	os << ' ' << scope_stack.back() << 
		// normally bus width spec would go here, if arrays supported
			" $end\n";
	// print newline without flushing for performance
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
VCDwriter::print_id(ostream& o, const size_t i, const bool m) {
	if (m) {
		mangle_id(o, i);
	} else {
		o << '<' << i << '>';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static const long vcd_id_base = 94;
static const char vcd_id_offset = '!';

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	VCD identifier symbols use base-94 with ASCII characters.
	Uses upper 96/128 characters, except SPACE and DEL.
	TODO: profile whether or not this ldiv loop is bottleneck.
 */
void
VCDwriter::mangle_id(ostream& o, const size_t i) {
	static vector<char> __stack__;
	__stack__.reserve(32);		// no-op after first time
	long N = long(i);
	ldiv_t qr;
	do {
		qr = ldiv(N, vcd_id_base);
		__stack__.push_back(qr.rem);
		N = qr.quot;
	} while (N);
	std::transform(__stack__.rbegin(), __stack__.rend(), 
		std::ostream_iterator<char>(o),
		std::bind2nd(std::plus<char>(), vcd_id_offset));
	__stack__.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param id is the ASCII string identifier, NUL-terminated.
 */
size_t
VCDwriter::demangle_id(const char* id) {
	NEVER_NULL(id);
	size_t ret = 0;
	while (*id) {
		ret *= vcd_id_base;
		ret += *id;
		++id;
	}
	return ret;
}

//=============================================================================
}	// end namespace VCD
}	// end namespace HAC

