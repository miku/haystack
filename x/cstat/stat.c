#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
  DIR *d;
  struct dirent *dent;
  d = opendir(".");

  struct stat buf;
  int status;

  if (d) {
    while ((dent = readdir(d)) != NULL) {
      status = stat(dent->d_name, &buf);
      if (status != 0) {
        return -1;
      }
      // st_blocks: this is the amount of disk space that the file occupies,
      // measured in units of 512-byte blocks.
      printf("%20s\t%lu\t%lu\t%u\t%lu\t%u\t%u\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\n",
             dent->d_name, buf.st_dev, buf.st_ino, buf.st_mode, buf.st_nlink,
             buf.st_uid, buf.st_gid, buf.st_size, buf.st_atime, buf.st_mtime,
             buf.st_ctime, buf.st_blocks, buf.st_blksize);
    }
    closedir(d);
  }
}
