/*==============================================================================
File    utcl.h

Brief   uTCL library.

        The MIT License (MIT)

        Copyright (C) 2016 Serge Zaitsev
        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

        Permission is hereby granted, free of charge, to any person obtaining a copy
        of this software and associated documentation files (the "Software"), to deal
        in the Software without restriction, including without limitation the rights
        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
        furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
        OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
        SOFTWARE.

==============================================================================*/

#ifndef _UTCL_H_
#define _UTCL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>

/*==============================================================================
  Exported macros
==============================================================================*/
#define UTCL_DEBUG 0

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Value type.
 */
typedef char tcl_value_t;

/**
 * uTCL container (context).
 */
struct tcl {
        struct tcl_env *env;
        struct tcl_cmd *cmds;
        tcl_value_t *result;
        int exit;
};

/**
 * Command function.
 */
typedef int (*tcl_cmd_fn_t)(struct tcl *, tcl_value_t *, void *);

/* Control flow constants */
enum { FERROR, FNORMAL, FRETURN, FBREAK, FAGAIN };

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  tcl_init(struct tcl *tcl);
extern void tcl_destroy(struct tcl *tcl);
extern const char *tcl_string(const tcl_value_t *v);
extern float tcl_float(tcl_value_t *v);
extern int  tcl_length(tcl_value_t *v);
extern void tcl_free(tcl_value_t *v);
extern void tcl_list_free(tcl_value_t *v);
extern int  tcl_result(struct tcl *tcl, int flow, tcl_value_t *result);
extern int  tcl_subst(struct tcl *tcl, const char *s, size_t len);
extern int  tcl_eval(struct tcl *tcl, const char *s, size_t len);
extern int  tcl_register(struct tcl *tcl, const char *name, tcl_cmd_fn_t fn, int arity, void *arg);
extern int  tcl_register_const(struct tcl *tcl, const char *name, tcl_cmd_fn_t fn, int arity, void *arg);
extern int  tcl_loadfile(struct tcl *tcl, const char *filename);
extern int  tcl_list_length(tcl_value_t *v);
extern tcl_value_t *tcl_append_string(tcl_value_t *v, const char *s, size_t len);
extern tcl_value_t *tcl_append(tcl_value_t *v, tcl_value_t *tail);
extern tcl_value_t *tcl_alloc(const char *s, size_t len);
extern tcl_value_t *tcl_dup(tcl_value_t *v);
extern tcl_value_t *tcl_list_alloc();
extern tcl_value_t *tcl_list_at(tcl_value_t *v, int index);
extern tcl_value_t *tcl_list_append(tcl_value_t *v, tcl_value_t *tail);
extern tcl_value_t *tcl_var(struct tcl *tcl, tcl_value_t *name, tcl_value_t *v);

#endif /* _UTCL_H_ */

/*==============================================================================
  End of file
==============================================================================*/
