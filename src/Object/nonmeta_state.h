/**
	\file "Object/nonmeta_state.h"
	$Id: nonmeta_state.h,v 1.1.2.6 2007/01/13 21:06:54 fang Exp $
	Structure that contains the run-time state information of chpsim.  
 */

#ifndef	__HAC_OBJECT_NONMETA_STATE_H__
#define	__HAC_OBJECT_NONMETA_STATE_H__

#include <iosfwd>
#include <vector>

#include "Object/nonmeta_variable.h"

namespace HAC {
namespace entity {
class state_manager;
class footprint;
class bool_tag;
class int_tag;
class enum_tag;
class channel_tag;
using std::vector;
using std::ostream;
using std::istream;

//=============================================================================
/**
	Policy-based base class wrapper for simulator state.  
 */
template <class Tag>
class nonmeta_state_base {
public:
	typedef	variable_type<Tag>		implementation;
	typedef	typename implementation::type	instance_type;
	typedef	vector<instance_type>		pool_type;
protected:
	vector<instance_type>			pool;

	explicit
	nonmeta_state_base(const state_manager&);

	~nonmeta_state_base();

	void
	reset(void);

};	// end class nonmeta_state_base

//=============================================================================
/**
	The complete state of the CHPSIM simulator.  
	TODO: consider how to partition and distribute statically
	for parallel simulation, with min-cut locking.  
	TODO: think about hooks for profiling.
	TODO: how to estimate energy and delay.
 */
class nonmeta_state_manager :
	protected nonmeta_state_base<bool_tag>,
	protected nonmeta_state_base<int_tag>,
	protected nonmeta_state_base<enum_tag>,
	protected nonmeta_state_base<channel_tag> {
	typedef	nonmeta_state_manager			this_type;
	typedef	nonmeta_state_base<bool_tag>	bool_base_type;
	typedef	nonmeta_state_base<int_tag>	int_base_type;
	typedef	nonmeta_state_base<enum_tag>	enum_base_type;
	typedef	nonmeta_state_base<channel_tag>	channel_base_type;
	enum {
		FIRST_VALID_NODE = 1
			// == SIM::INVALID_NODE_INDEX +1
	};
public:
	explicit
	nonmeta_state_manager(const state_manager&);

	~nonmeta_state_manager();

	template <class Tag>
	const typename nonmeta_state_base<Tag>::pool_type&
	get_pool(void) const {
		return nonmeta_state_base<Tag>::pool;
	}

	template <class Tag>
	typename nonmeta_state_base<Tag>::pool_type&
	get_pool(void) {
		return nonmeta_state_base<Tag>::pool;
	}

	void
	initialize(void);

	void
	reset(void);

	ostream&
	dump_struct(ostream&, const state_manager&, const footprint&) const;

#if 0
	ostream&
	dump_struct_dot(ostream&) const;
#endif

	bool
	save_checkpoint(ostream&) const;

	bool
	load_checkpoint(istream&);

};	// end class State

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_NONMETA_STATE_H__

