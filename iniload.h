#ifndef INILOAD_H
#define INILOAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INILOAD_NAME_MAXLEN 128
#define INILOAD_INITIAL_CAP 16

/**
 * @brief Supported INI key types.
 */
typedef enum ini_key_type {
  INI_KEY_INT,   /**< Signed integer key */
  INI_KEY_FLOAT, /**< Single-precision floating point number key */
  INI_KEY_STRING /**< String key */
} ini_key_type;

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
 * @endcode
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

/******************
 * Implementation *
 ******************/
#define INILOAD_IMPLEMENTATION
#ifdef INILOAD_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif

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
  if (ini->num_sections == ini->cap_sections) {
    /* Allocate more memory to add a new section */
    ini_section *ptr_sec_new = (ini_section *)realloc(
        ini->ptr_sections, sizeof(ini_section) * (ini->cap_sections * 2));
    if (!ptr_sec_new) {
      return NULL;
    }
    ini->ptr_sections = ptr_sec_new;
    ini->cap_sections = ini->cap_sections * 2;
  }

  strcpy(ini->ptr_sections[ini->num_sections].name, section_name);
  ini->ptr_sections[ini->num_sections].num_keys = 0;
  ini->ptr_sections[ini->num_sections].cap_keys = INILOAD_INITIAL_CAP;
  ini_key *ptr_keys = (ini_key *)malloc(sizeof(ini_key) * INILOAD_INITIAL_CAP);
  if (!ptr_keys) {
    return NULL;
  }
  ini->ptr_sections[ini->num_sections].ptr_keys = ptr_keys;
  ini->num_sections++;
  return &(ini->ptr_sections[ini->num_sections - 1]);
}

int __ini_add_key(ini_section *section, const char *key_name, ini_key_type type,
                  void *value) {
  if (section->num_keys == section->cap_keys) {
    ini_key *new_ptr = (ini_key *)realloc(
        section->ptr_keys, sizeof(ini_key) * (section->cap_keys * 2));
    if (new_ptr) {
      return 0;
    }
    section->ptr_keys = new_ptr;
    section->cap_keys = section->cap_keys * 2;
  }
  strcpy(section->ptr_keys[section->num_keys].name, key_name);
  section->ptr_keys[section->num_keys].type = type;
  switch (type) {
  case INI_KEY_INT:
    memcpy((void *)&section->ptr_keys[section->num_keys].value, value,
           sizeof(int));
    section->ptr_keys[section->num_keys].type = INI_KEY_INT;
    break;
  case INI_KEY_FLOAT:
    memcpy((void *)&section->ptr_keys[section->num_keys].value, value,
           sizeof(float));
    section->ptr_keys[section->num_keys].type = INI_KEY_FLOAT;
    break;
  case INI_KEY_STRING:
    char *copy = (char *)malloc(sizeof(char) * (strlen((char *)value) + 1));
    if (!copy) {
      return 0;
    }
    strcpy(copy, (char *)value);
    memcpy((void *)&section->ptr_keys[section->num_keys].value, (void *)&copy,
           sizeof(char *));
    section->ptr_keys[section->num_keys].type = INI_KEY_STRING;
    break;
  default:
    break;
  }
  section->num_keys++;
  return 1;
}

ini_file *ini_load(const char *path) {
  /* Declarations to comply with ISO C90 */
  long file_size;
  char *buf;
  size_t read_bytes;
  ini_file *ptr;

  size_t i;
  char *curr_str;
  char *val_str;
  size_t curr_pos = 0;
  size_t val_pos = 0;
  ini_section *curr_section = NULL;
  int bad_syntax = 0;
  int alloc_error = 0;

  enum {
    INIPS_NONE,
    INIPS_COMMENT,
    INIPS_SECTION_NAME,
    INIPS_AFTER_SECTION_NAME,
    INIPS_KEY_NAME,
    INIPS_AFTER_KEY_NAME,
    INIPS_BEFORE_KEY_VALUE,
    INIPS_KEY_VALUE
  } state = INIPS_NONE;

  int quotes = 0;

  FILE *f = fopen(path, "rt");

  if (!f) {
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  /* Read the entire file */
  buf = (char *)malloc(sizeof(char) * file_size + 1);
  if (!buf) {
    fclose(f);
    return NULL;
  }
  curr_str = (char *)malloc(sizeof(char) * file_size + 1);
  if (!curr_str) {
    free(buf);
    fclose(f);
    return NULL;
  }
  val_str = (char *)malloc(sizeof(char) * file_size + 1);
  if (!val_str) {
    free(buf);
    free(curr_str);
    fclose(f);
    return NULL;
  }
  read_bytes = fread(buf, 1, file_size, f);
  fclose(f);
  if (read_bytes != file_size) {
    free(buf);
    free(curr_str);
    return NULL;
  }
  buf[file_size] = '\0';

  /* Allocate memory for the ini file struct */
  ptr = (ini_file *)malloc(sizeof(ini_file));
  if (!ptr) {
    free(buf);
    free(curr_str);
    return NULL;
  }

  /* Allocate memory for the array of sections */
  ptr->ptr_sections =
      (ini_section *)malloc(sizeof(ini_section) * INILOAD_INITIAL_CAP);
  if (!ptr->ptr_sections) {
    free(buf);
    free(curr_str);
    free(ptr);
    return NULL;
  }
  ptr->cap_sections = INILOAD_INITIAL_CAP;
  ptr->num_sections = 0;

  /* Parse using a state machine/automaton */
  for (i = 0; i < file_size + 1 && !bad_syntax && !alloc_error; i++) {
    switch (state) {

    case INIPS_NONE:
      if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == ' ' || buf[i] == '\t' ||
          buf[i] == '\0') {
        /* Skip empty lines or spaces */
      } else if (buf[i] == ';' || buf[i] == '#') {
        /* Found a comment */
        state = INIPS_COMMENT;
      } else if (buf[i] == '[') {
        /* Found a section name */
        state = INIPS_SECTION_NAME;
        curr_pos = 0;
      } else {
        /* Found a key name */
        state = INIPS_KEY_NAME;
        curr_pos = 0;
        curr_str[curr_pos++] = buf[i];
      }
      break;

    case INIPS_COMMENT:
      if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\0') {
        /* End of the comment */
        state = INIPS_NONE;
      } else {
        /* Read through the comment */
      }
      break;

    case INIPS_SECTION_NAME:
      if (buf[i] == ']') {
        /* End of section name */
        curr_str[curr_pos] = '\0';

        /* Add this section (if its name is not too long) */
        if (curr_pos > INILOAD_NAME_MAXLEN) {
          bad_syntax = 1;
          break;
        }

        curr_section = __ini_add_section(ptr, curr_str);
        if (curr_section == NULL) {
          alloc_error = 1;
          break;
        }

        state = INIPS_AFTER_SECTION_NAME;
      } else if (buf[i] == '[' || buf[i] == '=' || buf[i] == ';' ||
                 buf[i] == '#' || buf[i] == '\n' || buf[i] == '\r' ||
                 buf[i] == '\0') {
        /* Bad syntax */
        bad_syntax = 1;
      } else {
        /* Read the section name */
        curr_str[curr_pos++] = buf[i];
      }
      break;

    case INIPS_AFTER_SECTION_NAME:
      if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\0') {
        /* We expect a newline or end of file after a section name */
        state = INIPS_NONE;
      } else {
        bad_syntax = 1;
      }
      break;

    case INIPS_KEY_NAME:
      if (buf[i] == ' ' || buf[i] == '\t' || buf[i] == '=') {
        curr_str[curr_pos] = '\0';
        state = INIPS_AFTER_KEY_NAME;
      } else if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\0' ||
                 buf[i] == '[' || buf[i] == ']') {
        /* Unexpected newline or end of file in the key name */
        bad_syntax = 1;
      } else {
        /* Read the key name */
        curr_str[curr_pos++] = buf[i];
      }
      break;

    case INIPS_AFTER_KEY_NAME:
      if (buf[i] == ' ' || buf[i] == '\t') {
        /* Skip through the trailing spaces */
      } else if (buf[i] == '=') {
        /* Found a (possible) beginning of a key value */
        state = INIPS_BEFORE_KEY_VALUE;
        val_pos = 0;
        quotes = 0;
      } else {
        /* Everything else is bad syntax */
        bad_syntax = 1;
      }
      break;

    case INIPS_BEFORE_KEY_VALUE:
      if (buf[i] == ' ' || buf[i] == '\t') {
        /* Skip possible spaces before the value */
      } else if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\0' ||
                 buf[i] == '[' || buf[i] == ']') {
        /* This characters cannot be before the value, bad syntax */
        bad_syntax = 1;
      } else if (buf[i] == '\"') {
        /* Optional quotes tell us that the value is a string */
        quotes = 1;
        state = INIPS_KEY_VALUE;
      } else {
        /* Found the value already */
        val_str[val_pos++] = buf[i];
        state = INIPS_KEY_VALUE;
      }
      break;

    case INIPS_KEY_VALUE:
      if (buf[i] == '\n' || buf[i] == '\r' || buf[i] == '\0' ||
          buf[i] == '\"') {
        if (buf[i] == '\"' && !quotes) {
          /* Didn't have quotes in the beginning, not expecting them now */
          bad_syntax = 1;
          break;
        }

        /* Otherwise found the end of the value */
        val_str[val_pos] = '\0';

        if (curr_section == NULL) {
          /* The key is not in any section but we don't have a section with an
             empty name yet */
          curr_section = __ini_add_section(ptr, "");
          if (curr_section == NULL) {
            alloc_error = 1;
            break;
          }
        }
        __ini_add_key(curr_section, curr_str, INI_KEY_STRING, val_str);

        state = INIPS_NONE;
      } else if (buf[i] == '[' || buf[i] == ']' || buf[i] == '=') {
        /* These symbols can only be inside a quoted string */
        if (!quotes) {
          bad_syntax = 1;
        } else {
          val_str[val_pos++] = buf[i];
        }
      } else {
        /* Everything else can be a part of the value */
        val_str[val_pos++] = buf[i];
      }
      break;

    default:
      break;
    }

    /* printf("%c <-- %d\n", buf[i], state); */
  }

  free(buf);
  free(curr_str);
  free(val_str);

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
  size_t s, k;
  for (s = 0; s < ini->num_sections; s++) {
    if (strcmp(ini->ptr_sections[s].name, section_name) == 0) {
      return ini->ptr_sections[s].num_keys;
    }
  }
  return 0;
}

int ini_has_key(ini_file *ini, const char *section_name, const char *key_name) {
  size_t s, k;
  for (s = 0; s < ini->num_sections; s++) {
    if (strcmp(ini->ptr_sections[s].name, section_name) == 0) {
      for (k = 0; k < ini->ptr_sections[s].num_keys; k++) {
        if (strcmp(ini->ptr_sections[s].ptr_keys[k].name, key_name) == 0) {
          return 1;
        }
      }
    }
  }
  return 0;
}

int ini_get_int(ini_file *ini, const char *section_name, const char *key_name,
                int default_val) {
  size_t s, k;
  for (s = 0; s < ini->num_sections; s++) {
    if (strcmp(ini->ptr_sections[s].name, section_name) == 0) {
      for (k = 0; k < ini->ptr_sections[s].num_keys; k++) {
        if (strcmp(ini->ptr_sections[s].ptr_keys[k].name, key_name) == 0) {
          return (ini->ptr_sections[s].ptr_keys[k].type == INI_KEY_INT)
                     ? ini->ptr_sections[s].ptr_keys[k].value.int_val
                     : default_val;
        }
      }
    }
  }
  return default_val;
}

float ini_get_float(ini_file *ini, const char *section_name,
                    const char *key_name, float default_val) {
  size_t s, k;
  for (s = 0; s < ini->num_sections; s++) {
    if (strcmp(ini->ptr_sections[s].name, section_name) == 0) {
      for (k = 0; k < ini->ptr_sections[s].num_keys; k++) {
        if (strcmp(ini->ptr_sections[s].ptr_keys[k].name, key_name) == 0) {
          return (ini->ptr_sections[s].ptr_keys[k].type == INI_KEY_FLOAT)
                     ? ini->ptr_sections[s].ptr_keys[k].value.float_val
                     : default_val;
        }
      }
    }
  }
  return default_val;
}

char *ini_get_string(ini_file *ini, const char *section_name,
                     const char *key_name, char *default_val) {
  size_t s, k;
  for (s = 0; s < ini->num_sections; s++) {
    if (strcmp(ini->ptr_sections[s].name, section_name) == 0) {
      for (k = 0; k < ini->ptr_sections[s].num_keys; k++) {
        if (strcmp(ini->ptr_sections[s].ptr_keys[k].name, key_name) == 0) {
          return (ini->ptr_sections[s].ptr_keys[k].type == INI_KEY_STRING)
                     ? ini->ptr_sections[s].ptr_keys[k].value.string_val
                     : default_val;
        }
      }
    }
  }
  return default_val;
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

#endif /* INILOAD_H */