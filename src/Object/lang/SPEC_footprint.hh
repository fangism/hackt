/**
	\file "Object/lang/SPEC_footprint.hh"
	$Id: SPEC_footprint.hh,v 1.7 2010/07/14 18:12:33 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__

#include "Object/lang/directive_base.hh"
#include "Object/lang/SPEC_fwd.hh"
#include "Object/lang/cflat_visitee.hh"
#include "util/persistent_fwd.hh"
#include "util/memory/count_ptr.hh"

namespace HAC {
namespace entity {
class footprint;
struct global_entry_context;
namespace SPEC {
typedef	PRS::cflat_visitor		spec_visitor;

//=============================================================================
/**
	A create-time resolved spec directive.  
	Modeled after PRS::footprint_macro.
 */
class footprint_directive : 
		public PRS::cflat_visitee, 
		public bool_directive_base {
public:
	footprint_directive() : bool_directive_base() { }

	explicit
	footprint_directive(const string& k) : bool_directive_base(k) { }

	// everything else inherited from directive_base

	void
	accept(spec_visitor&) const;

};	// end class footprint directive.

//=============================================================================
// could use list_vector? not anticipated to be huge...
typedef	std::vector<footprint_directive>	footprint_base_type;
//=============================================================================
/**
	This spec footprint is a signature of a complete type
	whose references are resolved.  
 */
class footprint : private footprint_base_type
		, public PRS::cflat_visitee
		{
public:
	footprint();
	~footprint();

private:
	static
	ostream&
	dump_directive(const footprint_directive&, ostream&,
		const entity::footprint&);
public:

	ostream&
	dump(ostream&, const entity::footprint&) const;

	using footprint_base_type::begin;
	using footprint_base_type::end;

	footprint_directive&
	push_back_directive(const string&);

	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

	void
	accept(spec_visitor&) const;

};	// end class footprint

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__

