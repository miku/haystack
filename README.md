# Haystack Object Storage and Go Implementation

FB published the design of their object store in 2010:

> [Finding a needle in Haystack: Facebook’s photo storage](https://www.usenix.org/legacy/event/osdi10/tech/full_papers/Beaver.pdf)

At that time, FB stored 260B images, or 20PB. 60TB new images per day. This
short talk is about the design and about an open source implementation
[seaweedfs](https://github.com/chrislusf/seaweedfs).

## Finding a needle in Haystack: Facebook’s photo storage

> Our key observation is that this traditional
design incurs an excessive number of disk operations because of metadata
lookups. [...] We carefully reduce this per photo metadata so that Haystack
storage machines can perform all metadata lookups in main memory. This choice
conserves disk operations for reading actual data and thus increases overall
throughput.

### What is a metadata lookup?

* [stat](https://man7.org/linux/man-pages/man2/stat.2.html) system call, [IEEE
  Std
1003.1-2017](https://pubs.opengroup.org/onlinepubs/9699919799/functions/stat.html)

How fast is is really?

> in the range 3-150 microseconds, probably, depending on file system cache

### FB use case (2010)

* 1B new photos per week (60TB)
* they generate four versions

Let's sketch (just guessing):

* 4B files
* accessed twice, 8B accesses per week
* 10mu/access
* 22h per week spent in metadata lookups; maybe much more as popular photos are requested more than twice

### Access mode, waste

> data is written once, read often, never modified, and rarely deleted.

Some waste.

> For the Photos application most of this metadata, such as permissions, is unused

For example, `stat` will return `st_mode` which is of type `mode_t` which is
probably [4
bytes](https://stackoverflow.com/questions/9602685/why-does-mode-t-use-4-byte)
or your native CPU word size - even though it mostly (boolean) flags.

That's about 15G/week only for the perms, which the paper says they did not
need (total stat size would be 500G/week, file metadata amounted for about 1%
of storage size).

### How many ops per photo?

> one (or typically more)
to translate the filename to an inode number, another to read the inode from
disk, and a final one to read the file itself. In short, using disk IOs for
metadata was the limiting factor for our read throughput.

### Goals and Features

* Haystack achieves **high throughput** and **low latency** by requiring **at
  most one disk operation per read** (with metadata in main memory).
* Redundancy.
* Efficiency, 28% fewer cost in storage, 4x reads/s.
* Simple.

## seaweedfs
