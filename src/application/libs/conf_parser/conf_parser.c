/*==============================================================================
File    config_parser.c

Author  Daniel Zorychta

Brief   Configuration parser.

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "conf_parser.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dnx/misc.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define LINE_LEN                128

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct pair {
        struct pair *next;
        size_t value_offset;
        char   data[];
} pair_t;

struct conf_parser {
        pair_t *pair;
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static pair_t *interpret_line(char *line);
static pair_t *pair_new(const char *key, const char *val);
static const char *pair_get_key(const pair_t *pair);
static const char *pair_get_value(const pair_t *pair);
static char *skip_leading_spaces(char *str);
static void remove_trailing_spaces(char *str);

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Create new pareser instance.
 *
 * @param  path         file to parse (can be NULL)
 * @param  status       operation stats (can be NULL)
 *
 * @return Parser object.
 */
//==============================================================================
conf_parser_t *conf_parser__new(const char *path)
{
        conf_parser_t *this = calloc(1, sizeof(*this));
        if (not this) {
                return NULL;
        }

        if (path) {
                int n = conf_parser__load_file(this, path);
                if (n < 0) {
                        goto error;
                }
        }

        return this;

        error:
        conf_parser__delete(this);
        return NULL;
}

//==============================================================================
/**
 * @brief  Delete existing parser instance.
 *
 * @param  this         parser instance
 */
//==============================================================================
void conf_parser__delete(conf_parser_t *this)
{
        pair_t *pair = this->pair;

        while (pair) {
                pair_t *next = pair->next;
                free(pair);
                pair = next;
        }

        free(this);
}

//==============================================================================
/**
 * @brief  Load selected configuration file.
 *
 * @param  this         parser instance
 * @param  path         file path
 *
 * @return Number of loaded parameters. On error one of conf_parser_status_t value.
 */
//==============================================================================
int conf_parser__load_file(conf_parser_t *this, const char *path)
{
        if (not this or not path) return conf_parser_status__arg_error;

        FILE *fp = fopen(path, "r");
        if (not fp) {
                return conf_parser_status__file_error;
        }

        size_t n = 0;

        pair_t **pair = &this->pair;

        char line[LINE_LEN];

        while (fgets(line, sizeof(line), fp)) {

                *pair = interpret_line(line);
                if (*pair) {
                        pair = &(*pair)->next;
                        n++;
                }
        }

        fclose(fp);

        return n;
}

//==============================================================================
/**
 * @brief  Load configuration from buffer or string
 *
 * @param  this         parser instance
 * @param  buf          buffer
 * @param  bufsz        buffer size
 *
 * @return Number of loaded parameters. On error one of conf_parser_status_t value.
 */
//==============================================================================
int conf_parser__load_buffer(conf_parser_t *this, const char *buf, size_t bufsz)
{
        if (not this and not buf and not bufsz) return conf_parser_status__arg_error;

        size_t n = 0;

        pair_t **pair = &this->pair;

        while ((*buf != '\0') and (bufsz > 0)) {
                size_t i = 0;
                char line[LINE_LEN];

                while ((*buf != '\0') and (bufsz > 0) and (i < LINE_LEN - 1)) {
                        int c = *buf;
                        line[i++] = *buf++;
                        bufsz--;

                        if (c == '\n') break;
                }

                line[i] = '\0';

                if (i > 0) {
                        *pair = interpret_line(line);
                        if (*pair) {
                                pair = &(*pair)->next;
                                n++;
                        }
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief  Save configuration file.
 *
 * @param  this         parser instance
 * @param  path         file to save
 *
 * @return One of conf_parser_status_t.
 */
//==============================================================================
int conf_parser__save_file(conf_parser_t *this, const char *path)
{
        if (not this or not path) return conf_parser_status__arg_error;

        FILE *fp = fopen(path, "w");
        if (not fp) {
                return conf_parser_status__file_error;
        }

        int err = conf_parser_status__success;

        for (pair_t *pair = this->pair; pair; pair = pair->next) {

                int n = fprintf(fp, "%s = %s\n", pair_get_key(pair), pair_get_value(pair));
                if (n <= 0) {
                        err = conf_parser_status__file_error;
                        break;
                }
        }

        fclose(fp);

        return err;
}

//==============================================================================
/**
 * @brief  Save configuration to buffer.
 *
 * @param  this         parser instance
 * @param  buf          buffer
 * @param  bufsz        buffer size
 *
 * @return Number of loaded parameters. On error one of conf_parser_status_t value.
 */
//==============================================================================
int conf_parser__save_buffer(conf_parser_t *this, char *buf, size_t bufsz)
{
        if (not this and not buf and not bufsz) return conf_parser_status__arg_error;

        size_t n = 0;

        for (pair_t *pair = this->pair; pair; pair = pair->next) {
                n += snprintf(&buf[n], bufsz - n, "%s = %s\n", pair_get_key(pair), pair_get_value(pair));
        }

        return n;
}

//==============================================================================
/**
 * @brief  Get value by key.
 *
 * @param  this         parser instance
 * @param  key          key
 *
 * @return Reference to value. NULL if key not found.
 */
//==============================================================================
const char *conf_parser__get_value(conf_parser_t *this, const char *key)
{
        if (not this or not key) return NULL;

        for (pair_t *pair = this->pair; pair; pair = pair->next) {

                if (strcmp(pair_get_key(pair), key) == 0) {
                        return pair_get_value(pair);
                }
        }

        return NULL;
}

//==============================================================================
/**
 * @brief  Get value by key of selected file (directly).
 *
 * @param  path         file path
 * @param  key          key
 *
 * @return Reference to value. NULL if key not found.
 */
//==============================================================================
const char *conf_parser__get_file_value(const char *path, const char *key)
{
        const char *value = NULL;

        auto_type p = conf_parser__new(path);
        if (p) {
                value = conf_parser__get_value(p, key);
                conf_parser__delete(p);
        }

        return value;
}

//==============================================================================
/**
 * @brief  Set selected value. If value exists then is updated, otherwise new one
 *         is created.
 *
 * @param  this         parser instance
 * @param  key          key
 * @param  value        value
 *
 * @return One of conf_parser_status_t.
 */
//==============================================================================
int conf_parser__set_value(conf_parser_t *this, const char *key, const char *value)
{
        if (not this or not key or not value) return conf_parser_status__arg_error;

        pair_t *pair_prev = NULL;

        for (pair_t *pair = this->pair; pair; pair = pair->next) {
                if (strcmp(pair_get_key(pair), key) == 0) {
                        pair_t *p = pair_new(key, value);

                        if (pair_prev) {
                                pair_prev->next = p;
                        } else {
                                this->pair = p;
                        }

                        p->next = pair->next;
                        free(pair);

                        return conf_parser_status__success;
                }

                pair_prev = pair;
        }

        pair_t *new_pair = pair_new(key, value);

        if (pair_prev) {
                pair_prev->next = new_pair;
        } else {
                this->pair = new_pair;
        }

        return conf_parser_status__success;
}

//==============================================================================
/**
 * @brief  Set specified value in file.
 *
 * @param  path         file
 * @param  key          key
 * @param  value        value
 *
 * @return One of conf_parser_status_t.
 */
//==============================================================================
int conf_parser__set_file_value(const char *path, const char *key, const char *value)
{
        int err = conf_parser_status__file_error;

        auto_type p = conf_parser__new(path);
        if (p) {
                err = conf_parser__set_value(p, key, value);

                if (not err) {
                        conf_parser__save_file(p, path);
                }

                conf_parser__delete(p);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Get key-value pair by index.
 *
 * @param  this         parser instance
 * @param  i            iterator
 * @param  key          pointer to key
 * @param  value        pointer to value
 *
 * @return One of conf_parser_status_t.
 */
//==============================================================================
int conf_parser__get_pair(conf_parser_t *this, size_t i, const char **key, const char **value)
{
        if (not this or not key or not value) {
                return conf_parser_status__arg_error;
        }

        size_t n = 0;

        for (pair_t *pair = this->pair; pair; pair = pair->next, n++) {

                if (n == i) {
                        *key = pair_get_key(pair);
                        *value = pair_get_value(pair);
                        return conf_parser_status__success;
                }
        }

        return conf_parser_status__no_entry;
}

//==============================================================================
/**
 * @brief  Parse configration line.
 *
 * @param  line         line to parse
 *
 * @return Key-value pair object or NULL.
 */
//==============================================================================
static pair_t *interpret_line(char *line)
{
        line = skip_leading_spaces(line);
        if (*line == '#') return NULL;
        char *key = line;

        if (not line) return NULL;

        line = strchr(line, '=');
        if (not line) return NULL;
        if (*line != '=') return NULL;
        *line++ = '\0';

        char *comment = strchr(key, '#');
        if (comment) return NULL;

        line = skip_leading_spaces(line);
        if (not line) return NULL;
        char *val = line;

        comment = strchr(val, '#');
        if (comment) {
                *comment = '\0';
        }

        remove_trailing_spaces(key);
        remove_trailing_spaces(val);

        return pair_new(key, val);
}

//==============================================================================
/**
 * @brief  Skip leading spaces.
 *
 * @param  str          string
 *
 * @return New string position or NULL if string finished.
 */
//==============================================================================
static char *skip_leading_spaces(char *str)
{
        while (isspace(*str)) {
                str++;
        }

        return *str == '\0' ? NULL : str;
}

//==============================================================================
/**
 * @brief  Remove trailing spaces in string.
 *
 * @param  str          string to modify
 */
//==============================================================================
static void remove_trailing_spaces(char *str)
{
        if (isstrempty(str)) {
                return;
        }

        str += strlen(str) - 1;

        while (isspace(*str)) {
                *str = '\0';
                str--;
        }
}

//==============================================================================
/**
 * @brief  Create new pair chain.
 *
 * @param  key          key
 * @param  val          value
 *
 * @return Pair object or NULL.
 */
//==============================================================================
static pair_t *pair_new(const char *key, const char *val)
{
        size_t key_sz = strsize(key);
        size_t val_sz = strsize(val);

        pair_t *pair = calloc(1, sizeof(*pair) + key_sz + val_sz);
        if (pair) {
                pair->value_offset = key_sz;
                memcpy(&pair->data[0], key, key_sz);
                memcpy(&pair->data[pair->value_offset], val, val_sz);
        }

        return pair;
}

//==============================================================================
/**
 * @brief  Get pair key.
 *
 * @param  pair         pair
 *
 * @return Key.
 */
//==============================================================================
static const char *pair_get_key(const pair_t *pair)
{
        return &pair->data[0];
}

//==============================================================================
/**
 * @brief  Get pair value.
 *
 * @param  pair         pair
 *
 * @return Value.
 */
//==============================================================================
static const char *pair_get_value(const pair_t *pair)
{
        return &pair->data[pair->value_offset];
}

/*==============================================================================
  End of file
==============================================================================*/
