/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license in the file COPYING
 * or http://www.opensource.org/licenses/CDDL-1.0.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file COPYING.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2018 Saso Kiselkov. All rights reserved.
 */

#ifndef	_ACF_UTILS_GLUTILS_H_
#define	_ACF_UTILS_GLUTILS_H_

#include <GL/glew.h>

#include <acfutils/assert.h>
#include <acfutils/geom.h>
#include <acfutils/log.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
	GLuint	vbo;
	size_t	num_vtx;
} glutils_quads_t;

typedef struct {
	GLuint	vbo;
	size_t	num_vtx;
} glutils_lines_t;

typedef void (*glutils_texsz_enum_cb_t)(const char *token, int64_t bytes,
    void *userinfo);

API_EXPORT void glutils_disable_all_client_state(void);

API_EXPORT GLuint glutils_make_quads_IBO(size_t num_vtx);

#define	glutils_init_2D_quads(__quads, __p, __t, __num_pts) \
	glutils_init_2D_quads_impl((__quads), log_basename(__FILE__), \
	    __LINE__, (__p), (__t), (__num_pts))
API_EXPORT void glutils_init_2D_quads_impl(glutils_quads_t *quads,
    const char *filename, int line, const vect2_t *p, const vect2_t *t,
    size_t num_pts);

#define	glutils_init_3D_quads(__quads, __p, __t, __num_pts) \
	glutils_init_3D_quads_impl((__quads), log_basename(__FILE__), \
	    __LINE__, (__p), (__t), (__num_pts))
API_EXPORT void glutils_init_3D_quads_impl(glutils_quads_t *quads,
    const char *filename, int line, const vect3_t *p, const vect2_t *t,
    size_t num_pts);

API_EXPORT void glutils_destroy_quads(glutils_quads_t *quads);
API_EXPORT void glutils_draw_quads(const glutils_quads_t *quads, GLint prog);

#define	glutils_init_3D_lines(__lines, __p, __num_pts) \
	glutils_init_3D_lines_impl((__lines), log_basename(__FILE__), \
	    __LINE__, (__p), (__num_pts))
API_EXPORT void glutils_init_3D_lines_impl(glutils_lines_t *lines,
    const char *filename, int line, vect3_t *p, size_t num_pts);

API_EXPORT void glutils_destroy_lines(glutils_lines_t *lines);
API_EXPORT void glutils_draw_lines(const glutils_lines_t *lines, GLint prog);

API_EXPORT void glutils_vp2pvm(GLfloat pvm[16]);

#define	GLUTILS_VALIDATE_INDICES(indices, num_idx, num_vtx) \
	do { \
		for (unsigned i = 0; i < (num_idx); i++) { \
			VERIFY_MSG((indices)[i] < (num_vtx), "invalid index " \
			    "specification encountered, index %d (value %d) " \
			    "is outside of vertex range %d", i, (indices)[i], \
			    (num_vtx)); \
		} \
	} while (0)

/*
 * The infrastructure below is for debugging GPU VRAM memory leaks.
 *
 * At plugin load time (in XPluginStart and before doing any libacfutils
 * calls that might generate OpenGL calls), you must first initialize the
 * system using a call to glutils_texsz_init(). At plugin exit time, and
 * after having torn down all resources, call glutils_texsz_fini(). This
 * collects all garbage and crashes the app with diagnostic information
 * in case any leaks have been detected.
 *
 * Each allocation can be tracked in a two-level hierarchy:
 * - using a symbolic token name
 *   - each token can track allocations to a particular anonymous pointer
 *     (plus filename:line where it occurred)
 *
 * The tokens are used to identify large blocks of functionality. You'd
 * use a token for, for example, "efis_textures" or "custom_drawing_pbo",
 * etc. These must be declared ahead at the top of each module file using
 * the TEXSZ_MK_TOKEN macro, for example:
 *
 * TEXSZ_MK_TOKEN(efis_textures);
 *
 * Don't put strings into the token name, the name must be a valid C
 * identifier. You can subsequently track allocations to this token using
 * the TEXSZ_ALLOC and TEXSZ_FREE macros. These macros take 5 arguments:
 *	1) the token name
 *	2) the texture format (e.g. GL_RGBA)
 *	3) the texture data type (e.g. GL_UNSIGNED_BYTE)
 *	4) texture width
 *	5) texture height
 * They then either increment or decrement the token byte counter. If a
 * token is non-zero at the time glutils_texsz_fini() is called, the
 * offending token name(s) are printed in sequence, with the amount of
 * bytes leaked in them. In this mode, no filenames or line numbers are
 * printed.
 *
 * In case you are using a generic facility (e.g. a picture loader) to
 * provide texturing service to other parts of the code, a simple token
 * name might not be specific enough to pinpoint the offending allocation.
 * In that case, you can use the TEXSZ_ALLOC_INSTANCE and
 * TEXSZ_FREE_INSTANCE macros to generate per-pointer statistics. The
 * TEX_ALLOC_INSTANCE macro takes two additional parameters over TEXSZ_ALLOC:
 *	1) An instance pointer - this is used to discriminate individual
 *	   allocations. Usually you'd want to pass a containing structure
 *	   pointer or something similar in here.
 *	2) An allocation point filename.
 *	3) An allocation point line number.
 * The filename should be shortened at build time using log_backtrace()
 * to only contain the last portion of the filename of the call site.
 * You should wrap your functions which use TEXSZ_ALLOC_INSTANCE into a
 * macro and extract the call site information automatically using the
 * __FILE__ and __LINE__ built-in pre-processor variables (see the
 * glutils_init_*_quads functions above for an example on how to do that).
 * With instancing, glutils_texsz_fini() will print a list of leaked
 * instance pointers and call sites in your code where that resource was
 * leaked.
 *
 * In addition to the TEXSZ_ALLOC_* and TEXSZ_FREE_* macros, there are
 * variations of these macros with the "_BYTES" suffix. These let you
 * pass the raw byte size of the allocation directly, instead of having
 * to pass texture formats and sizes. Use these when the data isn't a
 * texture, but instead something more generic, like a vertex buffer.
 */

#define	TEXSZ_MK_TOKEN(name) \
	static const char *__texsz_token_ ## name = #name
#define	TEXSZ_ALLOC(__token_id, __format, __type, __w, __h) \
	TEXSZ_ALLOC_INSTANCE(__token_id, NULL, NULL, -1, (__format), \
	    (__type), (__w), (__h))
#define	TEXSZ_FREE(__token_id, __format, __type, __w, __h) \
	TEXSZ_FREE_INSTANCE(__token_id, NULL, (__format), \
	    (__type), (__w), (__h))
#define	TEXSZ_ALLOC_INSTANCE(__token_id, __instance, __filename, __line, \
    __format, __type, __w, __h) \
	glutils_texsz_alloc(__texsz_token_ ## __token_id, (__instance), \
	    (__filename), (__line), (__format), (__type), (__w), (__h))
#define	TEXSZ_FREE_INSTANCE(__token_id, __instance, __format, __type, \
    __w, __h) \
	glutils_texsz_free(__texsz_token_ ## __token_id, (__instance), \
	    (__format), (__type), (__w), (__h))
#define	TEXSZ_ALLOC_BYTES(__token_id, __bytes) \
	TEXSZ_ALLOC_BYTES_INSTANCE(__token_id, NULL, NULL, -1, (__bytes))
#define	TEXSZ_FREE_BYTES(__token_id, __bytes) \
	TEXSZ_FREE_BYTES_INSTANCE(__token_id, NULL, (__bytes))
#define	TEXSZ_ALLOC_BYTES_INSTANCE(__token_id, __instance, __filename, \
    __line, __bytes) \
	glutils_texsz_alloc_bytes(__texsz_token_ ## __token_id, (__instance), \
	    (__filename), (__line), (__bytes))
#define	TEXSZ_FREE_BYTES_INSTANCE(__token_id, __instance, __bytes) \
	glutils_texsz_free_bytes(__texsz_token_ ## __token_id, (__instance), \
	    (__bytes))

API_EXPORT void glutils_texsz_init(void);
API_EXPORT void glutils_texsz_fini(void);
API_EXPORT void glutils_texsz_alloc(const char *token, const void *instance,
    const char *filename, int line, GLenum format, GLenum type,
    unsigned w, unsigned h);
API_EXPORT void glutils_texsz_free(const char *token, const void *instance,
    GLenum format, GLenum type, unsigned w, unsigned h);
API_EXPORT void glutils_texsz_alloc_bytes(const char *token,
    const void *instance, const char *filename, int line, int64_t bytes);
API_EXPORT void glutils_texsz_free_bytes(const char *token,
    const void *instance, int64_t bytes);

/*
 * Returns the total amount of bytes tracked. Useful for estimating GPU
 * memory load due to custom avionics code.
 */
API_EXPORT uint64_t glutils_texsz_get(void);
API_EXPORT void glutils_texsz_enum(glutils_texsz_enum_cb_t cb, void *userinfo);

#define	IF_TEXSZ(__xxx) \
	do { \
		if (glutils_texsz_inited()) { \
			__xxx; \
		} \
	} while (0)
API_EXPORT bool_t glutils_texsz_inited(void);

#ifdef	__cplusplus
}
#endif

#endif	/* _ACF_UTILS_GLUTILS_H_ */
