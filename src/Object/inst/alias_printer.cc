/**
	\file "Object/inst/alias_printer.cc"
	$Id: alias_printer.cc,v 1.1.2.2 2006/04/07 22:54:29 fang Exp $
 */

#include "Object/inst/alias_printer.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"

namespace HAC {
namespace entity {
//=============================================================================
// class alias_printer method definitions

template <class Tag>
void
alias_printer::__visit(const instance_alias_info<Tag>& a) {
	a.cflat_aliases(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEFINE_INSTANCE_ALIAS_INFO_VISITOR(Tag)				\
void									\
alias_printer::visit(const instance_alias_info<Tag>& a) {		\
	__visit(a);							\
}

DEFINE_INSTANCE_ALIAS_INFO_VISITOR(bool_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(int_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(enum_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(datastruct_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(channel_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(process_tag)

#undef	DEFINE_INSTANCE_ALIAS_INFO_VISITOR

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

