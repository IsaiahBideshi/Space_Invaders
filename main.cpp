//  Ammo system shoot and reload
/*  Game over screen not working:
 *      - Freezes and stops responding after some time
 *      - AI says to not use a while loop for game over screen and use an if statement instead
*/

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
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
    int size = rand() % 3, health = size + 1, spawnRate = 1000;
    float x = -100, y = -100;

    sf::Sprite asteroidSprite;
    sf::Texture asteroidTexture;

    asteroid() {
        if (size == 0) {
            asteroidTexture.loadFromFile("asteroidSmall.png");
        } else if (size == 1) {
            asteroidTexture.loadFromFile("asteroidMedium.png");
        } else {
            asteroidTexture.loadFromFile("asteroidLarge.png");
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
    int score=0, fireRate=300, ammo, lives=6;
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
};

int main(){
    sf::RenderWindow window(sf::VideoMode(480 , 800), "Space Invaders");
    player player1;
    sf::Clock cooldown, asteroidSpawn, speedTimer, scoreTimer;
    asteroid asteroids[100];

    sf::Texture fullHeart, halfHeart, emptyHeart;
    fullHeart.loadFromFile("full-heart1.png");
    halfHeart.loadFromFile("half-heart1.png");
    emptyHeart.loadFromFile("empty-heart1.png");

    sf::Sprite lifeSlots[3];
    for (int i=0;i<3;i++) {
        lifeSlots[i].setTexture(fullHeart);
        lifeSlots[i].setPosition(330 + (50 * i), 0);
    }
//  Text:
    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text scoreText("",font), gameOver("Game Over!", font), restartText("Restart", font);
    scoreText.setCharacterSize(30);
    scoreText.setOutlineColor(sf::Color::Black);
    scoreText.setOutlineThickness(2);

    gameOver.setStyle(sf::Text::Bold);
    gameOver.setFillColor(sf::Color::Red);
    gameOver.setCharacterSize(50);
    gameOver.setOutlineColor(sf::Color::Black);
    gameOver.setOutlineThickness(2);

    restartText.setColor(sf::Color::Black);
    restartText.setCharacterSize(18);

    sf::RectangleShape blackBackground(sf::Vector2f(480, 800));
    blackBackground.setFillColor(sf::Color::Black);
    sf::RectangleShape restartButton(sf::Vector2f(100, 30));

    int bulletCount = 0, asteroidCount = 0, asteroidSpeed = 4;
    bool isGameOver = false;

    // Start the game loop
    while (window.isOpen()){
        sf::Event event;

        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
            else if (isGameOver and event.type == sf::Event::MouseButtonPressed and event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (restartButton.getGlobalBounds().contains(mousePos.x, mousePos.y)){
                    for (int i=0;i<3;i++)
                        lifeSlots[i].setTexture(fullHeart);

                    scoreText.setPosition(0, 0);
                    isGameOver = false;
                    player1.lives = 6;
                }
            }
        }
        window.clear();
        if (speedTimer.getElapsedTime().asSeconds() > 10) {
            asteroidSpeed++;
            speedTimer.restart();
        }

        // Game Over, Player runs out of lives.
        if (player1.lives == 0) {
            isGameOver = true;
        }
        if (isGameOver) {
            cout << "Game Over" << endl;
            int scoreTextWidth = scoreText.getLocalBounds().width;
            int scoreTextHeight = scoreText.getLocalBounds().height;
            int gameOverWidth = gameOver.getLocalBounds().width;
            int gameOverHeight = gameOver.getLocalBounds().height;

            gameOver.setPosition(240 - gameOverWidth/2, 300 - gameOverHeight);
            scoreText.setPosition(240 - scoreTextWidth/2, gameOver.getPosition().y + gameOverHeight + 20);

            restartButton.setPosition(190 , scoreText.getPosition().y + 50);

            restartText.setPosition(restartButton.getPosition().x + 22, restartButton.getPosition().y + 4);

            window.draw(blackBackground);
            window.draw(scoreText);
            window.draw(gameOver);
            window.draw(restartButton);
            window.draw(restartText);
            window.display();

        } else {
            sf::CircleShape bulletShape(4);
            sf::RectangleShape shootArea(sf::Vector2f(480, 200));
            shootArea.setPosition(0, 600);
            shootArea.setFillColor(sf::Color::Transparent);
            shootArea.setOutlineThickness(4);
            shootArea.setOutlineColor(sf::Color::White);
            scoreText.setString("Score: " + to_string(player1.score));

        // Background Image
            sf::Texture background;
            background.loadFromFile("background.jpg");
            window.draw(sf::Sprite(background));
            window.draw(shootArea);
            for (int i=0;i<3;i++)
                window.draw(lifeSlots[i]);


        // Asteroid Proccessing
            if (asteroidSpawn.getElapsedTime().asMilliseconds() > asteroids[0].spawnRate) {
                asteroids[asteroidCount].spawn();
                asteroidCount++;

                if (asteroidCount == 100)
                    asteroidCount = 0;

                asteroidSpawn.restart();
            }

            for (int i=0;i<100;i++) {
                if (asteroids[i].y != -100 and asteroids[i].x != -100) {
                    asteroids[i].asteroidSprite.setPosition(asteroids[i].x, asteroids[i].y);
                    window.draw(asteroids[i].asteroidSprite);
                    asteroids[i].y += asteroidSpeed;
                }
                if (asteroids[i].y > 800) {
                    asteroids[i].x = -100;
                    asteroids[i].y = -100;
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
            for (int i = 0; i < 100; i++) {
                if (player1.bullets[i].y < 0) {
                    player1.bullets[i].x = -100;
                    player1.bullets[i].y = -100;
                    continue;
                }
                player1.bullets[i].y -= 10;
                bulletShape.setPosition(player1.bullets[i].x, player1.bullets[i].y);
                window.draw(bulletShape);
                cout << "Bullet: " << i+1 << " Y: " << player1.bullets[i].y << endl;
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                if (cooldown.getElapsedTime().asMilliseconds() > player1.fireRate) {
                    if (player1.shoot(player1.x, player1.y, bulletCount)) {
                        bulletCount++;
                        cout << "Bullet Fired: " << bulletCount << endl;
                    }

                    cooldown.restart();
                }
            }

            // Collision Detection
            for (int i=0;i<100;i++) {
                for (int j=0;j<100;j++) {
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
                        player1.score += 10;
                        player1.bullets[i].x = -100;
                        player1.bullets[i].y = -100;
                        asteroids[j].x = -100;
                        asteroids[j].y = -100;
                    }
                }
            }

            asteroidSpeed = 10;

            window.setSize({480, 800});
            window.draw(scoreText);
            window.display();
        }

    }
    int score = scoreTimer.getElapsedTime().asMilliseconds();
    cout << endl << "Bullets Fired: " << bulletCount << endl;
    cout << "Score: " << thousandSeparator(score) << endl;

    return EXIT_SUCCESS;
}
