/**
	\file "Object/expr/channel_probe.h"
	$Id: channel_probe.h,v 1.1.2.2 2007/02/12 04:51:22 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_CHANNEL_PROBE_H__
#define	__HAC_OBJECT_EXPR_CHANNEL_PROBE_H__

#include "Object/expr/bool_expr.h"
#include "Object/ref/references_fwd.h"
#include "util/memory/count_ptr.h"

namespace HAC {
namespace entity {
using util::persistent_object_manager;

//=============================================================================
/**
	Nonmeta expression representing the state of a channel.  
	Probe evaluates true if channel is full, i.e. is ready to receive.
 */
class channel_probe : public bool_expr {
	typedef channel_probe				this_type;
	typedef	bool_expr				parent_type;
	typedef	simple_channel_nonmeta_instance_reference
							chan_ref_type;
	typedef	count_ptr<const chan_ref_type>		chan_ref_ptr_type;
	chan_ref_ptr_type				chan;
private:
	channel_probe();
public:
	channel_probe(const chan_ref_ptr_type&);
	~channel_probe();

	const chan_ref_ptr_type&
	get_channel(void) const { return chan; }

	ostream&
	what(ostream&) const;

	ostream&
	dump(ostream&, const expr_dump_context&) const;

	size_t
	dimensions(void) const { return 0; }

	GET_UNRESOLVED_DATA_TYPE_REF_PROTO;
	GET_RESOLVED_DATA_TYPE_REF_PROTO;

	UNROLL_RESOLVE_COPY_BOOL_PROTO;
	NONMETA_RESOLVE_RVALUE_BOOL_PROTO;
	NONMETA_RESOLVE_COPY_BOOL_PROTO;
	EXPR_ACCEPT_VISITOR_PROTO;

protected:
	using parent_type::unroll_resolve_copy;
	using parent_type::nonmeta_resolve_copy;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

};	// end class channel_probe

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_CHANNEL_PROBE_H__

