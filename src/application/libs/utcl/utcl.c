/*==============================================================================
File    utcl.c

Brief   uTCL library based on partcl.

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

#include "utcl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define CHUNK_LEN 256

#if UTCL_DEBUG
#define DBG printf

struct alloc {
        size_t  sz;
};

static size_t used_mem = 0;
static size_t used_mem_max = 0;

void *user_malloc(size_t sz)
{
        struct alloc *a = malloc(sizeof(struct alloc) + sz);
        void *mem = NULL;

        if (a) {
                a->sz = sz;
                mem = &a[1];
                used_mem += sz;

                used_mem_max = used_mem > used_mem_max ? used_mem : used_mem_max;
        }

        return mem;
}

void user_free(void *mem)
{
        if (mem) {
                struct alloc *a = (struct alloc*)(mem);
                if (a) {
                        a--;
                        used_mem -= a->sz;
                        DBG("DBG: freed %d bytes\n", (int)a->sz);
                        free(a);
                }
        }
}

void *user_realloc(void *ptr, size_t size)
{
        void *mem = user_malloc(size);

        if (mem && ptr) {
                memcpy(mem, ptr, size);
        }

        if (ptr) {
                user_free(ptr);
        }

        return mem;
}

void *user_calloc(size_t count, size_t size)
{
        return user_malloc(count * size);
}



#define malloc user_malloc
#define calloc user_calloc
#define realloc user_realloc
#define free user_free
#else
#define DBG(...)
#endif

#define tcl_each(s, len, skiperr)                                              \
  for (struct tcl_parser p = {NULL, NULL, s, s + len, 0, TERROR};              \
       p.start < p.end &&                                                      \
           (((p.token = tcl_next(p.start, p.end - p.start, &p.from, &p.to,     \
                                 &p.q)) != TERROR) ||                          \
            skiperr);                                                          \
       p.start = p.to)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* A helper parser struct and macro (requires C99) */
struct tcl_parser {
        const char *from;
        const char *to;
        const char *start;
        const char *end;
        int q;
        int token;
};

struct tcl_cmd {
        const tcl_value_t *name;
        bool constname;
        int arity;
        tcl_cmd_fn_t fn;
        void *arg;
        struct tcl_cmd *next;
};

struct tcl_var {
        tcl_value_t *name;
        tcl_value_t *value;
        struct tcl_var *next;
};

struct tcl_env {
        struct tcl_var *vars;
        struct tcl_env *parent;
};

/* Token type */
enum {TCMD, TWORD, TPART, TERROR};

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function check that character is a special character.
 *
 * @param c     character to check
 * @param q     quit character
 *
 * @return Return 1 if character is special, 0 otherwise.
 */
//==============================================================================
static int tcl_is_special(char c, int q)
{
        return (c == '$'
               || (!q && (c == '{' || c == '}' || c == ';' || c == '\r' || c == '\n'))
               || c == '[' || c == ']' || c == '"' || c == '\0');
}

//==============================================================================
/**
 * @brief Function check if character is space or tab.
 *
 * @param c     character to check
 *
 * @return Return 1 if character is space, 0 otherwise.
 */
//==============================================================================
static int tcl_is_space(char c)
{
        return (c == ' ' || c == '\t');
}

//==============================================================================
/**
 * @brief Function check if character is an end character.
 *
 * @param c     character to check
 *
 * @return Return 1 if character is end, 0 otherwise.
 */
//==============================================================================
static int tcl_is_end(char c)
{
        return (c == '\n' || c == '\r' || c == ';' || c == '\0');
}

//==============================================================================
/**
 * @brief Function allocate new environment container.
 *
 * @param tcl           tcl container
 * @param parent        parent environment container
 *
 * @return Pointer to new object, NULL otherwise.
 */
//==============================================================================
static struct tcl_env *tcl_env_alloc(struct tcl_env *parent)
{
        struct tcl_env *env = malloc(sizeof(*env));
        if (env) {
                env->vars = NULL;
                env->parent = parent;
        } else {
                puts("Out of memory!");
        }
        return env;
}

//==============================================================================
/**
 * @brief Function add new variable to environment container.
 *
 * @param env   environment container
 * @param name  variable name
 *
 * @return Return new variable object, NULL otherwise.
 */
//==============================================================================
static struct tcl_var *tcl_env_var(struct tcl_env *env, tcl_value_t *name)
{
        struct tcl_var *var = malloc(sizeof(struct tcl_var));
        if (var) {
                var->name = tcl_dup(name);
                var->next = env->vars;
                var->value = tcl_alloc("", 0);
                env->vars = var;
        } else {
                puts("Out of memory!");
        }
        return var;
}

//==============================================================================
/**
 * @brief Function free selected environment container.
 *
 * @param env   container to free
 *
 * @return Pointer to parent container.
 */
//==============================================================================
static struct tcl_env *tcl_env_free(struct tcl_env *env)
{
        struct tcl_env *parent = env->parent;
        while (env->vars) {
                struct tcl_var *var = env->vars;
                env->vars = env->vars->next;
                tcl_free(var->name);
                tcl_free(var->value);
                free(var);
        }
        free(env);
        return parent;
}

//==============================================================================
/**
 * @brief Command set selected variable.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_set(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *var = tcl_list_at(args, 1);
        tcl_value_t *val = tcl_list_at(args, 2);
        int r = tcl_result(tcl, FNORMAL, tcl_dup(tcl_var(tcl, var, val)));
        tcl_free(var);
        return r;
}

//==============================================================================
/**
 * @brief Command unset selected variable.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_unset(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *name = tcl_list_at(args, 1);

        struct tcl_var *var;
        struct tcl_var *var_prev = NULL;
        for (var = tcl->env->vars; var != NULL; var = var->next) {
                if (strcmp(var->name, tcl_string(name)) == 0) {
                        tcl_free(var->name);
                        tcl_free(var->value);

                        if (var_prev) {
                                var_prev->next = var->next;
                        } else {
                                tcl->env->vars = var->next;
                        }

                        tcl_free((void*)var);

                        DBG("DBG: unsed '%s' variable.\n", tcl_string(name));

                        break;
                }
                var_prev = var;
        }

        tcl_free(name);
        return FNORMAL;
}

//==============================================================================
/**
 * @brief Command remove selected function.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_unproc(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *name = tcl_list_at(args, 1);

        struct tcl_cmd *cmd_prev = NULL;

        for (struct tcl_cmd *cmd = tcl->cmds; cmd != NULL; cmd = cmd->next) {

                if (strcmp(cmd->name, tcl_string(name)) == 0) {

                        if (cmd_prev) {
                                cmd_prev = cmd->next;
                        } else {
                                tcl->cmds = cmd->next;
                        }

                        if (cmd->constname == false) {
                                tcl_free((tcl_value_t*)cmd->name);
                        }
                        free(cmd->arg);
                        free(cmd);

                        break;
                }
                cmd_prev = cmd;
        }

        tcl_free(name);
        return FNORMAL;
}

//==============================================================================
/**
 * @brief Command subst selected variable.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_subst(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *s = tcl_list_at(args, 1);
        int r = tcl_subst(tcl, tcl_string(s), tcl_length(s));
        tcl_free(s);
        return r;
}

//==============================================================================
/**
 * @brief Command puts string to terminal.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_puts(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *text = tcl_list_at(args, 1);
        puts(tcl_string(text));
        return tcl_result(tcl, FNORMAL, text);
}

//==============================================================================
/**
 * @brief Command get string from terminal.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_gets(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)args;
        (void)arg;

        char buf[128];
        fgets(buf, sizeof(buf), stdin);
        return tcl_result(tcl, FNORMAL, tcl_alloc(buf, strlen(buf)-1));
}

//==============================================================================
/**
 * @brief Command print formatted text to terminal.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_printf(struct tcl *tcl, tcl_value_t *argv, void *arg)
{
        (void)tcl;
        (void)arg;

        //size_t argc = tcl_list_length(argv);
        size_t argi = 2;

        tcl_value_t *s = tcl_list_at(argv, 1);
        const char *fmt = s;

        while (fmt && *fmt != '\0') {
                if (*fmt == '%' /*&& arg < argc*/) {

                        char format[16];
                        size_t i = 0;
                        format[i++] = *fmt++;

                        while (isdigit(*fmt) && i < sizeof(format) - 2) {
                                format[i++] = *fmt++;
                        }

                        format[i++] = *fmt++;
                        format[i] = '\0';

                        if (strchr("c", format[i - 1])) {
                                tcl_value_t *v = tcl_list_at(argv, argi++);
                                printf(format, tcl_string(v)[0]);
                                tcl_free(v);
                        } else if (strchr("%", format[i - 1])) {
                                printf(format);
                        } else if (strchr("diuxX", format[i - 1])) {
                                tcl_value_t *v = tcl_list_at(argv, argi++);
                                printf(format, (int)tcl_float(v));
                                tcl_free(v);
                        } else if (strchr("f", format[i - 1])) {
                                tcl_value_t *v = tcl_list_at(argv, argi++);
                                printf(format, tcl_float(v));
                                tcl_free(v);
                        } else if (strchr("n", format[i - 1])) {
                                puts("");
                        } else {
                                tcl_value_t *v = tcl_list_at(argv, argi++);
                                printf(format, tcl_string(v));
                                tcl_free(v);
                        }
                } else {
                        putchar(*fmt++);
                }
        }

        tcl_free(s);

        return FNORMAL;
}

//==============================================================================
/**
 * @brief Command exit from interpreter.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_exit(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)args;
        (void)arg;

        tcl->exit = 1;
        return FRETURN;
}

//==============================================================================
/**
 * @brief Command eval selected string as script.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_eval(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *expr = tcl_list_at(args, 1);
        int r = tcl_eval(tcl, tcl_string(expr), strlen(tcl_string(expr)) + 1);
        tcl_free(expr);
        return r;
}

//==============================================================================
/**
 * @brief Command get list length.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_llength(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *lst = tcl_list_at(args, 1);
        tcl_value_t *r   = NULL;

        int err = FNORMAL;

        if (lst) {
                char buf[8];
                snprintf(buf, sizeof(buf), "%d", tcl_list_length(lst));
                r = tcl_alloc(buf, strlen(buf));
        } else {
                err = FERROR;
        }

        tcl_free(lst);

        return tcl_result(tcl, err, r);
}

//==============================================================================
/**
 * @brief Command return element from list at selected index.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_lindex(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *lst = tcl_list_at(args, 1);
        tcl_value_t *idx = tcl_list_at(args, 2);
        tcl_value_t *r   = NULL;

        int err = FNORMAL;

        if (lst && idx) {
                r = tcl_list_at(lst, tcl_float(idx));
        } else {
                err = FERROR;
        }

        tcl_free(lst);
        tcl_free(idx);

        return tcl_result(tcl, err, r);
}

//==============================================================================
/**
 * @brief Command include selected file.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_source(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *filename = tcl_list_at(args, 1);

        int r = tcl_loadfile(tcl, filename);
        tcl_free(filename);

        return r;
}

//==============================================================================
/**
 * @brief Function create new procedure.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument (code chunk)
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_user_proc(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        tcl_value_t *code = (tcl_value_t *)arg;
        tcl_value_t *params = tcl_list_at(code, 2);
        tcl_value_t *body = tcl_list_at(code, 3);
        int i;
        tcl->env = tcl_env_alloc(tcl->env);
        for (i = 0; i < tcl_list_length(params); i++) {
                tcl_value_t *param = tcl_list_at(params, i);
                tcl_value_t *v = tcl_list_at(args, i + 1);
                tcl_var(tcl, param, v);
                tcl_free(param);
        }
        i = tcl_eval(tcl, tcl_string(body), tcl_length(body) + 1);
        tcl->env = tcl_env_free(tcl->env);
        tcl_free(params);
        tcl_free(body);
        return FNORMAL;
}

//==============================================================================
/**
 * @brief Command register new user procedure (from script).
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_proc(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *name = tcl_list_at(args, 1);
        tcl_register(tcl, tcl_string(name), tcl_user_proc, 0, tcl_dup(args));
        tcl_free(name);
        return tcl_result(tcl, FNORMAL, tcl_alloc("", 0));
}

//==============================================================================
/**
 * @brief Command realize IF condition.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_if(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        int i = 1;
        int n = tcl_list_length(args);
        int r = FNORMAL;
        while (i < n) {
                tcl_value_t *cond = tcl_list_at(args, i);
                tcl_value_t *branch = NULL;
                if (i + 1 < n) {
                        branch = tcl_list_at(args, i + 1);
                }
                r = tcl_eval(tcl, tcl_string(cond), tcl_length(cond) + 1);
                tcl_free(cond);
                if (r != FNORMAL) {
                        tcl_free(branch);
                        break;
                }
                if ((int)tcl_float(tcl->result)) {
                        r = tcl_eval(tcl, tcl_string(branch),
                                     tcl_length(branch) + 1);
                        tcl_free(branch);
                        break;
                }
                i = i + 2;
                tcl_free(branch);
        }
        return r;
}

//==============================================================================
/**
 * @brief Command control instruction flow (for while, if, etc instructions).
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_flow(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *flowval = tcl_list_at(args, 0);
        const char *flow = tcl_string(flowval);
        if (strcmp(flow, "break") == 0) {
                tcl_free(flowval);
                return FBREAK;
        } else if (strcmp(flow, "continue") == 0) {
                tcl_free(flowval);
                return FAGAIN;
        } else if (strcmp(flow, "return") == 0) {
                tcl_free(flowval);
                return tcl_result(tcl, FRETURN, tcl_list_at(args, 1));
        }
        return FERROR;
}

//==============================================================================
/**
 * @brief Command realize while operation.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_while(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *cond = tcl_list_at(args, 1);
        tcl_value_t *loop = tcl_list_at(args, 2);
        int r;
        for (;;) {
                r = tcl_eval(tcl, tcl_string(cond), tcl_length(cond) + 1);
                if (r != FNORMAL) {
                        tcl_free(cond);
                        tcl_free(loop);
                        return r;
                }
                if (!(int)tcl_float(tcl->result)) {
                        tcl_free(cond);
                        tcl_free(loop);
                        return FNORMAL;
                }
                int r = tcl_eval(tcl, tcl_string(loop), tcl_length(loop) + 1);
                switch (r) {
                case FBREAK:
                        tcl_free(cond);
                        tcl_free(loop);
                        return FNORMAL;
                case FRETURN:
                        tcl_free(cond);
                        tcl_free(loop);
                        return FRETURN;
                case FAGAIN:
                        continue;
                case FERROR:
                        tcl_free(cond);
                        tcl_free(loop);
                        return FERROR;
                }
        }

        return FERROR;
}

//==============================================================================
/**
 * @brief Command realize math primitives.
 *
 * @param tcl   context container
 * @param args  argument list
 * @param arg   user argument
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
static int tcl_cmd_math(struct tcl *tcl, tcl_value_t *args, void *arg)
{
        (void)arg;

        tcl_value_t *opval = tcl_list_at(args, 0);
        tcl_value_t *aval = tcl_list_at(args, 1);
        tcl_value_t *bval = tcl_list_at(args, 2);

        const char *op = tcl_string(opval);
        float a = tcl_float(aval);
        float b = tcl_float(bval);
        float c = 0.0f;

        if (op[0] == '+')
                c = a + b;
        else if (op[0] == '-')
                c = a - b;
        else if (op[0] == '*')
                c = a * b;
        else if (op[0] == '/')
                c = a / b;
        else if (op[0] == '%')
                c = fmod(a, b);
        else if (op[0] == '>' && op[1] == '\0')
                c = a > b;
        else if (op[0] == '>' && op[1] == '=')
                c = a >= b;
        else if (op[0] == '<' && op[1] == '\0')
                c = a < b;
        else if (op[0] == '<' && op[1] == '=')
                c = a <= b;
        else if (op[0] == '=' && op[1] == '=' && op[2] == '\0')
                c = a == b;
        else if (op[0] == '!' && op[1] == '=')
                c = a != b;
        else if (op[0] == '&' && op[1] == '\0')
                c = (int)a & (int)b;
        else if (op[0] == '|' && op[1] == '\0')
                c = (int)a | (int)b;
        else if (op[0] == '^' && op[1] == '\0')
                c = (int)a ^ (int)b;
        else if (op[0] == '=' && op[1] == '=' && op[2] == '=')
                c = strcmp(tcl_string(aval), tcl_string(bval)) == 0;
        else if (op[0] == '~' && op[1] == '=')
                c = strstr(tcl_string(aval), tcl_string(bval)) != NULL;

        char buf[32];
        snprintf(buf, sizeof(buf), "%f", c);

        tcl_free(opval);
        tcl_free(aval);
        tcl_free(bval);

        return tcl_result(tcl, FNORMAL, tcl_alloc(buf, strlen(buf)));
}

//==============================================================================
/**
 * @brief Function
 *
 * @param s     string to interpret
 * @param n     string length
 * @param from
 * @param to
 * @param c
 *
 * @return One of T* statuses.
 */
//==============================================================================
int tcl_next(const char *s, size_t n, const char **from, const char **to, int *q)
{
        unsigned int i = 0;
        int depth = 0;
        char open;
        char close;

        /* Skip leading spaces if not quoted */
        for (; !*q && n > 0 && tcl_is_space(*s); s++, n--)
                ;
        *from = s;
        /* Terminate command if not quoted */
        if (!*q && n > 0 && tcl_is_end(*s)) {
                *to = s + 1;
                return TCMD;
        }
        if (*s == '$') { /* Variable token, must not start with a space or quote */
                if (tcl_is_space(s[1]) || s[1] == '"') {
                        return TERROR;
                }
                int mode = *q;
                *q = 0;
                int r = tcl_next(s + 1, n - 1, to, to, q);
                *q = mode;
                return ((r == TWORD && *q) ? TPART : r);
        } else if (*s == '[' || (!*q && *s == '{')) {
                /* Interleaving pairs are not welcome, but it simplifies the code */
                open = *s;
                close = (open == '[' ? ']' : '}');
                for (i = 0, depth = 1; i < n && depth != 0; i++) {
                        if (i > 0 && s[i] == open) {
                                depth++;
                        } else if (s[i] == close) {
                                depth--;
                        }
                }
        } else if (*s == '"') {
                *q = !*q;
                *from = *to = s + 1;
                if (*q) {
                        return TPART;
                } else if (n < 2 || (!tcl_is_space(s[1]) && !tcl_is_end(s[1]))) {
                        return TERROR;
                } else {
                        *from = *to = s + 1;
                        return TWORD;
                }
        } else {
                while (i < n && (*q || !tcl_is_space(s[i])) && !tcl_is_special(s[i], *q))
                        i++;
        }
        *to = s + i;
        if (i == n) {
                return TERROR;
        } else if (*q) {
                return TPART;
        } else {
                return (tcl_is_space(s[i]) || tcl_is_end(s[i])) ? TWORD : TPART;
        }
}

//==============================================================================
/**
 * @brief Function return string content of TCL variable.
 *
 * @param v     variable
 *
 * @return String value.
 */
//==============================================================================
const char *tcl_string(const tcl_value_t *v)
{
        return (const char *)v;
}

//==============================================================================
/**
 * @brief Function return float value of selected variable.
 *
 * @param v     variable
 *
 * @return Float value.
 */
//==============================================================================
float tcl_float(tcl_value_t *v)
{
        return strtof((char *)v, NULL);
}

//==============================================================================
/**
 * @brief Function return string length of selected variable.
 *
 * @param v     variable
 *
 * @return Variable length (characters).
 */
//==============================================================================
int tcl_length(tcl_value_t *v)
{
        return v == NULL ? 0 : strlen(v);
}

//==============================================================================
/**
 * @brief Function free selected variable.
 *
 * @param v     variable to free
 */
//==============================================================================
void tcl_free(tcl_value_t *v)
{
        free(v);
}

//==============================================================================
/**
 * @brief Function append string to variable.
 *
 * @param v     variable to append
 * @param s     buffer to append
 * @param len   buffer length
 *
 * @return New appended variable (or NULL if error).
 */
//==============================================================================
tcl_value_t *tcl_append_string(tcl_value_t *v, const char *s, size_t len)
{
        size_t n = tcl_length(v);
        v = realloc(v, n + len + 1);
        if (v) {
                memset((char *)tcl_string(v) + n, 0, len + 1);
                strncpy((char *)tcl_string(v) + n, s, len);
        } else {
                puts("Out of memory!");
        }
        return v;
}

//==============================================================================
/**
 * @brief Function append next variable to chain.
 *
 * @param v     base variable
 * @param tail
 *
 * @return
 */
//==============================================================================
tcl_value_t *tcl_append(tcl_value_t *v, tcl_value_t *tail)
{
        v = tcl_append_string(v, tcl_string(tail), tcl_length(tail));
        tcl_free(tail);
        return v;
}

//==============================================================================
/**
 * @brief Function allocate new buffer as variable.
 *
 * @param s     string
 * @param len   string length
 *
 * @return New TCL value.
 */
//==============================================================================
tcl_value_t *tcl_alloc(const char *s, size_t len)
{
        return tcl_append_string(NULL, s, len);
}

//==============================================================================
/**
 * @brief Function duplicate selecte variable.
 *
 * @param v     variable
 *
 * @return Duplicated variable.
 */
//==============================================================================
tcl_value_t *tcl_dup(tcl_value_t *v)
{
        return tcl_alloc(tcl_string(v), tcl_length(v));
}

//==============================================================================
/**
 * @brief Function allocate new list.
 *
 * @return List variable.
 */
//==============================================================================
tcl_value_t *tcl_list_alloc()
{
        return tcl_alloc("", 0);
}

//==============================================================================
/**
 * @brief Function return list length.
 *
 * @param v     list
 *
 * @return List length.
 */
//==============================================================================
int tcl_list_length(tcl_value_t *v)
{
        int count = 0;
        tcl_each(tcl_string(v), tcl_length(v) + 1, 0)
        {
                if (p.token == TWORD) {
                        count++;
                }
        }
        return count;
}

//==============================================================================
/**
 * @brief Function free selected list.
 *
 * @param v     list
 */
//==============================================================================
void tcl_list_free(tcl_value_t *v)
{
        free(v);
}

//==============================================================================
/**
 * @brief Function return element from list at selected index.
 *
 * @param v     list
 * @param index index
 *
 * @return New variable with selected value.
 */
//==============================================================================
tcl_value_t *tcl_list_at(tcl_value_t *v, int index)
{
        int i = 0;
        tcl_each(tcl_string(v), tcl_length(v) + 1, 0)
        {
                if (p.token == TWORD) {
                        if (i == index) {
                                if (p.from[0] == '{') {
                                        return tcl_alloc(p.from + 1, p.to - p.from - 2);
                                } else {
                                        return tcl_alloc(p.from, p.to - p.from);
                                }
                        }
                        i++;
                }
        }
        return NULL;
}

//==============================================================================
/**
 * @brief Function append new element to list.
 *
 * @param v     list
 * @parma tail
 *
 * @return Updated list.
 */
//==============================================================================
tcl_value_t *tcl_list_append(tcl_value_t *v, tcl_value_t *tail)
{
        if (tcl_length(v) > 0) {
                v = tcl_append(v, tcl_alloc(" ", 2));
        }
        if (tcl_length(tail) > 0) {
                int q = 0;
                const char *p;
                for (p = tcl_string(tail); *p; p++) {
                        if (tcl_is_space(*p) || tcl_is_special(*p, 0)) {
                                q = 1;
                                break;
                        }
                }
                if (q) {
                        v = tcl_append(v, tcl_alloc("{", 1));
                }
                v = tcl_append(v, tcl_dup(tail));
                if (q) {
                        v = tcl_append(v, tcl_alloc("}", 1));
                }
        } else {
                v = tcl_append(v, tcl_alloc("{}", 2));
        }
        return v;
}

//==============================================================================
/**
 * @brief Function get/modify value of selected variable.
 *
 * @param tcl   TCL container
 * @param name  variable name
 * @parma v     new value
 *
 * @return Updated value.
 */
//==============================================================================
tcl_value_t *tcl_var(struct tcl *tcl, tcl_value_t *name, tcl_value_t *v)
{
        DBG("var(%s := %.*s)\n", tcl_string(name), tcl_length(v), tcl_string(v));
        struct tcl_var *var;
        for (var = tcl->env->vars; var != NULL; var = var->next) {
                if (strcmp(var->name, tcl_string(name)) == 0) {
                        break;
                }
        }
        if (var == NULL) {
                var = tcl_env_var(tcl->env, name);
        }
        if (v != NULL) {
                tcl_free(var->value);
                var->value = tcl_dup(v);
                tcl_free(v);
        }
        return var->value;
}

//==============================================================================
/**
 * @brief Function is used to return result of command.
 *
 * @param tcl           TCL container
 * @param flow          flow
 * @param result        result variable
 *
 * @return Flow value.
 */
//==============================================================================
int tcl_result(struct tcl *tcl, int flow, tcl_value_t *result)
{
        DBG("tcl_result %.*s, flow=%d\n", tcl_length(result), tcl_string(result), flow);
        tcl_free(tcl->result);
        tcl->result = result;
        return flow;
}

//==============================================================================
/**
 * @brief Function substitute variable.
 *
 * @param tcl   TCL container
 * @param s     string
 * @param len   string length
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
int tcl_subst(struct tcl *tcl, const char *s, size_t len)
{
        DBG("subst(%.*s)\n", (int)len, s);
        if (len == 0) {
                return tcl_result(tcl, FNORMAL, tcl_alloc("", 0));
        }
        if (s[0] == '{') {
                return tcl_result(tcl, FNORMAL, tcl_alloc(s + 1, len - 2));
        } else if (s[0] == '$') {
                char buf[256] = "set ";
                strncat(buf, s + 1, len - 1);
                return tcl_eval(tcl, buf, strlen(buf) + 1);
        } else if (s[0] == '[') {
                tcl_value_t *expr = tcl_alloc(s + 1, len - 2);
                int r = tcl_eval(tcl, tcl_string(expr), tcl_length(expr) + 1);
                tcl_free(expr);
                return r;
        } else {
                return tcl_result(tcl, FNORMAL, tcl_alloc(s, len));
        }
}

//==============================================================================
/**
 * @brief Function evaluate string as script.
 *
 * @param tcl   TCL container
 * @param s     string to evaluate
 * @param len   string length
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
int tcl_eval(struct tcl *tcl, const char *s, size_t len)
{
        DBG("eval(%.*s)->\n", (int)len, s);
        tcl_value_t *list = tcl_list_alloc();
        tcl_value_t *cur = NULL;
        tcl_each(s, len, 1)
        {
                /*DBG("tcl_next %d %.*s\n", p.token, (int)(p.to - p.from), p.from);*/
                switch (p.token) {
                case TERROR:
                        DBG("eval: FERROR, lexer error\n");
                        return tcl_result(tcl, FERROR, tcl_alloc("", 0));
                case TWORD:
                        DBG("token %.*s, length=%d, cur=%p (3.1.1)\n", (int)(p.to - p.from),
                                p.from, (int)(p.to - p.from), cur);
                        if (cur != NULL) {
                                tcl_subst(tcl, p.from, p.to - p.from);
                                tcl_value_t *part = tcl_dup(tcl->result);
                                cur = tcl_append(cur, part);
                        } else {
                                tcl_subst(tcl, p.from, p.to - p.from);
                                cur = tcl_dup(tcl->result);
                        }
                        list = tcl_list_append(list, cur);
                        tcl_free(cur);
                        cur = NULL;
                        break;
                case TPART:
                        tcl_subst(tcl, p.from, p.to - p.from);
                        tcl_value_t *part = tcl_dup(tcl->result);
                        cur = tcl_append(cur, part);
                        break;
                case TCMD:
                        if (tcl_list_length(list) == 0) {
                                tcl_result(tcl, FNORMAL, tcl_alloc("", 0));
                        } else {
                                tcl_value_t *cmdname = tcl_list_at(list, 0);
                                struct tcl_cmd *cmd = NULL;
                                int r = FERROR;
                                for (cmd = tcl->cmds; cmd != NULL; cmd = cmd->next) {
                                        if (*cmdname == '#') {
                                                r = FNORMAL;
                                                break;
                                        } else if (strcmp(tcl_string(cmdname), tcl_string(cmd->name)) == 0) {
                                                if (cmd->arity == 0 || cmd->arity == tcl_list_length(list)) {
                                                        if (cmd->fn) {
                                                                r = cmd->fn(tcl, list, cmd->arg);
                                                        } else {
                                                                r = FNORMAL;
                                                        }
                                                        break;
                                                }
                                        }
                                }
                                tcl_free(cmdname);
                                if (cmd == NULL || r != FNORMAL) {
                                        tcl_list_free(list);
                                        return r;
                                }
                        }
                        tcl_list_free(list);
                        list = tcl_list_alloc();
                        break;
                }

                if (tcl->exit) {
                        break;
                }
        }
        tcl_list_free(list);
        return FNORMAL;
}

//==============================================================================
/**
 * @brief Function register C function.
 *
 * @param tcl           TCL container
 * @param name          function name in TCL
 * @param fn            function implementation in C
 * @param arity         number of arguments (0 for variable number of args)
 * @param arg           user argument
 *
 * @param One of flow status (Fxx).
 */
//==============================================================================
int tcl_register(struct tcl *tcl, const char *name, tcl_cmd_fn_t fn, int arity,
                  void *arg)
{
        // remove previous function if exists
        struct tcl_cmd *cmd_prev = NULL;

        for (struct tcl_cmd *cmd = tcl->cmds; cmd != NULL; cmd = cmd->next) {

                if (strcmp(cmd->name, tcl_string(name)) == 0) {

                        if (cmd_prev) {
                                cmd_prev = cmd->next;
                        } else {
                                tcl->cmds = cmd->next;
                        }

                        if (cmd->constname == false) {
                                tcl_free((tcl_value_t*)cmd->name);
                        }
                        free(cmd->arg);
                        free(cmd);

                        break;
                }
                cmd_prev = cmd;
        }

        // add new function
        struct tcl_cmd *cmd = malloc(sizeof(struct tcl_cmd));
        if (cmd) {
                cmd->name = tcl_alloc(name, strlen(name));

                if (cmd->name) {
                        cmd->constname = false;
                        cmd->fn = fn;
                        cmd->arg = arg;
                        cmd->arity = arity;
                        cmd->next = tcl->cmds;
                        tcl->cmds = cmd;

                        return FNORMAL;
                } else {
                        free(cmd);
                }
        }

        return FERROR;
}

//==============================================================================
/**
 * @brief Function register C function with constant name.
 *
 * @param tcl           TCL container
 * @param name          function name in TCL
 * @param fn            function implementation in C
 * @param arity         number of arguments (0 for variable number of args)
 * @param arg           user argument
 *
 * @param One of flow status (Fxx).
 */
//==============================================================================
int tcl_register_const(struct tcl *tcl, const char *name, tcl_cmd_fn_t fn,
                        int arity, void *arg)
{
        struct tcl_cmd *cmd = malloc(sizeof(struct tcl_cmd));
        if (cmd) {
                cmd->name = name;
                cmd->constname = true;
                cmd->fn = fn;
                cmd->arg = arg;
                cmd->arity = arity;
                cmd->next = tcl->cmds;
                tcl->cmds = cmd;

                return FNORMAL;
        }

        return FERROR;
}

//==============================================================================
/**
 * @brief Function initialize TCL container.
 *
 * @param tcl   container to initialize
 */
//==============================================================================
int tcl_init(struct tcl *tcl)
{
        memset(tcl, 0, sizeof(struct tcl));

        tcl->env = tcl_env_alloc(NULL);
        if (!tcl->env) goto error;

        tcl->result = tcl_alloc("", 0);
        if (!tcl->result) goto error;

        tcl->cmds = NULL;
        tcl->exit = 0;
        if (tcl_register_const(tcl, "set", tcl_cmd_set, 0, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "unset", tcl_cmd_unset, 0, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "subst", tcl_cmd_subst, 2, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "puts", tcl_cmd_puts, 2, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "gets", tcl_cmd_gets, 1, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "proc", tcl_cmd_proc, 4, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "unproc", tcl_cmd_unproc, 0, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "if", tcl_cmd_if, 0, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "while", tcl_cmd_while, 3, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "return", tcl_cmd_flow, 0, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "break", tcl_cmd_flow, 1, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "continue", tcl_cmd_flow, 1, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "eval", tcl_cmd_eval, 2, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "source", tcl_cmd_source, 2, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "exit", tcl_cmd_exit, 1, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "printf", tcl_cmd_printf, 0, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "llength", tcl_cmd_llength, 2, NULL) != FNORMAL) goto error;
        if (tcl_register_const(tcl, "lindex", tcl_cmd_lindex, 0, NULL) != FNORMAL) goto error;

        static const char *math[] = {"+", "-", "*", "/", "%", ">", ">=", "<",
                                     "<=", "==", "!=", "&", "|", "^", "===", "~="};
        for (size_t i = 0; i < (sizeof(math) / sizeof(math[0])); i++) {
                if (tcl_register_const(tcl, math[i], tcl_cmd_math, 3, NULL) != FNORMAL) goto error;
        }

        return FNORMAL;

        error:
        tcl_destroy(tcl);
        return FERROR;
}

//==============================================================================
/**
 * @brief Function destroy TCL container.
 *
 * @param tcl   TCL container to destroy
 */
//==============================================================================
void tcl_destroy(struct tcl *tcl)
{
        while (tcl->env) {
                tcl->env = tcl_env_free(tcl->env);
        }
        while (tcl->cmds) {
                struct tcl_cmd *cmd = tcl->cmds;
                tcl->cmds = tcl->cmds->next;
                if (cmd->constname == false) {
                        tcl_free((tcl_value_t*)cmd->name);
                }
                free(cmd->arg);
                free(cmd);
        }
        tcl_free(tcl->result);

        DBG("DBG: Exit memory usage: %ld\n", used_mem);
        DBG("DBG: Max memory usage: %ld\n", used_mem_max);
}

//==============================================================================
/**
 * @brief Function load selected file as script to interpret.
 *
 * @param filename      file name to open
 *
 * @return One of flow status (Fxx).
 */
//==============================================================================
int tcl_loadfile(struct tcl *tcl, const char *filename)
{
        FILE *f = fopen(filename, "r");
        if (!f) {
                perror(filename);
                return FERROR;
        }

        int   buflen = CHUNK_LEN;
        char *buf    = calloc(1, buflen);
        int   i      = 0;

        while (buf && f && !tcl->exit) {
                int c = fgetc(f);

                if (i > buflen - 1) {
                        buf = realloc(buf, buflen += CHUNK_LEN);
                        memset(buf + i, 0, buflen - i);
                }

                if (c == 0 || c == EOF) {
                        break;
                }

                buf[i++] = c;

                tcl_each(buf, i, 1) {

                        if (p.token == TERROR && (p.to - buf) != i) {

                                memset(buf, 0, buflen);
                                i = 0;
                                break;

                        } else if (p.token == TCMD && *(p.from) != '\0') {

                                int r = tcl_eval(tcl, buf, strlen(buf));
                                if (r == FERROR) {
                                        printf("Chunk error: %s:\n%s\n", filename, buf);
                                        tcl->exit = 1;
                                        break;
                                }

                                memset(buf, 0, buflen);
                                i = 0;
                                break;
                        }
                }
        }

        free(buf);

        fclose(f);

        if (i) {
                printf("TCL: incomplete input\n");
                return FERROR;
        }

        return FNORMAL;
}

/*==============================================================================
  End of file
==============================================================================*/
