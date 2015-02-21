#include "NutcrackerPCH.h"
#include "LaunchUtil.h"
#include "Project.h"
#include "Interpreter.h"
#include "FileEditorGroupWnd.h"
#include "UIDefs.h"


namespace nutcracker
{

bool launch(bool debug)
{
	auto file = gFileEditorGroupWnd->getCurrentFile();
	if (!file)
		return false;

	Variables vars;
	vars.set("%FILE%", [&]()
	{
		return UTF8String("\"") + file->fullpath + "\"";
	});

	gUIState->debugSession = gUIState->currentInterpreter->launch(vars, file->fullpath, debug);
	return true;
}

} // namespace nutcracker

