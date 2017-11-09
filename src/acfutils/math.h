/*
 * CDDL HEADER START
 *
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2017 Saso Kiselkov. All rights reserved.
 */

#ifndef	_ACF_UTILS_MATH_H_
#define	_ACF_UTILS_MATH_H_

#include "assert.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define	POW3(x)	((x) * (x) * (x))
#define	POW2(x)	((x) * (x))
#define	ROUND_ERROR	1e-10

unsigned quadratic_solve(double a, double b, double c, double x[2]);
double fx_lin(double x, double x1, double y1, double x2, double y2);

/*
 * Weighted avg, 'w' is weight fraction from 0.0 = all of x to 1.0 = all of y.
 */
static inline double
wavg(double x, double y, double w)
{
	ASSERT(w >= 0.0 && w <= 1.0);
	return (x + (y - x) * w);
}

/*
 * Given two values min_val and max_val (such that min_val < max_val),
 * returns how far between min_val and max_val a third value 'x' lies.
 * If `clamp' is true, 'x' is clamped such that it always lies between
 * min_val and max_val. In essence, this function computes:
 *
 *      ^
 *      |
 *    1 -------------------+
 *      |               /  |
 *      |             /    |
 * f(x) ------------+      |
 *      |         / |      |
 *      |       /   |      |
 *    0 ------+     |      |
 *      |     |     |      |
 *      +-----|-----|------|----->
 *         min_val  x   max_val
 */
static inline double
iter_fract(double x, double min_val, double max_val, bool_t clamp)
{
	ASSERT3F(max_val, >, min_val);
	x = (x - min_val) / (max_val - min_val);
	if (clamp) {
		x = MAX(x, 0);
		x = MIN(x, 1);
	}
	return (x);
}

#ifdef	__cplusplus
}
#endif

#endif	/* _ACF_UTILS_GEOM_H_ */
