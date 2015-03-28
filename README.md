[TOC]

# Nutcracker IDE (Squirrel scripting Editor) #

Nutracker is an IDE purely focused on the [Squirrel scripting language](http://squirrel-lang.org).
Over the years I've used Squirrel on and off, but I was always put off by the lack of tools, so I've reused code from another project ([https://bitbucket.org/ruifig/g4devkit](https://bitbucket.org/ruifig/g4devkit)) to quickly create a usable IDE for Squirrel.

# Features #

* Simple workspace management
* Debugger. Supports step in, out, over. Also supports inspecting locals and watches.
    * Step in, out, over.
    * Breakpoints
    * Watches and Locals (only inspectable, not editable)
* Easy to add more interpreters.
    * For the debugger to work, the interpreter needs to implement the SQDBG protocol:
        * [http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SQDBG.html](http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SQDBG.html)
        * [http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SQDBGProtocolReference.html](http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SQDBGProtocolReference.html).
    * At the moment it doesn't allow connecting to an already running interpreter, but that's easily done. Lets me know if you need that feature.
* Basic syntax colouring.
    * Current parser is actually a C parser, so not everything is perfect.


## Available shortcuts

They mostly emulate Visual Studio and/or Visual Assist shortcuts where applicable

### Editor shortcuts

* **F5** - Starts the current script with the debugger
* **CTRL + F5** - Starts the current script without the debugger
* **CTRL + S** - Save your bacon (current file) . **SAVE YOUR BACON!**
* **CTRL + F** - Find in current file
* **CTRL + SHIFT + F** - Find in workspace
* **SHIFT + ALT + O** - Go to file. Start typing and it will filter
* **F9** - Sets/Remove a breakpoint in the current line

### Debugger shortcuts

* **F10** - Step over
* **F11** - Step into
* **SHIFT + F11** - Step out (exit current function)

# How it looks like at the moment (Work in progress) #

![Screenshot3.png](https://bitbucket.org/repo/jnoqe9/images/3573107134-Screenshot3.png)