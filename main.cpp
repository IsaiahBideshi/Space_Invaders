//  BUGS:
//  - After 3 random sized asteroids spawn the same size asteroid will spawn in a rotation

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
    }

    void reset() {
        size = rand() % 3;
        x = -100, y = -100;
        asteroid();
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
    int score=0, fireRate=300;
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

    int bulletCount = 0, asteroidCount = 0, asteroidSpeed = 4;

    // Start the game loop
    while (window.isOpen()){
        if (speedTimer.getElapsedTime().asSeconds() > 10) {
            asteroidSpeed++;
            speedTimer.restart();
        }
        window.display();

        sf::Event event;
        sf::CircleShape bulletShape(4);

    //  Text For Score:
        sf::Font font;
        font.loadFromFile("arial.ttf");
        sf::Text text("Score: " + to_string(player1.score), font);
        text.setCharacterSize(20);
        text.setOutlineColor(sf::Color::Black);
        text.setOutlineThickness(2);

    // Background Image
        sf::Texture background;
        background.loadFromFile("background.jpg");
        window.draw(sf::Sprite(background));

    // Asteroid Proccessing
        if (asteroidSpawn.getElapsedTime().asMilliseconds() > asteroids[0].spawnRate) {
            for (int i=0;i<=100;i++) {
                if (asteroids[i].x == -100 and asteroids[i].y == -100) {
                    asteroids[i].reset();
                    asteroids[i].spawn();
                    asteroidCount++;
                    break;
                }
            }
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
                    player1.bullets[i].x = -100;
                    player1.bullets[i].y = -100;
                    asteroids[j].x = -100;
                    asteroids[j].y = -100;
                }
            }
        }

        while (window.pollEvent(event)) {
        // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.draw(text);
        window.setSize({480, 800});

    }
    int score = scoreTimer.getElapsedTime().asMilliseconds();
    cout << endl << "Bullets Fired: " << bulletCount << endl;
    cout << "Score: " << thousandSeparator(score) << endl;

    return EXIT_SUCCESS;
}
