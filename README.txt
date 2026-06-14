jmake
---

About

A very simple build script using JSON as its config language.

---

Compiling

Once you have the source code, run:
	./configure
	make
To install jmake, run:
	sudo make install

You can also make jmake with jmake:
	jmake
Then:
	sudo jmake install

---

Usage

To compile normally:
	jmake

Or with verbosity:
	jmake -V

To install:
	jmake install

To clean:
	jmake clean

To generate a make.json file:
	jmake project <project name>

---

Configuration

The config (make.json) takes four keys:
	"cc" - the compiler
	"cflags" - the flags for the compiler
	"src" - the source C files
	"exe" - the executable name

---

Note

This project depends on cJSON, so...install that.
