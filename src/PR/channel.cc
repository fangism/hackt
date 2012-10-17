/**
	\file "PR/channel.cc"
	$Id: channel.cc,v 1.3 2011/05/05 06:50:45 fang Exp $
 */

#include <iostream>
#include "PR/channel.hh"
#include "PR/tile_instance.hh"
#include "PR/placer_options.hh"
#include "util/string.hh"
#include "util/fig/xfig.hh"
#include "util/IO_utils.tcc"

namespace HAC {
namespace PR {
#include "util/using_ostream.hh"
using util::strings::string_to_num;
using util::write_value;
using util::read_value;
using util::fig::xfig::polyline;
using util::fig::xfig::point;
using util::fig::xfig::default_resolution;

//=============================================================================
// class channel_type method definitions

/**
	Just assign some arbitrary default values.
 */
channel_type::channel_type() :
#if PL_CHANNEL_WIRES
		wires(),
#endif
		spring_coeff(1.0),
		equilibrium_distance(0.0)	// garbage, to be calculated
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_type::~channel_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use property function map,
		see hacknet::netlist_options implementation
	\param s is a string of the form "KEY=VALUE"
	\return true on error
 */
bool
channel_type::parse_property(const string& s) {
	return parse_property(optparse(s));
}

bool
channel_type::parse_property(const option_value& o) {
	if (o.key == "spring_coeff") {
		if (o.values.empty()) {
			cerr << "channel_type.property.parse: missing value"
				<< endl;
			// or print current value
			return true;
		}
		if (string_to_num(o.values.front(), spring_coeff)) {
			cerr << "channel_type.property.parse: bad vector value"
				<< endl;
			return true;
		}
		return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_type::dump(ostream& o) const {
	o << "spring_coeff=" << spring_coeff;
	o << " equilibrium=" << equilibrium_distance << ' ';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type::save_checkpoint(ostream& o) const {
	write_value(o, spring_coeff);
	write_value(o, equilibrium_distance);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type::load_checkpoint(istream& i) {
	read_value(i, spring_coeff);
	read_value(i, equilibrium_distance);
	return !i;
}

//=============================================================================
// class channel_state method definitions

bool
channel_state::save_checkpoint(ostream& o) const {
	write_value(o, potential_energy);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_state::load_checkpoint(istream& i) {
	read_value(i, potential_energy);
	return !i;
}

//=============================================================================
// class channel_instance method definitions

channel_instance::channel_instance() :
		properties(), source(0), destination(0)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::channel_instance(const size_t s, const size_t d) :
		properties(), source(s), destination(d)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::channel_instance(const channel_type& t) :
		properties(t), source(0), destination(0)
		{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::~channel_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instance::dump(ostream& o) const {
	o << '(' << source << ',' << destination << ") ";
	properties.dump(o << '[') << ']';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instance::emit_dot(ostream& o, const placer_options&) const {
	// leave edges as drawn automatically
	return o << "N" << source << " -> N" << destination
		<< " [weight=" << properties.spring_coeff << "];";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instance::emit_fig(ostream& o, const position_type& s,
		const position_type& d, const placer_options& opt) const {
	polyline p;
	p.depth = 150;
	const real_type r = opt.xfig_scale *default_resolution;
	p.define_segment(point(s[0]*r, s[1]*r), point(d[0]*r, d[1]*r));
	return p.emit(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_instance::save_checkpoint(ostream& o) const {
	write_value(o, source);
	write_value(o, destination);
	// equilibrium_distance could be recalculated
	properties.save_checkpoint(o);
	channel_state::save_checkpoint(o);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_instance::load_checkpoint(istream& i) {
	read_value(i, source);
	read_value(i, destination);
	// equilibrium_distance could be recalculated
	properties.load_checkpoint(i);
	channel_state::load_checkpoint(i);
	return !i;
}

//=============================================================================
}	// end namespace PR
}	// end namespace HAC

