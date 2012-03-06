/**
	\file "Object/interfaces/VCDwriter.h"
	$Id: $
 */

#ifndef	__HAC_OBJECT_INTERFACES_VCDWRITER_H__
#define	__HAC_OBJECT_INTERFACES_VCDWRITER_H__

#include "Object/hierarchical_alias_visitor.h"

namespace HAC {
namespace VCD {
using std::ostream;
using entity::footprint;
using entity::footprint_frame;
using entity::global_offset;
using entity::hierarchical_alias_visitor;
using entity::state_instance;
using entity::bool_tag;
using entity::process_tag;

/**
	This class is made for writing out the HAC object hierarchy
	in a VCD header format for exporting trace files.  
	Implementation resembles cflat and global_allocation_dumper.
	cflat: traversal
	global_allocation_dumper: output information
		however, no process-ids are needed
 */
class VCDwriter : public hierarchical_alias_visitor {
	typedef	hierarchical_alias_visitor	parent_type;
public:
	ostream&				os;
// TODO: options struct?
	// use cflat_options?

	explicit
	VCDwriter(const footprint_frame& ff, const global_offset& g, 
		ostream& o) : hierarchical_alias_visitor(ff, g), os(o) { }

	~VCDwriter();

protected:
	using parent_type::visit;

	void
	__visit(const footprint&);

	void
	visit(const footprint&);

	// override
	void
	visit(const state_instance<bool_tag>&);

#if 0
	void
	visit(const state_instance<process_tag>&);
#endif

	// no need for PRS, macros, spec directives
	// may want parameters and constants eventually

};	// end class VCDwriter

}	// end namespace VCD
}	// end namespace HAC

#endif	// __HAC_OBJECT_INTERFACES_VCDWRITER_H__
