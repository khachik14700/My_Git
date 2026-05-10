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
| Commit objects | ✅ Done |
| Refs and HEAD management | ✅ Done |
| Branch management | ✅ Done |
| Switch | ✅ Done |
| .mygitignore | ✅ Done |
| Log command | 📋 Planned |
| Status | 📋 Planned |
| Diff | 📋 Planned |
| Checkout | 📋 Planned |
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
./mygit config user.name "Khachik"
./mygit config user.email "khachik@email.com"
./mygit config user.name
```

Supported sections: `core`, `user`

Config file format:
```ini
[core]
    repositoryformatversion = 0
    filemode = true
    bare = false
[user]
    name = Khachik
    email = khachik@email.com
```

---

### hash-object
Creates a blob object from a file.
```bash
./mygit hash-object file.txt        # compute hash only
./mygit hash-object -w file.txt     # hash and write to .git/objects
```

---

### add
Stage files for commit.
```bash
./mygit add file.txt     # stage a single file
./mygit add .            # stage all files recursively
```

Reads the file, creates a blob object, and records the entry in `.git/index`.
Automatically skips the My_Git binary itself and any paths listed in `.mygitignore`.

---

### rm
Remove files from the working directory and/or the staging area.
```bash
./mygit rm file.txt           # remove from disk and index
./mygit rm --cached file.txt  # remove from index only, keep file on disk
```

---

### write-tree
Build a tree object from the staging area.
```bash
./mygit write-tree           # build from .git/index (standard)
./mygit write-tree --from-fs # build by traversing the filesystem directly
```

---

### commit
Create a commit from the current staging area.
```bash
./mygit commit -m "message"  # commit with inline message
./mygit commit               # open $EDITOR (or nano) to write message
```

Records the current tree, the parent commit, and author/committer metadata.
Updates the current branch ref and HEAD.

---

### branch
Manage branches.
```bash
./mygit branch            # list all branches, mark current with *
./mygit branch dev        # create branch dev at current commit
./mygit branch -d dev     # delete branch dev
./mygit branch -m old new # rename branch old to new
```

---

### switch
Switch to a different branch.
```bash
./mygit switch dev      # switch to existing branch
./mygit switch -c dev   # create branch dev and switch to it
```

Files present in the current branch but absent in the target branch are removed from disk.
Untracked files (not in the index or any commit) are left untouched.

---

### cat-file
Inspect stored objects.
```bash
./mygit cat-file -t <hash>   # show object type (blob, tree, commit)
./mygit cat-file -s <hash>   # show object size in bytes
./mygit cat-file -e <hash>   # check if object exists (exit code)
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

### Commit format
```
tree <tree_hash>
parent <parent_hash>
author <name> <email> <timestamp> <timezone>
committer <name> <email> <timestamp> <timezone>

<message>
```

The `parent` line is absent for the first commit. Timestamp is stored as `int64_t` (Unix seconds) to avoid the Y2K38 overflow problem.

---

## The .git/ Directory

```
.git/
├── HEAD              → "ref: refs/heads/main"
├── config            → repository settings (INI format)
├── index             → staging area (text format)
├── COMMIT_EDITMSG    → temporary file used by the editor
├── objects/          → all objects (blob, tree, commit)
│   └── xx/           ← first 2 characters of hash
│       └── yyyy...   ← remaining 62 characters (compressed)
└── refs/
    └── heads/
        ├── main      → hash of latest commit on main
        └── dev       → hash of latest commit on dev
```

---

## .mygitignore

Place a `.mygitignore` file in the repository root to exclude files from staging. One pattern per line. Lines starting with `#` are comments.

```
# ignore the compiled binary
mygit
# ignore object files
*.o
# ignore the build folder
build/
```

The My_Git binary is always excluded automatically, even without a `.mygitignore` file, using a platform-specific check (`/proc/self/exe` on Linux, `_NSGetExecutablePath()` on macOS, `GetModuleFileNameA()` on Windows).

---

## Architecture

The project is divided into five layers, each with a single responsibility:

```
cli → core → objects
          ↑
         ops
          ↑
        infra
```

```
src/
├── main.cpp
├── cli/
│   ├── CommandParser.h / .cpp    ← parses argc/argv into ParsedCommand
│   ├── CommandType.h             ← enum: Init, Add, Commit, Branch, Switch, ...
│   ├── Editor.h / .cpp           ← opens $EDITOR for commit messages
├── core/
│   ├── Repository.h / .cpp       ← init, exists, isValid
│   ├── RepositoryPaths.h / .cpp  ← all .git/ path resolution
│   ├── RepositoryConfig.h / .cpp ← read/write .git/config
│   ├── ObjectStore.h / .cpp      ← write/read objects with compression
│   └── Refs.h / .cpp             ← HEAD, branch refs read/write
├── objects/
│   ├── Blob.h / .cpp             ← file content as Git object
│   ├── Tree.h / .cpp             ← directory snapshot as Git object
│   ├── TreeEntry.h / .cpp        ← single entry in a tree
│   ├── Commit.h / .cpp           ← commit object: tree + parent + metadata
│   ├── GitActor.h                ← author/committer: name, email, timestamp, timezone
│   ├── Index.h / .cpp            ← staging area read/write
│   ├── IndexEntry.h / .cpp       ← single entry in the index
│   ├── ObjectHeader.h / .cpp     ← builds "blob 12\0" style headers
│   ├── ObjectType.h / .cpp       ← enum: Blob, Tree, Commit, Unknown
│   └── ParsedObject.h / .cpp     ← parses raw objects from disk
├── ops/
│   ├── Add.h / .cpp              ← addPath, addSingleFile, buildIndexFromTree
│   ├── WriteTree.h / .cpp        ← buildTree, buildTreeFromIndex
│   ├── CommitObject.h / .cpp     ← createCommit, orchestrates tree + refs + actor
│   └── Checkout.h / .cpp         ← restoreTree, getTreeFiles, switch logic
└── infra/
    ├── Hash.h / .cpp             ← SHA-256, hexToBytes, bytesToHex
    ├── Compression.h / .cpp      ← zlib compress/decompress
    ├── FileSystemUtils.h / .cpp  ← file and directory operations
    └── GitIgnore.h / .cpp        ← .mygitignore parsing, isOwnExecutable
```

### Layer responsibilities

**cli** — argument parsing only. Converts raw `argv` into a structured `ParsedCommand`. Has no knowledge of Git internals.

**core** — repository and storage logic. Knows where `.git/` is, how to read/write objects, config, refs, and index.

**objects** — Git object model. Defines what blob, tree, and commit are. Knows how to serialize and deserialize each format.

**ops** — high-level operations. Combines objects, core, and infra to implement commands like `add`, `write-tree`, `commit`, and `switch`.

**infra** — low-level utilities. Hashing, compression, filesystem operations, ignore rules. No Git-specific logic.

## Architecture Diagram

![Architecture Diagram](./architecture.svg)

> The diagram illustrates the layered design of the project and how data flows through the system — from user commands down to object storage.

[Open full diagram](https://raw.githubusercontent.com/khachik14700/My_Git/d0fd783eb018bf02c2a261a196db43f926cd83c0/architecture.svg)

---

## Key Design Decisions

**SHA-256 instead of SHA-1**
Git uses SHA-1 historically. This project uses SHA-256 from the start — more modern and collision-resistant.

**Same object pipeline for all types**
Every object (blob, tree, commit) goes through the same storage pipeline:
```
serialize() → sha256() → compress() → write to disk
```

**int64_t for timestamps**
A 32-bit `int` overflows in January 2038 (Y2K38). All timestamps use `int64_t` to avoid this.

**Index before write-tree**
`write-tree` reads from `.git/index`, not from the filesystem directly.
This matches real Git behavior — `add` stages files, `write-tree` snapshots the index.

**`--from-fs` flag (non-standard)**
A custom flag that lets `write-tree` traverse the filesystem directly without using the index. Not present in real Git — added as a learning and debugging tool.

**Branch as file**
A branch is simply a file in `.git/refs/heads/` containing the hash of the latest commit. HEAD is a file pointing to the current branch. No special data structures needed.

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

## Example Workflow

```bash
# Initialize repository
./mygit init

# Set identity
./mygit config user.name "Khachik Khachatryan"
./mygit config user.email "khachik@email.com"

# Create files
echo "hello" > file.txt
mkdir src && echo "void foo(){}" > src/utils.cpp

# Stage everything
./mygit add .

# Commit
./mygit commit -m "initial commit"

# Create and switch to a new branch
./mygit switch -c dev

# Make changes and commit on dev
echo "new feature" > feature.txt
./mygit add feature.txt
./mygit commit -m "add feature"

# Switch back to main
./mygit switch main

# Inspect an object
./mygit cat-file -p <commit_hash>
```

---

## About

Built as a learning project to deeply understand how Git works internally —
not by reading about it, but by implementing every piece from scratch.