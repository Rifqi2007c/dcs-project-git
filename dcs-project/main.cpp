#include <iostream>
#include <algorithm>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace sf;
using namespace std;

// Simple entity with health 
struct Entity {
    Sprite sprite;
    float health;
    Entity(const Sprite& s, float h) : sprite(s), health(h) {}
};

enum GameState { MENU, PLAYING, STAGE_COMPLETE, WIN, GAME_OVER };

// normalize helper
sf::Vector2f normalize(Vector2f source) {
    float length = sqrt(source.x * source.x + source.y * source.y);
    if (length != 0)
        return Vector2f(source.x / length, source.y / length);
    else
        return Vector2f(0, 0);
}

int main() {
    RenderWindow window(VideoMode({1300, 900}), "SFML window");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(true);

    // font
    Font font("asset/arial.ttf");

    // enemy template + texture + sprite
    const Texture enemyT("asset/enemy_spritesheet.png");
    vector<Entity> enemies;
    float enemyHealth = 3.0f;
    float enemySpeed = 150.0f;
    
    // spawn enemy clock
    Clock spawnClock;


    // player
    const Texture playerT("asset/player_spritesheet.png");
    Sprite player(playerT);
    player.setPosition({650.f, 450.f});
    player.setScale({3.f, 3.f});
    float playerSpeed = 300.0f;
    float playerHealth = 100.0f;
    float maxPlayerHealth = 100.0f;

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
    Clock dtClock;
    bool isMoving = false, isAttack = false;

    // game state + scoring + stages
    GameState gameState = MENU;
    int score = 0;
    // no separate win score; final stage threshold defines victory
    int stage = 1;
    const int maxStage = 3;
    const int stageThresholds[] = {1000, 2000, 4000};
    const float baseSpawnInterval = 3.0f;

    auto resetGame = [&]() {
        playerHealth = maxPlayerHealth;
        enemies.clear();
        spawnClock.restart();
        player.setPosition({650.f, 450.f});
        currentFrame = 0;
        isMoving = false;
        isAttack = false;
        dtClock.restart();
        score = 0;
        stage = 1;
    };

    // seed RNG
    std::srand((unsigned)std::time(nullptr));

    while (window.isOpen()) {
        Time dtTime = dtClock.restart();
        float dt = dtTime.asSeconds();

        // events
        while (const std::optional ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();
            if (const auto* kp = ev->getIf<sf::Event::KeyPressed>()) {
                auto code = kp->code;
                if (gameState == MENU) {
                    if (code == Keyboard::Key::Enter || code == Keyboard::Key::Space) {
                        resetGame(); gameState = PLAYING;
                    }
                }
            }
        }

        if (gameState == PLAYING) {
            // compute spawn interval depending on stage; later stages spawn much faster
            float spawnInterval;
            switch(stage) {
                case 1: spawnInterval = baseSpawnInterval; break;
                case 2: spawnInterval = baseSpawnInterval * 0.6f; break;
                case 3: spawnInterval = baseSpawnInterval * 0.4f; break;
                default: spawnInterval = baseSpawnInterval * 0.4f; break;
            }
            spawnInterval = std::max(spawnInterval, 0.3f); // floor

            // enemy speed gradually increases, more sharply on higher stages
            float currentEnemySpeed = enemySpeed + (stage - 1) * 100.f;

            // spawn
            if (spawnClock.getElapsedTime().asSeconds() > spawnInterval) {
                float x = static_cast<float>(std::rand() % (window.getSize().x - 50));
                float y = static_cast<float>(std::rand() % (window.getSize().y - 50));
                Sprite enemy(enemyT);
                enemy.setScale({1.3f, 1.3f});
                enemy.setTextureRect({{0,0},{64,64}});
                enemy.setPosition({x,y});
                enemies.push_back(Entity(enemy, enemyHealth));
                spawnClock.restart();
            }

            // update enemies
            for (auto& ent : enemies) {
                Vector2f playerPos = player.getPosition();
                Vector2f enemyPos = ent.sprite.getPosition();
                Vector2f dir = playerPos - enemyPos;
                dir = normalize(dir);
                ent.sprite.move(dir * currentEnemySpeed * dt);
            }

            // inputs & movement
            isMoving = false; isAttack = false;
            if (Keyboard::isKeyPressed(Keyboard::Key::D)) { player.move({playerSpeed*dt,0.f}); player.setScale({3.f,3.f}); sword.setScale({4.f,4.f}); isMoving = true; }
            if (Keyboard::isKeyPressed(Keyboard::Key::A)) { player.move({-playerSpeed*dt,0.f}); player.setScale({-3.f,3.f}); sword.setScale({-4.f,4.f}); isMoving = true; }
            if (Keyboard::isKeyPressed(Keyboard::Key::S)) { player.move({0.f,playerSpeed*dt}); isMoving = true; }
            if (Keyboard::isKeyPressed(Keyboard::Key::W)) { player.move({0.f,-playerSpeed*dt}); isMoving = true; }

            //player walking animation logic
            if (isMoving && animationClock.getElapsedTime().asSeconds() > animationSpeed) {
                currentFrame = (currentFrame + 1) % 4;
                player.setTextureRect(IntRect({currentFrame * frame_width, currentRow * frame_height}, {frame_width, frame_height}));
                animationClock.restart();
            } else if (!isMoving) {
                player.setTextureRect(IntRect({0,0},{frame_width, frame_height}));
            }
            //sword stick to player
            sword.setPosition(player.getPosition());
            
            //player attack
            if (Keyboard::isKeyPressed(Keyboard::Key::L)) isAttack = true;
            
            //attack animation
            if (isAttack && swordClock.getElapsedTime().asSeconds() > swordAniSpeed) {
                cSwordFrame = (cSwordFrame + 1) % 4;
                sword.setTextureRect(IntRect({cSwordFrame * sword_width, cSwordRow * sword_height}, {sword_width, sword_height}));
                swordClock.restart();
            } else if (!isAttack) {
                sword.setTextureRect(IntRect({0,0},{sword_width, sword_height}));
            }
            
        // sword follow cursor location
        //Vector2i mousePos = Mouse::getPosition(window);
        //Vector2f worldMousePos = window.mapPixelToCoords(mousePos);
        //Vector2f diff = worldMousePos - sword.getPosition();

        // Calculate sword angle in degrees
        //float angle = atan2(diff.y, diff.x) * 180.f / 3.14159f;
        //sword.setRotation(sf::degrees(angle));


            // collisions: sword vs enemies
            FloatRect swordBounds = sword.getGlobalBounds();
            for (auto it = enemies.begin(); it != enemies.end(); ) {
                FloatRect eb = it->sprite.getGlobalBounds();
                if (swordBounds.findIntersection(eb).has_value() && isAttack) {
                    it->health -= 1.0f;
                    if (it->health <= 0) {
                        score += 100; // 100 points per enemy
                        // check for stage completion
                        if (score >= stageThresholds[stage-1]) {
                            if (stage < maxStage) {
                                gameState = STAGE_COMPLETE;
                            } else {
                                gameState = WIN;
                            }
                        }
                        // stage increment will occur when player acknowledges completion
                        it = enemies.erase(it);
                        continue;
                    }
                }
                ++it;
            }

            // collisions: player vs enemies
            FloatRect pBounds = player.getGlobalBounds();
            for (auto& e : enemies) {
                FloatRect eb = e.sprite.getGlobalBounds();
                if (pBounds.findIntersection(eb).has_value()) {
                    playerHealth -= 10.f * dt;
                }
            }

            if (playerHealth < 0) playerHealth = 0;
            if (playerHealth > maxPlayerHealth) playerHealth = maxPlayerHealth;
            if (playerHealth <= 0) gameState = GAME_OVER;
        }

        // game over, stage complete, and win handling
        if (gameState == GAME_OVER || gameState == WIN) {
            if (Keyboard::isKeyPressed(Keyboard::Key::Space) || Keyboard::isKeyPressed(Keyboard::Key::R)) {
                resetGame(); gameState = PLAYING;
            }
        } else if (gameState == STAGE_COMPLETE) {
            if (Keyboard::isKeyPressed(Keyboard::Key::Space) || Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
                // increment to next stage then resume
                if (stage < maxStage) ++stage;
                gameState = PLAYING;
            }
        }

        // menu and HUD rendering
        window.clear();
        if (gameState == MENU) {
            Text title(font, "DCS PROJECT"); title.setCharacterSize(80); title.setFillColor(Color::White);
            { auto b = title.getLocalBounds(); title.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
            title.setPosition({650.f,250.f}); window.draw(title);
            Text instr(font, "Press ENTER or SPACE to Start"); instr.setCharacterSize(30); instr.setFillColor(Color::Yellow);
            { auto b = instr.getLocalBounds(); instr.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
            instr.setPosition({650.f,360.f}); window.draw(instr);
        } else {
            window.draw(player);
            window.draw(sword);
            for (const auto& e : enemies) window.draw(e.sprite);

            Text healthText(font, string("Health: ") + to_string((int)playerHealth));
            healthText.setCharacterSize(20); healthText.setFillColor(Color::White); healthText.setOrigin({0.f,0.f}); healthText.setPosition({10.f,10.f}); window.draw(healthText);

            Text scoreHudText(font, string("Score: ") + to_string(score)); scoreHudText.setCharacterSize(20); scoreHudText.setFillColor(Color::White); scoreHudText.setOrigin({0.f,0.f}); scoreHudText.setPosition({10.f,40.f}); window.draw(scoreHudText);

            Text stageHudText(font, string("Stage: ") + to_string(stage)); stageHudText.setCharacterSize(20); stageHudText.setFillColor(Color::White); stageHudText.setOrigin({0.f,0.f}); stageHudText.setPosition({10.f,70.f}); window.draw(stageHudText);

           // game over, stage complete, and win screens
            if (gameState == GAME_OVER) {
                RectangleShape overlay(Vector2f(1300,900)); overlay.setFillColor(Color(0,0,0,150)); window.draw(overlay);
                Text gameOverText(font, "GAME OVER"); gameOverText.setCharacterSize(80); gameOverText.setFillColor(Color::Red);
                { auto b = gameOverText.getLocalBounds(); gameOverText.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
                gameOverText.setPosition({650.f,300.f}); window.draw(gameOverText);
                Text scoreText(font, string("Final Health: ") + to_string((int)playerHealth)); scoreText.setCharacterSize(30); scoreText.setFillColor(Color::White);
                { auto b = scoreText.getLocalBounds(); scoreText.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
                scoreText.setPosition({650.f,420.f}); window.draw(scoreText);
                Text restartText(font, "Press SPACE or R to Restart"); restartText.setCharacterSize(25); restartText.setFillColor(Color::Yellow);
                { auto b = restartText.getLocalBounds(); restartText.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
                restartText.setPosition({650.f,550.f}); window.draw(restartText);
            }
            if (gameState == STAGE_COMPLETE) {
                RectangleShape overlay(Vector2f(1300,900)); overlay.setFillColor(Color(0,0,0,150)); window.draw(overlay);
                Text completeText(font, string("Stage ") + to_string(stage) + " Complete!");
                completeText.setCharacterSize(60);
                completeText.setFillColor(Color::Green);
                { auto b = completeText.getLocalBounds(); completeText.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
                completeText.setPosition({650.f,300.f}); window.draw(completeText);
                Text instr2(font, "Press SPACE or ENTER to continue"); instr2.setCharacterSize(25); instr2.setFillColor(Color::Yellow);
                { auto b = instr2.getLocalBounds(); instr2.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
                instr2.setPosition({650.f,450.f}); window.draw(instr2);
            }

            if (gameState == WIN) {
                RectangleShape overlay(Vector2f(1300,900)); overlay.setFillColor(Color(0,0,0,150)); window.draw(overlay);
                Text winText(font, "YOU WIN!"); winText.setCharacterSize(80); winText.setFillColor(Color::Green);
                { auto b = winText.getLocalBounds(); winText.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
                winText.setPosition({650.f,300.f}); window.draw(winText);
                Text scoreText(font, string("Score: ") + to_string(score)); scoreText.setCharacterSize(30); scoreText.setFillColor(Color::White);
                { auto b = scoreText.getLocalBounds(); scoreText.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
                scoreText.setPosition({650.f,420.f}); window.draw(scoreText);
                Text restartText(font, "Press SPACE or R to Restart"); restartText.setCharacterSize(25); restartText.setFillColor(Color::Yellow);
                { auto b = restartText.getLocalBounds(); restartText.setOrigin({b.size.x/2.f, b.size.y/2.f}); }
                restartText.setPosition({650.f,550.f}); window.draw(restartText);
            }
        }

            window.display();
        }
        return 0;
        
        //move player with WASD
        //attack with L key
        //space or enter to start, continue, and restart
    }
