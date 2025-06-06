//  Ammo system shoot and reload

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <windows.h>
#include <fstream>
using namespace std;

string thousandSeparator(int n) {
    string ans = "", num = to_string(n);
    int count = 0;
    for (int i=num.size()-1;i >= 0;i--) {
        count++;
        ans += num[i];
        if (count == 3) {
            ans += ',';
            count = 0;
        }
    }
    reverse(ans.begin(), ans.end());
    if (ans.size() % 4 == 0) {
        ans.erase(ans.begin());
    }
    return ans;
}


class bullet {
public:
    int x=-100, y=-100;
};

class asteroid {
public:
    int size = rand() % 3, health = size + 1;
    float x = -100, y = -100;

    sf::Sprite asteroidSprite;
    sf::Texture asteroidTexture;

    asteroid() {
        if (size == 0) {
            asteroidTexture.loadFromFile("assets/asteroidSmall.png");
        } else if (size == 1) {
            asteroidTexture.loadFromFile("assets/asteroidMedium.png");
        } else {
            asteroidTexture.loadFromFile("assets/asteroidLarge.png");
        }
        asteroidSprite.setTexture(asteroidTexture);
    }

    void spawn() {
        if (size == 0) {
            health = 1;
            asteroidSprite.setPosition(rand() % 460, 0);
        } else if (size == 1) {
            health = 2;
            asteroidSprite.setPosition(rand() % 450, 0);
        } else {
            health = 3;
            asteroidSprite.setPosition(rand() % 430, 0);
        }
        asteroidSprite.setTexture(asteroidTexture);

        x = asteroidSprite.getPosition().x;
        y = asteroidSprite.getPosition().y;
    }
};

class player {
public:
    int highScore = 0, lastScore = 0, score=0, fireRate=300, ammo, lives=6;
    int x, y;

    bullet bullets[100];

    bool shoot(int x, int y, int bulletCount) {
        if (x < 0 || y < 600 || x > 480 || y > 800) {
            cout << "Bullet out of bounds" << endl;
            return false;
        }

        if (bulletCount == 0) {
            bullets[0].x = x;
            bullets[0].y = y;
            return true;
        }

        for (int i=0;i<100;i++) {
            if (bullets[i].x == -100 and bullets[i].y == -100) {
                bullets[i].x = x;
                bullets[i].y = y;
                break;
            }
        }

        return true;
    }

    void getHighScore() {
        string highScorePath = getenv("APPDATA");
        highScorePath += "\\Space Invaders\\highscore.txt";

        if (!filesystem::exists(highScorePath)) {
            cout << "Highscore file !exists." << endl;
        }

        ifstream file(highScorePath);


        int localHighScore;
        if (file.is_open()) {
            file >> localHighScore;
            highScore = localHighScore;
            if (highScore < 0)
                highScore = 0;

            file.close();
        } else {
            cout << "Error opening highscore file." << endl;
            filesystem::create_directory(getenv("APPDATA") + string("\\Space Invaders")); // Create directory if it doesn't exist
        }
    }

    void saveHighScore() {
        string highScorePath = getenv("APPDATA");
        highScorePath += "\\Space Invaders\\highscore.txt";
        ofstream file(highScorePath);

        if (file.is_open()) {
            file << highScore;
            file.close();
        } else {
            cout << "Error saving highscore." << endl;
        }
    }

};

int main(){
    int bulletCount = 0, asteroidIndex = 0, asteroidSpeed = 3, MAX_ASTEROIDS = 500, asteroidSpawnRate = 1000;
    bool isGameOver = true, godMode = false;

    sf::RenderWindow window(sf::VideoMode(480 , 800), "Space Invaders");
    window.setFramerateLimit(60);
    player player1;
    player1.getHighScore();

    sf::Clock cooldown, asteroidSpawn, speedTimer, overallTimer;
    asteroid asteroids[MAX_ASTEROIDS];

    sf::Texture fullHeart, halfHeart, emptyHeart;
    fullHeart.loadFromFile("assets/full-heart1.png");
    halfHeart.loadFromFile("assets/half-heart1.png");
    emptyHeart.loadFromFile("assets/empty-heart1.png");

    sf::Sprite lifeSlots[3];
    for (int i=0;i<3;i++) {
        lifeSlots[i].setTexture(fullHeart);
        lifeSlots[i].setScale(0.08, 0.08);
        lifeSlots[i].setPosition(330 + (50 * i), 0);
    }
//  Text:
    sf::Font font;
    font.loadFromFile("assets/arial.ttf");
    sf::Text scoreText("",font), titleText("Space Invaders", font), startText("Start", font), timerText("", font);
    scoreText.setCharacterSize(30);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(2);

    timerText.setCharacterSize(30);
    timerText.setOutlineColor(sf::Color::Black);
    timerText.setOutlineThickness(2);


    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color::Red);
    titleText.setCharacterSize(50);
    titleText.setOutlineColor(sf::Color::Black);
    titleText.setOutlineThickness(2);

    startText.setFillColor(sf::Color::Black);
    startText.setCharacterSize(25);

    sf::RectangleShape blackBackground(sf::Vector2f(480, 800));
    blackBackground.setFillColor(sf::Color::Black);
    sf::RectangleShape restartButton(sf::Vector2f(100, 30));



    sf::Event event;
    while (window.isOpen()){
        //  Check if game is over and if the restart button is clicked then reset the game.
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();

            else if (isGameOver and event.type == sf::Event::MouseButtonPressed and event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (restartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)){
                    for (int i=0;i<MAX_ASTEROIDS;i++) {
                        if (i<3)
                            lifeSlots[i].setTexture(fullHeart);
                        asteroids[i].x = -100;
                        asteroids[i].y = -100;
                    }

                    asteroidSpeed = 3;
                    player1.score = 0;
                    scoreText.setPosition(0, 0);
                    isGameOver = false;
                    player1.lives = 6;
                    bulletCount = 0;
                    asteroidIndex = 0;
                    asteroidSpawnRate = 1000;
                    overallTimer.restart();
                    speedTimer.restart();
                    asteroidSpawn.restart();
                }
            }
        }
        window.clear();
        if (speedTimer.getElapsedTime().asSeconds() > 10) {
            asteroidSpeed++;
            if (asteroidSpawnRate >= 400)
                asteroidSpawnRate -= 100;
            speedTimer.restart();
        }

        // Game Over, Player runs out of lives.
        if (player1.lives == 0) {
            player1.lastScore = player1.score;
            if (player1.score > player1.highScore)
                player1.highScore = player1.score;
            isGameOver = true;
        }
        if (isGameOver) {
            int scoreTextWidth = scoreText.getLocalBounds().width;
            int gameOverWidth = titleText.getLocalBounds().width;
            int gameOverHeight = titleText.getLocalBounds().height;

            titleText.setPosition(240 - gameOverWidth/2, 200 - gameOverHeight);
            scoreText.setPosition(240 - scoreTextWidth/2, titleText.getPosition().y + gameOverHeight + 20);

            restartButton.setPosition(200 , 350);

            startText.setPosition(restartButton.getPosition().x + 22, restartButton.getPosition().y);


            sf::Text highScoreText("High Score: " + to_string(player1.highScore), font);
            sf::Text lastScoreText("Last Score: " + to_string(player1.lastScore), font);
            highScoreText.setCharacterSize(25);
            lastScoreText.setCharacterSize(25);
            highScoreText.setPosition(240 - highScoreText.getGlobalBounds().width/2, 250);
            lastScoreText.setPosition(240 - lastScoreText.getGlobalBounds().width/2, 280);
            highScoreText.setFillColor(sf::Color::White);
            lastScoreText.setFillColor(sf::Color::White);


            window.draw(blackBackground);
            window.draw(titleText);
            window.draw(restartButton);
            window.draw(startText);
            window.draw(lastScoreText);
            window.draw(highScoreText);
            window.display();

            continue;
        }

        sf::CircleShape bulletShape(4);
        sf::RectangleShape shootArea(sf::Vector2f(480, 200));
        shootArea.setPosition(0, 600);
        shootArea.setFillColor(sf::Color::Transparent);
        shootArea.setOutlineThickness(4);
        shootArea.setOutlineColor(sf::Color::White);
        scoreText.setString("Score: " + to_string(player1.score));
        timerText.setString(to_string( (int) overallTimer.getElapsedTime().asSeconds()) );
        timerText.setPosition(240 - timerText.getGlobalBounds().width/2, 0);


        // Background Image
        sf::Texture background;
        background.loadFromFile("assets/background.jpg");
        window.draw(sf::Sprite(background));
        window.draw(shootArea);
        for (int i=0;i<3;i++)
            window.draw(lifeSlots[i]);


        // Asteroid Proccessing
        if (asteroidSpawn.getElapsedTime().asMilliseconds() > asteroidSpawnRate) { // Time to spawn a new asteroid
            if (overallTimer.getElapsedTime().asSeconds() > 60) {   //  After 60 seconds, spawn only small asteroids
                while (asteroids[asteroidIndex].size != 0) {
                    asteroidIndex++;
                    if (asteroidIndex >= MAX_ASTEROIDS)
                        asteroidIndex = 0;
                }
            }
            else if (overallTimer.getElapsedTime().asSeconds() > 30) {  //  After 30 seconds, spawn only small and medium asteroids
                while (asteroids[asteroidIndex].size != 0 and asteroids[asteroidIndex].size != 1) {
                    asteroidIndex++;
                    if (asteroidIndex >= MAX_ASTEROIDS)
                        asteroidIndex = 0;
                }
            }
            else {                                                      //  Before 30 seconds, don't spawn small asteroids
                while (asteroids[asteroidIndex].size != 2 and asteroids[asteroidIndex].size != 1) {
                    asteroidIndex++;
                    if (asteroidIndex >= MAX_ASTEROIDS)
                        asteroidIndex = 0;
                }
            }

            asteroids[asteroidIndex].spawn();
            asteroidIndex++;

            if (asteroidIndex >= MAX_ASTEROIDS) {
                cout << "Reset AsteroidIndex" << endl;
                asteroidIndex = 0;
            }

            asteroidSpawn.restart();
        }

        for (int i=0;i<MAX_ASTEROIDS;i++) {
            if (asteroids[i].y != -100 and asteroids[i].x != -100) {
                asteroids[i].asteroidSprite.setPosition(asteroids[i].x, asteroids[i].y);
                window.draw(asteroids[i].asteroidSprite);
                asteroids[i].y += asteroidSpeed;
            }
            if (asteroids[i].y > 800) {
                asteroids[i].x = -100;
                asteroids[i].y = -100;
                if (!godMode)
                    player1.lives--;
                if (player1.lives % 2 == 0) lifeSlots[player1.lives/2].setTexture(emptyHeart);
                else lifeSlots[player1.lives/2].setTexture(halfHeart);
            }
        }


        //  Mouse Tracker
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        player1.x = mousePosition.x, player1.y = mousePosition.y;
        // cout << "X: " << player1.x << " Y: " << player1.y << endl;

        //  Update Bullet Positions
        player1.bullets[7].x = -100;    //  For some reason bullet 7's x coord gets changed to a random number, so I reset it here

        for (int i = 0; i < 100; i++) {
            if (player1.bullets[i].x != -100 and player1.bullets[i].y != -100) {
                if (player1.bullets[i].y < 0) {
                    player1.bullets[i].x = -100;
                    player1.bullets[i].y = -100;
                    continue;
                }
                player1.bullets[i].y -= 10;
                bulletShape.setPosition(player1.bullets[i].x, player1.bullets[i].y);
                window.draw(bulletShape);
                // cout << "Bullet: " << i+1 << " (" << player1.bullets[i].x << ", " << player1.bullets[i].y << ")" <<  endl;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (cooldown.getElapsedTime().asMilliseconds() > player1.fireRate) {
                if (player1.shoot(player1.x, player1.y, bulletCount)) {
                    bulletCount++;
                    // cout << "Bullet Fired: " << bulletCount << endl;
                }

                cooldown.restart();
            }
        }

        // Collision Detection
        for (int i=0;i<100;i++) {
            for (int j=0;j<MAX_ASTEROIDS;j++) {
                bool reset = false;
                if (asteroids[j].size == 0) {
                    if (player1.bullets[i].x+10 > asteroids[j].x and player1.bullets[i].x < asteroids[j].x + 20
                        and player1.bullets[i].y < asteroids[j].y + 27 and player1.bullets[i].y > asteroids[j].y) {

                        reset = true;
                    }
                }
                if (asteroids[j].size == 1) {
                    if (player1.bullets[i].x+10 > asteroids[j].x and player1.bullets[i].x < asteroids[j].x + 30
                        and player1.bullets[i].y < asteroids[j].y + 27 and player1.bullets[i].y > asteroids[j].y) {

                        reset = true;
                    }
                }
                if (asteroids[j].size == 2) {
                    if (player1.bullets[i].x+10 > asteroids[j].x and player1.bullets[i].x < asteroids[j].x + 50
                        and player1.bullets[i].y < asteroids[j].y + 27 and player1.bullets[i].y > asteroids[j].y) {

                        reset = true;
                    }
                }
                if (reset) {
                    if (asteroids[j].size == 2) player1.score += 10;
                    else if (asteroids[j].size == 1) player1.score += 30;
                    else player1.score += 50;


                    player1.bullets[i].x = -100;
                    player1.bullets[i].y = -100;
                    asteroids[j].x = -100;
                    asteroids[j].y = -100;
                }
            }
        }

        window.setSize({480, 800});
        window.draw(scoreText);
        window.draw(timerText);
        window.display();

    }
    // cout << endl << "Bullets Fired: " << bulletCount << endl;

    player1.saveHighScore();

    return EXIT_SUCCESS;
}
