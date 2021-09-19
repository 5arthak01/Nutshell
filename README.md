# Nutshell

### Kyonki isse zyada ka aukaat nhi hai baba

## Minimal shell implimentation in C

## Supports:

- Built-in commands:
  - `cd [path]`
  - `echo [string]`
  - `ls [-al] [dirs]`
  - `pinfo [pid]`
  - `pwd`
  - `repeat [n] [command]`
- Foreground and Background processes
- External programs

## Installation

```shell
$ make
$ ./nutshell
```

## File structure

- The REPL is in the `nutshell.c` file
- Builtin commands have their separate files with the same name. (`pwd` is exception, with file `getcwd.c`)
- `builtins.c` is for modular one-stop handling of the builtin commands
- `constants.h` stores all the constant headers for the shell
- `execution.c` executes a command given in the string or tokenised form
- `includes.h` is the common template of headers
- `prompt.c` contains functionality to generate the shell prompt
- `types.h` is for data structures used internally in the shell
- `utils.c` provides convenience functions, such as string manipulations
