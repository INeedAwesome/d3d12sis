#include "sspch.h"
#include "HelloTriangle.h"

#ifdef SS_DEBUG
int main ()
{
	SS::HelloTriangle app;
	app.Start();

	return 0;
}
#endif // SS_DEBUG

#ifdef SS_RELEASE
int main()
{
	SS::HelloTriangle app;
	app.Start();

	return 0;
}
#endif // SS_RELEASE

#ifdef SS_DIST
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	SS::HelloTriangle app;
	app.Start();

	return 0;
}
#endif // SS_DIST



