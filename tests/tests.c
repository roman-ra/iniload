#define INILOAD_IMPLEMENTATION
#define INILOAD_NAME_MAXLEN 30
#include "iniload.h"

#include <assert.h>
#include <string.h>

void test_empty_file() {
  ini_file *ini;
  printf("test_empty_file()...");
  ini = ini_load("inis/test_empty.ini");

  assert(ini != NULL);
  assert(ini_num_sections(ini) == 0);

  ini_free(ini);

  printf("SUCCESS\n");
}

void test_empty_section() {
  ini_file *ini;
  printf("test_empty_section()...");
  ini = ini_load("inis/test_empty_section.ini");

  assert(ini != NULL);
  assert(ini_num_sections(ini) == 1);
  assert(ini_has_section(ini, "empty_section"));
  assert(ini_num_keys(ini, "empty_section") == 0);

  ini_free(ini);

  printf("SUCCESS\n");
}

void test_many_empty_sections() {
  ini_file *ini;
  printf("test_many_empty_sections()...");
  ini = ini_load("inis/test_many_empty_sections.ini");

  assert(ini != NULL);
  assert(ini_num_sections(ini) == 9);

  ini_free(ini);

  printf("SUCCESS\n");
}

void test_keys_without_section() {
  ini_file *ini;
  printf("test_keys_without_section()...");
  ini = ini_load("inis/test_keys_without_section.ini");

  assert(ini != NULL);
  assert(ini_num_sections(ini) == 1);
  assert(ini_has_section(ini, ""));
  assert(ini_get_int(ini, "", "key1", 1337) == 1);
  assert(strcmp(ini_get_string(ini, "", "key2", "wrong"), "no section") == 0);
  assert(ini_get_int(ini, "", "key", 1337) == -1);

  ini_free(ini);

  printf("SUCCESS\n");
}

void test_multiple_sections() {
  ini_file *ini;
  printf("test_multiple_sections()...");
  ini = ini_load("inis/test_multiple_sections.ini");

  assert(ini != NULL);
  assert(ini_num_sections(ini) == 4);
  assert(ini_has_section(ini, "s1"));
  assert(ini_has_section(ini, "s2"));
  assert(ini_has_section(ini, "s3"));
  assert(ini_has_section(ini, "s4"));
  assert(strcmp(ini_get_string(ini, "s1", "test", "wrong"), "test") == 0);
  assert(strcmp(ini_get_string(ini, "s4", "key", "wrong"), "value") == 0);
  assert(ini_get_int(ini, "s4", "key2", -1) == 42);

  ini_free(ini);

  printf("SUCCESS\n");
}

void test_large_section() {
  ini_file *ini;
  printf("test_large_section()...");
  ini = ini_load("inis/test_large_section.ini");

  assert(ini != NULL);
  assert(ini_num_sections(ini) == 1);
  assert(ini_has_section(ini, "large"));
  assert(ini_num_keys(ini, "large") == 200);

  ini_free(ini);

  printf("SUCCESS\n");
}

void test_spaces() {
  ini_file *ini;
  printf("test_spaces()...");
  ini = ini_load("inis/test_spaces.ini");

  assert(ini != NULL);
  assert(ini_num_sections(ini) == 2);
  assert(ini_has_section(ini, "section"));
  assert(ini_has_section(ini, "section2"));
  

  ini_free(ini);

  printf("SUCCESS\n");
}

void test_long_section_name() {
  ini_file *ini;
  printf("test_long_section_name()...");
  ini = ini_load("inis/test_long_section_name.ini");

  assert(ini == NULL);

  printf("SUCCESS\n");
}

void test_long_key_name() {
  ini_file *ini;
  printf("test_long_key_name()...");
  ini = ini_load("inis/test_long_key_name.ini");

  assert(ini == NULL);

  printf("SUCCESS\n");
}

void test_bad_syntax() {
  ini_file *ini[8];
  printf("test_bad_syntax()...");
  
  ini[0] = ini_load("inis/test_bad_syntax_1.ini");
  assert(ini[0] == NULL);

  ini[1] = ini_load("inis/test_bad_syntax_2.ini");
  assert(ini[1] == NULL);

  ini[2] = ini_load("inis/test_bad_syntax_3.ini");
  assert(ini[2] == NULL);

  ini[3] = ini_load("inis/test_bad_syntax_4.ini");
  assert(ini[3] == NULL);

  ini[4] = ini_load("inis/test_bad_syntax_5.ini");
  assert(ini[4] == NULL);

  ini[5] = ini_load("inis/test_bad_syntax_6.ini");
  assert(ini[5] == NULL);

  ini[6] = ini_load("inis/test_bad_syntax_7.ini");
  assert(ini[6] == NULL);

  ini[7] = ini_load("inis/test_bad_syntax_8.ini");
  assert(ini[7] == NULL);

  printf("SUCCESS\n");
}

int main(int argc, char *argv[]) {
  test_empty_file();
  test_empty_section();
  test_many_empty_sections();
  test_keys_without_section();
  test_multiple_sections();
  test_large_section();
  test_spaces();
  test_long_section_name();
  test_long_key_name();
  test_bad_syntax();

  return 0;
}