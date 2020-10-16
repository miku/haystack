# Notes

* seaweedfs has commands, "server" combines:

* master
* volume
* filer
* s3

They use the same code path.

```
// weed/commands/server.go

if *isStartingFiler {
    go func() {
        time.Sleep(1 * time.Second)

        filerOptions.startFiler()

    }()
}

if *isStartingS3 {
    go func() {
        time.Sleep(2 * time.Second)

        s3Options.startS3Server()

    }()
}

if *isStartingMsgBroker {
    go func() {
        time.Sleep(2 * time.Second)
        msgBrokerOptions.startQueueServer()
    }()
}
```

What are the roles? How do they communicate?

## Master

* ip, port, "peers"
* volumeSizeLimitMB
* garbageThreshold
* whiteList, raftResumeState

Some metrics options.

## Volume

A volume server manages a number of volumes; set by `-max`

```
  -max string
        maximum numbers of volumes, count[,count]... If set to zero, the limit will be auto configured. (default "8")
```

The volume servers can run across the cluster.

* important opt, "index"

```
  -index string
        Choose [memory|leveldb|leveldbMedium|leveldbLarge] mode for memory~performance balance. (default "memory")
```

> What is in an index?

Starting up:

```
$ weed volume -max=100 -dir=vol-0
I1016 12:01:25 55983 disk_location.go:126] Store started on dir: vol-0 with 0 ec shards
I1016 12:01:25 55983 volume_grpc_client_to_master.go:52] Volume server start with seed master nodes: [localhost:9333]
I1016 12:01:25 55983 volume.go:331] Start Seaweed volume server 30GB 2.03 d155f907 at 0.0.0.0:8080
I1016 12:01:25 55983 volume_grpc_client_to_master.go:114] Heartbeat to: localhost:9333
I1016 12:01:25 55983 volume_grpc_client_to_master.go:135] Volume Server found a new master newLeader: 172.24.235.158:9333 instead of localhost:9333
I1016 12:01:28 55983 volume_grpc_client_to_master.go:114] Heartbeat to: 172.24.235.158:9333
```


## Filer


## S3

----

# Testdrives

* master and volume server up, can upload file

Initial state:

```
$ tree -sh
.
├── [4.0K]  mdir-0
│   ├── [  28]  conf
│   ├── [ 118]  log
│   └── [4.0K]  snapshot
└── [4.0K]  vol-0

3 directories, 2 files
```

Upload goes to master first, but data move to volume directly (I think):

```
Example: weed upload -master=localhost:9333 file1 [file2 file3]
         weed upload -master=localhost:9333 -dir=one_directory -include=*.pdf
```

Option of splitting large files, seemingly transparently.

A few tuning options:

```
  -collection string
        optional collection name
  -dataCenter string
        optional data center name
```

> Q: What is a collection? A bucket?

On upload, JSON is returned:

```
$ weed upload -master "localhost:9333" ../README.md
[{"fileName":"README.md","fileUrl":"172.24.235.158:8080/1,0146cabbc4","fid":"1,0146cabbc4","size":6998}]
```

Leveldb gets content:

```
$ tree -sh
.
├── [4.0K]  mdir-0
│   ├── [  28]  conf
│   ├── [ 426]  log
│   └── [4.0K]  snapshot
└── [4.0K]  vol-0
    ├── [3.3K]  1.dat
    ├── [  16]  1.idx
    ├── [  57]  1.vif
    ├── [   8]  2.dat
    ├── [   0]  2.idx
    ├── [  57]  2.vif
    ├── [   8]  3.dat
    ├── [   0]  3.idx
    ├── [  57]  3.vif
    ├── [   8]  4.dat
    ├── [   0]  4.idx
    ├── [  57]  4.vif
    ├── [   8]  5.dat
    ├── [   0]  5.idx
    ├── [  57]  5.vif
    ├── [   8]  6.dat
    ├── [   0]  6.idx
    ├── [  57]  6.vif
    ├── [   8]  7.dat
    ├── [   0]  7.idx
    └── [  57]  7.vif

```

What is fid?

```
[
  {
    "fileName": "README.md",
    "fileUrl": "172.24.235.158:8080/1,0146cabbc4",
    "fid": "1,0146cabbc4",
    "size": 6998
  }
]
```

Weed preallocates 7 volumes?

```
I1016 12:08:01 55983 store.go:123] In dir vol-0 adds volume:1 collection: replicaPlacement:000 ttl:
I1016 12:08:01 55983 volume_loading.go:104] loading index vol-0/1.idx to memory
I1016 12:08:01 55983 store.go:127] add volume 1
I1016 12:08:01 55983 store.go:123] In dir vol-0 adds volume:2 collection: replicaPlacement:000 ttl:
I1016 12:08:01 55983 volume_loading.go:104] loading index vol-0/2.idx to memory
I1016 12:08:01 55983 store.go:127] add volume 2
I1016 12:08:01 55983 store.go:123] In dir vol-0 adds volume:3 collection: replicaPlacement:000 ttl:
I1016 12:08:01 55983 volume_loading.go:104] loading index vol-0/3.idx to memory
I1016 12:08:01 55983 store.go:127] add volume 3
I1016 12:08:01 55983 store.go:123] In dir vol-0 adds volume:4 collection: replicaPlacement:000 ttl:
I1016 12:08:01 55983 volume_loading.go:104] loading index vol-0/4.idx to memory
I1016 12:08:01 55983 store.go:127] add volume 4
I1016 12:08:01 55983 store.go:123] In dir vol-0 adds volume:5 collection: replicaPlacement:000 ttl:
I1016 12:08:01 55983 volume_loading.go:104] loading index vol-0/5.idx to memory
I1016 12:08:01 55983 store.go:127] add volume 5
I1016 12:08:01 55983 store.go:123] In dir vol-0 adds volume:6 collection: replicaPlacement:000 ttl:
I1016 12:08:01 55983 volume_loading.go:104] loading index vol-0/6.idx to memory
I1016 12:08:01 55983 store.go:127] add volume 6
I1016 12:08:01 55983 store.go:123] In dir vol-0 adds volume:7 collection: replicaPlacement:000 ttl:
I1016 12:08:01 55983 volume_loading.go:104] loading index vol-0/7.idx to memory
I1016 12:08:01 55983 store.go:127] add volume 7
```

Master says:

```
I1016 12:01:28 52917 master_grpc_server.go:73] added volume server 172.24.235.158:8080
I1016 12:08:01 52917 volume_layout.go:333] Volume 1 becomes writable
I1016 12:08:01 52917 volume_growth.go:224] Created Volume 1 on topo:DefaultDataCenter:DefaultRack:172.24.235.158:8080
I1016 12:08:01 52917 master_grpc_server.go:155] master send to master@172.24.235.158:60710: url:"172.24.235.158:8080"  public_url:"172.24.235.158:8080"  new_vids:1
I1016 12:08:01 52917 volume_layout.go:333] Volume 2 becomes writable
I1016 12:08:01 52917 master_grpc_server.go:155] master send to master@172.24.235.158:60710: url:"172.24.235.158:8080"  public_url:"172.24.235.158:8080"  new_vids:2
```

* automatically compressed

----

# Neat code

* limit background compaction or copying speed in mega bytes per second; how to rate limit ops like this?
* limits of filesize (might live in ram once?) - -fileSizeLimitMB

# UI/UX Aspects

* config file scaffolding
* has makefile, responds to "make"
* sensible defaults; e.g. "/tmp" for data files, minimal assumptions
* flags with units; -fileSizeLimitMB

Production issues:

```
  -minFreeSpacePercent string
        minimum free disk space (default to 1%). Low disk space will mark all volumes as ReadOnly. (default "1")
```

Dev friendly, hacking, debugging:

```
  -pprof
        enable pprof http handlers. precludes --memprofile and --cpuprofile
```

Simple security:

```
  -whiteList string
        comma separated Ip addresses having write permission. No limit if empty.
```

# Surprises

```
$ weed volume -h
...
  -images.fix.orientation
        Adjust jpg orientation when uploading.
```

# Questions

```
$ grep -r "break init cycle"
weed/command/msg_broker.go:     cmdMsgBroker.Run = runMsgBroker // break init cycle
weed/command/upload.go: cmdUpload.Run = runUpload // break init cycle
weed/command/fix.go:    cmdFix.Run = runFix // break init cycle
weed/command/download.go:       cmdDownload.Run = runDownload // break init cycle
weed/command/mount.go:  cmdMount.Run = runMount // break init cycle
weed/command/server.go: cmdServer.Run = runServer // break init cycle
weed/command/filer_copy.go:     cmdCopy.Run = runCopy // break init cycle
weed/command/backup.go: cmdBackup.Run = runBackup // break init cycle
weed/command/filer_sync.go:     cmdFilerSynchronize.Run = runFilerSynchronize // break init cycle
weed/command/watch.go:  cmdWatch.Run = runWatch // break init cycle
weed/command/shell.go:  cmdShell.Run = runShell // break init cycle
weed/command/benchmark.go:      cmdBenchmark.Run = runBenchmark // break init cycle
weed/command/volume.go: cmdVolume.Run = runVolume // break init cycle
weed/command/compact.go:        cmdCompact.Run = runCompact // break init cycle
weed/command/s3.go:     cmdS3.Run = runS3 // break init cycle
weed/command/filer_replication.go:      cmdFilerReplicate.Run = runFilerReplicate // break init cycle
weed/command/filer.go:  cmdFiler.Run = runFiler // break init cycle
weed/command/master.go: cmdMaster.Run = runMaster // break init cycle
weed/command/export.go: cmdExport.Run = runExport // break init cycle
weed/command/scaffold.go:       cmdScaffold.Run = runScaffold // break init cycle
weed/command/webdav.go: cmdWebDav.Run = runWebDav // break init cycle
```

----

# Usage

## Starting master

```
$ weed master -mdir mdir-0
I1016 11:49:09 52917 file_util.go:23] Folder mdir-0 Permission: -rwxrwxr-x
I1016 11:49:09 52917 master.go:164] current: 172.24.235.158:9333 peers:
I1016 11:49:09 52917 master_server.go:107] Volume Size Limit is 30000 MB
I1016 11:49:09 52917 master_server.go:192] adminScripts:
I1016 11:49:09 52917 master.go:118] Start Seaweed Master 30GB 2.03 d155f907 at 0.0.0.0:9333
I1016 11:49:09 52917 raft_server.go:69] Starting RaftServer with 172.24.235.158:9333
I1016 11:49:09 52917 raft_server.go:128] current cluster leader:
I1016 11:49:09 52917 master.go:142] Start Seaweed Master 30GB 2.03 d155f907 grpc server at 0.0.0.0:19333
I1016 11:49:10 52917 masterclient.go:77] No existing leader found!
I1016 11:49:10 52917 raft_server.go:153] Initializing new cluster
I1016 11:49:10 52917 master_server.go:141] leader change event:  => 172.24.235.158:9333
I1016 11:49:10 52917 master_server.go:143] [ 172.24.235.158:9333 ] 172.24.235.158:9333 becomes leader.
I1016 11:49:14 52917 masterclient.go:125] redirected to leader 172.24.235.158:9333
I1016 11:49:14 52917 master_grpc_server.go:250] + client master@172.24.235.158:60710
```

Directory has to exist, otherwise panic - maybe a PR?

* I1016 11:49:09 52917 master.go:118] Start Seaweed Master 30GB 2.03 d155f907 at 0.0.0.0:9333
* I1016 11:49:09 52917 master.go:142] Start Seaweed Master 30GB 2.03 d155f907 grpc server at 0.0.0.0:19333


----

# Misc

## Tracing

* want to instrument code, so I see method call graph as the server run

There is some information on that:

* https://rakyll.org/latency-profiling/

> Instrumentation requires manual labor. Go code cannot be auto-instrumented by intercepting calls.
