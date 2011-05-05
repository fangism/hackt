/**
	\file "util/fig/xfig.h"
	Data structures for xfig objects.
	Reverse engineered based on documentation.
	http://www.xfig.org/userman/fig-format.html
	$Id: xfig.h,v 1.1 2011/05/05 06:50:49 fang Exp $
 */

#ifndef	__UTIL_FIG_XFIG_H__
#define	__UTIL_FIG_XFIG_H__

#include <iosfwd>
#include <vector>
#include <string>
#include "util/memory/count_ptr.h"

namespace util {
namespace fig {
namespace xfig {
using std::ostream;
using std::string;
using std::vector;
using memory::count_ptr;

//-----------------------------------------------------------------------------
enum object_code {
	XFIG_PSEUDO_OBJECT = 0,
	XFIG_ELLIPSE = 1,
	XFIG_POLYLINE = 2,
	XFIG_SPLINE = 3,
	XFIG_TEXT = 4,
	XFIG_ARC = 5,
	XFIG_COMPOUND = 6
};	// end enum object_code

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum orientation_enum {
	XFIG_LANDSCAPE = 0,
	XFIG_PORTRAIT = 1
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum doc_justification_enum {
	XFIG_DOC_CENTER = 0,
	XFIG_DOC_LEFT = 1
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum units_enum {
	XFIG_UNITS_METRIC = 0,
	XFIG_UNITS_INCHES = 1
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum multipage_enum {
	XFIG_MULTIPAGE_SINGLE = 0,
	XFIG_MULTIPAGE_MULTI = 1
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Xfig's primary colors.
 */
enum color_enum {
	XFIG_NOFILL = -1,
	XFIG_BLACK = 0,
	XFIG_BLUE = 1,
	XFIG_GREEN = 2,
	XFIG_CYAN = 3,
	XFIG_RED = 4,
	XFIG_MAGENTA = 5,
	XFIG_YELLOW = 6,
	XFIG_WHITE = 7
};	// end enum color_enum

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct point {
	int					x;
	int					y;

	point() : x(0), y(0) { }

	point(const int _x, const int _y) : x(_x), y(_y) { }

	template <class T>
	point(const T _x, const T _y) : x(int(_x)), y(int(_y)) { }

};	// end struct point

extern
ostream&
operator << (ostream&, const point&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A box is defined as lower-left, upper-right points.  
 */
struct box {
	point			ll, ur;
};	// end struct box

//-----------------------------------------------------------------------------
/**
	Abstract base class for xfig objects.
 */
class object {
public:
virtual	~object() { }

#if 0
virtual	object_code
	get_object_code(void) const = 0;
#endif

#if 0
virtual	box
	bounding_box(void) const = 0;
#endif

virtual	ostream&
	emit(ostream&) const = 0;

#if 0
virtual	void
	translate(const point&) = 0;
#endif

};	// end class xfig_object

typedef	count_ptr<object>			object_ptr;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// constants

extern
const int default_resolution;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// vector of objects?

/**
	Properties to be emitted in preamble.

	string	orientation		("Landscape" or "Portrait")
	string	justification		("Center" or "Flush Left")
	string	units			("Metric" or "Inches")
	string	papersize		("Letter", "Legal", "Ledger", "Tabloid",
					 "A", "B", "C", "D", "E",
					 "A4",   "A3", "A2", "A1", "A0" and "B5")
	float	magnification		(export and print magnification, %)
	string	multiple-page		("Single" or "Multiple" pages)
	int	transparent color	(color number for transparent color 
					for GIF export. 
					-3=background, -2=None, -1=Default,
					 0-31 for standard colors or 
					32- for user colors)
	# optional comment		(An optional set of comments may be 
					here, which are associated with the 
					whole figure)
	int	resolution coord_system	(Fig units/inch and coordinate system:
					   1: origin at lower left corner (NOT USED)
					   2: upper left)

 */
class document {
public:
	int					orientation;
	int					justification;
	int					length_unit;
	string					paper_size;
	float					magnification;
	/// {Single,Multiple}
	int					multiple_page;
	// -2
	int					transparent_color;
	string					comment;
	/// fig units/inch
	const int				resolution;
	const int				coordinate_system;
	/// 1 or 2

	document();

	ostream&
	emit(ostream&) const;
};	// end class xfig_document

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: finish me
 */
struct pseudo_object : public object {
};	// end struct pseudo_object

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
The line_style field is defined as follows:
	-1 = Default
	 0 = Solid
	 1 = Dashed
	 2 = Dotted
	 3 = Dash-dotted
	 4 = Dash-double-dotted
	 5 = Dash-triple-dotted
 */
enum line_style_enum {
	XFIG_LINE_DEFAULT = -1,
	XFIG_LINE_SOLID = 0,
	XFIG_LINE_DASHED = 1,
	XFIG_LINE_DOTTED = 2,
	XFIG_LINE_DASH_DOTTED = 3,
	XFIG_LINE_DASH_2DOTTED = 4,
	XFIG_LINE_DASH_3DOTTED = 5
};

struct pen_object : public object {
	int					pen_color;
	const int				pen_style;	// never used
	int					depth;

	pen_object();

};	// end struct pen_object

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common properties of many objects.
	All properties are publicly modifiable.
 */
struct pen_fill_object : public pen_object {
	int					line_style;
	/// in 1/80 of inches
	int					thickness;
	int					fill_color;
	int					area_fill;
	/**
		The style_val field is defined as the length, in 1/80 inches, 
		of the on/off dashes for dashed lines, and the distance 
		between the dots, in 1/80 inches, for dotted lines.
	 */
	float					style_val;

	// default ctor with default values
	pen_fill_object();

};	// end class pen_fill_object

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ellipse : public pen_fill_object {
public:
	enum sub_type_enum {
		XFIG_ELLIPSE_RADIUS = 1,
		XFIG_ELLIPSE_DIAMETER = 2,
		XFIG_CIRCLE_RADIUS = 3,
		XFIG_CIRCLE_DIAMETER = 4
	};

	sub_type_enum				sub_type;
	static const int			direction;
	float					angle;
	point					center;
	point					radius;
	point					start;
	point					end;


	ellipse();
	~ellipse();

	object_code
	get_object_code(void) const { return XFIG_ELLIPSE; }

#if 0
	sub_type_enum
	get_sub_type(void) const { return sub_type; }
#endif

	ostream&
	emit(ostream&) const;

};	// end class ellipse

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct arrow {
	enum type_enum {
		XFIG_ARROW_STICK = 0,
		XFIG_ARROW_CLOSED = 1,
		XFIG_ARROW_INDENTED = 2,
		XFIG_ARROW_POINTED = 3
	};

	enum style_enum {
		XFIG_ARROW_HOLLOW = 0,
		XFIG_ARROW_FILLED = 1
	};

	bool					on;
	int					type;
	int					style;
	float					thickness;
	float					width;
	float					height;

	arrow();

	ostream&
	emit(ostream&) const;

};	// end class arrow

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
    The join_style field is defined FOR LINES only as follows:
	0 = Miter (the default in xfig 2.1 and earlier)
	1 = Round
	2 = Bevel
 */
enum join_style_enum {
	XFIG_JOIN_MITER = 0,
	XFIG_JOIN_ROUND = 1,
	XFIG_JOIN_BEVEL = 2
};

/**
    The cap_style field is defined FOR LINES, OPEN SPLINES and ARCS 
	only as follows:
	0 = Butt (the default in xfig 2.1 and earlier)
	1 = Round
	2 = Projecting
 */
enum cap_style_enum {
	XFIG_CAP_BUTT = 0,
	XFIG_CAP_ROUND = 1,
	XFIG_CAP_PROJECTING = 2
};

/**
	Multi-segment line object.
 */
class polyline : public pen_fill_object {
public:
	enum sub_type_enum {
		XFIG_POLYLINE_POLYLINE = 1,
		XFIG_POLYLINE_BOX = 2,
		XFIG_POLYLINE_POLYGON = 3,
		XFIG_POLYLINE_ARC_BOX = 4,
		XFIG_POLYLINE_IMPORT = 5	// picture/bounding-box
	};

	sub_type_enum				sub_type;

	int					join_style;
	int					cap_style;
	// radius of arc-boxes
	int					radius;
	arrow					forward_arrow;
	arrow					backward_arrow;
	vector<point>				points;

	polyline();

	explicit
	polyline(const sub_type_enum);

	~polyline();

	object_code
	get_object_code(void) const { return XFIG_POLYLINE; }

	void
	define_segment(const point&, const point&);

	void
	define_box(const point&, const point&);

	void
	close(void);

	ostream&
	emit(ostream&) const;

};	// end class pen_fill_object

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum text_just_enum {
	XFIG_TEXT_LEFT = 0,
	XFIG_TEXT_CENTER = 1,
	XFIG_TEXT_RIGHT = 2
};


/**
    The font_flags field is defined as follows:
	 Bit	Description
	  0	Rigid text (text doesn't scale when scaling compound objects)
	  1	Special text (for LaTeX)
	  2	PostScript font (otherwise LaTeX font is used)
	  3	Hidden text
 */
enum font_flags_enum {
	XFIG_TEXT_RIGID = 0x01,
	XFIG_TEXT_SPECIAL = 0x02,
	XFIG_TEXT_POSTSCRIPT = 0x04,
	XFIG_TEXT_HIDDEN = 0x08,
	XFIG_TEXT_DEFAULT_FLAGS = 0x00
};

class text : public pen_object {
public:
	/// justification
	text_just_enum				sub_type;
	int					font;
	int					font_size;
	float					angle;
	int					font_flags;
	float					height;
	float					width;
	point					origin;
	string					contents;

	text();
	~text();

	object_code
	get_object_code(void) const { return XFIG_TEXT; }

	ostream&
	emit(ostream&) const;

};	// end class text

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct control_point : public point {
	float					shape_factor;
};	// end struct control_point

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class spline : public pen_fill_object {
public:
	int					cap_style;
	arrow					forward_arrow;
	arrow					backward_arrow;
	vector<control_point>			control_points;
};	// end class spline

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class arc : public pen_fill_object {
public:
	int					sub_type;
	int					cap_style;
	/// CW or CCW
	int					direction;
	arrow					forward_arrow;
	arrow					backward_arrow;
	float					center_x, center_y;
	point					p[3];

	arc();
};	// end class arc

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class compound : public object {
public:
	/// bounding-box
	box					bb;
	vector<object_ptr>			objects;

	compound();
	~compound();

	object_code
	get_object_code(void) const { return XFIG_COMPOUND; }

	ostream&
	emit(ostream&) const;

};	// end class compound

//-----------------------------------------------------------------------------
}	// end namespace xfig
}	// end namespace fig
}	// end namespace util

#endif	// __UTIL_FIG_XFIG_H__
