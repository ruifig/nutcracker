#include "NutcrackerPCH.h"
#include "LaunchUtil.h"
#include "Interpreter.h"
#include "FileEditorGroupWnd.h"
#include "UIDefs.h"
#include "Workspace.h"

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
	fireAppEvent(AppEventID::DebugStarted);
	return true;
}

} // namespace nutcracker

