#include "common.h"

//------------------------------- SHARED VARIABLES  --------------------------------------
char err_msg [1024];

//extern HWND hListBox;

//--------------------------------------------------------------------------------------------
// dodaje komunikat message do dziennika (LOG
void PrintLog(char *message)
{
	//if (IS_WIN32)
	//	if (hListBox) SendMessage( hListBox, LB_ADDSTRING, 0,( LPARAM ) message );
	//else
	//	printf("Log: %s\n");	
}
//--------------------------------------------------------------------------------------------
// rzuca wyjatek z komunikatem diagnostycznym 
void ThrowException(System::String^ msg)
{	
	System::Windows::Forms::MessageBox::Show(msg);
	System::Windows::Forms::Application::Exit();
}
void Msg(System::String^ msg)
{
	System::Windows::Forms::MessageBox::Show(msg);
}
//--------------------------------------------------------------------------------------------