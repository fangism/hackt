/**
	\file "main/create_options.hh"
	Unroll-time behavior options.
	These are considered compile-time options, as they are
	set during initial compilation, and not set
	during the create phase. 
	This allows auto-create and auto-compile to work properly.  
	$Id: create_options.hh,v 1.1 2010/08/05 18:25:35 fang Exp $
 */

#ifndef	__HAC_MAIN_CREATE_OPTIONS_H__
#define	__HAC_MAIN_CREATE_OPTIONS_H__

#include <iosfwd>
// #include "util/IO_utils_fwd.hh"

//-----------------------------------------------------------------------------
namespace HAC {
using std::ostream;
using std::istream;

/**
	Select method of canonicalizing preferred alias.  
 */
enum canonicalize_policy {
	SHORTEST_HIER_NO_LENGTH,
	SHORTEST_HIER_MIN_LENGTH,
	SHORTEST_EMULATE_ACT	///< TODO: unimplemented, emulate ACT
	// see "act/connect.c":_pick_shortest_id for reference
};

// should be able to de/serialize these objects
struct create_options {
	/// determines how aliases are canonicalized
	canonicalize_policy		canonicalize_mode;

	/// deafult ctor, default modes
	create_options() :
		canonicalize_mode(SHORTEST_HIER_NO_LENGTH)
//		canonicalize_mode(SHORTEST_HIER_MIN_LENGTH)
		{ }

	ostream&
	dump(ostream&) const;

	void
	write_object(ostream&) const;

	void
	load_object(istream&);

};	// end struct create_options

extern
create_options
global_create_options;

}	// end namespace HAC

//-----------------------------------------------------------------------------
#if 0
// specialize serializer for options class
namespace util {
using std::ostream;
using std::istream;
using HAC::create_options;

template
void write_value(ostream&, const create_options&);
template
void read_value(istream&, create_options&);
}	// end namespace util
#endif

//-----------------------------------------------------------------------------

#endif	// __HAC_MAIN_CREATE_OPTIONS_H__
