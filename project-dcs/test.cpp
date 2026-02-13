#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;


int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({1300, 900}), "SFML window");

    // Load a sprite to display
    const sf::Texture texture("asset/player.png");
    sf::Sprite sprite(texture);
    sprite.setPosition({50, 50});

      const sf:: Font font1("asset/arial.ttf");
      sf::Text text1(font1, "click me", 30);

    // Create a graphical text to display
    // const sf::Font font("arial.ttf");
    // sf::Text text(font, "silly creatura --->", 50);
    // text.setPosition({10.f, 800.f});
    // text.setFillColor(sf::Color::Red);
  
    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();
        }
 
        // Clear screen
        window.clear();
 
        // Draw the sprite
        window.draw(sprite);
        window.draw(sprite1);
 
        // Draw the string
        window.draw(text);
        window.draw(text1);
 
        // Update the window
        window.display();
    }
}




