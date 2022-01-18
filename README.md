## iniload
**iniload** is an easy-to-use, header-only INI file loader written in ANSI C.

#### Usage example
Do not forget to `#define INILOAD_IMPLEMENTATION` in one of your source files before including iniload.h:

A minimal example:
```
#define INILOAD_IMPLEMENTATION
#include "iniload.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
  ini_file *ini = ini_load("settings.ini");
  
  if (ini == NULL) {
    return -1;
  }

  float volume = ini_get_float(ini, "audio", "volume", 1.0f);
  int nproc = ini_get_int(ini, "", "nproc", 1);
  char *path = ini_get_string(ini, "", "path", "default");
  
  ini_free(ini); 
  
  printf("%f, %d, %s", volume, nproc, path);
  
  return 0;
}
```

By default, the names of sections and keys cannot be longer than 128. This limit can be increased by `#define INILOAD_NAME_MAXLEN *your value*` before including iniload.h in the file where you defined `INILOAD_IMPLEMENTATION`.
