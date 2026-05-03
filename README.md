# My_Git — Git Implementation in C++

A Git-like version control system implemented in C++ from scratch.
Built step by step, understanding every algorithm and data structure along the way.

> Alongside this project, a companion compression library is being built that implements the algorithms used internally by zlib — including RLE, LZ77, Huffman Coding, and DEFLATE.
> **Compression project:** [github.com/khachik14700/zlib](https://github.com/khachik14700/zlib)

---

## Features

| Feature | Status |
|---------|--------|
| Repository initialization | ✅ Done |
| SHA-256 hashing via OpenSSL | ✅ Done |
| zlib compression | ✅ Done |
| Blob object storage | ✅ Done |
| Tree object storage | ✅ Done |
| Object inspection (cat-file) | ✅ Done |
| Staging area (add) | ✅ Done |
| Config (user.name, user.email) | ✅ Done |
| write-tree from index | ✅ Done |
| write-tree from filesystem | ✅ Done |
| Commit objects | 🔧 In Progress |
| Refs and HEAD management | 📋 Planned |
| Log command | 📋 Planned |
| Checkout | 📋 Planned |
| Diff | 📋 Planned |
| Merge | 📋 Planned |

---

## Commands

### init
Initializes a new repository.
```bash
./mygit init
./mygit init <path>
```

Creates the following structure:
```
.git/
├── HEAD
├── config
├── index
├── objects/
└── refs/
    └── heads/
```

---

### config
Read or write repository configuration values.
```bash
./mygit config user.name "John Doe"
./mygit config user.email "john@example.com"
./mygit config user.name
```

Supported sections: `core`, `user`

Config file format:
```
[core]
    repositoryformatversion = 0
    filemode = true
    bare = false
[user]
    name = John Doe
    email = john@example.com
```

---

### hash-object
Creates a blob object from a file.
```bash
./mygit hash-object file.txt         # compute hash only
./mygit hash-object -w file.txt      # hash and write to .git/objects
```

---

### add
Stage files for commit.
```bash
./mygit add file.txt      # stage a single file
./mygit add .             # stage all files recursively
```

Reads the file, creates a blob object, and records the entry in `.git/index`.

---

### write-tree
Build a tree object from the staging area.
```bash
./mygit write-tree           # build from .git/index (standard)
./mygit write-tree --from-fs # build by traversing the filesystem directly
```

---

### cat-file
Inspect stored objects.
```bash
./mygit cat-file -t <hash>   # show object type (blob, tree, commit)
./mygit cat-file -s <hash>   # show object size in bytes
./mygit cat-file -e <hash>   # check if object exists
./mygit cat-file -p <hash>   # pretty-print object contents
```

---

## How Object Storage Works

Every object goes through the same pipeline:

```
Content → Serialize → SHA-256 → zlib compress → Write to .git/objects
```

Objects are stored by their hash:
```
hash: f0c38b23b154f77a...
path: .git/objects/f0/c38b23b154f77a...
```

### Blob format
```
blob <size>\0<content>
```

### Tree format
```
tree <size>\0<entries>
```

Each entry in binary:
```
<mode> <name>\0<32 binary bytes of hash>
```

Example pretty-printed output:
```
100644 blob 66224663...    file.txt
040000 tree 3d36babe...    src
```

### Commit format (coming soon)
```
tree <tree_hash>
parent <parent_hash>
author <name> <email> <timestamp> <timezone>
committer <name> <email> <timestamp> <timezone>

<message>
```

---

## Architecture

The project is divided into four layers, each with a clear responsibility:

```
cli → core → objects → infra
          ↑
         ops
```

```
src/
├── main.cpp
├── cli/
│   ├── CommandParser.h / .cpp    ← parses argc/argv into ParsedCommand
│   └── CommandType.h             ← enum: Init, HashObject, CatFile, Add, ...
├── core/
│   ├── Repository.h / .cpp       ← init, exists, isValid
│   ├── RepositoryPaths.h / .cpp  ← all .git/ path resolution
│   ├── RepositoryConfig.h / .cpp ← read/write .git/config
│   └── ObjectStore.h / .cpp      ← write/read objects with compression
├── objects/
│   ├── Blob.h / .cpp             ← file content as Git object
│   ├── Tree.h / .cpp             ← directory snapshot as Git object
│   ├── TreeEntry.h / .cpp        ← single entry in a tree
│   ├── Index.h / .cpp            ← staging area read/write
│   ├── IndexEntry.h / .cpp       ← single entry in the index
│   ├── ObjectHeader.h / .cpp     ← builds "blob 12\0" style headers
│   ├── ObjectType.h / .cpp       ← enum: Blob, Tree, Commit, Unknown
│   └── ParsedObject.h / .cpp     ← parses raw objects from disk
├── ops/
│   ├── WriteTree.h / .cpp        ← buildTree, buildTreeFromIndex
│   └── Add.h / .cpp              ← addPath, addSingleFile
└── infra/
    ├── Hash.h / .cpp             ← SHA-256, hexToBytes, bytesToHex
    ├── Compression.h / .cpp      ← zlib compress/decompress
    └── FileSystemUtils.h / .cpp  ← file and directory operations
```

### Layer responsibilities

**cli** — argument parsing only. Converts raw `argv` into a structured `ParsedCommand`. Has no knowledge of Git internals.

**core** — repository and storage logic. Knows where `.git/` is, how to read/write objects, config, and index.

**objects** — Git object model. Defines what blob, tree, and commit are. Knows how to serialize and deserialize each format.

**ops** — high-level operations. Combines objects, core, and infra to implement commands like `add` and `write-tree`.

**infra** — low-level utilities. Hashing, compression, filesystem operations. No Git-specific logic.

## Architecture Diagram

Below is a visual representation of the system architecture:

![Architecture Diagram](./architecture.svg)

> The diagram illustrates the layered design of the project (`cli → core → objects → infra → ops`) and how data flows through the system — from user commands down to object storage.

[Open full diagram](./architecture.svg)

---

## Key Design Decisions

**SHA-256 instead of SHA-1**
Git uses SHA-1 historically. This project uses SHA-256 from the start — more modern and collision-resistant.

**Same object pipeline for all types**
Every object (blob, tree, commit) goes through the same storage pipeline:
```
serialize() → sha256() → compress() → write to disk
```

**Index before write-tree**
`write-tree` reads from `.git/index`, not from the filesystem directly.
This matches real Git behavior — `add` stages files, `write-tree` snapshots the index.

**`--from-fs` flag (non-standard)**
A custom flag that lets `write-tree` traverse the filesystem directly without using the index. Not present in Git — added as a learning and debugging tool.

---

## Build

Requirements:
- g++ with C++17 support
- OpenSSL (for SHA-256)
- zlib
- make

```bash
make        # build
make fclean # clean build artifacts
make re     # rebuild from scratch
```

---

## Example workflow

```bash
# Initialize repository
./mygit init

# Set identity
./mygit config user.name "Khachik Khachatryan"
./mygit config user.email "khachik@email.com"

# Create files
echo "hello" > file.txt
mkdir src && echo "void foo(){}" > src/utils.cpp

# Stage files
./mygit add .

# Inspect index
cat .git/index

# Build tree from index
./mygit write-tree

# Inspect tree
./mygit cat-file -p <tree_hash>

# Inspect blob
./mygit cat-file -p <blob_hash>
```

---

## About

Built as a learning project to deeply understand how Git works internally —
not by reading about it, but by implementing every piece from scratch.
