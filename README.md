# Mini-Shell

This is a mini-shell implementation for CS3650, written in C. It supports tokenizing user input, executing commands, handling pipes, redirection, and built-in commands like `cd`, `source`, `prev`, and `help`.

Starter files and tokenizer examples were provided, and expanded into a functioning shell interface.

# Authors

- **Noah Haggerty** — haggerty.no@northeastern.edu  
- **Lubaina Malvi** — malvi.l@northeastern.edu

# Features

- **Custom Tokenizer**: Parses command line input into tokens.
- **Built-in Commands**:
  - `cd <dir>`: Change directory
  - `source <file>`: Execute commands from file
  - `prev`: Repeat previous command
  - `help`: Show list of built-ins
  - `exit`: Exit the shell
- **Operators Support**:
  - `;`: Run multiple commands in sequence
  - `|`: Pipe output from one command to the next
  - `<` / `>`: Input/output redirection

# File Structure

| File | Description |
|------|-------------|
| `shell.c` | Main shell loop and built-in command handling |
| `tokenize.c/.h` | Custom tokenizer and helper methods |
| `tokenize_expr.c` | Simple example tokenizer for arithmetic expressions |
| `tokenize_tests.py` | Unit tests for tokenizer |
| `shell_tests.py` | Unit tests for shell behavior |
| `shell_test_helpers.py` | Helpers used in test scripts |
| `Makefile` | Build automation for shell, tokenizer, and tests |
| `metadata.yml` | Submission metadata for project tracking |

# Building the Project

This project uses a `Makefile` for compilation:

```bash
make all             # Build everything
make tokenize        # Build just the tokenizer
make shell           # Build the shell
make test            # Run all tests
make shell-tests     # Run shell-specific tests
make clean           # Clean up built binaries


Start the shell by running:

bash
./shell
You will see a prompt:

shell $
You can now type commands just like in a standard Unix shell, including chaining them, piping, or using redirection:

ls -l; echo Hello | grep H > out.txt

Example Commands
bash
Copy
Edit
cd /tmp
source script.txt
prev
help
echo Hello World > hello.txt
cat < hello.txt | grep World

Testing
To run all test suites:

make test
Tests cover:
Tokenization of input strings

Built-in command behaviors

File redirection

Piping and command chaining

Notes
The tokenizer correctly handles strings, special characters, and quoted substrings.
Shell features are implemented in modular functions with proper error handling.
Pipes and redirection follow Unix-style semantics.

Learned Concepts
Process creation and fork()/exec()/wait()

File descriptor duplication for I/O redirection

Writing a tokenizer from scratch

Building a simple shell interpreter in C
