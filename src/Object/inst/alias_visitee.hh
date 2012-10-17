/**
	\file "Object/inst/alias_visitee.hh"
	Base class interface for classes visitable by alias_visitor.
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_VISITEE_H__
#define	__HAC_OBJECT_INST_ALIAS_VISITEE_H__

#include "util/visitee.hh"

namespace HAC {
namespace entity {
class alias_visitor;

typedef	util::visitee_const<alias_visitor, void>	alias_visitee;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_VISITEE_H__

