#include "App.h"
#include <SFML/Graphics.hpp>
#include "Utils.h"

int main()
{
	Application app;
    sf::RenderWindow window(sf::VideoMode(WINDOWS_SIZE.x, WINDOWS_SIZE.y), "Mines");
    app.StartUp(&window);

    app.MainLoop();

    app.ShutDown();
	return 0;
}
