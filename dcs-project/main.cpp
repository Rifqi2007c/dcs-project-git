#include <iostream>
#include <algorithm>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <utility> // Needed for std::move

using namespace sf;
using namespace std;

// Simple entity with health
struct Entity
{
    Sprite sprite;
    float health;

    // Animation specific variables
    Clock animClock;
    int currentFrame = 0;
    float animSpeed = 0.15f;

    Entity(Sprite s, float h) : sprite(std::move(s)), health(h) {}
};

enum GameState
{
    MENU,
    PLAYING,
    PAUSED,
    STAGE_COMPLETE,
    WIN,
    GAME_OVER
};

// normalize helper
sf::Vector2f normalize(Vector2f source)
{
    float length = sqrt(source.x * source.x + source.y * source.y);
    if (length != 0)
        return Vector2f(source.x / length, source.y / length);
    else
        return Vector2f(0, 0);
}

int main()
{
    RenderWindow window(VideoMode({1300, 900}), "SFML window");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(true);

    // Font
    Font font("asset/arial.ttf"); 

    // Background
    const Texture backgroundT("asset/background.png");
    Vector2u windowSize = window.getSize();
    Vector2u textureSize = backgroundT.getSize();

    Sprite background(backgroundT);
    background.setScale({
        static_cast<float>(windowSize.x) / textureSize.x, static_cast<float>(windowSize.y) / textureSize.y
    });
    
    // Bg overlay
    RectangleShape overlay(Vector2f(1300, 900));
    overlay.setFillColor(sf::Color(0, 0, 0, 60));

    // Enemy setup
    const Texture enemyT("asset/enemy_spritesheet.png");
    vector<Entity> enemies;
    float enemyHealth = 3.0f;
    float enemySpeed = 150.0f;

    // Enemy Animation Constants
    const int enemy_frame_width = 64;
    const int enemy_frame_height = 64;

    // Spawn clock
    Clock spawnClock;

    // Player setup
    const Texture playerT("asset/player_spritesheet.png");
    Sprite player(playerT);
    player.setPosition({650.f, 450.f});
    player.setScale({3.f, 3.f});
    float playerSpeed = 300.0f;
    float playerHealth = 100.0f;
    float maxPlayerHealth = 100.0f;

    // Player origin
    player.setOrigin({20.f, 20.f}); 

    // Player animation
    const int frame_width = 32;
    const int frame_height = 32;
    int currentFrame = 0;
    int currentRow = 0;
    player.setTextureRect({{0, 0}, {frame_width, frame_height}});

    Clock animationClock;
    float animationSpeed = 0.10f;

    // Sword setup
    const Texture swordT("asset/sword_spritesheet.png");
    Sprite sword(swordT);
    sword.setScale({4.f, 4.f});

    // Sword animation
    const int sword_width = 64;
    const int sword_height = 64;
    int cSwordFrame = 0;
    int cSwordRow = 0;
    sword.setTextureRect({{0, 0}, {sword_width, sword_height}});

    FloatRect bounds = sword.getLocalBounds();
    sword.setOrigin({bounds.size.x / 4.f, bounds.size.y / 2.5f});

    Clock swordClock;
    float swordAniSpeed = 0.07f;
    Clock dtClock;
    bool isMoving = false;
    
    // Attack State Variables
    bool isAttack = false;     // Is the animation currently playing?
    bool keyLHeld = false;     // Is the L key currently physically held down?

    // Game State
    GameState gameState = MENU;
    int score = 0;
    int stage = 1;
    const int maxStage = 3;
    const int stageThresholds[] = {1000, 2000, 4000};
    const float baseSpawnInterval = 3.0f;

    // Paused UI
    Text pauseText(font, "Game Paused", 50);
    pauseText.setFillColor(sf::Color::Red);
    {
        FloatRect b = pauseText.getLocalBounds();
        pauseText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    }
    pauseText.setPosition({650.f, 400.f});

    Text exitText(font, "Press Esc to Exit", 20);
    exitText.setFillColor(sf::Color::Red);
    {
        FloatRect b = exitText.getLocalBounds();
        exitText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
    }
    exitText.setPosition({650.f, 450.f});

    // Reset
    auto resetGame = [&]()
    {
        playerHealth = maxPlayerHealth;
        enemies.clear();
        spawnClock.restart();
        player.setPosition({650.f, 450.f});
        currentFrame = 0;
        isMoving = false;
        isAttack = false;
        keyLHeld = false;
        dtClock.restart();
        score = 0;
        stage = 1;
    };

    srand((unsigned)std::time(nullptr));

    while (window.isOpen())
    {
        Time dtTime = dtClock.restart();
        float dt = dtTime.asSeconds();

        // Event Processing
        while (const std::optional ev = window.pollEvent())
        {
            if (ev->is<sf::Event::Closed>())
                window.close();

            if (const auto *kp = ev->getIf<sf::Event::KeyPressed>())
            {
                if (kp->code == Keyboard::Key::P)
                {
                    if (gameState == PLAYING)
                        gameState = PAUSED;
                    else if (gameState == PAUSED)
                        gameState = PLAYING;
                }

                if (gameState == PAUSED)
                {
                    if (kp->code == Keyboard::Key::Escape)
                        window.close();
                }
                
                if (gameState == MENU)
                {
                    if (kp->code == Keyboard::Key::Enter || kp->code == Keyboard::Key::Space)
                    {
                        resetGame();
                        gameState = PLAYING;
                    }
                }

                if (gameState == STAGE_COMPLETE)
                {
                    if (kp->code == Keyboard::Key::Space || kp->code == Keyboard::Key::Enter)
                    {
                        if (stage < maxStage)
                            ++stage;
                        gameState = PLAYING;
                    }
                }

                if (gameState == GAME_OVER || gameState == WIN)
                {
                    if (kp->code == Keyboard::Key::Space)
                    {
                        resetGame();
                        gameState = PLAYING;
                    }
                    else if (kp->code == Keyboard::Key::Escape)
                    {
                        window.close();
                    }
                }

                // Attack Input Logic
                if (kp->code == Keyboard::Key::L) {
                    // Only trigger a new attack if:
                    // 1. Key wasn't already held (prevents auto-repeat holding)
                    // 2. We aren't already attacking (finish animation first)
                    if (!keyLHeld && !isAttack) {
                        isAttack = true;
                        keyLHeld = true;
                        cSwordFrame = 0;
                        swordClock.restart();
                    }
                }
            }

            if (const auto *kr = ev->getIf<sf::Event::KeyReleased>())
            {
                if (kr->code == Keyboard::Key::L)
                {
                    keyLHeld = false; // Reset held state when key is released
                }
            }
        } 



        if (gameState == PLAYING)
        {
            // Spawn Logic
            float spawnInterval;
            switch (stage)
            {
            case 1: spawnInterval = baseSpawnInterval; break;
            case 2: spawnInterval = baseSpawnInterval * 0.4f; break;
            case 3: spawnInterval = baseSpawnInterval * 0.3f; break;
            default: spawnInterval = baseSpawnInterval * 0.2f; break;
            }
            spawnInterval = std::max(spawnInterval, 0.3f);

            float currentEnemySpeed = enemySpeed + (stage - 1) * 100.f;

            if (spawnClock.getElapsedTime().asSeconds() > spawnInterval)
            {
                float x = static_cast<float>(std::rand() % (window.getSize().x - 50));
                float y = static_cast<float>(std::rand() % (window.getSize().y - 50));
                Sprite enemy(enemyT);
                enemy.setScale({1.3f, 1.3f});
                enemy.setTextureRect({{0, 0}, {64, 64}});
                
                // Set origin to center for proper rotation/flipping
                enemy.setOrigin({32.f, 32.f});
                
                enemy.setPosition({x, y});
                enemies.push_back(Entity(std::move(enemy), enemyHealth));
                spawnClock.restart();
            }

            // Enemy Movement & Turning
            for (auto &ent : enemies)
            {
                Vector2f playerPos = player.getPosition();
                Vector2f enemyPos = ent.sprite.getPosition();
                Vector2f dir = playerPos - enemyPos;
                
                // Facing Logic:
                // If player is to the right (dir.x > 0), face right (scale +)
                // If player is to the left (dir.x < 0), face left (scale -)
                if (dir.x > 0.f) 
                {
                    ent.sprite.setScale({-1.3f, 1.3f});
                }
                else if (dir.x < 0.f)
                {
                    ent.sprite.setScale({1.3f, 1.3f});
                }

                dir = normalize(dir);
                ent.sprite.move(dir * currentEnemySpeed * dt);

                // Enemy Animation Logic
                if (ent.animClock.getElapsedTime().asSeconds() > ent.animSpeed)
                {
                    ent.currentFrame = (ent.currentFrame + 1) % 4;
                    ent.sprite.setTextureRect(IntRect({ent.currentFrame * enemy_frame_width, 0}, {enemy_frame_width, enemy_frame_height}));
                    ent.animClock.restart();
                }
            }

            // Player Input
            isMoving = false;

            if (Keyboard::isKeyPressed(Keyboard::Key::D))
            {
                player.move({playerSpeed * dt, 0.f});
                player.setScale({3.f, 3.f});
                sword.setScale({4.f, 4.f});
                isMoving = true;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::A))
            {
                player.move({-playerSpeed * dt, 0.f});
                player.setScale({-3.f, 3.f});
                sword.setScale({-4.f, 4.f});
                isMoving = true;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::S))
            {
                player.move({0.f, playerSpeed * dt});
                isMoving = true;
            }
            if (Keyboard::isKeyPressed(Keyboard::Key::W))
            {
                player.move({0.f, -playerSpeed * dt});
                isMoving = true;
            }

            // Player Animation
            if (isMoving && animationClock.getElapsedTime().asSeconds() > animationSpeed)
            {
                currentFrame = (currentFrame + 1) % 4;
                player.setTextureRect(IntRect({currentFrame * frame_width, currentRow * frame_height}, {frame_width, frame_height}));
                animationClock.restart();
            }
            else if (!isMoving)
            {
                currentFrame = 0; 
                player.setTextureRect(IntRect({0, 0}, {frame_width, frame_height}));
            }

            // Sword Logic
            sword.setPosition(player.getPosition());

            // Sword Animation
            if (isAttack)
            {
                if (swordClock.getElapsedTime().asSeconds() > swordAniSpeed)
                {
                    cSwordFrame++;
                    if (cSwordFrame >= 4)
                    {
                        // Animation finished
                        cSwordFrame = 0; // Reset frame
                        isAttack = false; // Reset state
                    }
                    
                    sword.setTextureRect(IntRect({cSwordFrame * sword_width, cSwordRow * sword_height}, {sword_width, sword_height}));
                    
                    // Only restart clock if we are still attacking (prevents extra tick after done)
                    if(isAttack) swordClock.restart();
                }
            }
            
            // Reset sword sprite to idle frame if not attacking
            if (!isAttack) {
                 sword.setTextureRect(IntRect({0, 0}, {sword_width, sword_height}));
            }

            // Collisions: Sword vs Enemies
            FloatRect swordBounds = sword.getGlobalBounds();
            bool enemyDied = false;

            for (auto it = enemies.begin(); it != enemies.end(); )
            {
                FloatRect eb = it->sprite.getGlobalBounds();
                if (isAttack && swordBounds.findIntersection(eb).has_value())
                {
                    it->health -= 1.0f;
                    if (it->health <= 0)
                    {
                        score += 100;
                        it = enemies.erase(it);
                        enemyDied = true;
                        continue;
                    }
                }
                ++it;
            }

            // Stage Check
            if (enemyDied)
            {
                if (score >= stageThresholds[stage - 1])
                {
                    if (stage < maxStage)
                        gameState = STAGE_COMPLETE;
                    else
                        gameState = WIN;
                }
            }

            // Collisions: Player vs Enemies
            FloatRect pBounds = player.getGlobalBounds();
            for (auto &e : enemies)
            {
                FloatRect eb = e.sprite.getGlobalBounds();
                if (pBounds.findIntersection(eb).has_value())
                {
                    playerHealth -= 10.f * dt;
                }
            }

            if (playerHealth <= 0)
            {
                playerHealth = 0;
                gameState = GAME_OVER;
            }
        }

        // Rendering
        window.clear();
      
        // Background
        window.draw(background);

        if (gameState == MENU)
        {
            window.draw(overlay);

            Text title(font, "DCS PROJECT");
            title.setCharacterSize(80);
            title.setFillColor(Color::White);
            {
                auto b = title.getLocalBounds();
                title.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
            }
            title.setPosition({650.f, 250.f});
            window.draw(title);

            Text instr(font, "Press ENTER or SPACE to Start");
            instr.setCharacterSize(30);
            instr.setFillColor(Color::Yellow);
            {
                auto b = instr.getLocalBounds();
                instr.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
            }
            instr.setPosition({650.f, 360.f});
            window.draw(instr);
        }
        else
        {
            // Draw Game Objects
            window.draw(player);
            window.draw(sword);
            for (const auto &e : enemies)
                window.draw(e.sprite);

            // HUD
            Text healthText(font, string("Health: ") + to_string((int)playerHealth));
            healthText.setCharacterSize(20);
            healthText.setFillColor(Color::White);
            healthText.setPosition({10.f, 10.f});
            window.draw(healthText);

            Text scoreHudText(font, string("Score: ") + to_string(score));
            scoreHudText.setCharacterSize(20);
            scoreHudText.setFillColor(Color::White);
            scoreHudText.setPosition({10.f, 40.f});
            window.draw(scoreHudText);

            Text stageHudText(font, string("Stage: ") + to_string(stage));
            stageHudText.setCharacterSize(20);
            stageHudText.setFillColor(Color::White);
            stageHudText.setPosition({10.f, 70.f});
            window.draw(stageHudText);

            // Overlay Screens
            if (gameState == GAME_OVER)
            {
                window.draw(overlay);

                Text gameOverText(font, "GAME OVER");
                gameOverText.setCharacterSize(80);
                gameOverText.setFillColor(Color::Red);
                auto b = gameOverText.getLocalBounds();
                gameOverText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                gameOverText.setPosition({650.f, 300.f});
                window.draw(gameOverText);

                Text restartText(font, "Press SPACE to Restart or Esc to Exit");
                restartText.setCharacterSize(25);
                restartText.setFillColor(Color::Yellow);
                b = restartText.getLocalBounds();
                restartText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                restartText.setPosition({650.f, 550.f});
                window.draw(restartText);
            }
            if (gameState == PAUSED)
            {
                window.draw(overlay);
                window.draw(pauseText);
                window.draw(exitText);
                window.display();
                continue;
            }
            else if (gameState == STAGE_COMPLETE)
            {
                window.draw(overlay);

                Text completeText(font, string("Stage ") + to_string(stage) + " Complete!");
                completeText.setCharacterSize(60);
                completeText.setFillColor(Color::Green);
                auto b = completeText.getLocalBounds();
                completeText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                completeText.setPosition({650.f, 300.f});
                window.draw(completeText);

                Text instr2(font, "Press SPACE or ENTER to continue");
                instr2.setCharacterSize(25);
                instr2.setFillColor(Color::Yellow);
                b = instr2.getLocalBounds();
                instr2.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                instr2.setPosition({650.f, 450.f});
                window.draw(instr2);
            }
            else if (gameState == WIN)
            {
                window.draw(overlay);

                Text winText(font, "YOU WIN!");
                winText.setCharacterSize(80);
                winText.setFillColor(Color::Green);
                auto b = winText.getLocalBounds();
                winText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                winText.setPosition({650.f, 300.f});
                window.draw(winText);

                Text scoreText(font, string("Score: ") + to_string(score));
                scoreText.setCharacterSize(30);
                scoreText.setFillColor(Color::White);
                b = scoreText.getLocalBounds();
                scoreText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                scoreText.setPosition({650.f, 420.f});
                window.draw(scoreText);

                Text restartText(font, "Press SPACE to Restart or Esc to Exit");
                restartText.setCharacterSize(25);
                restartText.setFillColor(Color::Yellow);
                b = restartText.getLocalBounds();
                restartText.setOrigin({b.size.x / 2.f, b.size.y / 2.f});
                restartText.setPosition({650.f, 550.f});
                window.draw(restartText);
            }
        }

        window.display();
    }

    return 0;
}

