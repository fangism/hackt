/**
	\file "Object/inst/connection_policy.tcc"
	$Id: connection_policy.tcc,v 1.17 2011/03/24 20:47:32 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

#include <iostream>
#include <sstream>
#include "Object/def/footprint.hh"
#include "Object/inst/connection_policy.hh"
#include "Object/inst/instance_collection.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/type/channel_direction_enum.hh"	// for direction enum
#include "Object/unroll/unroll_context.hh"
#include "Object/type/canonical_type.hh"
#include "Object/type/data_type_reference.hh"
#if	defined(ENABLE_STACKTRACE) && ENABLE_STACKTRACE
#include "Object/type/channel_type_reference_base.hh"
#endif
#include "Object/common/dump_flags.hh"		// debug only
#include "common/ICE.hh"

#include "util/stacktrace.hh"
#include "util/macros.h"
#include "util/memory/excl_ptr.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
//=============================================================================
// class bool_connect_policy method definitions

template <class AliasType>
void
bool_connect_policy::initialize_direction(const AliasType& a) {
	typedef	typename AliasType::container_type	container_type;
	typedef	container_type		collection_interface_type;
	typedef	typename collection_interface_type::traits_type
					traits_type;
	typedef	typename traits_type::tag_type		tag_type;
	typedef	instance_collection<tag_type>	instance_collection_type;
	STACKTRACE_VERBOSE;
	const container_type& p(*a.container);
	const bool f = p.is_formal();
	const instance_collection_type& c(p.get_canonical_collection());
	const direction_type d = c.get_unresolved_type_ref()
		.template is_a<const data_type_reference>()->get_direction();
	switch (d) {
	case CHANNEL_TYPE_BIDIRECTIONAL: break;
		// leave as initial value
	case CHANNEL_TYPE_RECEIVE:
		if (f) {
			attributes |= BOOL_PORT_FORMAL_INPUT;
		}
		break;
	case CHANNEL_TYPE_SEND:
		if (f) {
			attributes |= BOOL_PORT_FORMAL_OUTPUT;
		}
		break;
	default:
		ICE(cerr, cerr << "Invalid direction: " << d << endl;)
	}
#if ENABLE_STACKTRACE
	c.dump_hierarchical_name(STACKTRACE_INDENT << "collection: ", 
		dump_flags::default_value) << endl;
	STACKTRACE_INDENT_PRINT("attributes = 0x" <<
		std::hex << size_t(attributes) << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forces a flag synchronization.  
	No checking.  No error.  
 */
template <class AliasType>
void
bool_connect_policy::__update_flags(AliasType& a) {
	this->attributes = a.find()->attributes;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static connectivity property checking, regarding drive/use.  
 */
template <class AliasType>
error_count
bool_connect_policy::__check_connection(const AliasType& a) {
	// TODO: check must/must-not directions!
	error_count ret;
#if BOOL_PRS_CONNECTIVITY_CHECKING
	STACKTRACE_VERBOSE;
	const bool any_fanout_dn = a.attributes & BOOL_ANY_FANOUT_PULL_DN;
	const bool any_fanout_up = a.attributes & BOOL_ANY_FANOUT_PULL_UP;
	const bool any_fanin_dn = a.attributes & BOOL_ANY_FANIN_PULL_DN;
	const bool any_fanin_up = a.attributes & BOOL_ANY_FANIN_PULL_UP;
	const bool any_fanout = any_fanout_dn || any_fanout_up;
	const bool any_fanin = any_fanin_dn || any_fanin_up;
//	const bool dead = !any_fanout && !any_fanin;
	std::ostringstream oss;
// (!a.is_port_alias())	// wrong: misses aliases to direct ports
if (a.is_aliased_to_port()) {
#if 0
	// TODO: configurable warnings
	if (a.is_input_port()) {
		// do we care if no fanout?
		// what about globals that are passed everywhere?
		// but sometimes unused?
		// power supplies?
		if (!any_fanout) {
		}
	}
#endif
	if (a.is_output_port()) {
		// allow route-through ports to have no fanin
	if (a.is_atomic()) {
		if (!((a.attributes & BOOL_ANY_RTE_FANIN) || a.is_input_port())) {
			a.dump_hierarchical_name(oss);
			const string& n(oss.str());
			cerr << "Warning: output atomic expression for " << n <<
				" is undefined." << endl;
			++ret.warnings;
		}
	} else {
		if (!(any_fanin || a.is_input_port())) {
			// don't evaluate name unless diagnostic is printed
			a.dump_hierarchical_name(oss);
			const string& n(oss.str());
			cerr << "Warning: output port node " << n <<
				" has no PRS fanin." << endl;
			++ret.warnings;
		}
	}
	}
} else {
	// node is local-only
	// only check local non-port aliases
	const bool floating = any_fanout && !any_fanin;
	const bool asym_fanin = any_fanout && (any_fanin_dn ^ any_fanin_up);
//	const bool asym_fanout = any_fanout_dn ^ any_fanout_up;
//	const bool warn = dead || floating || asym_fanin;
	const bool warn = floating || asym_fanin;
if (warn) {
	// don't evaluate name unless diagnostic is printed
	a.dump_hierarchical_name(oss);
	const string& n(oss.str());
	// TODO: configurable warnings
#if 0
	if (!any_fanout_dn) {
		cerr << "Warning: node " << n <<
			" does not fan-out to any pull-down rules." << endl;
		++ret.warnings;
	}
	if (!any_fanout_up) {
		cerr << "Warning: node " << n <<
			" does not fan-out to any pull-up rules." << endl;
		++ret.warnings;
	}
	if (!any_fanin_dn) {
		cerr << "Warning: node " << n <<
			" has no pull-up fan-ins." << endl;
		++ret.warnings;
	}
	if (!any_fanin_up) {
		cerr << "Warning: node " << n <<
			" has no pull-down fan-ins." << endl;
		++ret.warnings;
	}
#else
#if 0
	if (dead) {
		cerr << "Warning: unused node " << n <<
			" has neither PRS fanin nor fanout." << endl;
		++ret.warnings;
	}
#endif
	if (floating) {
		cerr << "Warning: node " << n <<
			" has no pull-up nor pull-dn PRS fanin." << endl;
		++ret.warnings;
	}
	if (asym_fanin) {
		if (any_fanin_dn) {
			cerr << "Warning: node " << n <<
				" has no PRS pull-up fanin." << endl;
			++ret.warnings;
		} else {
			cerr << "Warning: node " << n <<
				" has no PRS pull-dn fanin." << endl;
			++ret.warnings;
		}
	}
#endif
	// never error out for now
}
}
#endif
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
void
bool_connect_policy::connection_flag_setter::operator () (AliasType& a) {
	// important that this is done with the *canoncical* node
	if (!a.find()->set_connection_flags(update).good) {
		status.good = false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
bool
bool_connect_policy::matched_flags(const AliasType& r) const {
	const AliasType& l(AS_A(const AliasType&, *this));
	if (attributes != r.attributes) {
		cerr << "ll.flags = 0x" <<
			std::hex << size_t(attributes) << endl;
		cerr << "rr.flags = 0x" <<
			std::hex << size_t(r.attributes) << endl;
		l.dump_hierarchical_name(cerr << "l: ") << endl;
		r.dump_hierarchical_name(cerr << "r: ") << endl;
		return false;
	}
	return true;
}

//=============================================================================
// class channel_connect_policy method definitions

/**
	Forces a flag synchronization.  
	No checking.  No error.  
 */
template <class AliasType>
void
channel_connect_policy::__update_flags(AliasType& a) {
	this->direction_flags = a.find()->direction_flags;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Issue an error/diagnostic if:
	both canonical node are already connected to a producer
	TODO: precise direction connection checking, w.r.t CHP,
		meta-references and nonmeta-references.
 */
template <class AliasType>
good_bool
channel_connect_policy::synchronize_flags(
		AliasType& l, AliasType& r) {
	typedef	typename AliasType::traits_type		traits_type;
	STACKTRACE_VERBOSE;
	if (&l == &r) {
		// permit self-aliases, of course
		return good_bool(true);
	}
	this_type& ll(l);	// static_cast
	this_type& rr(r);	// static_cast
	const connection_flags_type& lld(ll.direction_flags);
	const connection_flags_type& rrd(rr.direction_flags);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("ll.flags = 0x" <<
		std::hex << size_t(lld) << endl);
	STACKTRACE_INDENT_PRINT("rr.flags = 0x" <<
		std::hex << size_t(rrd) << endl);
	l.dump_hierarchical_name(STACKTRACE_INDENT << "l: ") << endl;
	r.dump_hierarchical_name(STACKTRACE_INDENT << "r: ") << endl;
#endif
	// convenience aliases
	const connection_flags_type _and = lld & rrd;
	const connection_flags_type _or = lld | rrd;
	bool good = true;
	if ((lld & CONNECTED_TO_ANY_PRODUCER) &&
			(rrd & CONNECTED_TO_ANY_PRODUCER))
	{
		// multiple producers
		// TODO: strengthen condition?
		// shared, but connection must also be consistent?
		if (!(_and & CONNECTED_PRODUCER_IS_SHARED)) {
			// at least one of them not sharing
			cerr << "Error: cannot alias two "
				"non-sharing producers of type " <<
				traits_type::tag_name << "." << endl;
			good = false;
		}
	}
	if ((lld & CONNECTED_TO_ANY_CONSUMER) &&
			(rrd & CONNECTED_TO_ANY_CONSUMER))
	{
		// multiple consumers
		if (!(_and & CONNECTED_CONSUMER_IS_SHARED)) {
			// at least one of them not sharing
			cerr << "Error: cannot alias two "
				"non-sharing consumers of type " <<
				traits_type::tag_name << "." << endl;
			good = false;
		}
	}
	if (!check_meta_nonmeta_usage(_or, traits_type::tag_name).good) {
		// already have error message
		good = false;
	}
	if (!good) {
		l.dump_hierarchical_name(cerr << "\tgot: ") << endl;
		r.dump_hierarchical_name(cerr << "\tand: ") << endl;
	} else {
		STACKTRACE_INDENT_PRINT("new flags = " << size_t(_or) << endl);
		ll.direction_flags = _or;
		rr.direction_flags = _or;
	}
	return good_bool(good);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: propagate local connection information to external, 
		bottom-up, formal to actual.  (done?)
	Cannot error, as this only initializes direction flags.  
	Policy: we assume port_formals are connected to their environment
		to suppress unconnected diagnostics.  
		However, port actual initializations are left up to
		an implementation switch: 
		PROPAGATE_CHANNEL_CONNECTIONS_HIERARCHICALLY
		if enabled, we don't initialize actuals with their directions, 
		we infer them from their port summaries.  
	Ports marked as shared not need their aliases flagged as shared
		because they should 
	NOTE: called by instance_alias_info::instantiate().
	optimization: refactor code to avoid repeated calls to same container.
	TODO: what if channel is a member of a port/struct, and direction
		is only annotated locally, inside the definition body, e.g.
		foo.mychan?;
		Should the language allow this b/c it is inconveinent to
		specify the direction of a port member?
 */
template <class AliasType>
void
channel_connect_policy::initialize_direction(const AliasType& a) {
	typedef	typename AliasType::container_type	container_type;
	typedef	container_type		collection_interface_type;
	typedef	typename collection_interface_type::traits_type
					traits_type;
	typedef	typename traits_type::tag_type		tag_type;
	typedef	instance_collection<tag_type>	instance_collection_type;
	STACKTRACE_VERBOSE;
	const container_type& p(*a.container);
	const bool f = p.is_formal();
	const instance_collection_type& c(p.get_canonical_collection());
	const direction_type d = c.__get_raw_type().get_direction();
	switch (d) {
	case CHANNEL_TYPE_BIDIRECTIONAL: break;
		// leave as initial value
#if ENABLE_SHARED_CHANNELS
	case CHANNEL_TYPE_RECEIVE_SHARED:
		if (f) {
			direction_flags |= CONNECTED_PRODUCER_IS_SHARED;
			direction_flags |= CONNECTED_PORT_FORMAL_PRODUCER;
		} else {
			direction_flags |= CONNECTED_CONSUMER_IS_SHARED;
		}
		break;
#endif
	case CHANNEL_TYPE_RECEIVE:
		if (f) {
			direction_flags |= CONNECTED_PORT_FORMAL_PRODUCER;
		}
		break;
#if ENABLE_SHARED_CHANNELS
	case CHANNEL_TYPE_SEND_SHARED:
		if (f) {
			direction_flags |= CONNECTED_CONSUMER_IS_SHARED;
			direction_flags |= CONNECTED_PORT_FORMAL_CONSUMER;
		} else {
			direction_flags |= CONNECTED_PRODUCER_IS_SHARED;
		}
		break;
#endif
	case CHANNEL_TYPE_SEND:
		if (f) {
			direction_flags |= CONNECTED_PORT_FORMAL_CONSUMER;
		}
		break;
	default:
		ICE(cerr, cerr << "Invalid direction: " << d << endl;)
	}
#if ENABLE_STACKTRACE
	c.dump_hierarchical_name(STACKTRACE_INDENT << "collection: ", 
		dump_flags::default_value) << endl;
	STACKTRACE_INDENT_PRINT("direction_flags = 0x" <<
		std::hex << size_t(direction_flags) << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variation initializes actuals' aliases (from subinstances)
	based on how they were connected in their corresponding
	formal definition collections.  
	This unmasks the bits that were set in the formal collection
		to prevent connection in the wrong direction.  
		Unmasking bits lets us check hierarchically whether or not
		channels WERE connected properly.  
	TODO: refactor to avoid repeated redundant calls.  
	\param a the corresponding alias in the formal collection.  
 */
template <class AliasType>
void
channel_connect_policy::initialize_actual_direction(
		const AliasType& a) {
	typedef	typename AliasType::container_type	collection_interface_type;
	typedef	typename collection_interface_type::traits_type
					traits_type;
	typedef	typename traits_type::tag_type		tag_type;
	typedef	instance_collection<tag_type>	instance_collection_type;
	STACKTRACE_VERBOSE;
	// const bool f = p.is_formal();
	// const instance_collection_type& c(p.get_canonical_collection());
	const instance_collection_type&
		c(a.container->get_canonical_collection());
	const direction_type d = c.__get_raw_type().get_direction();
	// with bit fields, could just twiddle the consumer/producer halves...
	direction_flags = a.direction_flags & ~ACTUAL_INITIALIZATION_MASK;
	if (a.direction_flags & CONNECTED_TO_NONPORT_PRODUCER)
		direction_flags |= CONNECTED_TO_SUBSTRUCT_PRODUCER;
	if (a.direction_flags & CONNECTED_TO_NONPORT_CONSUMER)
		direction_flags |= CONNECTED_TO_SUBSTRUCT_CONSUMER;
#if ENABLE_SHARED_CHANNELS
	switch (d) {
	case CHANNEL_TYPE_RECEIVE_SHARED:
#if 0
		// note: this clears out the META flag as well
		direction_flags =
			(a.direction_flags &
				~(CONNECTED_PORT_FORMAL_PRODUCER | CONNECTED_PRODUCER_IS_SHARED));
#endif
		if (a.direction_flags & CONNECTED_TO_NONPORT_CONSUMER) {
//			direction_flags |= CONNECTED_TO_SUBSTRUCT_CONSUMER;
			direction_flags |= CONNECTED_CONSUMER_IS_SHARED;
		}
		break;
	case CHANNEL_TYPE_SEND_SHARED:
#if 0
		// note: this clears out the META flag as well
		direction_flags =
			(a.direction_flags &
				~(CONNECTED_PORT_FORMAL_CONSUMER | CONNECTED_CONSUMER_IS_SHARED));
#endif
		if (a.direction_flags & CONNECTED_TO_NONPORT_PRODUCER) {
//			direction_flags |= CONNECTED_TO_SUBSTRUCT_PRODUCER;
			direction_flags |= CONNECTED_PRODUCER_IS_SHARED;
		}
		break;
#endif
	default:
		break;
//		ICE(cerr, cerr << "Invalid direction: " << d << endl;)
	}	// end switch
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("a.direction_flags = 0x" <<
		std::hex << size_t(a.direction_flags) << endl);
	c.dump_hierarchical_name(STACKTRACE_INDENT << "collection: ",
		dump_flags::default_value) << endl;
	STACKTRACE_INDENT_PRINT("direction_flags = 0x" <<
		std::hex << size_t(direction_flags) << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Issues diagnostic for dangling channel connections.  
	Could just make this a non-template function by passing
		the tag_name as a string.  
	TODO: finer diagnostic control to determine whether or not
		to promote warning to error.  
 */
template <class AliasType>
error_count
channel_connect_policy::__check_connection(const AliasType& a) {
	typedef	typename AliasType::traits_type		traits_type;
	STACKTRACE_VERBOSE;
	error_count ret;
	const connection_flags_type f = a.direction_flags;
	if (!(f & CONNECTED_TO_ANY_PRODUCER)) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a producer." << endl;
		++ret.warnings;
	}
	if (!(f & CONNECTED_TO_ANY_CONSUMER)) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a consumer." << endl;
		++ret.warnings;
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor's forwarding operator.  
	Sets flags of each element's canonical alias, 
	while checking for errors.
 */
template <class AliasType>
void
channel_connect_policy::connection_flag_setter::operator () (
		AliasType& a) {
	// important that this is done with the *canoncical* node
	if (!a.find()->set_connection_flags(update).good) {
		status.good = false;
	}
}

//=============================================================================
// class process_connect_policy method definitions

/**
	Take footprint of this type, from context and apply to subinstances.  
	This should be called upon instantiation, and also upon
	type completion, the case of relaxed actuals.  
	This could potentially be used to replay internal aliases, 
	which is currently managed by port_alias_tracker.  
	\param AliasType must be a substructured type, like process.
	TODO: calling type-completion for every alias that is 
		instantiated seems wasteful, especially if part of
		a strictly typed array... rewrite me.
 */
template <class AliasType>
void
process_connect_policy::initialize_direction(AliasType& a) {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	a.dump_hierarchical_name(cerr << "alias: ") << endl;
	a.dump_ports(cerr, dump_flags::default_value) << endl;
#endif
	// check to make sure type is complete (no longer relaxed)
	// lookup resolved type of the alias or its container
	// a.complete_type_actuals(*a.container); from "alias_actuals.tcc"

	typedef	typename AliasType::container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
						complete_type_type;
	typedef	typename complete_type_type::canonical_definition_type
						canonical_definition_type;
	// ordering issue: cannot complete type actuals if collection
	// was declared relaxed, so this work will have to be punted
if (a.has_complete_type()) {
	const complete_type_type
		_type(a.complete_type_actuals(*a.container));
	INVARIANT(_type);
	// acquire the (already created) footprint
	const footprint&
		f(_type.get_base_def()->get_footprint(
			_type.get_raw_template_params()));
	// walk ports and apply flags from the local unique instance pool
	const port_alias_tracker& pt(f.get_scope_alias_tracker());
	// b/c port_alias_tracker only lists ports with *aliases*
	// copy-initialize: alias actuals and attributes and flags
#if ENABLE_STACKTRACE
	f.dump_type(STACKTRACE_INDENT_PRINT("instantiating type: ")) << endl;
	pt.dump(STACKTRACE_INDENT_PRINT("scope_alias_tracker: ") << endl);
#endif
	pt.export_alias_properties(a);
	// TODO: use footprint.port_alias_tracker::replay_internal_aliases
	// TODO: should connections occur before or after copying properties?
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("after inheriting alias properties: " << endl);
	a.dump_ports(cerr, dump_flags::default_value) << endl;
#endif
	// now apply declared port directions, type need not be complete!
	const container_type& p(*a.container);
	const bool formal = p.is_formal();
	const direction_type d = _type.get_direction();
	switch (d) {
	case PROCESS_DIRECTION_DEFAULT: break;
		// leave as initial value
	case CHANNEL_TYPE_RECEIVE:
		if (formal) {
			direction_flags |= PROCESS_PORT_FORMAL_INPUT;
		}
		break;
	case CHANNEL_TYPE_SEND:
		if (formal) {
			direction_flags |= PROCESS_PORT_FORMAL_OUTPUT;
		}
		break;
	default:
		ICE(cerr, cerr << "Invalid direction: " << d << endl;)
	}	// end switch
	// else type is relaxed, skip this until type is complete
}
}	// end process_connect_policy::initialize_direction

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initialize flags based on attributes inherited from substructure.  
	TODO: this fixed the wire-buf false-positive, should be applied
		to channel_connect_policy above.
 */
template <class AliasType>
void
process_connect_policy::initialize_actual_direction(const AliasType& a) {
	STACKTRACE_VERBOSE;
	if (a.direction_flags & CONNECTED_TO_NONPORT_PRODUCER)
		direction_flags |= CONNECTED_TO_SUBSTRUCT_PRODUCER;
	if (a.direction_flags & CONNECTED_TO_NONPORT_CONSUMER)
		direction_flags |= CONNECTED_TO_SUBSTRUCT_CONSUMER;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("a.direction_flags = 0x" <<
		std::hex << size_t(a.direction_flags) << endl);
	a.dump_hierarchical_name(STACKTRACE_INDENT << "collection: ",
		dump_flags::default_value) << endl;
	STACKTRACE_INDENT_PRINT("direction_flags = 0x" <<
		std::hex << size_t(direction_flags) << endl);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For user-defined channels only.
	Diagnostic if channel is only connected to produce
	or a consumer exclusively.  
	Channels should check for single-source/single-destination
	connectivity.
	Datatypes need not have any consumer-destination.
	Most users may not bother with any directional annotations, 
	so those should be forgiven.  
 */
template <class AliasType>
error_count
process_connect_policy::__check_connection(const AliasType& a) {
	typedef	typename AliasType::traits_type		traits_type;
	STACKTRACE_VERBOSE;
	error_count ret;
// if (!a.is_aliased_to_port()) {
	const connection_flags_type f = a.direction_flags;
// if is user-defined channel
#if 0
	const bool local_prod = (f & CONNECTED_TO_NONPORT_PRODUCER);
	const bool local_cons = (f & CONNECTED_TO_NONPORT_CONSUMER);
#endif
	const bool have_prod = (f & CONNECTED_TO_ANY_PRODUCER);
	const bool have_cons = (f & CONNECTED_TO_ANY_CONSUMER);
	// TODO: tag_name should reflect actualy meta-type
	if (have_cons && !have_prod) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a producer." << endl;
		++ret.warnings;
	}
	if (have_prod && !have_cons) {
		typedef	typename AliasType::container_type
							container_type;
		typedef	typename traits_type::tag_type	tag_type;
		typedef	instance_collection<tag_type>
						instance_collection_type;
		const container_type& p(*a.container);
		const instance_collection_type& c(p.get_canonical_collection());
		const meta_type_tag_enum
			t(c.__get_raw_type().get_base_def()->get_meta_type());
		if (t == META_TYPE_CHANNEL) {
		a.dump_hierarchical_name(
			cerr << "WARNING: " << traits_type::tag_name << " ")
			<< " lacks connection to a consumer." << endl;
		++ret.warnings;
		}
		// else structs are allowed to have no consumer
	}
	// if (!have_prod && !have_cons) // we don't care
	// or better yet check the type, whether is actually user-defined chan
// end if
// }	// !is_aliased_to_port()
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
void
process_connect_policy::connection_flag_setter::operator () (
		AliasType& a) {
	// important that this is done with the *canoncical* node
	if (!a.find()->set_connection_flags(update).good) {
		status.good = false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
void
process_connect_policy::__update_flags(AliasType& a) {
	this->direction_flags = a.find()->direction_flags;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
bool
process_connect_policy::matched_flags(const AliasType& r) const {
	const AliasType& l(AS_A(const AliasType&, *this));
	if (direction_flags != r.direction_flags) {
		cerr << "ll.flags = 0x" <<
			std::hex << size_t(direction_flags) << endl;
		cerr << "rr.flags = 0x" <<
			std::hex << size_t(r.direction_flags) << endl;
		l.dump_hierarchical_name(cerr << "l: ") << endl;
		r.dump_hierarchical_name(cerr << "r: ") << endl;
		return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
good_bool
process_connect_policy::synchronize_flags(
		AliasType& l, AliasType& r) {
	typedef	typename AliasType::traits_type		traits_type;
	typedef	typename AliasType::container_type
						container_type;
	typedef	typename traits_type::tag_type		tag_type;
	typedef	instance_collection<tag_type>	instance_collection_type;
	STACKTRACE_VERBOSE;
	if (&l == &r) {
		// permit self-aliases, of course
		return good_bool(true);
	}
	this_type& ll(l);	// static_cast
	this_type& rr(r);	// static_cast
	const connection_flags_type& lld(ll.direction_flags);
	const connection_flags_type& rrd(rr.direction_flags);
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("ll.flags = 0x" <<
		std::hex << size_t(lld) << endl);
	STACKTRACE_INDENT_PRINT("rr.flags = 0x" <<
		std::hex << size_t(rrd) << endl);
	l.dump_hierarchical_name(STACKTRACE_INDENT << "l: ") << endl;
	r.dump_hierarchical_name(STACKTRACE_INDENT << "r: ") << endl;
#endif
	// convenience aliases
#if 0
	const connection_flags_type _and = lld & rrd;
#endif
	const connection_flags_type _or = lld | rrd;
	bool good = true;
	if ((lld & CONNECTED_TO_ANY_PRODUCER) &&
			(rrd & CONNECTED_TO_ANY_PRODUCER))
	{
		// multiple producers
		// TODO: strengthen condition?
		// shared, but connection must also be consistent?
		// applies to all meta-types
#if 0
		if (!(_and & CONNECTED_PRODUCER_IS_SHARED)) {
			// at least one of them not sharing
#endif
			cerr << "Error: cannot alias two "
				"non-sharing producers of type " <<
				traits_type::tag_name << "." << endl;
			good = false;
#if 0
		}
#endif
	}
	if ((lld & CONNECTED_TO_ANY_CONSUMER) &&
			(rrd & CONNECTED_TO_ANY_CONSUMER))
	{
		// multiple consumers
		// allow datatypes to always share consumers
		// but not channels
		const container_type& p(*l.container);
		const instance_collection_type& c(p.get_canonical_collection());
		const meta_type_tag_enum
			t(c.__get_raw_type().get_base_def()->get_meta_type());
		if (t != META_TYPE_STRUCT) {
#if 0
		if (!(_and & CONNECTED_CONSUMER_IS_SHARED)) {
			// at least one of them not sharing
#endif
			cerr << "Error: cannot alias two "
				"non-sharing consumers of type " <<
				traits_type::tag_name << "." << endl;
			good = false;
#if 0
		}
#endif
		}
	}
#if 0
	if (!check_meta_nonmeta_usage(_or, traits_type::tag_name).good) {
		// already have error message
		good = false;
	}
#endif
	if (!good) {
		l.dump_hierarchical_name(cerr << "\tgot: ") << endl;
		r.dump_hierarchical_name(cerr << "\tand: ") << endl;
	} else {
		STACKTRACE_INDENT_PRINT("new flags = " << size_t(_or) << endl);
		ll.direction_flags = _or;
		rr.direction_flags = _or;
	}
	return good_bool(good);
}	// end process_connect_policy::synchronize_flags


//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_TCC__

