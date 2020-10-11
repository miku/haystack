#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[]) {
  DIR *d;
  struct dirent *dent;
  d = opendir(".");

  struct stat buf;
  int status;
  int i = 0;

  clock_t begin = clock();
  if (d) {
    while ((dent = readdir(d)) != NULL) {
      status = stat(dent->d_name, &buf);
      i++;
      if (status != 0) {
        return -1;
      }
      // st_blocks: this is the amount of disk space that the file occupies,
      // measured in units of 512-byte blocks.
      int nlen = strlen(dent->d_name);
      char short_name[16];
      strncpy(short_name, dent->d_name, 15);
      short_name[15] = '\0';
      printf(
          "%16s\t%8lu\t%8lu\t%u\t%lu\t%u\t%u\t%10lu\t%lu\t%lu\t%lu\t%lu\t%lu\n",
          short_name, buf.st_dev, buf.st_ino, buf.st_mode, buf.st_nlink,
          buf.st_uid, buf.st_gid, buf.st_size, buf.st_atime, buf.st_mtime,
          buf.st_ctime, buf.st_blocks, buf.st_blksize);
    }
    closedir(d);
  } else {
    printf("failed to open dir\n");
    return 1;
  }
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  fprintf(stderr, "%d stat calls in %f (%f per op)\n", i, time_spent,
          time_spent / i);
}
