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

Planned features:

* Purely focused on Squirrel
* Debugger
* Very simple workspace management, as to not enforce any particular kind of project organization
* A good deal of keyboard shortcuts for code navigation, similar to Visual Studio and Visual Assist.
* Support for user defined Squirrel interpreters
    * The IDE can communicate with your game, or your customized standalone interpreter.
    * For the debugger to work, your game/interpreter must use the simple [SQDBG](http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SQDBG.html) library. See also [http://wiki.squirrel-lang.org/default.aspx/SquirrelWiki/SQDBGProtocolReference.html]()
* Some degree of code completion, as far as it can be done with a dynamically typed language.
    * I intend to extend SQDBG protocol to have the IDE query the Squirrel VM's process and build a code database.

# How it looks like at the moment (Work in progress) #

![Screenshot3.png](https://bitbucket.org/repo/jnoqe9/images/3573107134-Screenshot3.png)