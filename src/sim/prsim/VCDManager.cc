/**
	\file "sim/prsim/VCDManager.cc"
	$Id: $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include "sim/prsim/VCDManager.hh"
#include "sim/prsim/State-prsim.hh"
#include "main/cflat_options.hh"
#include "Object/module.hh"
#include "Object/def/footprint.hh"
#include "Object/global_entry.hh"
#include "Object/interfaces/VCDwriter.hh"
#include "cvstag.h"
#include "util/indent.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.hh"
using VCD::VCDwriter;
using util::auto_indent;

// switch:
static const bool mangled_vcd_ids = true;

//=============================================================================
// class VCDManager method definitions
VCDManager::VCDManager(const string& fn, const module& m, const real_time& t, 
		const double& ts) :
		trace_name(fn),
		event_count(1), last_time(t),
		trace_file(new ofstream(fn.c_str())),
		time_scale(ts) {
	STACKTRACE_VERBOSE;
if (!good()) {
	cerr << "Error opening vcd file '" << fn << "' for writing." << endl;
	trace_name.clear();
} else {
	STACKTRACE_INDENT_PRINT("writing vcd header section" << endl);
	STACKTRACE_INDENT_PRINT("opened file: " << trace_name << endl);
// $date, $version, $timescale
{
	time_t tm;
	std::time(&tm);
	*trace_file << "$date\n\t" << std::ctime(&tm) << "$end\n" <<
		"$version\n\thacprsim " << PACKAGE_VERSION
		<< " (" << CVSTAG << ")\n$end\n" <<
		"$timescale\n\t1ps\n$end\n";
}
	// need to wrap everything in a top-level module to get
	// top-level signals
	*trace_file << auto_indent << "$scope module hacprsim-TOP $end" << endl;
{
// write out header of variables in hierarchy first
	// this can be quite slow for large circuits!
	INDENT_SECTION(*trace_file);
	// TODO: pass some options to vcd writer

	cflat_options cf;
	cf.mangled_vcd_ids = mangled_vcd_ids;	// configurable?
	cf.primary_tool = cflat_options::TOOL_VCD;
	const entity::footprint& topfp(m.get_footprint());
	const entity::global_process_context gpc(topfp);
	VCDwriter vw(gpc, *trace_file, cf);
	topfp.accept(vw);
}
	*trace_file << auto_indent << "$upscope $end\n"
		<< "$enddefinitions $end\n";
	print_time(*trace_file) << endl;
// dump initial value of all variables (separately)
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
VCDManager::~VCDManager() {
	// print one last time-stamp?
	// trace_file should flush itself and close upon destruction
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
VCDManager::good(void) const {
	return trace_file && *trace_file;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Note: VCD only accepts integer-valued times, 
		so use time_scale to adjust granularity
		to avoid rounding or truncation issues.
 */
ostream&
VCDManager::print_time(ostream& o) const {
#if 0
	return o << '#' << std::fixed << last_time;
#else
	return o << '#' << size_t(last_time *time_scale);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
VCDManager::print_id(ostream& o, const size_t i) {
	return VCDwriter::print_id(o, i, mangled_vcd_ids);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_index_type
VCDManager::record_event(const real_time& t, const node_index_type ni, 
		const value_enum v) {
	INVARIANT(good());
	INVARIANT(t >= last_time);
	if (t > last_time) {
		last_time = t;
		print_time(*trace_file) << '\n';
	}
	// format: all variables/wires are scalar for now
	*trace_file << NodeState::translate_value_to_char(v);
	if (!mangled_vcd_ids) {
		*trace_file << ' ';	// extra space for readability
	}
	print_id(*trace_file, ni) << '\n';
	const event_index_type ret = event_count;
	++event_count;
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
VCDManager::dumpvars(const State& s) {
	*trace_file << "$dumpvars" << endl;
	const State::node_pool_type& np(s.get_node_pool());
	size_t i = FIRST_VALID_GLOBAL_NODE;
	for ( ; i<np.size(); ++i) {
		np[i].dump_value(*trace_file);
		if (!mangled_vcd_ids) {
			*trace_file << ' ';
		}
		print_id(*trace_file, i) << '\n';
	}
	*trace_file << "$end" << endl;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

