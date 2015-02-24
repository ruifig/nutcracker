# Nutcracker IDE (Squirrel scripting Editor) #

Nutracker is an IDE purely focused on the [Squirrel scripting language](http://squirrel-lang.org).
I've used Squirrel on and off over the years, but despite being a nice little scripting language, I was always put off by the lack of tools.

The code is mostly a refactoring of the C/Assembly IDE I've been working on for my game toolkit:

* [https://bitbucket.org/ruifig/g4devkit](https://bitbucket.org/ruifig/g4devkit)
* [https://www.youtube.com/watch?v=cIyHgQvfETE](https://www.youtube.com/watch?v=cIyHgQvfETE)

As such, it's pretty much unorganized while I refactor and bring things across as fast as possible, so don't expect pretty code.

Also, this repository references code from another one of my repositories which is not public at the moment, so you won't be able to build.
I'll get that sorted out as soon as I have a usable version.

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