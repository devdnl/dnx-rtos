/*==============================================================================
File    conf_parser.h

Author  Daniel Zorychta

Brief   Configuration parser.

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

==============================================================================*/

/**
@defgroup CONF_PARSER_H_ CONF_PARSER_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct conf_parser conf_parser_t;

typedef enum {
        conf_parser_status__success = 0,
        conf_parser_status__syntax_error = -1,
        conf_parser_status__no_memory = -2,
        conf_parser_status__file_error = -3,
        conf_parser_status__arg_error = -4,
        conf_parser_status__no_entry = -5,
} conf_parser_status_t;

/*==============================================================================
  Exported objects
==============================================================================*/
extern conf_parser_t *conf_parser__new(const char *path);
extern void conf_parser__delete(conf_parser_t *parser);
extern int conf_parser__load_file(conf_parser_t *parser, const char *path);
extern int conf_parser__load_buffer(conf_parser_t *parser, const char *buf, size_t bufsz);
extern int conf_parser__save_file(conf_parser_t *parser, const char *path);
extern int conf_parser__save_buffer(conf_parser_t *parser, char *buf, size_t bufsz);
extern const char *conf_parser__get_value(conf_parser_t *parser, const char *key);
extern const char *conf_parser__get_file_value(const char *path, const char *key);
extern int conf_parser__set_value(conf_parser_t *parser, const char *key, const char *value);
extern int conf_parser__set_file_value(const char *path, const char *key, const char *value);
extern int conf_parser__get_pair(conf_parser_t *parser, size_t i, const char **key, const char **val);

/*==============================================================================
  Exported functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
