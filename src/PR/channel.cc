/**
	\file "PR/channel.cc"
	$Id: channel.cc,v 1.1.2.2 2011/04/16 01:51:51 fang Exp $
 */

#include <iostream>
#include "PR/channel.h"
#include "util/string.h"
#include "util/IO_utils.tcc"

namespace PR {
#include "util/using_ostream.h"
using util::strings::string_to_num;
using util::write_value;
using util::read_value;

//=============================================================================
// class channel_type method definitions

/**
	Just assign some arbitrary default values.
 */
channel_type::channel_type() :
#if PL_CHANNEL_WIRES
		wires(),
#endif
		spring_coeff(1.0) {
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
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type::save_checkpoint(ostream& o) const {
	write_value(o, spring_coeff);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_type::load_checkpoint(istream& i) {
	read_value(i, spring_coeff);
	return !i;
}

//=============================================================================
// class channel_instance method definitions

channel_instance::channel_instance() :
		properties(), source(0), destination(0), tension(0.0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::channel_instance(const channel_type& t) :
		properties(t), source(0), destination(0), tension(0.0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::~channel_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
channel_instance::dump(ostream& o) const {
	o << '(' << source << ',' << destination << ") ";
	o << "tension=" << tension << ' ';
	properties.dump(o << '[') << ']';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_instance::save_checkpoint(ostream& o) const {
	write_value(o, source);
	write_value(o, destination);
	// tension should be recalculated
	properties.save_checkpoint(o);
	return !o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
channel_instance::load_checkpoint(istream& i) {
	read_value(i, source);
	read_value(i, destination);
	// tension should be recalculated
	properties.load_checkpoint(i);
	return !i;
}

//=============================================================================
}	// end namespace PR

