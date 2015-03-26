#!/usr/bin/env squirrelsh

// Match regular expression against text

print("Text: ");
local text = scan();
print("Pattern: ");
local pattern = scan();

local matches = regmatch(pattern, text);
if (!matches)
{
	printl("Failed to match regular expression - " + regerror());
	exit(1);
}

printl("Matches found:");
foreach (match in matches)
	printl("\t\"" + substr(text, match[0], match[1]) + "\"");
