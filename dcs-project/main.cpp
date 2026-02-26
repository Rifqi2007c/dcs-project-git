#include <iostream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace sf;
using namespace std;


// Helper function to normalize vectors
sf::Vector2f normalize(Vector2f source) {
    float length = sqrt(source.x * source.x + source.y * source.y);
    if (length != 0)
        return Vector2f(source.x / length, source.y / length);
    else
        return Vector2f(0, 0);
}

int main()
{
    // Create the main window
    RenderWindow window(sf::VideoMode({1300, 900}), "SFML window");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(true);

    // enemy template + texture + sprite
    const Texture enemyT("asset/enemy_spritesheet.png");
    vector<Sprite> enemies;
    Sprite enemy(enemyT);
    enemy.setScale({1.3f, 1.3f});
    enemy.setTextureRect({{0, 0}, {64, 64}});
    float enemySpeed = 150.0f;
    
    // spawn enemy clock
    Clock spawnClock;


    // player
    const Texture playerT("asset/player_spritesheet.png");
    Sprite player(playerT);
    player.setPosition({650.f, 450.f});
    player.setScale({3.f, 3.f});
    float playerSpeed = 300.0f;

    // player center origin
    player.setOrigin({20.f, 20.f});
    Vector2f origin = player.getOrigin();

    // player animation variable
    const int frame_width = 32;
    const int frame_height = 32;
    int currentFrame = 0;
    int currentRow = 0;
    player.setTextureRect({{0, 0}, {frame_width, frame_height}});

    // player walking animation clock
    Clock animationClock;
    float animationSpeed = 0.10f; // Seconds per frame
    
    
    // sword
    const Texture swordT("asset/sword_spritesheet.png");
    Sprite sword(swordT);
    sword.setScale({4.f, 4.f});
    
    // sword animation variable
    const int sword_width = 64;
    const int sword_height = 64;
    int cSwordFrame = 0;
    int cSwordRow = 0;
    sword.setTextureRect({{0, 0}, {sword_width, sword_height}});
    
    // sword bounds
    FloatRect bounds = sword.getLocalBounds();
    sword.setOrigin({bounds.size.x / 4.f, bounds.size.y / 2.5f});

    // sword animation clock
    Clock swordClock;
    float swordAniSpeed = 0.07f;


    // Clock for deltaTime
    Clock dtClock;

    // Start the game loop
    while (window.isOpen())
    {
        // deltaTime
        Time dtTime = dtClock.restart();
        float dt = dtTime.asSeconds();

        // Process events
        while (const std::optional event = window.pollEvent())
        {
          // Close window: exit
          if (event->is<sf::Event::Closed>())
                window.close();

          //if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
            //    if (keyReleased->scancode == sf::Keyboard::Scan::L) {
              //    sword.setTextureRect({{0, 0}, {sword_width, sword_height}});
                //}
          //}
        }
        
        // spwan logic
        float spawnInterval = 3.0f;
        
        // spawn enemy at random place
        if (spawnClock.getElapsedTime().asSeconds() > spawnInterval) {
            // Random position within window bounds
            float x = static_cast<float>(std::rand() % (window.getSize().x - 50));
            float y = static_cast<float>(std::rand() % (window.getSize().y - 50));
            enemy.setPosition({x, y});

            enemies.push_back(enemy);

            spawnClock.restart();
        }
        
        // update enemy movement (continuous Follow)
        for (auto& enemy : enemies) {
            sf::Vector2f playerPos = player.getPosition();
            sf::Vector2f enemyPos = enemy.getPosition();

            // Calculate direction vector
            sf::Vector2f direction = playerPos - enemyPos;
            
            // Normalize and move
            direction = normalize(direction);
            enemy.move(direction * enemySpeed * dt);
        }

        //animation logic
        bool isMoving = false;
        bool isAttack = false;

        // player movement
        if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
           player.move({playerSpeed * dt, 0.f});
           player.setScale({3.f, 3.f});
           sword.setScale({4.f, 4.f});
           isMoving = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
          player.move({-playerSpeed * dt, 0.f});
          player.setScale({-3.f, 3.f});
          sword.setScale({-4.f, 4.f});
          isMoving = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
          player.move({0.f, playerSpeed * dt});
          isMoving = true;
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
          player.move({0.f, -playerSpeed * dt});
          isMoving = true;
        }
        
        // player walking animation logic
        if (isMoving && animationClock.getElapsedTime().asSeconds() > animationSpeed) {
            currentFrame = (currentFrame + 1) % 4; // Assuming 3 frames per animation
            player.setTextureRect(IntRect({currentFrame * frame_width, currentRow * frame_height}, {frame_width, frame_height}));
            animationClock.restart();
        }
        
        // sword stick to player
        sword.setPosition(player.getPosition());

        // player attack
        if (Keyboard::isKeyPressed(Keyboard::Key::L)) {
          isAttack = true;
        }

        //attack animation
        if (isAttack && swordClock.getElapsedTime().asSeconds() > swordAniSpeed) {
            cSwordFrame = (cSwordFrame + 1) % 4; // Assuming 3 frames per animation
            sword.setTextureRect(IntRect({cSwordFrame * sword_width, cSwordRow * sword_height}, {sword_width, sword_height}));
            swordClock.restart();

        } if (!isAttack) {
            sword.setTextureRect(IntRect({0, 0,}, {sword_width, sword_height}));
          }

        // sword follow cursor location
        //Vector2i mousePos = Mouse::getPosition(window);
        //Vector2f worldMousePos = window.mapPixelToCoords(mousePos);
        //Vector2f diff = worldMousePos - sword.getPosition();

        // Calculate sword angle in degrees
        //float angle = atan2(diff.y, diff.x) * 180.f / 3.14159f;
        //sword.setRotation(sf::degrees(angle));

 
        // Clear screen
        window.clear();
 
        // Draw the sprite
        window.draw(player);
        window.draw(sword);

        for (const auto& enemy : enemies) {
            window.draw(enemy);
        }

        // Draw the string
        //window.draw(text);
 
        // Update the window
        window.display();
    }
}
