// Size of the stat struct: 144 bytes.
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
  struct stat buf;
  printf("%ld\n", sizeof(buf));
}
