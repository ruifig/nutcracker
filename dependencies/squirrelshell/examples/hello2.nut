#!/usr/bin/env squirrelsh

// Get user's name and welcome him.

print("Your name: ");
local name = scan();
print("Welcome to Squirrel Shell " + SHELL_VERSION + " for " + PLATFORM + ", " + name + "!\n");
