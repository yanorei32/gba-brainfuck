# GBA Brainfuck

## How to build

Dep:
* Docker

```bash
git clone https://github.com/yanorei32/gba-brainfuck/
cd gba-brainfuck
./make # It will output brainfuck.gba

# If you want to pass some arguments to make:
./make clean
```


## Key binding and mode

This application has two modes.

After power-on, it will go to Editor Mode.

### Editor Mode

| Key                | Binding                                           |
|:-------------------|:--------------------------------------------------|
| `→` or `A`         | Move the cursor to next Program Memory.           |
| `←`                | Move the cursor to forward Program Memory.        |
| `↑` or `B`         | Change Program Memory Operator.                   |
| `↓`'               | Change Program Memory Operator.                   |
| `START`            | Switch to Runner Mode and execute Program Memory. |
| `SELECT`, `SELECT` | Reset Program Memory and cursor position.         |

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

| Operator | Description                                             |
|:---------|:--------------------------------------------------------|
| ` `      | Blank. Do nothing.                                      |
| `+`      | Increment data memory.                                  |
| `-`      | Decrement data memory.                                  |
| `>`      | Increment data memory pointer.                          |
| `<`      | Decrement data memory pointer.                          |
| `.`      | Output data memory as ASCII character.                  |
| `,`      | Input data memory as hex. It will showing input prompt. |

## Exceptions

### Data memory access violation
If this happens, you did read or write out of the memory boundary.

Output:
```
ERROR: PTR < 0
ERROR: PTR >= RAM SIZE
```

### Null character output
If this happens, you tried to print NULL character.

Output:
```
ERROR: '\0' OUTPUT.
```


