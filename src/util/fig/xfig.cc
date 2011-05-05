/**
	\file "util/fig/xfig.cc"
	$Id: xfig.cc,v 1.1 2011/05/05 06:50:49 fang Exp $
 */

#include <iostream>
#include <iterator>
#include <algorithm>
#include "util/fig/xfig.h"
#include "util/memory/count_ptr.tcc"

namespace util {
namespace fig {
namespace xfig {
#include "util/using_ostream.h"
using std::ostream_iterator;
using std::copy;

//-----------------------------------------------------------------------------
// symbol defined here
const int default_resolution = 1200;

//-----------------------------------------------------------------------------
ostream&
operator << (ostream& o, const point& p) {
	return o << p.x << ' ' << p.y;
}

//-----------------------------------------------------------------------------
document::document() :
	orientation(XFIG_PORTRAIT),
	justification(XFIG_DOC_CENTER),
	length_unit(XFIG_UNITS_INCHES),
	paper_size("Letter"),
	magnification(100.0),
	multiple_page(XFIG_MULTIPAGE_SINGLE),
	transparent_color(-2),
	comment(),
	resolution(default_resolution),
	coordinate_system(2)
{
}

static const char format_version[] = "3.2";
static const char* orientation_text[] = { "Landscape", "Portrait" };
static const char* doc_justification_text[] = { "Center", "Flush Left" };
static const char* units_text[] = { "Metric", "Inches" };
static const char* multipage_text[] = { "Single", "Multiple" };

ostream&
document::emit(ostream& o) const {
	o << "#FIG " << format_version << endl;
	o << orientation_text[orientation] << endl;
	o << doc_justification_text[justification] << endl;
	o << units_text[length_unit] << endl;
	o << paper_size << endl;
	o << magnification << endl;
	o << multipage_text[multiple_page] << endl;
	o << transparent_color << endl;
	if (comment.length()) {
		o << "# " << comment << endl;
	}
	o << resolution << ' ' << coordinate_system << endl;
	return o;
}

//-----------------------------------------------------------------------------
static const int default_depth = 50;

pen_object::pen_object() : 
	pen_color(XFIG_BLACK),
	pen_style(-1),
	depth(default_depth) {
}

//-----------------------------------------------------------------------------
pen_fill_object::pen_fill_object() : pen_object(),
		line_style(XFIG_LINE_SOLID),
		thickness(1),
		fill_color(XFIG_WHITE),
		area_fill(XFIG_NOFILL),
		style_val(4.0) {
}

//-----------------------------------------------------------------------------
arrow::arrow() : on(false),
		type(XFIG_ARROW_STICK),
		style(XFIG_ARROW_HOLLOW),
		thickness(1.0),
		width(60.0),
		height(120.0) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
arrow::emit(ostream& o) const {
	return o << type << ' ' <<
		style << ' ' <<
		thickness << ' ' <<
		width << ' ' <<
		height;
}

//-----------------------------------------------------------------------------
polyline::polyline() :
		pen_fill_object(),
		sub_type(XFIG_POLYLINE_POLYLINE),
		join_style(XFIG_JOIN_MITER),
		cap_style(XFIG_CAP_BUTT),
		radius(-1),
		forward_arrow(),
		backward_arrow(),
		points() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
polyline::~polyline() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Shortcut for defining a simple line.
 */
void
polyline::define_segment(const point& a, const point& b) {
	sub_type = XFIG_POLYLINE_POLYLINE;
	points.clear();
	points.reserve(2);
	points.push_back(a);
	points.push_back(b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Shortcut for defining a box.
 */
void
polyline::define_box(const point& ll, const point& ur) {
	sub_type = XFIG_POLYLINE_BOX;
	points.clear();
	points.reserve(5);
	points.push_back(ll);
	points.push_back(point(ll.x, ur.y));
	points.push_back(ur);
	points.push_back(point(ur.x, ll.y));
	close();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	Closes shape be setting last point to the first point.
 */
void
polyline::close(void) {
	points.push_back(points.front());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
polyline::emit(ostream& o) const {
	o << XFIG_POLYLINE << ' ' <<
		sub_type << ' ' <<
		line_style << ' ' <<
		thickness << ' ' <<
		pen_color << ' ' <<
		fill_color << ' ' <<
		depth << ' ' <<
		pen_style << ' ' <<
		area_fill << ' ' <<
		style_val << ' ' <<
		join_style << ' ' <<
		cap_style << ' ' <<
		radius << ' ' <<
		forward_arrow.on << ' ' <<
		backward_arrow.on << ' ' <<
		points.size() << endl;
	if (forward_arrow.on)
		forward_arrow.emit(o << '\t') << endl;
	if (backward_arrow.on)
		backward_arrow.emit(o << '\t') << endl;
	o << '\t';
	// TODO: limit to 5 points per newline
	copy(points.begin(), points.end(), ostream_iterator<point>(o, " "));
	o << endl;
	return o;
}

//-----------------------------------------------------------------------------
text::text() : pen_object(),
	sub_type(XFIG_TEXT_CENTER),
	font(0),
	font_size(12),
	angle(0.0),
	font_flags(XFIG_TEXT_DEFAULT_FLAGS),
	height(120),		// needs to be calculated
	width(120),		// needs to be calculated
	origin(),
	contents() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
text::~text() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
text::emit(ostream& o) const {
	return o << XFIG_TEXT << ' ' <<
		sub_type << ' ' <<
		pen_color << ' ' <<
		depth << ' ' <<
		pen_style << ' ' <<
		font << ' ' <<
		font_size << ' ' <<
		angle << ' ' <<
		font_flags << ' ' <<
		height << ' ' <<
		width << ' ' <<
		origin << ' ' <<
		// TODO: any substitutions?
		contents << "\\001" << endl;
}

//-----------------------------------------------------------------------------
compound::compound() : bb(), objects() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
compound::~compound() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
compound::emit(ostream& o) const {
	o << XFIG_COMPOUND << ' ' << 
		bb.ll.x << ' ' <<
		bb.ll.y << ' ' <<
		bb.ur.x << ' ' <<
		bb.ur.y << endl;
	vector<object_ptr>::const_iterator
		i(objects.begin()), e(objects.end());
	for ( ; i!=e; ++i) {
		if (*i) (*i)->emit(o);
	}
	return o << -XFIG_COMPOUND << endl;
}

//-----------------------------------------------------------------------------
}	// end namespace xfig
}	// end namespace fig
}	// end namespace util
