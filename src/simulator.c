#include <stdio.h>
#include <Windows.h>

#include "fmi2Functions.h"

// model specific constants
#define GUID "{dc814245-ea29-7827-4563-22c494e2e8d9}"
#define RESOURCE_LOCATION "file:///C:/Users/schyan01/github/StandaloneFMU_RisingEdge_Endlagen" // absolut path to the unziped fmu

HANDLE hFile;
DWORD bytes_writed = 0;
DWORD bytes_readed = 0;

int write_file(LPCSTR absolut_pfad, LPCVOID write_buffer)					// Schreiben des Files
{
	hFile = CreateFileA(absolut_pfad,	// Filename
		GENERIC_WRITE | GENERIC_READ,	// Desired access
		0,								// Share flags
		NULL,							// Security Attributes
		OPEN_ALWAYS,					// Creation Disposition
		FILE_ATTRIBUTE_NORMAL,			// Flags and Attributes
		NULL);							// OVERLAPPED pointer	

	WriteFile(hFile,							// Handle
		write_buffer,			// Data to be write
		sizeof(write_buffer),	// Size of data, in bytes
		&bytes_writed,			// Number of bytes writed
		NULL);					// OVERLAPPED pointer

	CloseHandle(hFile);

	return 0;
}

int read_file(LPCSTR absolut_pfad, LPVOID read_buffer)						// Lesen des Files
{
	hFile = CreateFileA(absolut_pfad,	// Filename
		GENERIC_WRITE | GENERIC_READ,	// Desired access
		0,								// Share flags
		NULL,							// Security Attributes
		OPEN_ALWAYS,					// Creation Disposition
		FILE_ATTRIBUTE_NORMAL,			// Flags and Attributes
		NULL);							// OVERLAPPED pointer	

	ReadFile(hFile,					// Handle
		read_buffer,			// Data to be read
		sizeof(read_buffer),	// Size of data, in bytes
		&bytes_readed,			// Number of bytes readed
		NULL);					// OVERLAPPED pointer

	CloseHandle(hFile);

	return 0;
}
// callback functions
static void cb_logMessage(fmi2ComponentEnvironment componentEnvironment, fmi2String instanceName, fmi2Status status, fmi2String category, fmi2String message, ...) {
	printf("%s\n", message);
}

static void* cb_allocateMemory(size_t nobj, size_t size) {
	return calloc(nobj, size);
}

static void cb_freeMemory(void* obj) {
	free(obj);
}

#define CHECK_STATUS(S) { status = S; if (status != fmi2OK) goto TERMINATE; }

int main(int argc, char *argv[]) {
	HMODULE libraryHandle = LoadLibraryA("C:\\Users\\schyan01\\github\\StandaloneFMU_RisingEdge_Endlagen\\RisingEdge_Endlagen\\binaries\\win64\\RisingEdge_Endlagen.dll");

	if (!libraryHandle)
	{
		return EXIT_FAILURE;
	}

	fmi2InstantiateTYPE* InstantiatePtr = NULL;
	fmi2FreeInstanceTYPE* FreeInstancePtr = NULL;
	fmi2SetupExperimentTYPE* SetupExperimentPtr = NULL;
	fmi2EnterInitializationModeTYPE* EnterInitializationModePtr = NULL;
	fmi2ExitInitializationModeTYPE* ExitInitializationModePtr = NULL;
	fmi2TerminateTYPE* TerminatePtr = NULL;
	fmi2SetRealTYPE* SetRealPtr = NULL;
	fmi2GetRealTYPE* GetRealPtr = NULL;
	fmi2SetBooleanTYPE* SetBooleanPtr = NULL;
	fmi2GetBooleanTYPE* GetBooleanPtr = NULL;
	fmi2DoStepTYPE* DoStepPtr = NULL;
	fmi2GetTypesPlatformTYPE* GetTypesPlatform = NULL;
	fmi2GetVersionTYPE* GetVersion = NULL;

	InstantiatePtr = GetProcAddress(libraryHandle, "fmi2Instantiate");
	FreeInstancePtr = GetProcAddress(libraryHandle, "fmi2FreeInstance");
	SetupExperimentPtr = GetProcAddress(libraryHandle, "fmi2SetupExperiment");
	EnterInitializationModePtr = GetProcAddress(libraryHandle, "fmi2EnterInitializationMode");
	ExitInitializationModePtr = GetProcAddress(libraryHandle, "fmi2ExitInitializationMode");
	TerminatePtr = GetProcAddress(libraryHandle, "fmi2Terminate");
	SetRealPtr = GetProcAddress(libraryHandle, "fmi2SetReal");
	GetRealPtr = GetProcAddress(libraryHandle, "fmi2GetReal");
	SetBooleanPtr = GetProcAddress(libraryHandle, "fmi2SetBoolean");
	GetBooleanPtr = GetProcAddress(libraryHandle, "fmi2GetBoolean");
	DoStepPtr = GetProcAddress(libraryHandle, "fmi2DoStep");
	GetTypesPlatform = GetProcAddress(libraryHandle, "fmi2GetTypesPlatform");
	GetVersion = GetProcAddress(libraryHandle, "fmi2GetVersion");

	if (NULL == InstantiatePtr || NULL == FreeInstancePtr || NULL == SetupExperimentPtr || NULL == EnterInitializationModePtr || NULL == ExitInitializationModePtr
		|| NULL == SetRealPtr || NULL == GetRealPtr || NULL == SetBooleanPtr || NULL == GetBooleanPtr || NULL == DoStepPtr || NULL == TerminatePtr || NULL == GetTypesPlatform || NULL == GetVersion)
	{
		return EXIT_FAILURE;
	}

	fmi2Status status = fmi2OK;

	fmi2CallbackFunctions callbacks = {cb_logMessage, cb_allocateMemory, cb_freeMemory, NULL, NULL};
	
	fmi2Component c = InstantiatePtr("instance1", fmi2CoSimulation, GUID, RESOURCE_LOCATION, &callbacks, fmi2False, fmi2False);
	
	if (!c) return 1;
	
	fmi2Real Time = 0;
	fmi2Real stepSize = 0.5;
	fmi2Real stopTime = 10;

	// Informs the FMU to setup the experiment. Must be called after fmi2Instantiate and befor fmi2EnterInitializationMode
	CHECK_STATUS(SetupExperimentPtr(c, fmi2False, 0, Time, fmi2False, 0));

	// Informs the FMU to enter Initialization Mode.
	CHECK_STATUS(EnterInitializationModePtr(c));
	
	fmi2ValueReference AtFront_ref = 0;
	fmi2Boolean AtFront = 1;

	fmi2ValueReference AtBack_ref = 1;
	fmi2Boolean AtBack = 1;

	fmi2ValueReference Backward_ref = 2;
	fmi2Boolean Backward;

	fmi2ValueReference Forward_ref = 3;
	fmi2Boolean Forward;

	CHECK_STATUS(SetBooleanPtr(c, &AtFront_ref, 1, &AtFront));

	CHECK_STATUS(ExitInitializationModePtr(c));

	printf("time, AtFront, AtBack, Forward, Backward\n");
	
	for (int nSteps = 0; nSteps <= 20000; nSteps++) {

		Time = nSteps * stepSize;

		read_file("C:\\Users\\schyan01\\source\\repos\\mKinAtFront", &AtFront);
		read_file("C:\\Users\\schyan01\\source\\repos\\mKinAtBack", &AtBack);

		// set an input
		CHECK_STATUS(SetBooleanPtr(c, &AtFront_ref, 1, &AtFront));
		CHECK_STATUS(SetBooleanPtr(c, &AtBack_ref, 1, &AtBack));
		
		// perform a simulation step
		CHECK_STATUS(DoStepPtr(c, Time, stepSize, fmi2True));	//The computation of a time step is started.
		
		// get an output
		CHECK_STATUS(GetBooleanPtr(c, &Backward_ref, 1, &Backward));
		CHECK_STATUS(GetBooleanPtr(c, &Forward_ref, 1, &Forward));

		write_file("C:\\Users\\schyan01\\source\\repos\\mKinBackward", &Backward);
		write_file("C:\\Users\\schyan01\\source\\repos\\mKinForward", &Forward);

		printf("%d %d %d %d \n", AtFront, AtBack, Forward, Backward);
	}
	
TERMINATE:
	TerminatePtr(c);
	// clean up
	if (status < fmi2Fatal) {
		FreeInstancePtr(c);
	}
	
	return status;
}
