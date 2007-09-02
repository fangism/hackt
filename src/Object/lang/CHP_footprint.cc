/**
	\file "Object/lang/CHP_footprint.cc"
	$Id: CHP_footprint.cc,v 1.1.2.1 2007/09/02 20:49:27 fang Exp $
 */

#include <iostream>
#include "Object/lang/CHP_footprint.h"
// #include "Object/expr/expr_dump_context.h"
#include "util/indent.h"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
namespace CHP {
#include "util/using_ostream.h"
using util::auto_indent;
using util::write_value;
using util::read_value;

//=============================================================================
// class local_event_allocator method definitions

ostream&
local_event_footprint::dump(ostream& o, const expr_dump_context& dc) const {
if (size()) {
	o << auto_indent << "chp events: {" << endl;
	{
	INDENT_SECTION(o);
	size_t j = 0;
	const_iterator i(begin()), e(end());
	for ( ; i!=e; ++i, ++j) {
		o << auto_indent;
		i->dump_struct(o << "event[" << j << "]: ", dc);
		// includes endl
	}
	}
	o << auto_indent << "}" << endl;
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
local_event_footprint::collect_transient_info_base(
		persistent_object_manager& m) const {
	const_iterator i(begin()), e(end());
	for ( ; i!=e; ++i) {
		i->collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
local_event_footprint::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	const size_t s = size();
	write_value(o, s);
	const_iterator i(begin()), e(end());
	for ( ; i!=e; ++i) {
		i->write_object_base(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
local_event_footprint::load_object_base(
		const persistent_object_manager& m, istream& f) {
	size_t s;
	read_value(f, s);
	resize(s);
	iterator i(begin()), e(end());
	for ( ; i!=e; ++i) {
		i->load_object_base(m, f);
	}
}

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

