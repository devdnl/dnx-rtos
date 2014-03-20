/*=========================================================================*//**
@file    thread.h

@author  Daniel Zorychta

@brief   Library with functions which operate on threads.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/kwrapper.h"
#include "core/progman.h"
#include "core/sysmoni.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/
//==============================================================================
/**
 * @brief task_t *task_new(void (*func)(void*), const char *name, const uint stack_depth, void *arg)
 * The function <b>task_new</b>() creates new task in system with function
 * pointed by <i>func</i>, and name pointed by <i>name</i>. Task is created with
 * user-defined stack size determined by <i>stack_depth</i>. An additional
 * arguments can be passed by value pointed by <i>arg</i>. If task is created
 * then valid object is returned, on error <b>NULL</b>.
 *
 * @param[in ] func            task code
 * @param[in ] name            task name
 * @param[in ] stack_depth     stack deep
 * @param[in ] arg             argument pointer
 *
 * @errors None
 *
 * @return On success, valid object is returned. On error, <b>NULL</b> is
 * returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void my_task(void *arg)
 * {
 *         // task code
 *
 *         task_exit();
 * }
 *
 * void some_function()
 * {
 *        task_t *task = task_new(my_task,
 *                                "My task",
 *                                STACK_DEPTH_LOW,
 *                                NULL);
 *
 *        if (task) {
 *                 // task created ...
 *        } else {
 *                 // task not created ...
 *        }
 *
 *        // ...
 * }
 */
//==============================================================================
static inline task_t *task_new(void (*func)(void*), const char *name, const uint stack_depth, void *arg)
{
        return _task_new(func, name, stack_depth, arg);
}

//==============================================================================
/**
 * @brief void task_delete(task_t *taskhdl)
 * The function <b>task_delete</b>() kills task, and delete object of created task.
 *
 * @param taskhdl       task handler
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 * #include <unistd.h>
 *
 * void my_task(void *arg)
 * {
 *         while (true) {
 *                 // task do something (e.g. LED blinking)
 *         }
 * }
 *
 * void some_function()
 * {
 *        task_t *task = task_new(my_task,
 *                                "My task",
 *                                STACK_DEPTH_LOW,
 *                                NULL);
 *
 *        if (task) {
 *                 // task "My task" is created and working
 *
 *                 // code here do something for few seconds ...
 *
 *                 task_delete(task);
 *        } else {
 *                 // task not created ...
 *        }
 * }
 */
//==============================================================================
static inline void task_delete(task_t *taskhdl)
{
        return _task_kill(taskhdl);
}

//==============================================================================
/**
 * @brief void task_exit(void)
 * The function <b>task_exit</b>() terminate task which call this function.
 *
 * @param None
 *
 * @errors None
 *
 * @return This function does not return.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void my_task(void *arg)
 * {
 *         // task code
 *
 *         // task finished specified work and can be terminated
 *         task_exit();
 * }
 */
//==============================================================================
static inline void task_exit(void)
{
        _task_exit();
}

//==============================================================================
/**
 * @brief void task_suspend(task_t *taskhdl)
 * The function <b>task_suspend</b>() switch selected task <i>taskhdl</i> to
 * suspend state. From this state task can be switched using <b>task_resume</b>()
 * function. Suspended task does not use CPU time.
 *
 * @param taskhdl       task handler
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * void my_task(void *arg)
 * {
 *         while (true) {
 *                 // task do something (e.g. LED blinking)
 *         }
 * }
 *
 * void some_function()
 * {
 *        task_t *task = task_new(my_task,
 *                                "My task",
 *                                STACK_DEPTH_LOW,
 *                                NULL);
 *
 *        if (task) {
 *                 // task is suspended and will be used later
 *                 task_suspend(task);
 *        } else {
 *                 // task not created ...
 *                 return;
 *        }
 *
 *        // some code ...
 *
 *        // task is necessary, so will be resumed
 *        task_resume(task);
 *
 *        // some work ...
 *
 *        task_delete(task);
 * }
 */
//==============================================================================
static inline void task_suspend(task_t *taskhdl)
{
        _task_suspend(taskhdl);
}

//==============================================================================
/**
 * @brief void task_suspend_now(void)
 * The function <b>task_suspend_now</b>() switch current task (calling) to
 * suspend state.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * void my_task(void *arg)
 * {
 *         while (true) {
 *                 // task do something
 *
 *                 // task will be suspended and will wait for resume from
 *                 // other task or event (IRQ)
 *                 task_suspend_now();
 *         }
 * }
 */
//==============================================================================
static inline void task_suspend_now(void)
{
        _task_suspend_now();
}

//==============================================================================
/**
 * @brief void task_resume(task_t *taskhdl)
 * The function <b>task_resume</b>() switch selected task <i>taskhdl</i> to
 * run state.
 *
 * @param taskhdl       task handler
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * void my_task(void *arg)
 * {
 *         while (true) {
 *                 // task do something (e.g. LED blinking)
 *         }
 * }
 *
 * void some_function()
 * {
 *        task_t *task = task_new(my_task,
 *                                "My task",
 *                                STACK_DEPTH_LOW,
 *                                NULL);
 *
 *        if (task) {
 *                 // task is suspended and will be used later
 *                 task_suspend(task);
 *        } else {
 *                 // task not created ...
 *                 return;
 *        }
 *
 *        // some code ...
 *
 *        // task is necessary, so will be resumed
 *        task_resume(task);
 *
 *        // some work ...
 *
 *        task_delete(task);
 * }
 */
//==============================================================================
static inline void task_resume(task_t *taskhdl)
{
        _task_resume(taskhdl);
}

//==============================================================================
/**
 * @brief bool task_resume_from_ISR(task_t *taskhdl)
 * The function <b>task_resume_from_ISR</b>() is similar to <b>task_resume</b>()
 * except that can be called from interrupt routine.
 *
 * @param taskhdl       task handler
 *
 * @errors None
 *
 * @return If task shall be yielded then <b>true</b> is returned. Otherwise,
 * <b>false</b> is returned. Task yield behavior depends on used port.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         while (true) {
 *                 // task do something
 *
 *                 // task will be suspended and will wait for resume from IRQ
 *                 task_suspend_now();
 *         }
 * }
 *
 * void IRQ(void)
 * {
 *         // ...
 *
 *         if (task_resume_form_ISR(task)) {
 *                 task_yield_from_ISR();
 *         }
 * }
 */
//==============================================================================
static inline bool task_resume_from_ISR(task_t *taskhdl)
{
        return _task_resume_from_ISR(taskhdl);
}

//==============================================================================
/**
 * @brief void task_yield(void)
 * The function <b>task_yield</b>() force context switch. To release CPU for
 * other task use better <b>sleep</b>() family functions, because you can
 * control release time.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * void my_task(void *arg)
 * {
 *         while (true) {
 *                 // task do something
 *
 *                 // task will be suspended and will wait for resume from IRQ
 *                 task_yield();
 *         }
 * }
 */
//==============================================================================
static inline void task_yield(void)
{
        _task_yield();
}

//==============================================================================
/**
 * @brief void task_yield_from_ISR(void)
 * The function <b>task_yield_from_ISR</b>() force context switch from interrupt
 * routine. Some ports need force context switch before ISR API functions calls.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         while (true) {
 *                 // task do something
 *
 *                 // task will be suspended and will wait for resume from IRQ
 *                 task_suspend_now();
 *         }
 * }
 *
 * void IRQ(void)
 * {
 *         // ...
 *
 *         bool yield = task_resume_form_ISR(task);
 *
 *         // ...
 *
 *         if (yield) {
 *                 task_yield_from_ISR();
 *         }
 * }
 */
//==============================================================================
static inline void task_yield_from_ISR(void)
{
        _task_yield_from_ISR();
}

//==============================================================================
/**
 * @brief char *task_get_name(void)
 * The function <b>task_get_name</b>() returns name of task which calls function.
 *
 * @param None
 *
 * @errors None
 *
 * @return Task name string.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         char *name = task_get_name();
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline char *task_get_name(void)
{
        return _task_get_name();
}

//==============================================================================
/**
 * @brief char *task_get_name_of(task_t *taskhdl)
 * The function <b>task_get_name_of</b>() returns name of selected task pointed
 * by <i>taskhdl</i>.
 *
 * @param taskhdl       task handler
 *
 * @errors None
 *
 * @return Task name string.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         char *name = task_get_name();
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 *
 * void some_function()
 * {
 *         char *name = task_get_name_of(task);
 *
 *         printf("The name of task is %s\n", name);
 * }
 */
//==============================================================================
static inline char *task_get_name_of(task_t *taskhdl)
{
        return _task_get_name_of(taskhdl);
}

//==============================================================================
/**
 * @brief int task_get_priority(void)
 * The function <b>task_get_priority</b>() returns priority value of task which
 * calls function.
 *
 * @param None
 *
 * @errors None
 *
 * @return Priority value.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         if (task_get_priority() < 0) {
 *                 task_set_priority(0);
 *         }
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline int task_get_priority(void)
{
        return _task_get_priority();
}

//==============================================================================
/**
 * @brief int task_get_priority_of(task_t *taskhdl)
 * The function <b>task_get_priority_of</b>() returns priority value of selected
 * task pointed by <i>taskhdl</i>.
 *
 * @param taskhdl       task handler
 *
 * @errors None
 *
 * @return Priority value;
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         char *name = task_get_name();
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 *
 * void some_function()
 * {
 *         if (task_get_priority_of(task) < 0) {
 *                 task_set_priority_of(task, 0);
 *         }
 * }
 */
//==============================================================================
static inline int task_get_priority_of(task_t *taskhdl)
{
        return _task_get_priority_of(taskhdl);
}

//==============================================================================
/**
 * @brief void task_set_priority(const int priority)
 * The function <b>task_set_priority</b>() set priority of task which calls the
 * function to <i>priority</i>.
 *
 * @param priority      new priority value
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         if (task_get_priority() < 0) {
 *                 task_set_priority(0);
 *         }
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline void task_set_priority(const int priority)
{
        _task_set_priority(priority);
}

//==============================================================================
/**
 * @brief void task_set_priority_of(task_t *taskhdl, const int priority)
 * The function <b>task_set_priority_of</b>() set priority value of selected
 * task pointed by <i>taskhdl</i> to <i>priority</i>.
 *
 * @param taskhdl       task handler
 * @param priority      new priority value
 *
 * @errors None
 *
 * @return Priority value;
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         char *name = task_get_name();
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 *
 * void some_function()
 * {
 *         if (task_get_priority_of(task) < 0) {
 *                 task_set_priority_of(task, 0);
 *         }
 * }
 */
//==============================================================================
static inline void task_set_priority_of(task_t *taskhdl, const int priority)
{
        _task_set_priority_of(taskhdl, priority);
}

//==============================================================================
/**
 * @brief int task_get_free_stack(void)
 * The function <b>task_get_free_stack</b>() returns free stack value of
 * task which calls the function.
 *
 * @param None
 *
 * @errors None
 *
 * @return Free stack levels.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * void my_task(void *arg)
 * {
 *         free_stack = task_get_free_stack();
 *
 *         if (free_stack < 50) {
 *                 // ...
 *         }
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline int task_get_free_stack(void)
{
        return _task_get_free_stack();
}

//==============================================================================
/**
 * @brief int task_get_free_stack_of(task_t *taskhdl)
 * The function <b>task_get_free_stack_of</b>() returns the free stack value
 * of task pointed by <i>taskhdl</i>. If task object is not valid then -1 is
 * returned.
 *
 * @param taskhdl       task handler
 *
 * @errors None
 *
 * @return Free stack level or -1 on error.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         char *name = task_get_name();
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 *
 * void some_function()
 * {
 *         printf("Task free stack is %d levels\n", task_get_free_stack_of(task));
 * }
 */
//==============================================================================
static inline int task_get_free_stack_of(task_t *taskhdl)
{
        return _task_get_free_stack_of(taskhdl);
}

//==============================================================================
/**
 * @brief task_t *task_get_handle(void)
 * The function <b>task_get_handle</b>() returns pointer to the object of task
 * which calls the function.
 *
 * @param None
 *
 * @errors None
 *
 * @return Object of task which calls the function.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         task_t *hdl = task_get_handle();
 *         // ...
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline task_t *task_get_handle(void)
{
        return _task_get_handle();
}

//==============================================================================
/**
 * @brief task_t *task_get_parent_handle(void)
 * The function <b>task_get_parent_handle</b>() returns pointer to the object of
 * parent of current task.
 *
 * @param None
 *
 * @errors None
 *
 * @return Task handle of parent of the current task, or <b>NULL</b> if parent
 * does not exist.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * task_t *task;
 *
 * void my_task(void *arg)
 * {
 *         task_t *parhdl = task_get_parent_handle();
 *         // ...
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline task_t *task_get_parent_handle(void)
{
        return _task_get_parent_handle();
}

//==============================================================================
/**
 * @brief void task_set_stdin(FILE *stream)
 * The function <b>task_set_stdin</b>() set <b>stdin</b> stream for task which
 * calls the function.
 *
 * @param stream        stream to set as <b>stdin</b>
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 * #include <stdio.h>
 *
 * void my_task(void *arg)
 * {
 *         FILE *std_in = arg;
 *
 *         char c = getc(stdin);
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline void task_set_stdin(FILE *stream)
{
        _task_set_stdin(stream);
}

//==============================================================================
/**
 * @brief void task_set_stdout(FILE *stream)
 * The function <b>task_set_stdout</b>() set <b>stdout</b> stream for task which
 * calls the function.
 *
 * @param stream        stream to set as <b>stdout</b>
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 * #include <stdio.h>
 *
 * void my_task(void *arg)
 * {
 *         FILE *std_out = arg;
 *
 *         task_set_stdout(std_out);
 *         puts("stdio is configured");
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline void task_set_stdout(FILE *stream)
{
        _task_set_stdout(stream);
}

//==============================================================================
/**
 * @brief void task_set_stderr(FILE *stream)
 * The function <b>task_set_stderr</b>() set <b>stderr</b> stream for task which
 * calls the function.
 *
 * @param stream        stream to set as <b>stderr</b>
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 * #include <stdio.h>
 *
 * void my_task(void *arg)
 * {
 *         FILE *std_err = arg;
 *
 *         task_set_stderr(std_err);
 *         fputs("stderr configured\n", stderr);
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline void task_set_stderr(FILE *stream)
{
        _task_set_stderr(stream);
}

//==============================================================================
/**
 * @brief void task_set_cwd(const char *str)
 * The function <b>task_set_cwd</b>() set current working directory for task
 * which calls the function.
 *
 * @param str       path to set as current working directory
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 * #include <stdio.h>
 *
 * void my_task(void *arg)
 * {
 *         const char *cwd = arg;
 *
 *         task_set_cwd(cwd);
 *
 *         while (true) {
 *                 // task do something
 *         }
 * }
 */
//==============================================================================
static inline void task_set_cwd(const char *str)
{
        _task_set_cwd(str);
}

//==============================================================================
/**
 * @brief bool task_is_exist(task_t *taskhdl)
 * The function <b>task_is_exist</b>() check if task pointed by <i>taskhdl</i>
 * exist in system. If task exist then <b>true</b> is returned, otherwise
 * <b>false</b>.
 *
 * @param taskhdl       task handle
 *
 * @errors ESRCH
 *
 * @return If task exist then <b>true</b> is returned, otherwise <b>false</b>.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * task_t *task;
 *
 * // ...
 *
 * if (task_is_exist(task)) {
 *         // ...
 * } else {
 *         // ...
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool task_is_exist(task_t *taskhdl)
{
        return sysm_is_task_exist(taskhdl);
}

//==============================================================================
/**
 * @brief thread_t *thread_new(void (*func)(void*), const int stack_depth, void *arg)
 * The function <b>thread_new</b>() creates new thread using function pointed
 * by <i>func</i> with stack depth <i>stack_depth</i>. To thread can be passed
 * additional argument pointed by <i>arg</i>. If thread was created then
 * pointer to object is returned, otherwise <b>NULL</b> is returned, and
 * <b>errno</b> set appropriately. Threads are functions which are called as
 * new task and have own stack, but global variables are shared with parent
 * thread.
 *
 * @param func          thread function
 * @param stack_depth   stack depth
 * @param arg           thread argument
 *
 * @errors EINVAL, ENOMEM,
 *
 * @return If thread was created then pointer to object is returned, otherwise
 * <b>NULL</b> is returned, and <b>errno</b> set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * void thread(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void some_function()
 * {
 *         errno = 0;
 *         thread_t *thread = thread_new(thread, STACK_DEPTH_LOW, NULL);
 *         if (thread) {
 *                 // some code ...
 *
 *                 while (!thread_is_finished(thread)) {
 *                         sleep_ms(1);
 *                 }
 *
 *                 thread_delete(thread);
 *         } else {
 *                 perror("Thread error");
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline thread_t *thread_new(void (*func)(void*), const int stack_depth, void *arg)
{
        return _thread_new(func, stack_depth, arg);
}

//==============================================================================
/**
 * @brief int thread_join(thread_t *thread)
 * The function <b>thread_join</b>() joins selected thread pointed by
 * <i>thread</i> to parent program. Function wait until thread was closed.
 *
 *
 * @param thread        thread object
 *
 * @errors ETIME, EINVAL
 *
 * @return On success, 0 is returned. On error, 1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * void thread1(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void thread2(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void some_function()
 * {
 *         errno = 0;
 *         thread_t *thread1 = thread_new(thread1, STACK_DEPTH_LOW, NULL);
 *         thread_t *thread2 = thread_new(thread2, STACK_DEPTH_LOW, NULL);
 *
 *         if (thread1 && thread2) {
 *                 // some code ...
 *
 *                 thread_join(thread1);
 *                 thread_join(thread2);
 *
 *                 thread_delete(thread1);
 *                 thread_delete(thread2);
 *         } else {
 *                 perror("Thread error");
 *
 *                 if (thread1) {
 *                         thread_delete(thread1);
 *                 }
 *
 *                 if (thread2) {
 *                         thread_delete(thread2);
 *                 }
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline int thread_join(thread_t *thread)
{
        return _thread_join(thread);
}

//==============================================================================
/**
 * @brief int thread_cancel(thread_t *thread)
 * The function <b>thread_cancel</b>() kills running thread.
 *
 * @param thread        thread object
 *
 * @errors EINVAL
 *
 * @return On success, 0 is returned. On error, 1 is returned, and <b>errno</b>
 * is set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * void thread(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void some_function()
 * {
 *         errno = 0;
 *         thread_t *thread = thread_new(thread1, STACK_DEPTH_LOW, NULL);
 *
 *         if (thread) {
 *                 // some code ...
 *
 *                 while (thread_cancel(thread) != true) {
 *                         sleep_ms(10);
 *                 }
 *
 *                 thread_delete(thread);
 *         } else {
 *                 perror("Thread error");
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline int thread_cancel(thread_t *thread)
{
        return _thread_cancel(thread);
}

//==============================================================================
/**
 * @brief bool thread_is_finished(thread_t *thread)
 * The function <b>thread_is_finished</b>() examine that selected thread pointed
 * by <i>thread</i> is finished. If thread is finished then <b>true</b> is
 * returned, otherwise <b>false</b>. Function can be used to poll that selected
 * thread is finished. If would you like to wait for thread close, then use
 * <b>thread_join</b>() instead.
 *
 * @param thread        thread object
 *
 * @errors EINVAL
 *
 * @return If thread is finished then <b>true</b> is returned, otherwise
 * <b>false</b>, and <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * void thread(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void some_function()
 * {
 *         errno = 0;
 *         thread_t *thread = thread_new(thread, STACK_DEPTH_LOW, NULL);
 *         if (thread) {
 *                 // some code ...
 *
 *                 while (!thread_is_finished(thread)) {
 *                         sleep_ms(1);
 *                 }
 *
 *                 thread_delete(thread);
 *         } else {
 *                 perror("Thread error");
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool thread_is_finished(thread_t *thread)
{
        return _thread_is_finished(thread);
}

//==============================================================================
/**
 * @brief int thread_delete(thread_t *thread)
 * The function <b>thread_delete</b>() removes unused thread object pointed by
 * <i>thread</i>. This function can delete object only if thread is finished.
 * To kill running thread use <b>thread_cancel</b>() function. If thread object
 * was deleted then function returns 0, otherwise 1.
 *
 * @param thread        thread object
 *
 * @errors EINVAL, EAGAIN
 *
 * @return If thread object was deleted then function returns 0, otherwise 1, and
 * <b>errno</b> is set appropriately.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <unistd.h>
 *
 * // ...
 *
 * void thread(void *arg)
 * {
 *         // ...
 *
 *         // thread function exit without any function,
 *         // or just by return
 * }
 *
 * void some_function()
 * {
 *         errno = 0;
 *         thread_t *thread = thread_new(thread, STACK_DEPTH_LOW, NULL);
 *         if (thread) {
 *                 // some code ...
 *
 *                 thread_join(thread);
 *
 *                 thread_delete(thread);
 *         } else {
 *                 perror("Thread error");
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline int thread_delete(thread_t *thread)
{
        return _thread_delete(thread);
}

//==============================================================================
/**
 * @brief sem_t *semaphore_new(const uint cnt_max, const uint cnt_init)
 * The function <b>semaphore_new</b>() creates new semaphore object. The
 * semaphore can be counting or binary. If counting then <i>cnt_max</i>
 * is bigger that 2. <i>cnt_init</i> is an initial value of semaphore.
 * Semaphore can be used for task synchronization.
 *
 * @param cnt_max       max count value (1 for binary)
 * @param cnt_init      initial value (0 or 1 for binary)
 *
 * @errors None
 *
 * @return On success, semaphore object is returned. On error, <b>NULL</b> is
 * returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // ...
 *
 * void thread2(void *arg)
 * {
 *         while (true) {
 *                 // this task will wait for semaphore signal
 *                 semaphore_wait(sem, MAX_DELAY_MS);
 *
 *                 // ...
 *         }
 * }
 *
 * void thread1(void *arg)
 * {
 *         while (true) {
 *                // ...
 *
 *                // this task signal to thread2 that can execute part of code
 *                semaphore_signal(sem);
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline sem_t *semaphore_new(const uint cnt_max, const uint cnt_init)
{
        return _semaphore_new(cnt_max, cnt_init);
}

//==============================================================================
/**
 * @brief void semaphore_delete(sem_t *sem)
 * The function <b>semaphore_delete</b>() removes created semaphore pointed by
 * <i>sem</i>. Be aware that if semaphore was removed when tasks use it, then
 * process starvation can occur on tasks which wait for semaphore signal.
 *
 * @param sem       semaphore object pointer
 *
 * @errors None
 *
 * @return
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // operation on semaphore
 *
 * semaphore_delete(sem);
 *
 * // ...
 */
//==============================================================================
static inline void semaphore_delete(sem_t *sem)
{
        _semaphore_delete(sem);
}

//==============================================================================
/**
 * @brief bool semaphore_wait(sem_t *sem, const uint timeout)
 * The function <b>semaphore_wait</b>() waits for semaphore signal pointed by
 * <i>sem</i> by <i>timeout</i> milliseconds. If semaphore was signaled then
 * <b>true</b> is returned, otherwise (timeout) <b>false</b>. When <i>timeout</i>
 * value is set to 0 then semaphore is polling without timeout.
 *
 * @param sem       semaphore object pointer
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On timeout or if semaphore is
 * not signaled <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // ...
 *
 * void thread2(void *arg)
 * {
 *         while (true) {
 *                 // this task will wait for semaphore signal
 *                 semaphore_wait(sem, MAX_DELAY_MS);
 *
 *                 // ...
 *         }
 * }
 *
 * void thread1(void *arg)
 * {
 *         while (true) {
 *                // ...
 *
 *                // this task signal to thread2 that can execute part of code
 *                semaphore_signal(sem);
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool semaphore_wait(sem_t *sem, const uint timeout)
{
        return _semaphore_take(sem, timeout);
}

//==============================================================================
/**
 * @brief bool semaphore_signal(sem_t *sem)
 * The function <b>semaphore_signal</b>() signals semaphore pointed by <i>sem</i>.
 *
 * @param sem       semaphore object pointer
 *
 * @errors None
 *
 * @return On corrected signaling, <b>true</b> is returned. If semaphore cannot
 * be signaled or object is invalid then <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // ...
 *
 * void thread2(void *arg)
 * {
 *         while (true) {
 *                 // this task will wait for semaphore signal
 *                 semaphore_wait(sem, MAX_DELAY_MS);
 *
 *                 // ...
 *         }
 * }
 *
 * void thread1(void *arg)
 * {
 *         while (true) {
 *                // ...
 *
 *                // this task signal to thread2 that can execute part of code
 *                semaphore_signal(sem);
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool semaphore_signal(sem_t *sem)
{
        return _semaphore_give(sem);
}

//==============================================================================
/**
 * @brief bool semaphore_wait_from_ISR(sem_t *sem, bool *task_woken)
 * The function <b>semaphore_wait_from_ISR</b>() is similar to <b>semaphore_wait</b>()
 * except that can be called from interrupt. Function have limited application.
 *
 * @param sem           semaphore object pointer
 * @param task_woken    <b>true</b> if task woken, otherwise <b>false</b>. Can be <b>NULL</b>
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On timeout or if semaphore is
 * not signaled <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // ...
 *
 * void thread1(void *arg)
 * {
 *         while (true) {
 *                 if (...) {
 *                         semaphore_signal(sem);
 *                 }
 *
 *                 // ...
 *         }
 * }
 *
 * void ISR(void)
 * {
 *         // ...
 *
 *         bool woken = false;
 *         if (semaphore_wait_from_ISR(sem, &woken)) {
 *                 // ...
 *         }
 *
 *         if (woken) {
 *                 task_yield_from_ISR();
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool semaphore_wait_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_take_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief bool semaphore_signal_from_ISR(sem_t *sem, bool *task_woken)
 * The function <b>semaphore_signal_from_ISR</b>() is similar to
 * <b>semaphore_signal</b>() except that can be called from interrupt.
 *
 * @param sem           semaphore object pointer
 * @param task_woken    <b>true</b> if task woken, otherwise <b>false</b>. Can be <b>NULL</b>
 *
 * @errors None
 *
 * @return On corrected signaling, <b>true</b> is returned. If semaphore cannot
 * be signaled or object is invalid then <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 * #include <stdbool.h>
 *
 * // ...
 *
 * sem_t *sem = semaphore_new(1, 0);
 *
 * // ...
 *
 * void thread1(void *arg)
 * {
 *         while (true) {
 *                 // this task will wait for semaphore signal
 *                 semaphore_wait(sem, MAX_DELAY_MS);
 *
 *                 // ...
 *         }
 * }
 *
 * void ISR(void)
 * {
 *         // ...
 *
 *         bool woken = false;
 *         semaphore_signal_from_ISR(sem, &woken);
 *
 *         if (woken) {
 *                 task_yield_from_ISR();
 *         }
 * }
 *
 * // ...
 */
//==============================================================================
static inline bool semaphore_signal_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_give_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief mutex_t *mutex_new(enum mutex_type type)
 * The function <b>mutex_new</b>() creates new mutex of type <i>type</i>.
 * Two types of mutex can be created: <b>MUTEX_RECURSIVE</b> and <b>MUTEX_NORMAL</b>.
 *
 * @param type          mutex type (MUTEX_RECURSIVE or MUTEX_NORMAL)
 *
 * @errors None
 *
 * @return On success, pointer to the mutex object is returned. On error,
 * <b>NULL</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, mtx);
 *         thread_new(thread2, STACK_DEPTH_LOW, mtx);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline mutex_t *mutex_new(enum mutex_type type)
{
        return _mutex_new(type);
}

//==============================================================================
/**
 * @brief void mutex_delete(mutex_t *mutex)
 * The function <b>mutex_delete</b>() delete created mutex pointed by <i>mutex</i>.
 *
 * @param mutex     mutex
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, mtx);
 *         thread_new(thread2, STACK_DEPTH_LOW, mtx);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void mutex_delete(mutex_t *mutex)
{
        _mutex_delete(mutex);
}

//==============================================================================
/**
 * @brief bool mutex_lock(mutex_t *mutex, const uint timeout)
 * The function <b>mutex_lock</b>() lock mutex pointed by <i>mutex</i>. If
 * mutex is locked by other thread then system try to lock mutex by <i>timeout</i>
 * milliseconds. If mutex is recursive then task can lock mutex recursively, and
 * the same times shall be unlocked. If normal mutex is used then task can lock
 * mutex only one time (not recursively).
 *
 * @param mutex     mutex
 * @param timeout   timeout
 *
 * @errors None
 *
 * @return If mutex is locked then <b>true</b> is returned. If mutex is used or
 * timeout occur or object is incorrect, then <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, mtx);
 *         thread_new(thread2, STACK_DEPTH_LOW, mtx);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool mutex_lock(mutex_t *mutex, const uint timeout)
{
        return _mutex_lock(mutex, timeout);
}

//==============================================================================
/**
 * @brief bool mutex_unlock(mutex_t *mutex)
 * The function <b>mutex_unlock</b>() unlock mutex pointed by <i>mutex</i>.
 *
 * @param mutex     mutex
 *
 * @errors None
 *
 * @return If mutex is unlocked then <b>true</b> is returned. If mutex is not
 * unlocked or object is incorrect then <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * int resource;
 *
 * void thread1(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * void thread2(void *arg)
 * {
 *         mutex_t *mtx = arg;
 *
 *         // protected access to resource
 *         if (mutex_lock(mtx, MAX_DELAY_MS)) {
 *                 // write to buffer is allowed
 *                 resource = ...;
 *
 *                 // ...
 *
 *                 mutex_unlock(mtx);
 *         }
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         mutex_t *mtx = mutex_new(MUTEX_NORMAL);
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, mtx);
 *         thread_new(thread2, STACK_DEPTH_LOW, mtx);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         mutex_delete(mtx);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool mutex_unlock(mutex_t *mutex)
{
        return _mutex_unlock(mutex);
}

//==============================================================================
/**
 * @brief queue_t *queue_new(const uint length, const uint item_size)
 * The function <b>queue_new</b>() create new queue with length <i>length</i>
 * of item size <i>item_size</i>. Returns pointer to the created object or
 * <b>NULL</b> on error. Both, <i>length</i> and <i>item_size</i> cannot be zero.
 *
 * @param length        queue length
 * @param item_size     size of item
 *
 * @errors None
 *
 * @return On success returns pointer to the created object or <b>NULL</b> on
 * error.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline queue_t *queue_new(const uint length, const uint item_size)
{
        return _queue_new(length, item_size);
}

//==============================================================================
/**
 * @brief void queue_delete(queue_t *queue)
 * The function <b>queue_delete</b>() deletes the created queue pointed by
 * <i>queue</i>. Make sure that neither task use queue before delete.
 *
 * @param queue     queue object
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void queue_delete(queue_t *queue)
{
        _queue_delete(queue);
}

//==============================================================================
/**
 * @brief void queue_reset(queue_t *queue)
 * The function <b>queue_delete</b>() reset the selected queue pointed by
 * <i>queue</i>.
 *
 * @param queue     queue object
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         queue_reset(queue);
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void queue_reset(queue_t *queue)
{
        _queue_reset(queue);
}

//==============================================================================
/**
 * @brief bool queue_send(queue_t *queue, const void *item, const uint timeout)
 * The function <b>queue_send</b>() send specified item pointed by <i>item</i>
 * to queue pointed by <i>queue</i>. If queue is full then system try to send
 * item for <i>timeout</i> milliseconds. If <i>timeout</i> is set to zero then
 * sending is aborted immediately if queue is full, and <b>false</b> value is
 * returned.
 *
 * @param queue     queue object
 * @param item      item to send
 * @param timeout   send timeout (0 for polling)
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool queue_send(queue_t *queue, const void *item, const uint timeout)
{
        return _queue_send(queue, item, timeout);
}

//==============================================================================
/**
 * @brief bool queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
 * The function <b>queue_send_from_ISR</b>() is similar to  <b>queue_send</b>(),
 * expect that can be called from interrupt. The <i>task_woken</i> is set to
 * <b>true</b> if context switch shall be forced by using <b>task_yield_from_ISR<b>().
 *
 * @param queue         queue object
 * @param item          item to send
 * @param task_woken    context switch request
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * queue_t *queue = queue_new(10, sizeof(char));
 *
 * // ...
 *
 * void thread(void *arg)
 * {
 *         char c;
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void uart_rx_isr(void)
 * {
 *         char c = UART1->DR;
 *
 *         bool woken = false;
 *         queue_send_from_ISR(queue, &c, &woken);
 *
 *         if (woken) {
 *                 task_yield_from_ISR();
 *         }
 * }
 */
//==============================================================================
static inline bool queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
{
        return _queue_send_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief bool queue_receive(queue_t *queue, void *item, const uint timeout)
 * The function <b>queue_receive</b>() receive top item from queue pointed by
 * <i>queue</i> and copy it to the item pointed by <i>item</i>. The item is
 * removed from queue. Try of receive is doing for time <i>timeout</i>. If item
 * was successfully received, then <b>true</b> is returned, otherwise <b>false</b>.
 *
 * @param queue     queue object
 * @param item      item destination
 * @param timeout   send timeout (0 for polling)
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool queue_receive(queue_t *queue, void *item, const uint timeout)
{
        return _queue_receive(queue, item, timeout);
}

//==============================================================================
/**
 * @brief bool queue_recieve_from_ISR(queue_t *queue, void *item, bool *task_woken)
 * The function <b>queue_recieve_from_ISR</b>() is similar to  <b>queue_recieve</b>(),
 * expect that can be called from interrupt. The <i>task_woken</i> is set to
 * <b>true</b> if context switch shall be forced by using <b>task_yield_from_ISR<b>().
 *
 * @param queue         queue object
 * @param item          item destination
 * @param task_woken    context switch request
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * queue_t *queue = queue_new(10, sizeof(char));
 *
 * // ...
 *
 * void thread(void *arg)
 * {
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void some_isr(void)
 * {
 *         char c;
 *
 *         bool woken = false;
 *         if (queue_receive_from_ISR(queue, &c, &woken)) {
 *                 // some operations
 *         }
 *
 *         if (woken) {
 *                 task_yield_from_ISR();
 *         }
 * }
 */
//==============================================================================
static inline bool queue_recieve_from_ISR(queue_t *queue, void *item, bool *task_woken)
{
        return _queue_receive_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief bool queue_receive_peek(queue_t *queue, void *item, const uint timeout)
 * The function <b>queue_receive_peek</b>() is similar to <b>queue_receive</b>(),
 * expect that top item is not removed from the queue.
 *
 * @param queue     queue object
 * @param item      item destination
 * @param timeout   send timeout (0 for polling)
 *
 * @errors None
 *
 * @return On success, <b>true</b> is returned. On error, <b>false</b> is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c;
 *
 *         queue_receive_peek(queue, &c, MAX_DELAY_MS);
 *
 *         if (c == '1') {
 *                 // ...
 *         } else {
 *                 // ...
 *         }
 *
 *         // ...
 *
 *         queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline bool queue_receive_peek(queue_t *queue, void *item, const uint timeout)
{
        return _queue_receive_peek(queue, item, timeout);
}

//==============================================================================
/**
 * @brief int queue_get_number_of_items(queue_t *queue)
 * The function <b>queue_get_number_of_items</b>() returns a number of items
 * stored in the queue pointed by <i>queue</i>.
 *
 * @param queue     queue object
 *
 * @errors None
 *
 * @return Number of items stored in the queue. On error, -1 is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void thread1(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void thread2(void *arg)
 * {
 *         queue_t *queue = arg;
 *
 *         for (i = 0; i < queue_get_number_of_items(queue); i++) {
 *                 char c;
 *                 queue_receive(queue, &c, MAX_DELAY_MS);
 *
 *                 // some operations
 *         }
 *
 *         // ...
 * }
 *
 * int main()
 * {
 *         // ...
 *
 *         queue_t *queue = queue_new(10, sizeof(char));
 *
 *         thread_new(thread1, STACK_DEPTH_LOW, queue);
 *         thread_new(thread2, STACK_DEPTH_LOW, queue);
 *
 *         thread_join(thread1);
 *         thread_join(thread2);
 *
 *         queue_delete(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline int queue_get_number_of_items(queue_t *queue)
{
        return _queue_get_number_of_items(queue);
}

//==============================================================================
/**
 * @brief int queue_get_number_of_items_from_ISR(queue_t *queue)
 * The function <b>queue_get_number_of_items_from_ISR</b>() is similar to
 * <b>queue_get_number_of_items</b>(), except that can be called from interrupt.
 *
 * @param queue     queue object
 *
 * @errors None
 *
 * @return Number of items stored in the queue. On error, -1 is returned.
 *
 * @example
 * #include <dnx/thread.h>
 *
 * // ...
 *
 * queue_t *queue;
 *
 * // ...
 *
 * void thread1(void *arg)
 * {
 *         char c = '1';
 *         queue_send(queue, &c, MAX_DELAY_MS);
 *
 *         // some operations
 * }
 *
 * void some_isr(void *arg)
 * {
 *         int len = queue_get_number_of_items_from_ISR(queue);
 *
 *         // ...
 * }
 */
//==============================================================================
static inline int queue_get_number_of_items_from_ISR(queue_t *queue)
{
        return _queue_get_number_of_items_from_ISR(queue);
}

//==============================================================================
/**
 * @brief void critical_section_begin(void)
 * The function <b>critical_section_begin</b>() enters the code to the
 * critical section. From critical section masked interrupts and context switch
 * does not work. The critical section routine shall be short as possible.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void some_function(void)
 * {
 *         // ...
 *
 *         critical_section_begin();
 *
 *         // some short operations ...
 *
 *         critical_section_end();
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void critical_section_begin(void)
{
        _critical_section_begin();
}

//==============================================================================
/**
 * @brief void critical_section_end(void)
 * The function <b>critical_section_end</b>() exit from critical section.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void some_function(void)
 * {
 *         // ...
 *
 *         critical_section_begin();
 *
 *         // some short operations ...
 *
 *         critical_section_end();
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void critical_section_end(void)
{
        _critical_section_end();
}

//==============================================================================
/**
 * @brief void ISR_disable(void)
 * The function <b>ISR_disable</b>() disable interrupts.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void some_function(void)
 * {
 *         // ...
 *
 *         ISR_disable();
 *
 *         // some short operations ...
 *
 *         ISR_enable();
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void ISR_disable(void)
{
        _ISR_disable();
}

//==============================================================================
/**
 * @brief void ISR_disable(void)
 * The function <b>ISR_disable</b>() enable interrupts.
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 *
 * @example
 * #include <dnx/thread.h>
 *
 * void some_function(void)
 * {
 *         // ...
 *
 *         ISR_disable();
 *
 *         // some short operations ...
 *
 *         ISR_enable();
 *
 *         // ...
 * }
 */
//==============================================================================
static inline void ISR_enable(void)
{
        _ISR_enable();
}

#ifdef __cplusplus
}
#endif

#endif /* _THREAD_H_ */
/*==============================================================================
  End of file
==============================================================================*/
