# scribe

Experimental / toy script language for games

## Basics

### Types

Question:

* Is is possible to remove the ';' at the end of a statement?
* Note the friction in the type system; arrays (and maps) are variants, but the elements are not.
  And to read from them, you must know the type
* Does the copy semantics really work, in a practical language?

var x = 1                   // number
var x: num = 1
var y = true                // boolean
var z = "hello"             // string
var a = [1, "world", 3]     // array
var b = {x: 1, y: 2}        // map
var c = {}

var d = a;                  // array d is a *copy* of a  

### Expressions

### Statements

### Functions

### Control Flow

### Modules

