#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <iomanip>

enum class GameState {
    Menu,
    Playing
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1720, 1080), "X-Wing");
    srand(time(nullptr)); // Inicjalizacja generatora liczb losowych

    // Za�aduj obraz t�a o nazwie "image.png"
    sf::Texture tloTexture;
    if (!tloTexture.loadFromFile("image.png")) {
        return 1; // B��d �adowania obrazu t�a
    }
    sf::Sprite tloSprite(tloTexture);

    // Za�aduj obrazek X-Winga
    sf::Texture xwingTexture;
    if (!xwingTexture.loadFromFile("xwing.png")) {
        return 2; // B��d �adowania obrazu X-Winga
    }
    sf::Sprite xwingSprite(xwingTexture);
    xwingSprite.setScale(0.15f, 0.15f);
    xwingSprite.setOrigin(xwingSprite.getLocalBounds().width / 2.0f, xwingSprite.getLocalBounds().height / 2.0f);

    // Za�aduj obrazek TIE Fightera
    sf::Texture tieTexture;
    if (!tieTexture.loadFromFile("tie.png")) {
        return 3; // B��d �adowania obrazu TIE Fightera
    }
    sf::Sprite tieSprite(tieTexture);
    tieSprite.setScale(0.075f, 0.075f);

    // Zdefiniuj pocz�tkowy cel dla TIE Fightera
    sf::Vector2f tieTarget(rand() % window.getSize().x, rand() % window.getSize().y);

    // Ustawienia pr�dko�ci
    const float xwingSpeed = 300.0f;
    const float tieSpeed = 150.0f;

    // Ustawienia k�t�w obrotu
    const float ROTATION_RIGHT = 90.0f;
    const float ROTATION_LEFT = -90.0f;
    const float ROTATION_UP = 0.0f;
    const float ROTATION_DOWN = 180.0f;
    const float ROTATION_UP_RIGHT = 45.0f;
    const float ROTATION_UP_LEFT = 315.0f;
    const float ROTATION_DOWN_RIGHT = 135.0f;
    const float ROTATION_DOWN_LEFT = 225.0f;

    sf::Clock xwingClock;
    sf::Clock tieClock;
    sf::Clock directionChangeClock;
    sf::Clock gameClock; // Zegar gry

    // Importuj czcionk� "Cascadia Mono"
    sf::Font cascadiaFont;
    if (!cascadiaFont.loadFromFile("CascadiaMono-Bold.otf")) {
        return 4; // Obs�uga b��du �adowania czcionki
    }

    // Inicjalizacja tekstu do wy�wietlania czasu gry
    sf::Text gameTimeText;
    gameTimeText.setFont(cascadiaFont);
    gameTimeText.setCharacterSize(24);
    gameTimeText.setFillColor(sf::Color::White);
    gameTimeText.setPosition(10.0f, 10.0f);

    // Przycisk "Start" jako prostok�tny kafelek
    sf::RectangleShape startButton(sf::Vector2f(200.0f, 100.0f));
    startButton.setFillColor(sf::Color::Yellow);
    startButton.setPosition(window.getSize().x / 2.0f - startButton.getSize().x / 2.0f, window.getSize().y / 2.0f - startButton.getSize().y / 2.0f);

    // Tekst "START" na przycisku
    sf::Text startButtonText;
    startButtonText.setFont(cascadiaFont);
    startButtonText.setCharacterSize(36);
    startButtonText.setFillColor(sf::Color::Black);
    startButtonText.setString("START");
    startButtonText.setPosition(startButton.getPosition().x + startButton.getSize().x / 2.0f - startButtonText.getLocalBounds().width / 2.0f,
        startButton.getPosition().y + startButton.getSize().y / 2.0f - startButtonText.getLocalBounds().height / 2.0f);

    // Przycisk "TW�RCA" jako prostok�tny kafelek
    sf::RectangleShape creatorButton(sf::Vector2f(200.0f, 100.0f));
    creatorButton.setFillColor(sf::Color::Yellow);
    creatorButton.setPosition(window.getSize().x / 2.0f - creatorButton.getSize().x / 2.0f, startButton.getPosition().y + startButton.getSize().y + 50.0f);

    // Tekst "TW�RCA" na przycisku
    sf::Text creatorButtonText;
    creatorButtonText.setFont(cascadiaFont);
    creatorButtonText.setCharacterSize(36);
    creatorButtonText.setFillColor(sf::Color::Black);
    creatorButtonText.setString("TW�RCA");
    creatorButtonText.setPosition(creatorButton.getPosition().x + creatorButton.getSize().x / 2.0f - creatorButtonText.getLocalBounds().width / 2.0f,
        creatorButton.getPosition().y + creatorButton.getSize().y / 2.0f - creatorButtonText.getLocalBounds().height / 2.0f);

    float czasGry = 0.0f; // Ca�kowity czas gry w sekundach
    bool gameStarted = false; // Flaga informuj�ca o rozpocz�ciu gry
    bool showCreatorMessage = false; // Flaga do kontrolowania widoczno�ci okna wiadomo�ci tw�rcy
    GameState gameState = GameState::Menu; // Stan gry (menu lub gra)

    // Utw�rz obiekt tekstowy na komunikat tw�rcy
    sf::Text creatorMessageText;
    creatorMessageText.setFont(cascadiaFont);
    creatorMessageText.setCharacterSize(36);
    creatorMessageText.setFillColor(sf::Color::White);
    creatorMessageText.setString("Tw�rca gry: Jan Berdzik");
    creatorMessageText.setPosition(window.getSize().x / 2.0f - creatorMessageText.getLocalBounds().width / 2.0f, window.getSize().y / 2.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            // Obs�uga klawisza "E" do wy��czania gry
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                window.close();
            }
            // Obs�uga klikni�cia mysz�
            if (event.type == sf::Event::MouseButtonPressed) {
                if (gameState == GameState::Menu) {
                    // Sprawd�, czy myszka zosta�a klikni�ta na przycisku "Start"
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    if (startButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                        // Rozpocznij now� gr� tylko je�li jeste�my w menu i klikni�to na przycisk "Start"
                        gameState = GameState::Playing;
                        gameStarted = true; // Gra rozpocz�ta
                        // Zresetuj parametry gry, np. pozycj� statk�w, czas gry itp.
                        xwingSprite.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
                        tieSprite.setPosition(rand() % window.getSize().x, rand() % window.getSize().y);
                        tieTarget = sf::Vector2f(rand() % window.getSize().x, rand() % window.getSize().y);
                        czasGry = 0.0f;
                        gameClock.restart(); // Rozpocznij zegar gry
                    }
                    // Sprawd�, czy myszka zosta�a klikni�ta na przycisku "TW�RCA"
                    if (creatorButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                        // Ustaw flag�, aby wy�wietli� komunikat tw�rcy
                        showCreatorMessage = true;

                        // Tw�rz czarne t�o
                        sf::RectangleShape background;
                        background.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
                        background.setFillColor(sf::Color::Black);

                        // Tw�rz napis "Tw�rca Gry: Jan Berdzik"
                        sf::Text creatorText;
                        creatorText.setFont(cascadiaFont);
                        creatorText.setCharacterSize(36);
                        creatorText.setFillColor(sf::Color::White);
                        creatorText.setString("Tw�rca Gry: Jan Berdzik");

                        // Pozycjonuj napis na �rodku ekranu
                        creatorText.setPosition(
                            (window.getSize().x - creatorText.getLocalBounds().width) / 2.0f,
                            (window.getSize().y - creatorText.getLocalBounds().height) / 2.0f
                        );
                    }
                }
                // Obs�uga klikni�cia mysz� w oknie komunikatu tw�rcy
                else if (gameState == GameState::Menu && showCreatorMessage) {
                    // Je�li jeste�my w menu i widzimy okno komunikatu tw�rcy, zamknij je po klikni�ciu mysz�
                    showCreatorMessage = false;
                }
            }
            // Obs�uga klawisza "Esc" do powrotu z okna komunikatu tw�rcy do menu
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (showCreatorMessage) {
                    showCreatorMessage = false;
                }
            }
        }


        if (gameState == GameState::Playing && gameStarted) {
            // Logika gry tylko je�li jeste�my w stanie "Playing"
            float deltaTime = gameClock.restart().asSeconds(); // Pobierz czas od ostatniego przeliczenia

            // Logika poruszania X-Wingiem
            sf::Vector2f xwingMovement(0.0f, 0.0f);
            bool leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
            bool rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
            bool upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
            bool downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);

            if (leftPressed && upPressed) {
                xwingMovement.x = -xwingSpeed;
                xwingMovement.y = -xwingSpeed;
                xwingSprite.setRotation(ROTATION_DOWN_RIGHT);
            }
            else if (rightPressed && upPressed) {
                xwingMovement.x = xwingSpeed;
                xwingMovement.y = -xwingSpeed;
                xwingSprite.setRotation(ROTATION_DOWN_LEFT);
            }
            else if (leftPressed && downPressed) {
                xwingMovement.x = -xwingSpeed;
                xwingMovement.y = xwingSpeed;
                xwingSprite.setRotation(ROTATION_UP_RIGHT);
            }
            else if (rightPressed and downPressed) {
                xwingMovement.x = xwingSpeed;
                xwingMovement.y = xwingSpeed;
                xwingSprite.setRotation(ROTATION_UP_LEFT);
            }
            else if (leftPressed) {
                xwingMovement.x = -xwingSpeed;
                xwingSprite.setRotation(ROTATION_RIGHT);
            }
            else if (rightPressed) {
                xwingMovement.x = xwingSpeed;
                xwingSprite.setRotation(ROTATION_LEFT);
            }
            else if (upPressed) {
                xwingMovement.y = -xwingSpeed;
                xwingSprite.setRotation(ROTATION_DOWN);
            }
            else if (downPressed) {
                xwingMovement.y = xwingSpeed;
                xwingSprite.setRotation(ROTATION_UP);
            }

            // Ograniczenie ruchu X-Winga do granic ekranu
            sf::Vector2f xwingPosition = xwingSprite.getPosition();
            if (xwingPosition.x < 0) {
                xwingPosition.x = 0;
            }
            if (xwingPosition.x > window.getSize().x) {
                xwingPosition.x = window.getSize().x;
            }
            if (xwingPosition.y < 0) {
                xwingPosition.y = 0;
            }
            if (xwingPosition.y > window.getSize().y) {
                xwingPosition.y = window.getSize().y;
            }
            xwingSprite.setPosition(xwingPosition);

            xwingSprite.move(xwingMovement * deltaTime);

            // Logika poruszania TIE Fightera
            if (directionChangeClock.getElapsedTime().asSeconds() > 3.0f || tieSprite.getPosition() == tieTarget) {
                tieTarget = sf::Vector2f(rand() % window.getSize().x, rand() % window.getSize().y);
                directionChangeClock.restart();
            }

            sf::Vector2f tieDirection = tieTarget - tieSprite.getPosition();
            float distance = sqrt(pow(tieDirection.x, 2) + pow(tieDirection.y, 2));
            if (distance > 0) {
                tieDirection /= distance;
                tieSprite.move(tieDirection * tieSpeed * deltaTime);
            }

            czasGry += deltaTime;

            int sekundy = static_cast<int>(czasGry);

            std::stringstream ss;
            ss << "Czas: " << std::setw(2) << std::setfill('0') << sekundy << "s";

            gameTimeText.setString(ss.str());
        }

        window.clear();

        if (gameState == GameState::Menu) {
            window.draw(startButton);
            window.draw(startButtonText);
            window.draw(creatorButton);
            window.draw(creatorButtonText);

            // Dodaj rysowanie okna wiadomo�ci i tekstu komunikatu tw�rcy
            if (showCreatorMessage) {
                window.draw(creatorMessageText);
            }
        }
        else if     (gameState == GameState::Playing) {
            window.draw(tloSprite);
            window.draw(xwingSprite);
            window.draw(tieSprite);
            window.draw(gameTimeText);

        }

        window.display();
    }

    return 0;
}
