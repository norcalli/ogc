#ogc

*Offline Google Calculator. My continued attempt at making a calculator as amazing as Google's, and possibly even better.*

##Features
- Complex numbers
- Units!
- Unit conversion (x to y)
- Implicit multiplication, e.g. `2(4) = 2 * 4 = 8` (yes I took care of the case where you use `1e` and it interprets it as `1 * e` not `1e0`)
- Custom function addition (but only by changing the source file for now and compiling).
- Custom symbol (constant) definition in a symbol.txt file.
- Compilation of that text file into a binary format automatically.

###Possible features
- Money conversion
- Dynamic function definition in symbols.txt
- Plotting! Output as SVG specifically.
- Having it cook me pancakes.

##Compilation
### Requires
- boost for strings and filesystem, will probably update later to not require this.
- c++11 because it is convenient and nice.

###Instructions

I use OS X mainly, but the command should be similar for any system, just replace filesystem and boost_system with the appropriate flags, as Macports automatically separates them into separate multithreaded dylibs.

```
g++ -o godzilla -std=c++11 main.cc -I /opt/local/include/ -L/opt/local/lib/ -lboost_filesystem-mt -lboost_system-mt
```

###Compilation options:
You can compile it for expression tree creation by defining the EXPRESSION flag at the top of the file.

There is some EXPERIMENTAL flag that I have completely forgotten about. TODO: Figure that out.

#Usage

Execution without arguments prints usage ("Usage: ./godzilla <expression>")

##Symbols.txt
Look at my symbols.txt that I have defined already for an example of how to define custom expressions, but it's really simple:
```
e=exp(1)
N=kg m /s^2
kg=1e3 g
```

##Examples
`./godzilla "1+2((3 m)/ (2 cm)) + e^1.24"`

Output:
`1 + 2 * ((3 * m) / (2 * cm)) + e ^ 1.24 = 304.45561346476`

---
`./godzilla "4 cm to m"`

Output:
```
4 * cm = 0.04 [[ meter ]]m = 1 [[ meter ]]
Result: 0.04 [[ meter ]]
```

Whoops, look like I left some debug output on...

#Notes
So far, this calculator works, but I'm always looking for test cases where the output doesn't work as intended (googling the expression to confirm).

Don't hesitate to recommend features.
