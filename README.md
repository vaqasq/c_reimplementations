The real GNU Coreutils have many, many flags. I simply implemented the ones I use.

#### echo
Reimplements basic functionality as described on its respective man-page, including the -n flag. Your shell will interpret and parse the arguments. This is significant because, as you may have noticed, GNU echo's behavior regarding quotation marks might not be what your expect.

```bash
$ echo "Hello, World!""Hello"
Hello, World!Hello
```

Therefore, my understanding is that your shell's implementation argument parsing can change how echo will work.

#### cat
Reimplements basic functionality of cat, specifically concatenating files and or taking input from standard input. One difference is that my version of cat, given multiple invalid file paths, will return the failure of the first, while the core util cat will list out all the erroneous file paths.

```bash
$ echo "Hello World" | ./cat - file.txt
Hello World
... [file.txt contents]
```

#### pwd
Prints working directory, with "pwd" and "pwd -L" being equivalent, printing the logical path, while "pwd -P" resolves symlinks.

#### sleep
You can append the unit of time to the end of the singular flag (s, m, h, or d). You can also use && with this function, as this is handled by Bash.

#### whoami
Simply prints user.

#### ls
Provides the -l and -a options. Accepts -la, or -l -a, order-agnostic.

#### mkdir
Assumes parent dirs already exist. No available flags.

Lib C

#### arena allocator
A simple bump allocator

#### heap allocator
Uses sbrk and a free list. Kept in simple.
