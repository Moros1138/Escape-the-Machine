#include "Game.h"

int main()
{
	if (game->Construct(480, 320, 2, 2, false, true))
		game->Start();
	delete game;
	return 0;
}

#if defined(OLC_PLATFORM_WINAPI)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	main();
}
#endif
