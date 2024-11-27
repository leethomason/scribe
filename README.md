# scribe

Experimental / toy script language for games

## Types

* `num` - 64 bit floating point number
* `str` - string [wip: utf-8]
* `bool` - boolean
* `list` - array of values

```
var x // okay: creates w/ default value (0)
var y: num // okay: creates w/ default value (0)
var z: num = 1 // okay: creates w/ value (1)
w = 1 // ERROR: must declare type
```

```
var a: num[] = [1, 2, 3]
var b: num[] = []
var c = [1, 2, 3]
var d = [] // ERROR: must declare type
print a[0] // okay: prints 1
print a[3] // ERROR: out of bounds, throws exception
```

## Logical Operators

Logical operators use the C++/Swift syntax, and return
a bool. (Scribe isn't dynamically typed, so having typed
logical operators isn't very useful.)

* `&&` - logical "and"
* `||` - logical "or"

## Basics (WIP)

Question:

* Is is possible to remove the ';' at the end of a statement?
* Is the map v. list dichotomy worth it?

```
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
```

### logical operators

```
if a && b {
	// okay
}

or prefer:

if a and b {
	// okay
}
```

### 'if' clauses

```
if true {
    // okay, this is all dangerously WGSL. But it is pretty clean.
    // the left bracket is used at the condition terminator
}

var t = a > 0 ? b : c
// in this case, the ? is used as the condition terminator. Then we can read two expressions.
```

### Option 1: Classes can be null

```
// ERROR: only classes can be null.
//        e = null
f = null // okay, because this points to a class
         // removing this would be super great. But...then return values
         // need optional support, which isn't so great.
```

### Option 2: Removing Null

```
if var b = getMeal() {
    print(b.total())
}

if var b = getMeal(2) && var c = getMeal(b.index()) {
    print(b.total() + c.total())
}
```

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

