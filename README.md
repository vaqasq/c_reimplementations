# C Systems Programming Projects

A collection of small systems projects written in C:

- A custom shell with a REPL and asynchronous process management
- Reimplementations of a subset of GNU coreutils
- Two simple memory allocators (arena and free-list)

These were written as learning exercises for process management, file I/O, and dynamic memory.

## Shell

A minimal shell implemented in pure C.

- REPL loop
- Command parsing and execution via `fork`/`exec`
- Synchronous and asynchronous child process handling (background jobs)
- Environment variable support
- Integrates with the coreutils in this repository (looked up from a local path, not `/bin`)

**Not implemented:**
- Pipelines (`|`)
- Redirection (`>`, `>>`, `<`)
- Job control (`fg`, `bg`, `jobs`, signal handling beyond basic wait)

## Coreutils

Minimal reimplementations of utilities I actually use. Flag support is limited to the options listed below. Behavior is intentionally close to the GNU versions for the supported cases, but not a full drop-in replacement.

| Utility | Supported flags / behavior |
|---------|---------------------------|
| `echo`  | `-n` (no trailing newline). Argument parsing is left to the shell. |
| `cat`   | Concatenate files and/or stdin. On multiple invalid paths, exits on the first failure. |
| `pwd`   | Logical path by default (`-L` or no flag). `-P` resolves symlinks. |
| `sleep` | Accepts a single number with optional unit suffix (`s`, `m`, `h`, `d`). |
| `whoami`| Prints the current username. |
| `ls`    | `-l`, `-a`, and combinations (`-la`, `-l -a`, order-independent). |
| `mkdir` | Creates a single directory. Assumes parent directories already exist. No flags. |

## Memory Allocators

Two simple allocators for learning purposes:

- **Arena allocator** — bump-pointer style allocation from a single contiguous region.
- **Free-list allocator** — heap allocator that grows the break with `sbrk` and tracks free blocks with a free list.
