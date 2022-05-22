#include "MyForm.h"

using namespace import; // poprawiæ zgodnie z nazw¹ projektu

[STAThread]
int main(array<System::String ^> ^args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	Application::Run(gcnew MyForm());
	return 0;
}