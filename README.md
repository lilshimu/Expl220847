# Expl220847
Proof of Concept exploit for the vulnerability in the Linux kernel.
---

## Description

This repository contains a simplified implementation of the **CVE-2022-0847** (Dirty Pipe) exploit.

The vulnerability allows an unprivileged local user to overwrite data in files that are opened read-only (`O_RDONLY`), even on read-only filesystems. This can be used to modify critical system files such as `/etc/passwd`, `/etc/shadow`, setuid binaries, or scripts in `/usr/bin`.

### Affected Kernel Versions

- **Vulnerable**: Linux 5.8 and later (until patched)
- **Fixed in**:
  - 5.16.11
  - 5.15.25
  - 5.10.102
  - 5.4.179
  - 4.19.233
  - 4.14.299
  - and all newer versions

---

## Build

```bash
gcc -Wall -Wextra expl220847.c -o dirtypipe
```

## Usage
```bash
./dirtypipe <filename> "<data>"
```

## How It Works
1. A pipe is created and filled to its maximum capacity.
2. All but one byte is read from the pipe.
3. Using splice(), one byte from the target file is loaded into the pipe.
4. Due to a bug in the handling of the PIPE_BUF_FLAG_CAN_MERGE flag, the next write() operation merges data directly into the page cache of the target file, bypassing permission checks.

## Disclaimer
This exploit is provided for educational and research purposes only.
