# Haystack Object Storage and Go Implementation

FB published the design of their object store in 2010:

> [Finding a needle in Haystack: Facebook’s photo storage](https://www.usenix.org/legacy/event/osdi10/tech/full_papers/Beaver.pdf)

At that time, FB stored 260B images, or 20PB. 60TB new images per day. This
short talk is about the design and about an open source implementation
[seaweedfs](https://github.com/chrislusf/seaweedfs).

## Finding a needle in Haystack: Facebook’s photo storage

> Our key observation is that this traditional
design incurs an excessive number of disk operations because of metadata
lookups.

### What is a metadata lookup?

* [stat](https://man7.org/linux/man-pages/man2/stat.2.html) system call, [IEEE
  Std
1003.1-2017](https://pubs.opengroup.org/onlinepubs/9699919799/functions/stat.html)

## seaweedfs
