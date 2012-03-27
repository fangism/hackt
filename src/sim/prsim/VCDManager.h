/**
	\file "sim/prsim/VCDManager.h"
	$Id: $
 */

#ifndef	__HAC_SIM_PRSIM_VCD_MANAGER_H__
#define	__HAC_SIM_PRSIM_VCD_MANAGER_H__

#include <iosfwd>
#include <string>
#include "sim/common.h"
#include "sim/time.h"
#include "util/memory/excl_ptr.h"
#include "sim/prsim/enums.h"

namespace HAC {
namespace entity {
class module;
}
namespace SIM {
namespace PRSIM {
using std::string;
using std::ostream;
using std::ofstream;
using entity::module;
using util::memory::excl_ptr;
class State;

/**
	Class for writing vcd files as trace files.
	TODO: selectively limit scope of tracing, 
		use integer-set on indices (discrete_interval_set).
 */
class VCDManager {
protected:
	string					trace_name;
	event_index_type			event_count;
	real_time				last_time;
	excl_ptr<ofstream>			trace_file;
public:
	const double				time_scale;
public:
	VCDManager(const string&, const module&,
		const real_time&, const double&);
	~VCDManager();

	event_index_type
	record_event(const real_time&, const node_index_type, const value_enum);

	bool
	good(void) const;

	void
	dumpvars(const State&);

	const string&
	get_trace_name(void) const { return trace_name; }

	static
	ostream&
	print_id(ostream&, const size_t);

	ostream&
	print_time(ostream&) const;

};	// end class VCDManager

}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_VCD_MANAGER_H__

