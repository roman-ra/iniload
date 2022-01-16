#define INILOAD_IMPLEMENTATION
#include "iniload.h"

int main(int argc, char *argv[]) {
  ini_file *ini = ini_load("test.ini");
  printf("0x%lx\n", (unsigned long)ini);
  printf("%s\n", ini_get_string(ini, "", "no_section_key", "default"));
  printf("%s\n", ini_get_string(ini, "section1", "key1", "default"));
  printf("%s\n", ini_get_string(ini, "section1", "key2", "default"));
  printf("%s\n", ini_get_string(ini, "section1", "key3", "default"));
  if (ini) {
    ini_free(ini);
  }
  return 0;
}