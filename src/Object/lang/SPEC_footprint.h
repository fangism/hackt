/**
	\file "Object/lang/SPEC_footprint.h"
	$Id: SPEC_footprint.h,v 1.2 2006/02/04 06:43:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__
#define	__HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__

#include <iosfwd>
#include <string>
#include <vector>
#include "Object/lang/SPEC_fwd.h"
#include "Object/lang/cflat_visitee.h"
#include "util/size_t.h"
#include "util/persistent_fwd.h"

namespace HAC {
namespace entity {
class footprint;
namespace SPEC {
using std::ostream;
using std::istream;
using std::string;
using util::persistent_object_manager;

//=============================================================================
/**
	A create-time resolved spec directive.  
	Modeled after PRS::footprint_macro.
 */
class footprint_directive : public PRS::cflat_visitee {
public:
	/**
		The arguments of the spec directive are represented as
		a sequence of node IDs, corresponding to the IDs 
		locally assigned during the create-phase of the 
		complete type.  
	 */
	typedef	std::vector<size_t>		args_type;
	typedef	args_type::const_reference	const_reference;
	typedef	args_type::const_iterator	const_iterator;
public:
	/// the name of the spec directive.
	string					key;
	args_type				args;
public:
	footprint_directive() : key(), args() { }

	explicit
	footprint_directive(const string& k) : key(k), args() { }

	~footprint_directive() { }

	size_t
	first_error(void) const;

	const_iterator
	begin(void) const { return args.begin(); }

	const_iterator
	end(void) const { return args.end(); }

	void
	push_back(const_reference n) { args.push_back(n); }

	void
	write_object(ostream&) const;

	void
	load_object(istream&);

	void
	accept(PRS::cflat_visitor&) const;

};	// end class footprint directive.

//=============================================================================
// could use list_vector? not anticipated to be huge...
typedef	std::vector<footprint_directive>	footprint_base_type;
//=============================================================================
/**
	This spec footprint is a signature of a complete type
	whose references are resolved.  
 */
class footprint : private footprint_base_type, public PRS::cflat_visitee {
public:
	footprint();
	~footprint();

private:
	static
	ostream&
	dump_directive(const footprint_directive&, ostream&,
		const node_pool_type&);
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
	accept(PRS::cflat_visitor&) const;

};	// end class footprint

//=============================================================================
}	// end namespace SPEC
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_SPEC_FOOTPRINT_H__

