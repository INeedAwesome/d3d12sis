#include "sspch.h"
#include "HelloTriangle.h"
bool InitializeDirectX12Pipeline();

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 720;

GLFWwindow* g_glfwWindow;
HWND g_hwnd;

ID3D12Device* g_Device;
ID3D12CommandQueue* g_CommandQueue;



int main ()
{
	SS::HelloTriangle app;
	app.Start();

	return 0;
}


