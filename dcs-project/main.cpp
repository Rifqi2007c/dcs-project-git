#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;


int main()
{
    // Create the main window
    RenderWindow window(sf::VideoMode({1300, 900}), "SFML window");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(true);

    // player sprite
    const Texture playerT("asset/player_spritesheet.png");
    Sprite player(playerT);
    player.setPosition({50, 50});
    player.setScale({4.f, 4.f});
    player.setTextureRect({{0, 0}, {32, 32}});

    int texWidth = 0;
    player.setOrigin({player.getTextureRect().size.x / 2.0f, player.getTextureRect().size.y / 2.0f });

    float timer = 0.1f;
    float timerMax = 0.50f;

    // player speed
    float playerSpeed = 100.f;

    // clock variable for deltaTime
    Clock clock;
    float time;

    float isAttacking = false;

    // Start the game loop
    while (window.isOpen())
    {
      // store player move (to avoid OS delay)
        // Process events
        while (const std::optional event = window.pollEvent())
        {
          // Close window: exit
          if (event->is<sf::Event::Closed>())
                window.close();

          if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                if (keyReleased->scancode == sf::Keyboard::Scan::L) {
                  player.setTextureRect({{0, 0}, {32, 32}});
                }
          }
        }
        
        // movement
        if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
           player.move({3.f, 0.f});
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
          player.move({-3.f, 0.f});
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
          player.move({0.f, 3.f});
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
          player.move({0.f, -3.f});
        }
        // player animation
        if (Keyboard::isKeyPressed(Keyboard::Key::L)) {
          timer += 0.1f;
          if (timer >= timerMax) {
            
            texWidth += 32;

            if (texWidth >= playerT.getSize().x) {
              texWidth = 0;
            }
            if (texWidth < playerT.getSize().x) {
              player.setTextureRect({{texWidth, 0}, {32, 32}});
            }

            timer = 0.0f;
          }
        }
        /*if (Keyboard::isKeyPressed(Keyboard::Key::L)) {
          texWidth += 32;
          if (texWidth >= playerT.getSize().x) {
            texWidth = 0;
          }
          if (texWidth < playerT.getSize().x) {
            player.setTextureRect({{texWidth, 0}, {32, 32}});
          }
        }*/
 
        // Clear screen
        window.clear();
 
        // Draw the sprite
        window.draw(player);
 
        // Draw the string
        //window.draw(text);
 
        // Update the window
        window.display();
    }
}

