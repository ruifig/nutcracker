

Folder structure:

doc - Folder with misc documentation and notes
src -
	Source files. Files in here are not intended to accessed from user code.
	This folder can also contain header files if they are intended for internal use only
inc - Folder with user visible header files
tmp - Folder for all temporary files created during a build


Coding standards:

- Classes, types and structures are camel case (MyNewClass).
- Templates classes are prefixed with a T. (e.g: TDynamicArray)
- Methods and local variables are camel case, but start with a lower case letter (someFunction, someVariable).
- Member variables start with 'm' (mInstanceVar), static member variables start with 'ms' (msStaticMemberVar). Global variables start with 'g' (gSomeVar)
- Preprocessor macros are upper case, and are prefixed with CZ_
- Enum are named in title case (SomeEnumType), while enum values are upper case (SOMEVALUE)
- Methods intended for advanced use or for communicating internally (instead of using friend declarations), should be prefixed with "_". ( _someAdvancedMethod )