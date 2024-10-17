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

var a: number[] = [1, 2, 3]
var b: string[] = ["hello", "world"]

I like the class idea - a lot:

class Breakfast {
  var eggs: number
  var bacon: number
  var toast: number
  var juice: number

  def total(): number {
    return eggs + bacon + toast + juice
  }
}

class Brunch extends Breakfast {
  var coffee: number

  def total(): number {
    return super.total() + coffee
  }
}

That's great! But then you need:

var c: Breakfast[] = [Breakfast(), Brunch()]

And that's great! But...it's not a value type. Or at 
least a strict one.