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

To install:
	jmake install

To clean:
	jmake clean

To generate a make.json file:
	jmake project <project name>

---

Configuration

The config (make.json) takes four keys:
	"cc" - The compiler
	"cflags" - The flags for the compiler
	"src" - The source C files
	"exe" - The executable name
	"install_dir" - The directory where your binary will be copied to.

---

Note

This project depends on cJSON, so...install that.
