# Neuro

The Neuro Language is a mixed paradigm programming language I am developing trying to integrate features from
various modern languages while still remaining performant. It is my attempt at making something that
could replace C++ for programming projects.

While C++ is a powerful language, new languages have been created with powerful new features that
help make programming more enjoyable and that makes your code easier to think about. Additionally,
there are certain aspects of developing in C and C++ that could be improved upon such as compile
times, removal of the preprocessor, and a proper import system to name a few. Additionally, type
inferencing can be used so that we still have the benefit of static type checking while letting the
programmer write code quickly as though with dynamic typing.

## Inspirations
The following are languages I have been inspired by in the design of my own language:
- Rust
- C
- D
- Scala
- Haskell
- Jai (Jonathan Blow's language)

## Planned and Potential Features
The following are ideas that are planned or I am considering for the language:
* Planned
  - Templates
  - Lambdas
  - For-each Loops
  - UFCS
  - Type Introspection
  - ADTs
* Potential
  - Until loops
  - Owned memory

## Progress

- [x] Lexer
- [ ] Parser
  - [x] Loops
  - [x] Primitive Types
  - [x] Structs
  - [x] Struct member Access
  - [x] Pointer types
  - [ ] Bitwise operators
  - [ ] Arrays
  - [x] Casting
  - [x] Unary operators
  - [x] Logical operators
  - [ ] Strings
- [x] Semantic Checking
  - [x] Type Inferencing
  - [x] Type Checking
  - [x] Break and Continue used only within loops
- [x] Semantic Features/IR Code Generation
  - [x] Function definitions
  - [x] Prototypes
  - [x] Struct Types
  - [x] Expressions
    - [x] Pointer arithmetic
    - [x] Implicit casting
- [ ] Build System
- [ ] Standard Library

The current plan is to support a limited set of language features and get that working. Once that
is working I will add more complex features to the language like templates and lambdas.

Syntax highlighting is available for vim [here](https://github.com/lotusronin/neuro.vim) 

## Hello World
The following is Hello World in my language: 
```
extern fn puts(s : *u8) : s32;

fn main() : s32 {
    puts("Hello World");
    return 0;
}
```
** Note that the syntax, functions, and module names may change in the future. **

## Other Languages
If you are interested in my language or enjoy looking at new languages you may be interested in these other projects:
- [Kraken](https://github.com/Limvot/kraken)
- [Music](https://github.com/ChrisFadden/MusicCompiler)
