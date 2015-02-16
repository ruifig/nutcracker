#include "NutcrackerPCH.h"
#include "LaunchUtil.h"
#include "Document/Project.h"
#include "Document/Interpreter.h"
#include "View/FileEditorGroupWnd.h"
#include "UIDefs.h"


using namespace cz;
using namespace cz::view;
using namespace cz::document;

namespace cz
{

bool launch(bool debug)
{
	auto file = gFileEditorGroupWnd->getCurrentFile();
	if (!file)
		return false;

	Variables vars;
	vars.set("%FILE%", [&]()
	{
		return UTF8String("\"") + file->fullname + "\"";
	});

	uiState->currentInterpreter->launch(vars, file->fullname, debug);
	return true;
}

}

