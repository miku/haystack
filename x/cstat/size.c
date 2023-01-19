// Size of the stat struct: 144 bytes.
// https://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html#index-struct-stat
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main()
{
    struct stat buf;
    printf("%ld\n", sizeof(buf));
}
