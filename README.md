# My_Git — Git Implementation in C++


A Git-like version control system implemented in C++ from scratch.
Implements **repository initialization**, **blob object storage**, **SHA-256 hashing**, **zlib compression**, and **object inspection**.

> This project is built alongside a compression library that implements the algorithms used internally by zlib — including RLE, LZ77, Huffman Coding, and DEFLATE.
> **Compression project:** [github.com/khachik14700/zlib](https://github.com/khachik14700/zlib)

---

## Commands

### init
Initializes a new repository structure identical to Git.
```bash
./mygit init
```
Creates the following structure:
```
.git/
.git/objects/
.git/refs/
.git/refs/heads/
.git/HEAD
.git/config
```

### hash-object
Creates a blob object from a file, serializes it, hashes it with SHA-256, and optionally writes it to the object database.
```bash
./mygit hash-object file.txt        # hash only
./mygit hash-object -w file.txt     # hash and write to .git/objects
```

### cat-file
Reads a stored object from the database and displays information about it.
```bash
./mygit cat-file -t <object_id>     # show type
./mygit cat-file -s <object_id>     # show size
./mygit cat-file -e <object_id>     # check if object exists
./mygit cat-file -p <object_id>     # pretty-print content
```

---

## How Object Storage Works

When an object is written:
```
Input file → Blob serialization → SHA-256 hash → zlib compression → Binary write to .git/objects
```

The object is serialized using Git's exact format before hashing:
```
blob <size>\0<content>
```

For example, a file containing `hello` becomes:
```
blob 5\0hello
```

The SHA-256 hash of the full serialized object is used as the object ID. The object is then split into a directory and filename:
```
hash: abcdef1234...
stored at: .git/objects/ab/cdef1234...
```
This mirrors exactly how Git stores objects internally.

---

## Project Structure

```
.
├── CLI/
│   ├── CommandParser.h
│   ├── CommandParser.cpp
│   ├── CommandType.h
│   ├── CatFileMode.h
│   └── ParsedCommand.h
├── CORE/
│   ├── Repository.h
│   ├── Repository.cpp
│   ├── RepositoryPaths.h
│   ├── RepositoryPaths.cpp
│   ├── RepositoryConfig.h
│   ├── RepositoryConfig.cpp
│   ├── ObjectStore.h
│   └── ObjectStore.cpp
├── OBJECTS/
│   ├── Blob.h
│   ├── Blob.cpp
│   ├── ObjectHeader.h
│   ├── ObjectHeader.cpp
│   ├── ObjectType.h
│   ├── ObjectTypeUtils.h
│   ├── ParsedObject.h
│   └── ParsedObject.cpp
├── INFRA/
│   ├── Hash.h
│   ├── Hash.cpp
│   ├── Compression.h
│   ├── Compression.cpp
│   ├── FileSystemUtils.h
│   └── FileSystemUtils.cpp
├── main.cpp
└── Makefile
```

---

## Architecture

The project is divided into four modules, each with a clear responsibility.

### CLI
Handles command-line argument parsing. Converts raw `argv` into a structured `ParsedCommand` that the rest of the system consumes.

| Class | Responsibility |
|-------|---------------|
| `CommandParser` | Parses `argc / argv` into `ParsedCommand` |
| `ParsedCommand` | Stores command type, path, flags, and error state |
| `CommandType` | Enum — `Init`, `HashObject`, `CatFile`, `Unknown` |
| `CatFileMode` | Enum — `Type`, `Size`, `Exists`, `PrettyPrint` |

### CORE
Contains the main repository and object storage logic.

| Class | Responsibility |
|-------|---------------|
| `Repository` | Initializes and validates the repository |
| `RepositoryPaths` | Centralizes all internal path resolution |
| `RepositoryConfig` | Generates and serializes the config file |
| `ObjectStore` | Writes and reads objects from `.git/objects` |

### OBJECTS
Defines what Git objects are and how they are serialized and parsed.

| Class | Responsibility |
|-------|---------------|
| `Blob` | Represents file content as a Git blob object |
| `ObjectHeader` | Builds the `blob <size>\0` header |
| `ParsedObject` | Parses a raw object into type, size, and payload |
| `ObjectType` | Enum — `Blob`, `Tree`, `Commit`, `Unknown` |
| `ObjectTypeUtils` | Converts between `ObjectType` and string |

### INFRA
Low-level utilities used by the rest of the system.

| Class | Responsibility |
|-------|---------------|
| `Hash` | SHA-256 hashing via `Hash::sha256(...)` |
| `Compression` | zlib compress and decompress |
| `FileSystemUtils` | File and directory operations |

---

## Compression

This project uses zlib to compress and decompress objects before writing them to disk — the same way Git does it in production.

Alongside My_Git, I am also building a separate project where the compression algorithms are implemented manually:

```
RLE       — replaces repeated byte sequences with count and value pairs
LZ77      — finds repeated patterns and replaces them with (offset, length) references
Huffman   — assigns shorter bit codes to more frequent symbols
DEFLATE   — combines LZ77 and Huffman into a full compression pipeline
```

DEFLATE is the algorithm that zlib uses internally. Building it from scratch makes the compression call inside My_Git fully understood, not just used.

> **Compression project:** [github.com/khachik14700/zlib](https://github.com/khachik14700/zlib)

---

## Build

Requirements:
- g++ with C++17 support
- zlib
- make

```bash
make
```

Clean build artifacts:
```bash
make fclean
```

Rebuild from scratch:
```bash
make re
```

---

## Example

```bash
$ ./mygit init
Initialized empty repository at .git/

$ ./mygit hash-object -w hello.txt
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c4fa0b592d...

$ ./mygit cat-file -t 2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c4fa0b592d...
blob

$ ./mygit cat-file -s 2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c4fa0b592d...
5

$ ./mygit cat-file -p 2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c4fa0b592d...
hello
```

---

## Roadmap

| Step | Feature | Status |
|------|---------|--------|
| 1 | Repository initialization | ✅ Done |
| 2 | Object storage — hashing, compression, binary I/O | ✅ Done |
| 3 | Core commands — init, hash-object, cat-file | ✅ Done |
| 4 | UML architecture documentation | ✅ Done |
| 5 | Tree objects + write-tree | 🔧 Next |
| 6 | Staging area + add command | 📋 Planned |
| 7 | Commit objects | 📋 Planned |
| 8 | Branches and HEAD | 📋 Planned |
| 9 | Log command | 📋 Planned |
| 10 | Checkout | 📋 Planned |

---