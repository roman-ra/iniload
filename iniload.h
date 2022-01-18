#ifndef INILOAD_H
#define INILOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef INILOAD_NAME_MAXLEN
#define INILOAD_NAME_MAXLEN 128
#endif

#ifndef INILOAD_INITIAL_CAP
#define INILOAD_INITIAL_CAP 8
#endif

/* Forward declaration */
typedef struct ini_file ini_file;

/* Functions */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parses an INI file and returns a pointer to an object containing
 * the parsed data.
 *
 * @param path Path to the INI file
 * @return Pointer to an ini_file struct containing the parsed data or NULL if
 * there was an error parsing or dynamically allocating the memory.
 * @note Keys without a section are assigned to a section with an empty name
 * ("\0").
 */
ini_file *ini_load(const char *path);

/**
 * @brief Returns the total number of sections in the INI file.
 *
 * @param ini Pointer to a loaded INI file.
 * @return Total number of sections in the INI file.
 * @note Keys without a section are assigned to a section with an empty name
 * ("\0") which also counts as a section.
 */
size_t ini_num_sections(ini_file *ini);

/**
 * @brief Checks if a loaded INI file has a section with a given name.
 *
 * @param ini Pointer to a loaded INI file.
 * @param section_name Name of the section.
 * @return 1 if a section with the give name is present, 0 otherwise.
 */
int ini_has_section(ini_file *ini, const char *section_name);

/**
 * @brief Returns the total number of keys in a section with a given name.
 *
 * @param ini Pointer to a loaded INI file.
 * @param section_name Name of the section.
 * @return Total number of keys in the section with the given name.
 * @note If a section with the given name does not exist then 0 is returned.
 */
size_t ini_num_keys(ini_file *ini, const char *section_name);

/**
 * @brief Checks if a section with a given has a key with a given name.
 *
 * @param ini Pointer to a loaded INI file.
 * @param section_name Name of the section.
 * @param key_name Name of the key.
 * @return 1 if a key with the give name is present in the section, 0 if either
 * a key with the given name is not present in the section, or if the section
 * itself is not present.
 */
int ini_has_key(ini_file *ini, const char *section_name, const char *key_name);

/**
 * @brief Retrieves an integer-typed key's value.
 *
 * @param ini Pointer to a loaded INI file.
 * @param section_name Name of the section.
 * @param key_name Name of the key.
 * @param default_val Default value that is returned if the key's value is of a
 * different type or if the key or the section doesn't exist.
 * @return The key's value or the default value.
 */
int ini_get_int(ini_file *ini, const char *section_name, const char *key_name,
                int default_val);

/**
 * @brief Retrieves an float-typed key's value.
 *
 * @param ini Pointer to a loaded INI file.
 * @param section_name Name of the section.
 * @param key_name Name of the key.
 * @param default_val Default value that is returned if the key's value is of a
 * different type or if the key or the section doesn't exist.
 * @return The key's value or the default value.
 */
float ini_get_float(ini_file *ini, const char *section_name,
                    const char *key_name, float default_val);

/**
 * @brief Retrieves an string-typed key's value.
 *
 * @param ini Pointer to a loaded INI file.
 * @param section_name Name of the section.
 * @param key_name Name of the key.
 * @param default_val Default value that is returned if the key's value is of a
 * different type or if the key or the section doesn't exist.
 * @return The key's value or the default value.
 */
char *ini_get_string(ini_file *ini, const char *section_name,
                     const char *key_name, char *default_val);

/**
 * @brief Frees all dynamically allocated memory that is used by the parsed
 * INI file.
 *
 * @param ini Pointer to a loaded INI file.
 */
void ini_free(ini_file *ini);

#ifdef __cplusplus
}
#endif

#endif /* INILOAD_H */

/******************
 * Implementation *
 ******************/
#ifdef INILOAD_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Supported INI key types.
 */
typedef enum ini_key_type {
  INI_KEY_INT,   /**< Signed integer key */
  INI_KEY_FLOAT, /**< Single-precision floating point number key */
  INI_KEY_STRING /**< String key */
} ini_key_type;

/**
 * @brief Represents a single INI key (its name, data type and value).
 */
typedef struct ini_key {
  char name[INILOAD_NAME_MAXLEN + 1]; /**< Name of the key */
  ini_key_type type;                  /**< Data type of the key's value */
  union value {                       /**< Value of the key */
    int int_val;
    float float_val;
    char *string_val;
  } value;
} ini_key;

/**
 * @brief Represents a single INI section (a collection of keys).
 */
typedef struct ini_section {
  char name[INILOAD_NAME_MAXLEN + 1]; /**< Name of the section */
  size_t num_keys;                    /**< Number of keys in the section */
  size_t cap_keys;   /**< Capacity of the array holding the keys */
  ini_key *ptr_keys; /**< Pointer to the array holding the keys */
} ini_section;

/**
 * @brief Represents a collection of sections.
 */
struct ini_file {
  size_t num_sections;       /**< Number of sections */
  size_t cap_sections;       /**< Capacity of the array holding the sections */
  ini_section *ptr_sections; /**< Pointer to the array holding the sections */
};

ini_section *__ini_add_section(ini_file *ini, const char *section_name) {
  ini_key *ptr_keys = NULL;
  if (ini->num_sections == ini->cap_sections) {
    /* Allocate more memory to add a new section */
    ini_section *ptr_sec_new = (ini_section *)realloc(
        ini->ptr_sections, sizeof(ini_section) * (ini->cap_sections * 2));
    if (ptr_sec_new == NULL) {
      return NULL;
    }
    ini->ptr_sections = ptr_sec_new;
    ini->cap_sections = ini->cap_sections * 2;
  }

  strcpy(ini->ptr_sections[ini->num_sections].name, section_name);
  ini->ptr_sections[ini->num_sections].num_keys = 0;
  ini->ptr_sections[ini->num_sections].cap_keys = INILOAD_INITIAL_CAP;
  ptr_keys = (ini_key *)malloc(sizeof(ini_key) * INILOAD_INITIAL_CAP);
  if (ptr_keys == NULL) {
    return NULL;
  }
  ini->ptr_sections[ini->num_sections].ptr_keys = ptr_keys;
  ini->num_sections++;
  return &(ini->ptr_sections[ini->num_sections - 1]);
}

int __ini_add_key(ini_section *section, const char *key_name, const char *value,
                  size_t value_len, int quotes) {
  ini_key *ptr_keys_new;
  double float_val;
  long int int_val;
  char *endptr;
  char *copy = NULL;
  if (section->num_keys == section->cap_keys) {
    /* Allocate more memory to add a new key */
    ptr_keys_new = (ini_key *)realloc(
        section->ptr_keys, sizeof(ini_key) * (section->cap_keys * 2));
    if (ptr_keys_new == NULL) {
      return 0;
    }
    section->cap_keys = section->cap_keys * 2;
    section->ptr_keys = ptr_keys_new;
  }

  strcpy(section->ptr_keys[section->num_keys].name, key_name);

  /* Guess the data type of the key */
  if (quotes) {
    /* Definitely a string if it is quoted */
    copy = (char *)malloc(value_len + 1);
    if (copy == NULL) {
      return 0;
    }
    strcpy(copy, value);
    section->ptr_keys[section->num_keys].value.string_val = copy;
    section->ptr_keys[section->num_keys].type = INI_KEY_STRING;
  } else {
    int_val = strtol(value, &endptr, 0);
    if (*endptr == '\0') {
      /* This is an integer */
      section->ptr_keys[section->num_keys].value.int_val = int_val;
      section->ptr_keys[section->num_keys].type = INI_KEY_INT;
    } else {
      /* Have to use strtod instead of the (yet) unsupported strtof */
      float_val = strtod(value, &endptr);
      if (*endptr == '\0') {
        /* This is a float */
        section->ptr_keys[section->num_keys].value.float_val = (float)float_val;
        section->ptr_keys[section->num_keys].type = INI_KEY_FLOAT;
      } else {
        /* Must be a string then */
        copy = (char *)malloc(value_len + 1);
        if (copy == NULL) {
          return 0;
        }
        strcpy(copy, value);
        section->ptr_keys[section->num_keys].value.string_val = copy;
        section->ptr_keys[section->num_keys].type = INI_KEY_STRING;
      }
    }
  }

  section->num_keys++;
  return 1;
}

ini_key *__ini_get_key_ptr(ini_file *ini, const char *section_name,
                           const char *key_name) {
  size_t s, k;
  for (s = 0; s < ini->num_sections; s++) {
    if (strcmp(ini->ptr_sections[s].name, section_name) == 0) {
      for (k = 0; k < ini->ptr_sections[s].num_keys; k++) {
        if (strcmp(ini->ptr_sections[s].ptr_keys[k].name, key_name) == 0) {
          return &ini->ptr_sections[s].ptr_keys[k];
        }
      }
    }
  }
  return NULL;
}

ini_file *ini_load(const char *path) {
  FILE *f = NULL;
  long file_size = 0;
  char *buf = 0;
  size_t name_pos = 0;
  size_t name_len = 0;
  size_t value_pos = 0;
  size_t value_len = 0;
  ini_file *ptr = NULL;

  size_t i = 0;
  char c = 0;
  char tmp = 0;
  char tmp2 = 0;

  int alloc_error = 0;
  int bad_syntax = 0;

  ini_section *curr_section = NULL;

  enum {
    INIPS_NONE,
    INIPS_COMMENT,
    INIPS_SECTION_NAME,
    INIPS_AFTER_SECTION_NAME,
    INIPS_KEY_NAME,
    INIPS_AFTER_KEY_NAME,
    INIPS_BEFORE_KEY_VALUE,
    INIPS_QUOTED_VALUE,
    INIPS_NON_QUOTED_VALUE,
    INIPS_AFTER_KEY_VALUE
  } state = INIPS_NONE;

  f = fopen(path, "rt");

  if (f == NULL) {
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  /* Read the entire file */
  buf = (char *)malloc(sizeof(char) * file_size + 1);
  if (buf == NULL) {
    fclose(f);
    return NULL;
  }

  if (fread(buf, 1, file_size, f) != file_size) {
    fclose(f);
    free(buf);
    return NULL;
  }
  fclose(f);
  buf[file_size] = '\0';

  /* Allocate memory for the ini file struct */
  ptr = (ini_file *)malloc(sizeof(ini_file));
  if (ptr == NULL) {
    free(buf);
    return NULL;
  }

  /* Allocate memory for the array of sections */
  ptr->ptr_sections =
      (ini_section *)malloc(sizeof(ini_section) * INILOAD_INITIAL_CAP);
  if (!ptr->ptr_sections) {
    free(buf);
    free(ptr);
    return NULL;
  }
  ptr->cap_sections = INILOAD_INITIAL_CAP;
  ptr->num_sections = 0;

/* Parse using a state machine/automaton */
#define IS_SPACE(c) (c == ' ' || c == '\t')
#define IS_NEWLINE(c) (c == '\r' || c == '\n')
#define IS_EOF(c) (c == '\0')
#define IS_NEWLINE_OR_EOF(c) (IS_NEWLINE(c) || IS_EOF(c))
#define IS_COMMENT(c) (c == ';' || c == '#')

  for (i = 0; i < file_size + 1 && !bad_syntax && !alloc_error; i++) {
    c = buf[i];
    /*printf("%c <--- %d\n", c, state);*/

    switch (state) {
    case INIPS_NONE:
      if (IS_SPACE(c) || IS_NEWLINE_OR_EOF(c)) {
        /* Do nothing */
      } else if (IS_COMMENT(c)) {
        state = INIPS_COMMENT;
      } else if (c == '[') {
        name_pos = i + 1;
        name_len = 0;
        state = INIPS_SECTION_NAME;
      } else {
        /* Everything else could be a key name */
        name_pos = i;
        name_len = 1;
        state = INIPS_KEY_NAME;
      }
      break;

    case INIPS_COMMENT:
      if (IS_NEWLINE_OR_EOF(c)) {
        state = INIPS_NONE;
      } else {
        /* Do nothing, read the comment */
      }
      break;

    case INIPS_SECTION_NAME:
      if (c == ']') {
        /* Add the new section */
        if (name_len > INILOAD_NAME_MAXLEN) {
          /* Name is too long */
          bad_syntax = 1;
          break;
        }
        /* "Cut" the buffer to extract the section name */
        tmp = buf[name_pos + name_len];
        buf[name_pos + name_len] = '\0';
        curr_section = __ini_add_section(ptr, buf + name_pos);
        if (curr_section == NULL) {
          alloc_error = 1;
          break;
        }
        /* Restore the buffer */
        buf[name_pos + name_len] = tmp;
        state = INIPS_AFTER_SECTION_NAME;
      } else if (c == '[' || c == '=' || IS_NEWLINE_OR_EOF(c) ||
                 IS_COMMENT(c)) {
        bad_syntax = 1;
      } else {
        name_len++;
      }
      break;

    case INIPS_AFTER_SECTION_NAME:
      if (IS_NEWLINE_OR_EOF(c)) {
        state = INIPS_NONE;
      } else if (IS_SPACE(c)) {
      } else {
        bad_syntax = 1;
      }
      break;

    case INIPS_KEY_NAME:
      if (IS_SPACE(c)) {
        state = INIPS_AFTER_KEY_NAME;
      } else if (c == '=') {
        state = INIPS_BEFORE_KEY_VALUE;
      } else if (c == '[' || c == ']' || IS_NEWLINE_OR_EOF(c)) {
        bad_syntax = 1;
      } else {
        name_len++;
      }
      if (name_len > INILOAD_NAME_MAXLEN) {
        bad_syntax = 1;
        break;
      }
      break;

    case INIPS_AFTER_KEY_NAME:
      if (IS_SPACE(c)) {
      } else if (c == '=') {
        state = INIPS_BEFORE_KEY_VALUE;
      } else {
        bad_syntax = 1;
      }
      break;

    case INIPS_BEFORE_KEY_VALUE:
      if (IS_SPACE(c)) {
      } else if (IS_NEWLINE_OR_EOF(c) || c == '=' || c == '[' || c == ']') {
        bad_syntax = 1;
      } else if (c == '\"') {
        value_pos = i + 1;
        value_len = 0;
        state = INIPS_QUOTED_VALUE;
      } else {
        value_pos = i;
        value_len = 1;
        state = INIPS_NON_QUOTED_VALUE;
      }
      break;

    case INIPS_QUOTED_VALUE:
      if (c == '\"') {
        /* Insert the key */
        tmp = buf[name_pos + name_len];
        buf[name_pos + name_len] = '\0';
        tmp2 = buf[value_pos + value_len];
        buf[value_pos + value_len] = '\0';
        if (curr_section == NULL) {
          /* Empty section but we don't have it yet */
          curr_section = __ini_add_section(ptr, "");
          if (curr_section == NULL) {
            alloc_error = 1;
            break;
          }
        }

        __ini_add_key(curr_section, buf + name_pos, buf + value_pos, value_len,
                      1);

        /* Restore the buffer */
        buf[name_pos + name_len] = tmp;
        buf[value_pos + value_len] = tmp2;
        state = INIPS_AFTER_KEY_VALUE;
      } else if (IS_NEWLINE_OR_EOF(c)) {
        bad_syntax = 1;
      } else {
        value_len++;
      }
      break;

    case INIPS_NON_QUOTED_VALUE:
      if (IS_NEWLINE_OR_EOF(c)) {
        tmp = buf[name_pos + name_len];
        buf[name_pos + name_len] = '\0';
        tmp2 = buf[value_pos + value_len];
        buf[value_pos + value_len] = '\0';
        if (curr_section == NULL) {
          /* Empty section but we don't have it yet */
          curr_section = __ini_add_section(ptr, "");
          if (curr_section == NULL) {
            alloc_error = 1;
            break;
          }
        }

        __ini_add_key(curr_section, buf + name_pos, buf + value_pos, value_len,
                      0);

        /* Restore the buffer */
        buf[name_pos + name_len] = tmp;
        buf[value_pos + value_len] = tmp2;
        state = INIPS_NONE;
      }
      if (c == '[' || c == ']' || c == '=') {
        bad_syntax = 1;
      } else {
        value_len++;
      }
      break;

    case INIPS_AFTER_KEY_VALUE:
      if (IS_NEWLINE_OR_EOF(c)) {
        state = INIPS_NONE;
      } else {
        bad_syntax = 1;
      }
      break;

    default:
      break;
    }
  }

  free(buf);

#undef IS_SPACE
#undef IS_NEWLINE
#undef IS_EOF
#undef IS_NEWLINE_OR_EOF
#undef IS_COMMENT

  if (bad_syntax || alloc_error) {
    ini_free(ptr);
    return NULL;
  } else {
    return ptr;
  }
}
size_t ini_num_sections(ini_file *ini) { return ini->num_sections; }

int ini_has_section(ini_file *ini, const char *section_name) {
  size_t s;
  for (s = 0; s < ini->num_sections; s++) {
    if (strcmp(ini->ptr_sections[s].name, section_name) == 0) {
      return 1;
    }
  }
  return 0;
}

size_t ini_num_keys(ini_file *ini, const char *section_name) {
  size_t s;
  for (s = 0; s < ini->num_sections; s++) {
    if (strcmp(ini->ptr_sections[s].name, section_name) == 0) {
      return ini->ptr_sections[s].num_keys;
    }
  }
  return 0;
}

int ini_has_key(ini_file *ini, const char *section_name, const char *key_name) {
  return (__ini_get_key_ptr(ini, section_name, key_name) == NULL ? 0 : 1);
}

int ini_get_int(ini_file *ini, const char *section_name, const char *key_name,
                int default_val) {
  ini_key *ptr = __ini_get_key_ptr(ini, section_name, key_name);
  if (ptr == NULL || ptr->type != INI_KEY_INT) {
    return default_val;
  } else {
    return ptr->value.int_val;
  }
}

float ini_get_float(ini_file *ini, const char *section_name,
                    const char *key_name, float default_val) {
  ini_key *ptr = __ini_get_key_ptr(ini, section_name, key_name);
  if (ptr == NULL || ptr->type != INI_KEY_FLOAT) {
    return default_val;
  } else {
    return ptr->value.float_val;
  }
}

char *ini_get_string(ini_file *ini, const char *section_name,
                     const char *key_name, char *default_val) {
  ini_key *ptr = __ini_get_key_ptr(ini, section_name, key_name);
  if (ptr == NULL || ptr->type != INI_KEY_STRING) {
    return default_val;
  } else {
    return ptr->value.string_val;
  }
}

void ini_free(ini_file *ini) {
  size_t s, k;
  for (s = 0; s < ini->num_sections; s++) {
    for (k = 0; k < ini->ptr_sections[s].num_keys; k++) {
      if (ini->ptr_sections[s].ptr_keys[k].type == INI_KEY_STRING) {
        free(ini->ptr_sections[s].ptr_keys[k].value.string_val);
      }
    }
    free(ini->ptr_sections[s].ptr_keys);
  }
  free(ini->ptr_sections);
  free(ini);
}

#ifdef __cplusplus
}
#endif

#endif /* INILOAD_IMPLEMENTATION */