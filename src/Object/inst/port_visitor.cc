/**
	\file "Object/inst/port_visitor.cc"
	$Id: port_visitor.cc,v 1.1 2009/02/28 01:20:42 fang Exp $
 */

#include "Object/inst/port_visitor.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/traits/instance_traits.h"

namespace HAC {
namespace entity {
using std::vector;

//=============================================================================
// class port_visitor method definitions

// meta-types with substructure should recurse by default
// terminal meta-type will do nothing by default

#define	DEFINE_TERMINAL_VISIT(Tag)					\
void port_visitor::visit(const instance_alias_info<Tag>& a) { }

#define	DEFINE_NONTERMINAL_VISIT(Tag)					\
void									\
port_visitor::visit(const instance_alias_info<Tag>& a) {		\
	const subinstance_manager& s(a);				\
	s.accept(*this);						\
}

DEFINE_TERMINAL_VISIT(bool_tag)
DEFINE_TERMINAL_VISIT(int_tag)
DEFINE_TERMINAL_VISIT(enum_tag)
#if BUILTIN_CHANNEL_FOOTPRINTS
DEFINE_TERMINAL_VISIT(channel_tag)
#else
DEFINE_NONTERMINAL_VISIT(channel_tag)
#endif

DEFINE_NONTERMINAL_VISIT(process_tag)

#undef	DEFINE_TERMINAL_VISIT
#undef	DEFINE_NONTERMINAL_VISIT

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

