// Example program to get an impression of stat syscall performance.
//
//     $ tabstat
//
// To see output, run `tabstat -v`.
//
// Run stat on all files found in the current director and report the elapsed
// time and a time/op.
//
// nvme
//
//     728 stat calls in 0.002810 (0.000004 per op)
//     10087 stat calls in 0.089438 (0.000009 per op)
//     478 stat calls in 0.023035 (0.000048 per op)
//
//     ~4-50 microseconds/op
//
// spinning disk
//
//     4866 stat calls in 0.315178 (0.000065 per op)
//     258 stat calls in 0.042780 (0.000166 per op)
//
//     ~3-170 microseconds/op

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
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

      // Keep check low, but allow, e.g. tabstop -v.
      if (argc == 2) {
        int nlen = strlen(dent->d_name);
        char short_name[16];
        strncpy(short_name, dent->d_name, 15);
        short_name[15] = '\0';
        // st_blocks: this is the amount of disk space that the file occupies,
        // measured in units of 512-byte blocks.
        printf(
            "%16s\t%8lu\t%8lu\t%u\t%lu\t%u\t%u\t%10lu\t%lu\t%lu\t%lu\t%lu\t%"
            "lu\n",
            short_name, buf.st_dev, buf.st_ino, buf.st_mode, buf.st_nlink,
            buf.st_uid, buf.st_gid, buf.st_size, buf.st_atime, buf.st_mtime,
            buf.st_ctime, buf.st_blocks, buf.st_blksize);
      }
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
