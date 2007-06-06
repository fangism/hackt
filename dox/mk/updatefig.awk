#!/usr/bin/awk -f
#	$Id: updatefig.awk,v 1.1.2.1 2007/06/06 05:02:20 fang Exp $

# "mk/updatefig.awk'
# Fang's *crude* fig hack
# stream-edits fig objects of all types
# works whether or not input is stripped of header, 
# output will include header if input had header
# match_* arguments can be strings or ints
# update_* arguments can be strings or ints

# DOCUMENTATION:
#	scattered throughout this file...

# global unconditional transforms:
# update_global_dx, _dy -- Cartesian translation

# other useful variables to pass in:
# suppress -- if set to anything, will delete the matching objects

BEGIN {
	done_header = 0;
}

/#FIG/ {
	# eventually merge headers....
	# parse and echo out header
	if (!done_header) print;
	for (i=1; i<9; i++) {
		# header line count = 9
		getline;
		# but may contain additional comments
		if (match($0, "^#")) --i;
		if (!done_header) print;
	}
	done_header = 1;
}

# for all objects
/^0 / { parse_pseudo_object($0);	}

# could use parse_fig_object()...
/^1 / { parse_ellipse($0);		}
/^2 / { parse_polyline($0);		}
/^3 / { parse_spline($0);		}
/^4 / { parse_text($0);			}
/^5 / { parse_arc($0);			}
/^6 / { parse_compound($0);		}

!/^[0-6] / {
if (0) {
	# ignore everything else
	print "# unknown object?";
}}

######################## utility functions ####################################

# returns true if no match string is specified
function null_or_match(src, tgt) {
	return (src == "" || src == tgt);
}

# range is in format: [min,max], where either min/max are optional
# also accepts just a plain number for exact match
# optional '!' to prefix means the inverse
# be sure to compare numerical values, not string values!
function null_or_range_match(src, tgt, 
	# local vars
	src_copy, min, max, inv, lb, cm, rb, ret ) {
#	print "# null_or_range_match(" src "," tgt ")";
	src_copy = src;
	if (src == "")	return 1;
	# extract inclusive bounds
	inv = match(src,"[!]");
	lb = match(src,"[[]");
	cm = match(src,",");
	rb = match(src,"[]]");
	if (lb && cm && rb) {
		min = substr(src, lb+1, cm-lb-1);
		max = substr(src, cm+1, rb-cm-1);
	} else if (!lb && !cm && !rb) {
		gsub("[!]", "", src_copy);
		min = src_copy;
		max = src_copy;
	} else {
		print "# updatefig.awk: bad range argument.";
		exit 1;
	}
#	print "# MIN = " min;
#	print "# MAX = " max;
	ret = ((min == "" || min+0 <= tgt+0)&&(max == "" || max+0 >= tgt+0));
#	print "# return " ret;
	if (inv) return !ret;
	else return ret;
}

function null_or_regexp_match(src, tgt) {
	return (src == "" || match(tgt, src));
}

function update_field(fmt_str, oldval, update_str) {
	printf(fmt_str, (update_str == "") ? oldval : update_str);
}

# relative change in integer value
function update_field_int_rel(fmt_str, oldval, update_diff) {
	printf(fmt_str, (update_diff == "") ? oldval : oldval+update_diff);
}

# cartesian transformation
# xin, yin -- input coordinates
# origin -- origin/center of transformation of the form (x,y)
# matrix -- transformation of the form [[xx,xy],[yx,yy]] (no spaces)
# dx, dy -- post transformation offset
# prints a fixed format-string "%d %d"
function update_xy_transform(xin, yin, origin, matrix, dx, dy, 
	# local vars
	ox, oy, tx, ty, xout, yout, xx, xy, yx, yy, lp, cmx, mb, cmy, rp) {
	if (origin != "") {
		lp = match(origin,"[(]");
		cmx = match(origin,",");
		rp = match(origin,"[)]");
		if (lp && cmx && rp) {
			ox = substr(origin, lp+1, cmx-lp-1) +0;
			oy = substr(origin, cmx+1, rp-cmx-1) +0;
#			print "# origin = " ox " " oy;
		} else {
			print "# updatefig.awk: bad origin argument.";
			exit 1;
		}
	} else {
		ox = 0;		oy = 0;
	}
	if (matrix != "") {
		# dangerous parsing here
		lp = match(matrix,"[[][[]");
		cmx = match(matrix,"[,]");
		mb = match(matrix,"[]],[[]");
		cmy = mb+2 +match(substr(matrix,mb+3),"[,]");
		rp = match(matrix,"[]][]]");
#		print "# delimiters = " lp " " cmx " " mb " " cmy " " rp;
		if (lp && cmx && mb && cmy && rp) {
			xx = substr(matrix, lp+2, cmx-lp-2) +0.0;
			xy = substr(matrix, cmx+1, mb-cmx-1) +0.0;
			yx = substr(matrix, mb+3, cmy-mb-3) +0.0;
			yy = substr(matrix, cmy+1, rp-cmy-1) +0.0;
#			print "# coeffs = " xx " " xy " " yx " " yy;
		} else {
			print "# updatefig.awk: bad matrix argument.";
			exit 1;
		}
	} else {
		xx = 1.0;	xy = 0.0;
		yx = 0.0;	yy = 1.0;
	}
	tx = xin -ox;
	ty = yin -oy;
#	print "# tx, ty = " tx " " ty;
	xout = xx*tx +xy*ty +ox;
	yout = yx*tx +yy*ty +oy;
#	print "# dx, dy = " dx " " dy;
	if (dx != "")	xout += dx;
	if (dy != "")	yout += dy;
#	print "# xout, yout = " xout " " yout;
	printf("%d %d", xout, yout);
	# note: no trailing space
}

##################### parse-and-update routines ###############################
# main function:
function parse_fig_object(str, 
	# local varaiables
	tokens, n) {
# top level, recursive routine... parses one object at this level
	n = split(str, tokens);
	# can't have pseudo-objects in compound.
	if (tokens[1] == 1) {
		parse_ellipse(str);
	} else if (tokens[1] == 2) {
		parse_polyline(str);
	} else if (tokens[1] == 3) {
		parse_spline(str);
	} else if (tokens[1] == 4) {
		parse_text(str);
	} else if (tokens[1] == 5) {
		parse_arc(str);
	} else if (tokens[1] == 6) {
		parse_compound(str);
	} else {
#		print "# unknown object?";
#		empty block, awk will core dump...
	}
	return tokens[1];	# return the type of the object
}

function parse_pseudo_object(str) {
	print str;			# echo back out for now
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# (3.3) COMPOUND
# First line:
# 	type	name			(brief description)
# 	----	----			-------------------
# 	int	object_code		(always 6)
# 	int	upperright_corner_x	(Fig units)
# 	int	upperright_corner_y	(Fig units)
# 	int	lowerleft_corner_x	(Fig units)
# 	int	lowerleft_corner_y	(Fig units)
# 
# Subsequent lines:
# 	objects

function parse_compound(str, 
	# local variables
	type, n, tokens) {
# recursive descent
#	print str;			# contains box corner bounds
	n = split(str, tokens);
	if (tokens[1] != 6 || n != 5) exit;		# sanity check
	update_field("%d ",tokens[1],"");
	update_field_int_rel("%d ", tokens[2], update_global_dx);
	update_field_int_rel("%d ", tokens[3], update_global_dy);
	update_field_int_rel("%d ", tokens[4], update_global_dx);
	update_field_int_rel("%d", tokens[5], update_global_dy);
	print "";
	type = 0;
	while(type != -6) {
		getline;
		type = parse_fig_object($0);
	}
	print type;		# -6 end compound object
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# (3.5) POLYLINE
# First line:
# 	type	name                    (brief description)
# 	----	----                    -------------------
# $1	int	object_code		(always 2)
# $2	int	sub_type		(1: polyline
# 					 2: box
# 					 3: polygon
# 					 4: arc-box)
# 					 5: imported-picture bounding-box)
# $3	int	line_style		(enumeration type)
# $4	int	thickness		(1/80 inch)
# $5	int	pen_color		(enumeration type, pen color)
# $6	int	fill_color		(enumeration type, fill color)
# $7	int	depth			(enumeration type)
# $8	int	pen_style		(pen style, not used)
# $9	int	area_fill		(enumeration type, -1 = no fill)
# $10	float	style_val		(1/80 inch)
# $11	int	join_style		(enumeration type)
# $12	int	cap_style		(enumeration type, 
# 						only used for POLYLINE)
# $13	int	radius			(1/80 inch, radius of arc-boxes)
# $14	int	forward_arrow		(0: off, 1: on)
# $15	int	backward_arrow		(0: off, 1: on)
# $16	int	npoints			(number of points in line)
# 

function parse_polyline(str,
		# local variables
		match_condition, n, tokens) {
	n = split(str, tokens);
	if (tokens[1] != 2 || n != 16) exit;		# sanity check
	# check for matching condition, defaults to matching ALL
	# right now, no way of matching point positions,
	# wrong position in input stream... :(
	match_condition = ( \
		null_or_match(match_object_code, tokens[1]) &&
		null_or_match(match_sub_type, tokens[2]) &&
		null_or_match(match_line_style, tokens[3]) &&
		null_or_match(match_thickness, tokens[4]) &&
		null_or_match(match_pen_color, tokens[5]) &&
		null_or_match(match_fill_color, tokens[6]) &&
		null_or_range_match(match_depth, tokens[7]) &&
		null_or_match(match_pen_style, tokens[8]) &&
		null_or_match(match_area_fill, tokens[9]) &&
		null_or_match(match_style_val, tokens[10]) &&
		null_or_match(match_join_style, tokens[11]) &&
		null_or_match(match_cap_style, tokens[12]) &&
		null_or_match(match_radius, tokens[13]) &&
		null_or_match(match_forward_arrow, tokens[14]) &&
		null_or_match(match_backward_arrow, tokens[15]) &&
		null_or_match(match_npoints, tokens[16]) );
	if (match_condition) {
	# if match, apply changes
		update_field("%d ",tokens[1],"");	# can't change
		update_field("%d ",tokens[2],"");	# can't change
		update_field("%d ",tokens[3],update_line_style);
		update_field("%d ",tokens[4],update_thickness);
		update_field("%d ",tokens[5],update_pen_color);
		update_field("%d ",tokens[6],update_fill_color);
		update_field("%d ",tokens[7],update_depth);
		update_field("%d ",tokens[8],update_pen_style);
		update_field("%d ",tokens[9],update_area_fill);
		update_field("%.3f ",tokens[10],update_style_val);
		update_field("%d ",tokens[11],update_join_style);
		update_field("%d ",tokens[12],update_cap_style);
		update_field("%d ",tokens[13],update_radius);
		update_field("%d ",tokens[14],update_forward_arrow);
		update_field("%d ",tokens[15],update_backward_arrow);
		update_field("%d", tokens[16],"");	# can't change
		print "";
	} else {
	# doesn't match, keep unmodified
		print str;
	# all dependent lines for this object (arrows, points)
	}
	# check for old arrow lines
	if (tokens[14] == 1) {
		parse_arrow(match_condition);
	}
	if (tokens[15] == 1) {
		parse_arrow(match_condition);
	}
	# always has points
	parse_points(match_condition, tokens[16]);
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Forward arrow line (Optional; absent if forward_arrow is 0):
# 	type	name			(brief description)
# 	----	----			-------------------
# 	int	arrow_type		(enumeration type)
# 	int	arrow_style		(enumeration type)
# 	float	arrow_thickness		(1/80 inch)
# 	float	arrow_width		(Fig units)
# 	float	arrow_height		(Fig units)
# 
# Backward arrow line is analogous

function parse_arrow(match_condition, 
	# local variables
	arrow_match_condition, n, tokens) {
# right now, never modifies, later update this function...
	getline;
	n = split($0, tokens);
	arrow_match_condition = \
		match_condition &&
		null_or_match(match_arrow_type, tokens[1]) &&
		null_or_match(match_arrow_style, tokens[2]) &&
		null_or_match(match_arrow_thickness, tokens[3]) &&
		null_or_match(match_arrow_width, tokens[4]) &&
		null_or_match(match_arrow_height, tokens[5]);
	if (arrow_match_condition) {
		printf("\t");
	# if match, apply changes
		update_field("%d ",tokens[1],update_arrow_type);
		update_field("%d ",tokens[2],update_arrow_style);
		update_field("%.3f ",tokens[3],update_arrow_thickness);
		update_field("%.3f ",tokens[4],update_arrow_width);
		update_field("%.3f",tokens[5],update_arrow_height);
		print "";
	} else {
		print;
	}
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Points line:
# 	type    name                    (brief description)
# 	----    ----                    -------------------
# 	int	x1, y1			(Fig units)
# 	int	x2, y2			(Fig units)
# 	...
# 	int	xnpoints ynpoints	(this will be the same as the 1st
# 					point for polygon and box)

function parse_points(match_condition, np, 
	# local vars:
		sp, i, p, xy_tokens, x_array, y_array) {
	sp = 0;
	while (sp < np) {	# scanned points < expected points
		# assume points come in pairs!
		getline;
		p = split($0, xy_tokens);
		for (i=1; i<=(p/2); i++) {
			sp++;
			# unconditional global update
			x_array[sp] = xy_tokens[2*i-1] +update_global_dx;
			y_array[sp] = xy_tokens[2*i] +update_global_dy;
		}
	}
	# all points stored in x_array and y_array
	# print out all on one line, forget formatting
	printf("\t");
	if (match_condition) {
	for (i=1; i<=np; i++) {
		printf(" %d %d", 
			x_array[i] +update_points_dx, 
			y_array[i] +update_points_dy);
	}} else {
	for (i=1; i<=np; i++) {
		printf(" %d %d", x_array[i], y_array[i]);
	}}
	print "";
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# (3.7) TEXT
#	type	name		(brief description)
#	----	----		-------------------
# $1	int	object		(always 4)
# $2	int	sub_type	(0: Left justified
#				 1: Center justified
#				 2: Right justified)
# $3	int	color		(enumeration type)
# $4	int	depth		(enumeration type)
# $5	int	pen_style	(enumeration , not used)
# $6	int	font		(enumeration type)
# $7	float	font_size	(font size in points)
# $8	float	angle		(radians, the angle of the text)
# $9	int	font_flags	(bit vector)
# $10	float	height		(Fig units)
# $11	float	length		(Fig units)
# $12	int	x, y		(Fig units, coordinate of the origin
# $13				 of the string.  If sub_type = 0, it is
#				 the lower left corner of the string.
#				 If sub_type = 1, it is the lower
#				 center.  Otherwise it is the lower
#				 right corner of the string.)
# remainder
#	char	string[]	(ASCII characters; starts after a blank
#				 character following the last number and
#				 ends before the sequence '\001'.  This
#				 sequence is not part of the string.
#				 Characters above octal 177 are
#				 represented by \xxx where xxx is the
#				 octal value.  This permits fig files to
#				 be edited with 7-bit editors and sent
#				 by e-mail without data loss.
#				 Note that the string may contain '\n'.)

function parse_text(str,
		# local variables
		fig_text, match_condition, n, tokens) {
	n = split(str, tokens);
	fig_text = extract_fig_string($0);
	if (tokens[1] != 4 || n < 14) exit;		# sanity check
	# check for matching condition, defaults to matching ALL
	match_condition = ( \
		null_or_match(match_object_code, tokens[1]) &&
		null_or_match(match_sub_type, tokens[2]) &&	# justification
		null_or_match(match_color, tokens[3]) &&
		null_or_range_match(match_depth, tokens[4]) &&
		null_or_match(match_pen_style, tokens[5]) &&
		null_or_match(match_font, tokens[6]) &&
		null_or_match(match_font_size, tokens[7]) &&
		null_or_match(match_angle, tokens[8]) &&
		null_or_match(match_font_flags, tokens[9]) &&
		null_or_match(match_height, tokens[10]) &&
		null_or_match(match_length, tokens[11]) &&
		null_or_range_match(match_x_range, tokens[12]) &&
		null_or_range_match(match_y_range, tokens[13]) &&
		null_or_regexp_match(match_regexp, fig_text));
	# unconditional global update
	tokens[12] += update_global_dx;
	tokens[13] += update_global_dy;
	if (match_condition) {
	# if match, apply changes
	# if suppress, just filter out
		if (!length(suppress)) {
		update_field("%d ",tokens[1],"");	# can't change
		update_field("%d ",tokens[2],update_sub_type);	# justification
		update_field("%d ",tokens[3],update_color);
		update_field("%d ",tokens[4],update_depth);
		update_field("%d ",tokens[5],update_pen_style);
		update_field("%d ",tokens[6],update_font);
		update_field("%d ",tokens[7],update_font_size);
		update_field("%.4f ",tokens[8],update_angle);
		update_field("%d ",tokens[9],update_font_flags);
		update_field("%d ",tokens[10],update_height);
		update_field("%d ",tokens[11],update_length);
		update_xy_transform(tokens[12], tokens[13], transform_origin, 
			transform_matrix, update_dx, update_dy);
		printf(" ");
#		update_field_int_rel("%d ",tokens[12],update_dx);
#		update_field_int_rel("%d ",tokens[13],update_dy);
		update_field("%s\\001",fig_text,"");
			# fixed for now, enable update_text later
			# later, enable string substitution of fig_text
		print "";
		}
	} else {
	# doesn't match, keep unmodified
		print str;
	}
}

# extracts only the text
function extract_fig_string(str) {
	ext_str = substr(str, skip_non_text(str))
	return substr(ext_str,1,length(ext_str)-4);	# loses trailing \001
}

# returns position of start of text string of text object
function skip_non_text(str) {
	split(str, skip_tokens);
	skip_count = 1;
	for (i=1; i<=13; i++)
		skip_count += length(skip_tokens[i]) +1;
	return skip_count;
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# (3.4) ELLIPSE
# First line:
# 	type	name			(brief description)
# 	----	----			-------------------
# $1	int	object_code		(always 1)
# $2	int	sub_type		(1: ellipse defined by radiuses
# 					 2: ellipse defined by diameters
# 					 3: circle defined by radius
# 					 4: circle defined by diameter)
# $3	int	line_style		(enumeration type)
# $4	int	thickness		(1/80 inch)
# $5	int	pen_color		(enumeration type, pen color)
# $6	int	fill_color		(enumeration type, fill color)
# $7	int	depth			(enumeration type)
# $8	int	pen_style		(pen style, not used)
# $9	int	area_fill		(enumeration type, -1 = no fill)
# $10	float	style_val		(1/80 inch)
# $11	int	direction		(always 1)
# $12	float	angle			(radians, the angle of the x-axis)
# $13	int	center_x, center_y	(Fig units)
# $15	int	radius_x, radius_y	(Fig units)
# $17	int	start_x, start_y	(Fig units; the 1st point entered)
# $19	int	end_x, end_y		(Fig units; the last point entered)
# (total 20 args)

function parse_ellipse(str, 
		# local variables
		match_condition, n, tokens) {
	n = split(str, tokens);
	if (tokens[1] != 1 || n != 20) exit;		# sanity check
	# check for matching condition, defaults to matching ALL
	match_condition = ( \
		null_or_match(match_object_code, tokens[1]) &&
		null_or_match(match_sub_type, tokens[2]) &&
		null_or_match(match_line_style, tokens[3]) &&
		null_or_match(match_thickness, tokens[4]) &&
		null_or_match(match_pen_color, tokens[5]) &&
		null_or_match(match_fill_color, tokens[6]) &&
		null_or_range_match(match_depth, tokens[7]) &&
		null_or_match(match_pen_style, tokens[8]) &&
		null_or_match(match_area_fill, tokens[9]) &&
		null_or_match(match_style_val, tokens[10]) &&
		null_or_match(match_direction, tokens[11]) &&
		null_or_match(match_angle, tokens[12]) &&
		null_or_match(match_center_x, tokens[13]) &&
		null_or_match(match_center_y, tokens[14]) &&
		null_or_match(match_radius_x, tokens[15]) &&
		null_or_match(match_radius_y, tokens[16]) &&
		null_or_match(match_start_x, tokens[17]) &&
		null_or_match(match_start_y, tokens[18]) &&
		null_or_match(match_end_x, tokens[19]) &&
		null_or_match(match_end_y, tokens[20]) );
	# unconditional global changes
	tokens[13] += update_global_dx;
	tokens[14] += update_global_dy;
	tokens[15] += update_global_dx;
	tokens[16] += update_global_dy;
	tokens[17] += update_global_dx;
	tokens[18] += update_global_dy;
	tokens[19] += update_global_dx;
	tokens[20] += update_global_dy;
	if (match_condition) {
	# if match, apply changes
		update_field("%d ",tokens[1],"");	# can't change
		update_field("%d ",tokens[2],update_sub_type);
		update_field("%d ",tokens[3],update_line_style);
		update_field("%d ",tokens[4],update_thickness);
		update_field("%d ",tokens[5],update_pen_color);
		update_field("%d ",tokens[6],update_fill_color);
		update_field("%d ",tokens[7],update_depth);
		update_field("%d ",tokens[8],update_pen_style);
		update_field("%d ",tokens[9],update_area_fill);
		update_field("%.3f ",tokens[10],update_style_val);
		update_field("%d ",tokens[11],update_direction);
		update_field("%.4f ",tokens[12],update_angle);
		update_field("%d ",tokens[13],update_center_x);
		update_field("%d ",tokens[14],update_center_y);
		update_field("%d ",tokens[15],update_radius_x);
		update_field("%d ",tokens[16],update_radius_y);
		update_field("%d ",tokens[17],update_start_x);
		update_field("%d ",tokens[18],update_start_y);
		update_field("%d ",tokens[19],update_end_x);
		update_field("%d", tokens[20],update_end_y);
		print "";
	} else {
	# doesn't match, keep unmodified
		print str;
	}
}


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# (3.6) SPLINE
# First line:
# 	type	name			(brief description)
# 	----	----			-------------------
# $1	int	object_code		(always 3)
# $2	int	sub_type		(0: opened approximated spline
# 					 1: closed approximated spline
# 					 2: opened interpolated spline
# 					 3: closed interpolated spline
# 					 4: opened x-spline
# 					 5: closed x-spline)
# $3	int	line_style		(See the end of this section)
# $4	int	thickness		(1/80 inch)
# $5	int	pen_color		(enumeration type, pen color)
# $6	int	fill_color		(enumeration type, fill color)
# $7	int	depth			(enumeration type)
# $8	int	pen_style		(pen style, not used)
# $9	int	area_fill		(enumeration type, -1 = no fill)
# $10	float	style_val		(1/80 inch)
# $11	int	cap_style		(enumeration type, 
# 					 only used for open splines)
# $12	int	forward_arrow		(0: off, 1: on)
# $13	int	backward_arrow		(0: off, 1: on)
# $14	int	npoints			(number of control points in spline)
# 
# Control points line :
# There is one shape factor for each point. The value of this factor
# must be between -1 (which means that the spline is interpolated at
# this point) and 1 (which means that the spline is approximated at
# this point). The spline is always smooth in the neighbourhood of a
# control point, except when the value of the factor is 0 for which
# there is a first-order discontinuity (i.e. angular point).

function parse_spline(str,
		# local variables
		match_condition, n, tokens) {
	n = split(str, tokens);
	if (tokens[1] != 3 || n != 14) exit;		# sanity check
	# check for matching condition, defaults to matching ALL
	match_condition = ( \
		null_or_match(match_object_code, tokens[1]) &&
		null_or_match(match_sub_type, tokens[2]) &&
		null_or_match(match_line_style, tokens[3]) &&
		null_or_match(match_thickness, tokens[4]) &&
		null_or_match(match_pen_color, tokens[5]) &&
		null_or_match(match_fill_color, tokens[6]) &&
		null_or_range_match(match_depth, tokens[7]) &&
		null_or_match(match_pen_style, tokens[8]) &&
		null_or_match(match_area_fill, tokens[9]) &&
		null_or_match(match_style_val, tokens[10]) &&
		null_or_match(match_cap_style, tokens[11]) &&
		null_or_match(match_forward_arrow, tokens[12]) &&
		null_or_match(match_backward_arrow, tokens[13]) &&
		null_or_match(match_npoints, tokens[14]) );
	if (match_condition) {
	# if match, apply changes
		update_field("%d ",tokens[1],"");	# can't change
		update_field("%d ",tokens[2],update_sub_type);
		update_field("%d ",tokens[3],update_line_style);
		update_field("%d ",tokens[4],update_thickness);
		update_field("%d ",tokens[5],update_pen_color);
		update_field("%d ",tokens[6],update_fill_color);
		update_field("%d ",tokens[7],update_depth);
		update_field("%d ",tokens[8],update_pen_style);
		update_field("%d ",tokens[9],update_area_fill);
		update_field("%.3f ",tokens[10],update_style_val);
		update_field("%d ",tokens[11],update_cap_style);
		update_field("%d ",tokens[12],update_forward_arrow);
		update_field("%d ",tokens[13],update_backward_arrow);
		update_field("%d", tokens[14],"");	# can't change
		print "";
	} else {
	# doesn't match, keep unmodified
		print str;
	# all dependent lines for this object (arrows, points)
	}
	# check for old arrow lines
	if (tokens[12] == 1) {
		parse_arrow(match_condition);
	}
	if (tokens[13] == 1) {
		parse_arrow(match_condition);
	}
	# always has points and control points
	parse_points(match_condition, tokens[14]);
	parse_control_points(match_condition, tokens[14]);
}

function parse_control_points(match_condition, np) {
# for now, never changes
	getline;
	print;
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# (3.2) ARC
# First line:
# 	type	name			(brief description)
# 	----	----			-------------------
# $1	int	object_code		(always 5)
# $2	int	sub_type		(0: pie-wedge (closed)
# 					 1: open ended arc)
# $3	int	line_style		(enumeration type)
# $4	int	thickness		(1/80 inch)
# $5	int	pen_color		(enumeration type, pen color)
# $6	int	fill_color		(enumeration type, fill color)
# $7	int	depth			(enumeration type)
# $8	int	pen_style		(pen style, not used)
# $9	int	area_fill		(enumeration type, -1 = no fill)
# $10	float	style_val		(1/80 inch)
# $11	int	cap_style		(enumeration type)
# $12	int	direction		(0: clockwise, 1: counterclockwise)
# $13	int	forward_arrow		(0: no forward arrow, 1: on)
# $14	int	backward_arrow		(0: no forward arrow, 1: on)
# $15	float	center_x, center_y	(center of the arc)
# $17	int	x1, y1			(Fig units, the 1st point user entered)
# $19	int	x2, y2			(Fig units, the 2nd point)
# $21	int	x3, y3			(Fig units, the last point)

function parse_arc(str,
		# local variables
		match_condition, n, tokens) {
	n = split(str, tokens);
	if (tokens[1] != 5 || n != 22) exit;		# sanity check
	# check for matching condition, defaults to matching ALL
	match_condition = ( \
		null_or_match(match_object_code, tokens[1]) &&
		null_or_match(match_sub_type, tokens[2]) &&
		null_or_match(match_line_style, tokens[3]) &&
		null_or_match(match_thickness, tokens[4]) &&
		null_or_match(match_pen_color, tokens[5]) &&
		null_or_match(match_fill_color, tokens[6]) &&
		null_or_range_match(match_depth, tokens[7]) &&
		null_or_match(match_pen_style, tokens[8]) &&
		null_or_match(match_area_fill, tokens[9]) &&
		null_or_match(match_style_val, tokens[10]) &&
		null_or_match(match_cap_style, tokens[11]) &&
		null_or_match(match_direction, tokens[12]) &&
		null_or_match(match_forward_arrow, tokens[13]) &&
		null_or_match(match_backward_arrow, tokens[14]) &&
		null_or_match(match_center_x, tokens[15]) &&
		null_or_match(match_center_y, tokens[16]) &&
		null_or_match(match_x1, tokens[17]) &&
		null_or_match(match_y1, tokens[18]) &&
		null_or_match(match_x2, tokens[19]) &&
		null_or_match(match_y2, tokens[20]) &&
		null_or_match(match_x3, tokens[21]) &&
		null_or_match(match_y3, tokens[22]) );
	# unconditional global update
	tokens[15] += update_global_dx;
	tokens[16] += update_global_dy;
	tokens[17] += update_global_dx;
	tokens[18] += update_global_dy;
	tokens[19] += update_global_dx;
	tokens[20] += update_global_dy;
	tokens[21] += update_global_dx;
	tokens[22] += update_global_dy;
	if (match_condition) {
	# if match, apply changes
		update_field("%d ",tokens[1],"");	# can't change
		update_field("%d ",tokens[2],update_sub_type);
		update_field("%d ",tokens[3],update_line_style);
		update_field("%d ",tokens[4],update_thickness);
		update_field("%d ",tokens[5],update_pen_color);
		update_field("%d ",tokens[6],update_fill_color);
		update_field("%d ",tokens[7],update_depth);
		update_field("%d ",tokens[8],update_pen_style);
		update_field("%d ",tokens[9],update_area_fill);
		update_field("%.4f ",tokens[10],update_style_val);
		update_field("%d ",tokens[11],update_cap_style);
		update_field("%d ",tokens[12],update_direction);
		update_field("%d ",tokens[13],update_forward_arrow);
		update_field("%d ",tokens[14],update_backward_arrow);
		update_field("%.4f ",tokens[15],update_center_x);
		update_field("%.4f ",tokens[16],update_center_y);
		update_field("%d ",tokens[17],update_x1);
		update_field("%d ",tokens[18],update_y1);
		update_field("%d ",tokens[19],update_x2);
		update_field("%d ",tokens[20],update_y2);
		update_field("%d ",tokens[21],update_x3);
		update_field("%d" ,tokens[22],update_y3);
		print "";
	} else {
	# doesn't match, keep unmodified
		print str;
	# all dependent lines for this object (arrows, points)
	}
	# check for old arrow lines
	if (tokens[13] == 1) {
		parse_arrow(match_condition);
	}
	if (tokens[14] == 1) {
		parse_arrow(match_condition);
	}
}

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# end of file

