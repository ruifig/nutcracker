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

# Available shortcuts

Keyboard shortcuts mostly emulate Visual Studio and/or Visual Assist where applicable

## Editor shortcuts

* **F5** - Starts the current script with the debugger
* **CTRL + F5** - Starts the current script without the debugger
* **CTRL + S** - Save your bacon (current file) . **SAVE YOUR BACON!**
* **CTRL + F** - Find in current file
* **CTRL + SHIFT + F** - Find in workspace
* **SHIFT + ALT + O** - Go to file. Start typing and it will filter
* **F9** - Sets/Remove a breakpoint in the current line

## Debugger shortcuts

* **F10** - Step over
* **F11** - Step into
* **SHIFT + F11** - Step out (exit current function)
* **CTRL + ALT + BREAK** - Break

# Workspace management

The workspace simply keeps track of folders, breakpoints and watches. It does not keep track of any per-file specific options, or interpreter options.
A quick overview on how to manage files:

* Drag a folder into the workspace window. It will start tracking that folder
* At the moment you can't create/rename/delete files from the workspace. You need to externally create files, then click the "refresh" button in the workspace, to synchronize the folders.
* You can remove a folder from the workspace by right clicking it.

# TODO

* Only ansi and x86 provided at the moment. I need to refactor some things for proper Unicode support, and 64-bits. Nothing particularly hard.
* Would be nice to have some kind of autocompletion, although that's not an easy task for a dynamically typed language like Squirrel.

# A quick tour #

I recorded a small video explaining the basic features.
[http://www.youtube.com/watch?v=gf07Da3K1lw](http://www.youtube.com/watch?v=gf07Da3K1lw)

# How it looks like at the moment #

![Screenshot4.png](https://bitbucket.org/repo/jnoqe9/images/2965400008-Screenshot4.png)
