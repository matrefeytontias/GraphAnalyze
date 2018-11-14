# GraphAnalyze

Interactive function grapher and analyzer

### Building on Windows

Download MSYS2 from the official website ; when that's done, install `gcc` and `make`.
Go in the top-level folder and run `make` to compile or `make run` to compile and run.
Easy like that.

### Building on Linux

Install the package `libglfw3-dev`, and run `make` to compile or `make run` to compile and run. Even easier !

### Building the documentation

Install the `doxygen` package, and run `make docs`.

### Browsing the repository

All of the source code is contained in the `include` and `src` directories, as well as in their subdirectories. All docs are generated in the `docs` directory, both as $\LaTeX$ and as HTML. To browse the HTML doc, just open the `index.html` file in the `html` directory.

The program is built in the `bin` directory. To run it, simply launch the `GraphAnalyze` executable. On Windows, since this project was built using Msys2 on a computer that already had it installed, you might be lacking a couple DLLs. In that case, you should be able to download them individually on the internet.
