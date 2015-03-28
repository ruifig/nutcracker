
#####################################################
Interpreter configuration
#####################################################

You can add new interpreters by creating a new folder in the "interpreters" folder,
and creating the required configuration file.
These are the predefined variables that can be used in an config file:

NUTCRACKER_DIR
	Directory where Nutcracker.exe is
FILE
	File that to run
INTERPRETER_NAME
	Name of the interpreter, as specified in the "name" entry of the [General] section.
	Note that this is NOT the config file name.
INTERPRETER_CFG_DIR
	Directory where the config file is

#####################################################
Custom changes to Squirrel Shell
#####################################################

- Debugger mode can be specified with the "-dPORT" parameter. Ex:
	squirrelsh.exe -d20900 Script.nut
- Some functions from the Squirrel Standard Library are available:
	- Everything in the "The Input/Output library" part
	- Some functions from the "The String library":
		- format
		- strip, lstrip, rstrip
		- split
- A new function "printf" exists, which behaves similarly to the C printf function.		
- Function "copy" returns true/false depending on the success of the operation

#####################################################
Custom changes to Squirrel (sq.exe and sqdbg.exe)
#####################################################

- Debugger port can be specified with the "-dPORT" parameter:
	sqdbg.exe -d20900 Script.nut
