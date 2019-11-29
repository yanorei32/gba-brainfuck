# GBA Brainfuck
Brainfuck interpreter for GBA.

Demo: https://www.youtube.com/watch?v=Dc2VkSRkmuM

This application is based in part on the work of the devkitpro project (http://sourceforge.net/projects/devkitpro).

## How to build GBA file

### Depends
* git
* Docker

### Build
```bash
git clone https://github.com/yanorei32/gba-brainfuck/
cd gba-brainfuck
./make # It will output brainfuck.gba
```

#### If you want to pass some arguments to make:

```bash
./make clean
```

### Operability confirmed environments

* GBA AGB-001 AGB-JPN (with EZ Flash IV)
* GBA AGB-001 AGB-JPN-1 (with EZ Flash IV)
* GBA AGB-001 C/AGB-JPN (with EZ Flash IV)
* GBA AGB-001 C/AGB-JPN-1 (with EZ Flash IV)
* GBA SP AGS-001 C/AGS-JPN (with EZ Flash IV)
* [VisualBoyAdvance 1.7.2](https://ja.osdn.net/projects/sfnet_vba/) latest version as of 2019-11-28
* [VisualBoyAdvance-M 2.1.4](https://github.com/visualboyadvance-m/visualboyadvance-m) latest version as of 2019-11-28

## Key binding and mode

This application has two modes.

After power-on, it will go to Editor Mode.

### Editor Mode

| Key                | Binding                                           |
|:-------------------|:--------------------------------------------------|
| `↑` or `A`         | Change program memory operator.                   |
| `↓`'               | Change program memory operator.                   |
| `→` or `B`         | Move the cursor to next program memory.           |
| `←`                | Move the cursor to forward program memory.        |
| `START`            | Switch to Runner Mode and execute program memory. |
| `SELECT`, `SELECT` | Reset program memory and cursor position.         |

### Runner Mode

| Key      | Binding              |
|:---------|:---------------------|
| `SELECT` | Back to Editor Mode. |

#### In running only

| Key      | Binding             |
|:---------|:--------------------|
| `SELECT` | Keyboard Interrupt. |

#### In input prompt only

| Key        | Binding           |
|:-----------|:------------------|
| `↑` or `A` | Increment.        |
| `↓` or `B` | Decrement.        |
| `←`        | Decrement `0x10`. |
| `→`        | Increment `0x10`. |

## Operators

| Operator | Description                                                               |
|:---------|:--------------------------------------------------------------------------|
| ` `      | Blank. Do nothing.                                                        |
| `+`      | Increment current data memory.                                            |
| `-`      | Decrement current data memory.                                            |
| `>`      | Increment data memory pointer.                                            |
| `<`      | Decrement data memory pointer.                                            |
| `.`      | Output current data memory as ASCII character.                            |
| `,`      | Input current data memory as hex. It will showing input prompt.           |
| `[`      | If current data memory is zero, jump to matching parenthesis ( `]` ).     |
| `]`      | If current data memory is non-zero, jump to matching parenthesis ( `[` ). |

## Exceptions

### Data memory access violation
If this happens, you did read or write out of the memory boundary.

Minimum:
```
<+
+[>+]
```

Output:
```
ERROR: PTR < 0
ERROR: PTR >= RAM SIZE
```

### Null character output
If this happens, you tried to print NULL character.

Minimum:
```
.
```

Output:
```
ERROR: '\0' OUTPUT.
```

### Syntax error
It's your typo.

Minimum:
```
+]
[
```

Output:
```
ERROR: FAILED TO FIND '['
ERROR: FAILED TO FIND ']'
```

