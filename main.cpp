#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(1720, 1080), "X-Wing na tle");

    // Za³aduj obraz t³a o nazwie "tlo.png"
    sf::Texture tloTexture;
    if (!tloTexture.loadFromFile("image.png")) {
        // W przypadku niepowodzenia ³adowania obrazu
        return 1;
    }

    sf::Sprite tloSprite(tloTexture);

    // Za³aduj obrazek X-Winga
    sf::Texture xwingTexture;
    if (!xwingTexture.loadFromFile("xwing.png")) {
        // W przypadku niepowodzenia ³adowania obrazu
        return 2;
    }

    sf::Sprite xwingSprite(xwingTexture);
    xwingSprite.setScale(0.2f, 0.2f); // Zmniejszenie rozmiaru X-Winga
    xwingSprite.setOrigin(xwingSprite.getLocalBounds().width / 2.0f, xwingSprite.getLocalBounds().height / 2.0f);
    xwingSprite.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    const float xwingSpeed = 300.0f; // Prêdkoœæ poruszania siê X-Winga

    // Definicja k¹tów obrotu
    const float ROTATION_RIGHT = 90.0f;
    const float ROTATION_LEFT = -90.0f;
    const float ROTATION_UP = 0.0f;
    const float ROTATION_DOWN = 180.0f;
    const float ROTATION_UP_RIGHT = 45.0f;
    const float ROTATION_UP_LEFT = 315.0f;
    const float ROTATION_DOWN_RIGHT = 135.0f;
    const float ROTATION_DOWN_LEFT = 225.0f;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Delete) {
                // Zamknij program po naciœniêciu klawisza Delete
                window.close();
            }
        }

        // Poruszanie i obracanie X-Wingiem
        sf::Vector2f movement(0.0f, 0.0f);
        bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
        bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
        bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
        bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

        if (leftPressed && upPressed) {
            movement.x = -xwingSpeed;
            movement.y = -xwingSpeed;
            xwingSprite.setRotation(ROTATION_DOWN_RIGHT);
        }
        else if (rightPressed && upPressed) {
            movement.x = xwingSpeed;
            movement.y = -xwingSpeed;
            xwingSprite.setRotation(ROTATION_DOWN_LEFT);
        }
        else if (leftPressed && downPressed) {
            movement.x = -xwingSpeed;
            movement.y = xwingSpeed;
            xwingSprite.setRotation(ROTATION_UP_RIGHT);
        }
        else if (rightPressed && downPressed) {
            movement.x = xwingSpeed;
            movement.y = xwingSpeed;
            xwingSprite.setRotation(ROTATION_UP_LEFT);
        }
        else if (leftPressed) {
            movement.x = -xwingSpeed;
            xwingSprite.setRotation(ROTATION_RIGHT);
        }
        else if (rightPressed) {
            movement.x = xwingSpeed;
            xwingSprite.setRotation(ROTATION_LEFT);
        }
        else if (upPressed) {
            movement.y = -xwingSpeed;
            xwingSprite.setRotation(ROTATION_DOWN);
        }
        else if (downPressed) {
            movement.y = xwingSpeed;
            xwingSprite.setRotation(ROTATION_UP);
        }

        xwingSprite.move(movement * clock.restart().asSeconds());

        window.clear();
        window.draw(tloSprite);
        window.draw(xwingSprite);
        window.display();
    }

    return 0;
}
