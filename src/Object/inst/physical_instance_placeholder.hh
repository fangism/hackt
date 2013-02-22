/**
	\file "Object/inst/physical_instance_placeholder.hh"
	$Id: physical_instance_placeholder.h,v 1.4 2006/11/07 06:34:55 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PHYSICAL_INSTANCE_PLACEHOLDER_H__
#define	__HAC_OBJECT_INST_PHYSICAL_INSTANCE_PLACEHOLDER_H__

#include "Object/inst/instance_placeholder_base.hh"

namespace HAC {
struct cflat_options;
namespace entity {
class port_alias_tracker;
class state_manager;
class footprint_frame;
class port_collection_context;
class physical_instance_collection;
class alias_visitor;
struct dump_flags;

//=============================================================================
/**
	Base class for physical entity placeholders, 
	as opposed to value placeholders.  
	We don't bother deriving from alias_visitee because the traversal
	is not polymorphic, yet we implement the require accept() interface.  
 */
class physical_instance_placeholder : public instance_placeholder_base {
private:
	typedef	physical_instance_placeholder	this_type;
	typedef	instance_placeholder_base	parent_type;
protected:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
protected:
	physical_instance_placeholder() : parent_type() { }

	explicit
	physical_instance_placeholder(const size_t d) : parent_type(d) { }

	physical_instance_placeholder(const parent_type::owner_ptr_raw_type& o,
		const string& n, const size_t d);

protected:
virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;

public:
virtual	~physical_instance_placeholder();

private:
	using parent_type::dump;	// don't intend to use directly

public:
	ostream&
	dump(ostream&, const dump_flags&) const;

virtual	ostream&
	dump_formal(ostream&) const = 0;

	size_t
	is_port_formal(void) const;

#define	UNROLL_PORT_ONLY_PROTO						\
	never_ptr<physical_instance_collection>				\
	unroll_port_only(const unroll_context&) const

virtual	UNROLL_PORT_ONLY_PROTO = 0;

	bool
	port_formal_equivalent(const this_type&) const;

virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;

protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class physical_instance_placeholder

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PHYSICAL_INSTANCE_PLACEHOLDER_H__

