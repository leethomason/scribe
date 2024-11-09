# scribe

Experimental / toy script language for games

## Basics

### Types

Question:

* Is is possible to remove the ';' at the end of a statement?
* Is the map v. list dicotomy worth it?

class Breakfast {
    var eggs: number
    var bacon: number
    var toast: number
    var juice: number

    func total(): number {
        return eggs + bacon + toast + juice
    }
}

class Brunch extends Breakfast {
  var coffee: number

  func total(): number {
      return super.total() + coffee
  }
}

var x = 1                   // number
var x: num = 1              // number w/ type syntax
var x2: num = 1 + 2;        // type required - can't trivially infer
var y = true                // boolean
var z = "hello"             // string
var a: num[] = [1, 3];
var b: string[] = ["hello", "world"];
vac c: Breakfast[] = [Breakfast(), Brunch()]; // no slicing!
var d: num{} = { "x": 1, "y": 2}
var e: Breakfast{} = { "early": Breakfast(), "late": Brunch()}
var f = Breakfast()

e = {}   // empty, not null
// ERROR: only classes can be null.
//        e = null
f = null // okay, because this points to a class
         // removing this would be super great. But...then return values
         // need optional support, which isn't so great.

Primitive:

* num
* bool
* string

Unsure:

* list
* map

Reference:

* classes

list

* push
* pop
* insert
* remove
* clear
* size
* slice
* find
* empty
* []

map

* set
* get
* has
* remove
* clear
* size
* empty
* [] -- add or access only?

builtin

* print
* type() - returns the class name (even if null!) or the more primitive type
* baseType() - "class", "list", "map", "num", etc.

### Expressions

### Statements

### Functions

### Control Flow

### Modules

