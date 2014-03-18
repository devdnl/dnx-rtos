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
 * @brief Function delete semaphore
 *
 * @param[in] sem       semaphore object
 */
//==============================================================================
static inline void semaphore_delete(sem_t *sem)
{
        _semaphore_delete(sem);
}

//==============================================================================
/**
 * @brief Function wait for semaphore
 *
 * @param[in] sem               semaphore object
 * @param[in] timeout           semaphore polling time
 *
 * @retval true                 semaphore taken
 * @retval false                semaphore not taken
 */
//==============================================================================
static inline bool semaphore_wait(sem_t *sem, const uint timeout)
{
        return _semaphore_take(sem, timeout);
}

//==============================================================================
/**
 * @brief Function signal semaphore
 *
 * @param[in] sem       semaphore object
 *
 * @retval true         semaphore given
 * @retval false        semaphore not given
 */
//==============================================================================
static inline bool semaphore_signal(sem_t *sem)
{
        return _semaphore_give(sem);
}

//==============================================================================
/**
 * @brief Function wait for semaphore from ISR
 *
 * @param[in]  sem              semaphore object
 * @param[out] task_woken       true if higher priority task woke, otherwise false (can be NULL)
 *
 * @retval true                 semaphore taken
 * @retval false                semaphore not taken
 */
//==============================================================================
static inline bool semaphore_wait_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_take_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief Function signal semaphore from ISR
 *
 * @param[in]  sem              semaphore object
 * @param[out] task_woken       true if higher priority task woke, otherwise false (can be NULL)
 *
 * @retval true                 semaphore taken
 * @retval false                semaphore not taken
 */
//==============================================================================
static inline bool semaphore_signal_from_ISR(sem_t *sem, bool *task_woken)
{
        return _semaphore_give_from_ISR(sem, task_woken);
}

//==============================================================================
/**
 * @brief Function create new mutex
 *
 * @param type          mutex type
 *
 * @return pointer to mutex object, otherwise NULL if error
 */
//==============================================================================
static inline mutex_t *mutex_new(enum mutex_type type)
{
        return _mutex_new(type);
}

//==============================================================================
/**
 * @brief Function delete mutex
 *
 * @param[in] mutex     mutex object
 */
//==============================================================================
static inline void mutex_delete(mutex_t *mutex)
{
        _mutex_delete(mutex);
}

//==============================================================================
/**
 * @brief Function lock mutex
 *
 * @param[in] mutex            mutex object
 * @param[in] timeout          polling time
 *
 * @retval true                 mutex locked
 * @retval false                mutex not locked
 */
//==============================================================================
static inline bool mutex_lock(mutex_t *mutex, const uint timeout)
{
        return _mutex_lock(mutex, timeout);
}

//==============================================================================
/**
 * @brief Function unlock mutex
 *
 * @param[in] mutex             mutex object
 *
 * @retval true                 mutex unlocked
 * @retval false                mutex still locked
 */
//==============================================================================
static inline bool mutex_unlock(mutex_t *mutex)
{
        return _mutex_unlock(mutex);
}

//==============================================================================
/**
 * @brief Function create new queue
 *
 * @param[in] length            queue length
 * @param[in] item_size         queue item size
 *
 * @return pointer to queue object, otherwise NULL if error
 */
//==============================================================================
static inline queue_t *queue_new(const uint length, const uint item_size)
{
        return _queue_new(length, item_size);
}

//==============================================================================
/**
 * @brief Function delete queue
 *
 * @param[in] queue             queue object
 */
//==============================================================================
static inline void queue_delete(queue_t *queue)
{
        _queue_delete(queue);
}

//==============================================================================
/**
 * @brief Function reset queue
 *
 * @param[in] queue             queue object
 */
//==============================================================================
static inline void queue_reset(queue_t *queue)
{
        _queue_reset(queue);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in] queue             queue object
 * @param[in] item              item
 * @param[in] timeout           timeout
 *
 * @retval true                 item posted
 * @retval false                item not posted
 */
//==============================================================================
static inline bool queue_send(queue_t *queue, const void *item, const uint timeout)
{
        return _queue_send(queue, item, timeout);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  queue            queue object
 * @param[in]  item             item
 * @param[out] task_woken       true if higher priority task woke, otherwise false (can be NULL)
 *
 * @retval true                 item posted
 * @retval false                item not posted
 */
//==============================================================================
static inline bool queue_send_from_ISR(queue_t *queue, const void *item, bool *task_woken)
{
        return _queue_send_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief Function send queue
 *
 * @param[in]  queue            queue object
 * @param[out] item             item
 * @param[in]  timeout          timeout
 *
 * @retval true                 item received
 * @retval false                item not received
 */
//==============================================================================
static inline bool queue_receive(queue_t *queue, void *item, const uint timeout)
{
        return _queue_receive(queue, item, timeout);
}

//==============================================================================
/**
 * @brief Function receive queue from ISR
 *
 * @param[in]  queue            queue object
 * @param[out] item             item
 * @param[out] task_woken       true if higher priority task woke, otherwise false (can be NULL)
 *
 * @retval true                 item received
 * @retval false                item not received
 */
//==============================================================================
static inline bool queue_recieve_from_ISR(queue_t *queue, void *item, bool *task_woken)
{
        return _queue_receive_from_ISR(queue, item, task_woken);
}

//==============================================================================
/**
 * @brief Function peek queue
 *
 * @param[in]  queue            queue object
 * @param[out] item             item
 * @param[in]  waittime_ms      wait time
 *
 * @retval true                 item received
 * @retval false                item not received
 */
//==============================================================================
static inline bool queue_receive_peek(queue_t *queue, void *item, const uint timeout)
{
        return _queue_receive_peek(queue, item, timeout);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue
 *
 * @param[in] queue             queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
static inline int queue_get_number_of_items(queue_t *queue)
{
        return _queue_get_number_of_items(queue);
}

//==============================================================================
/**
 * @brief Function gets number of items in queue from ISR
 *
 * @param[in] *queue            queue object
 *
 * @return a number of items in queue, -1 if error
 */
//==============================================================================
static inline int queue_get_number_of_items_from_ISR(queue_t *queue)
{
        return _queue_get_number_of_items_from_ISR(queue);
}

//==============================================================================
/**
 * @brief Function enter to critical section
 */
//==============================================================================
static inline void critical_section_begin(void)
{
        _critical_section_begin();
}

//==============================================================================
/**
 * @brief Function exit from critical section
 */
//==============================================================================
static inline void critical_section_end(void)
{
        _critical_section_end();
}

//==============================================================================
/**
 * @brief Function disable interrupts
 */
//==============================================================================
static inline void ISR_disable(void)
{
        _ISR_disable();
}

//==============================================================================
/**
 * @brief Function enable interrupts
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
