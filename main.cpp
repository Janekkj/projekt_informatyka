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
    const float margin = 50.0f; // Margines 50 pikseli
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
    xwingSprite.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    // Za�aduj obrazek TIE Fightera
    sf::Texture tieTexture;
    if (!tieTexture.loadFromFile("tie.png")) {
        return 3; // B��d �adowania obrazu TIE Fightera
    }
    sf::Sprite tieSprite(tieTexture);
    tieSprite.setScale(0.075f, 0.075f);
    //Blaster
    struct Blaster {
        sf::Sprite sprite;
        sf::Vector2f direction;
    };

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
    std::vector<Blaster> blasters;

    // Importuj czcionk� "Cascadia Mono"
    sf::Font cascadiaFont;
    if (!cascadiaFont.loadFromFile("CascadiaMono-Bold.otf")) {
        return 4; // Obs�uga b��du �adowania czcionki
    }
    //Pociski
    const float blasterSpeed = 500.0f; // Pr�dko�� pocisk�w
    sf::Texture blastersTexture;
    if (!blastersTexture.loadFromFile("blaster.png")) {
        return 5; // B��d �adowania obrazu pocisku
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
    bool startButtonVisible = true; // Flaga do kontrolowania widoczno�ci przycisku "Start"
    bool creatorButtonVisible = true; // Flaga do kontrolowania widoczno�ci przycisku "Tw�rca"

    sf::Clock gameClock; // Deklaracja zegara gry

    // Inicjalizacja tekstu do wy�wietlania komunikatu tw�rcy
    sf::Text creatorMessageText;
    creatorMessageText.setFont(cascadiaFont);
    creatorMessageText.setCharacterSize(36);
    creatorMessageText.setFillColor(sf::Color::White);
    creatorMessageText.setString("Tw�rca gry: Jan Berdzik");
    creatorMessageText.setPosition(window.getSize().x / 2.0f - creatorMessageText.getLocalBounds().width / 2.0f, window.getSize().y / 2.0f);
    bool showButtons = true; // Flaga do kontrolowania widoczno�ci przycisk�w
    // Pocz�tkowo pociski lec� w g�r�
    sf::Vector2f blasterDirection(0.0f, -1.0f);

    // Zmienna do przechowywania liczby punkt�w
    int punkty = 0;

    // Inicjalizacja tekstu do wy�wietlania punkt�w
    sf::Text punktyText;
    punktyText.setFont(cascadiaFont);
    punktyText.setCharacterSize(24);
    punktyText.setFillColor(sf::Color::White);
    punktyText.setPosition(10.0f, 40.0f); // Umieszczenie podczasem

    sf::Texture bombaTexture;
    if (!bombaTexture.loadFromFile("czarnadziura.png")) {
        return 6; // B��d �adowania obrazu bomby
    }
    sf::Texture bomba2Texture;
    if (!bomba2Texture.loadFromFile("czarnadziura.png")) {
        return 7; // B��d �adowania obrazu drugiej bomby
    }
    sf::Texture gwiazdaTexture;
    if (!gwiazdaTexture.loadFromFile("gwiazda.png")) {
        return 8; // B��d �adowania obrazu gwiazdy
    }
    sf::Sprite gwiazdaSprite(gwiazdaTexture);
    gwiazdaSprite.setScale(0.5f, 0.5f); // Skaluj gwiazd� odpowiednio
    bool gwiazdaActive = false;
    sf::Vector2f gwiazdaTarget(rand() % window.getSize().x, rand() % window.getSize().y);
    const float gwiazdaSpeed = 150.0f; // Ustaw pr�dko�� gwiazdy

    sf::Sprite bomba2Sprite(bomba2Texture);
    bomba2Sprite.setScale(0.3f, 0.3f); // Skaluj drug� bomb� odpowiednio
    bool bomba2Visible = false;
    sf::Clock bomba2Clock; // Zegar do �ledzenia czasu pojawienia si� drugiej bomby

    sf::Sprite bombaSprite(bombaTexture);
    bombaSprite.setScale(0.3f, 0.3f); // Skaluj bomb� odpowiednio
    bool bombaVisible = false;
    sf::Clock bombaClock; // Zegar do �ledzenia czasu pojawienia si� bomby

    sf::Text koniecGryText;
    koniecGryText.setFont(cascadiaFont);
    koniecGryText.setCharacterSize(72);
    koniecGryText.setFillColor(sf::Color::Red);
    koniecGryText.setString("KONIEC GRY");
    koniecGryText.setPosition(window.getSize().x / 2.0f - koniecGryText.getLocalBounds().width / 2.0f,
        window.getSize().y / 2.0f - koniecGryText.getLocalBounds().height / 2.0f);
    bool koniecGry = false;

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
                    sf::Clock gameClock; // Zegar gryFg
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    if (showButtons && startButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                        if (showButtons && startButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                            // Rozpocznij now� gr� tylko je�li jeste�my w menu, przyciski s� widoczne i klikni�to na przycisk "Start"
                            gameState = GameState::Playing;
                            gameStarted = true; // Gra rozpocz�ta
                            showButtons = false; // Ukryj przyciski

                            // Inicjalizacja zegara gry tutaj
                            gameClock.restart(); // Rozpocznij zegar gry

                            // Resetowanie parametr�w gry
                            xwingSprite.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
                            tieSprite.setPosition(rand() % window.getSize().x, rand() % window.getSize().y);
                            tieTarget = sf::Vector2f(rand() % window.getSize().x, rand() % window.getSize().y);
                            czasGry = 0.0f;
                            // Dodaj inne resetowane parametry gry, je�li s�
                        }
                    }
                    // Sprawd�, czy myszka zosta�a klikni�ta na przycisku "TW�RCA"
                    if (showButtons && creatorButton.getGlobalBounds().contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y))) {
                        // Ukryj przyciski
                        showButtons = false;
                        // Ustaw flag�, aby wy�wietli� komunikat tw�rcy
                        showCreatorMessage = true;
                    }
                }
                // Obs�uga klikni�cia mysz� w oknie komunikatu tw�rcy
                else if (gameState == GameState::Menu && showCreatorMessage) {
                    // Je�li jeste�my w menu, widzimy okno komunikatu tw�rcy i klikn�li�my je, zmie� widoczno�� przycisk�w
                    showButtons = !showButtons;
                }
            }
            // Obs�uga klawisza "Esc" do powrotu z okna komunikatu tw�rcy do menu
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (showCreatorMessage) {
                    showCreatorMessage = false; // Ukryj okno komunikatu tw�rcy
                    showButtons = true; // Poka� przyciski
                }
            }
        }


        if (gameState == GameState::Playing && gameStarted && !koniecGry) {
            // Logika gry tylko je�li jeste�my w stanie "Playing"
            float deltaTime = gameClock.restart().asSeconds(); // Pobierz czas od ostatniego przeliczenia
            if (xwingClock.getElapsedTime().asSeconds() > 1.0f) { // Strzelaj co 1 sekund�
                Blaster newBlaster;
                newBlaster.sprite = sf::Sprite(blastersTexture);
                newBlaster.sprite.setOrigin(blastersTexture.getSize().x / 2.0f, blastersTexture.getSize().y / 2.0f);
                newBlaster.sprite.setPosition(xwingSprite.getPosition());

                // Zmniejsz rozmiar i obr�� o 90 stopni w lewo
                newBlaster.sprite.setScale(0.2f, 0.2f); // Zmniejsz rozmiar dziesi�ciokrotnie
                newBlaster.sprite.setRotation(-90.0f); // Obr�� o 90 stopni w lewo

                newBlaster.direction = sf::Vector2f(0.0f, -1.0f); // Pocz�tkowy kierunek pocisku
                blasters.push_back(newBlaster);
                xwingClock.restart();
            }
            // Sprawdzenie kolizji mi�dzy pociskami a statkiem TIE
            for (auto it = blasters.begin(); it != blasters.end();) {
                if (it->sprite.getGlobalBounds().intersects(tieSprite.getGlobalBounds())) {
                    // Kolizja! Przenie� TIE Fighter i usu� pocisk i zwi�ksz punkty
                    tieSprite.setPosition(rand() % window.getSize().x, rand() % window.getSize().y); // Nowa losowa pozycja dla TIE
                    it = blasters.erase(it); // Usu� pocisk
                    punkty++; // Zwi�ksz punkty
                }
                else {
                    ++it;
                }
            }
            // Aktualizacja tekstu punkt�w
            punktyText.setString("Punkty: " + std::to_string(punkty));
            // W obs�udze klawiszy strza�ek, zaktualizuj warto�� zmiennej blasterDirection
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                blasterDirection = sf::Vector2f(-1.0f, 0.0f);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                blasterDirection = sf::Vector2f(1.0f, 0.0f);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                blasterDirection = sf::Vector2f(0.0f, -1.0f);
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                blasterDirection = sf::Vector2f(0.0f, 1.0f);
            }
            // W miejscu, gdzie poruszane s� pociski, u�yj blasterDirection do ustalenia kierunku ich ruchu
            for (auto it = blasters.begin(); it != blasters.end(); ) {
                it->sprite.move(it->direction * blasterSpeed * deltaTime);
                if (it->sprite.getPosition().y < 0) {
                    it = blasters.erase(it);
                }
                else {
                    ++it;
                }
            }
            // Logika pojawiania si� bomby
            if (bombaClock.getElapsedTime().asSeconds() >= 5.0f) {
                bombaSprite.setPosition(rand() % (window.getSize().x - 100) + 50, rand() % (window.getSize().y - 100) + 50);
                bombaVisible = true;
                bombaClock.restart();
            }

            // Logika kolizji X-Winga z bomb�
            if (bombaVisible && xwingSprite.getGlobalBounds().intersects(bombaSprite.getGlobalBounds())) {
                koniecGry = true; // Ustaw flag� koniecGry na true, aby zatrzyma� gr�
            }
            if (bomba2Clock.getElapsedTime().asSeconds() >= 5.0f) {
                bomba2Sprite.setPosition(rand() % (window.getSize().x - 100) + 50, rand() % (window.getSize().y - 100) + 50);
                bomba2Visible = true;
                bomba2Clock.restart();
            }

            // Logika kolizji X-Winga z drug� bomb�
            if (bomba2Visible && xwingSprite.getGlobalBounds().intersects(bomba2Sprite.getGlobalBounds())) {
                koniecGry = true; // Ustaw flag� koniecGry na true, aby zatrzyma� gr�
            }
            if (punkty >= 10) {
                gwiazdaActive = true;
            }
            //gwiazda
            if (gwiazdaActive) {
                // Logika poruszania gwiazdy
                if (directionChangeClock.getElapsedTime().asSeconds() > 3.0f || gwiazdaSprite.getPosition() == gwiazdaTarget) {
                    gwiazdaTarget = sf::Vector2f(rand() % window.getSize().x, rand() % window.getSize().y);
                    directionChangeClock.restart();
                }

                sf::Vector2f gwiazdaDirection = gwiazdaTarget - gwiazdaSprite.getPosition();
                float distance = sqrt(pow(gwiazdaDirection.x, 2) + pow(gwiazdaDirection.y, 2));
                if (distance > 0) {
                    gwiazdaDirection /= distance;
                    gwiazdaSprite.move(gwiazdaDirection * gwiazdaSpeed * deltaTime);
                }
            }

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
            // Je�li showButtons jest ustawione na true, rysuj przyciski
            if (showButtons) {
                window.draw(startButton);
                window.draw(startButtonText);
                window.draw(creatorButton);
                window.draw(creatorButtonText);
            }
            // Rysuj napis "Tw�rca Gry: Jan Berdzik" zawsze, niezale�nie od showButtons
            if (showCreatorMessage) {
                window.draw(creatorMessageText);
            }
        }
        else if (gameState == GameState::Playing) {
            window.draw(tloSprite);
            window.draw(xwingSprite);
            window.draw(tieSprite);
            window.draw(gameTimeText);
            window.draw(punktyText);
        }
    for (const Blaster& blaster : blasters) {
    window.draw(blaster.sprite);
}
    if (koniecGry) {
        window.draw(koniecGryText); // Rysuj napis "KONIEC GRY"
    }
    else {
        // Logika rysowania element�w gry, gdy gra nie jest zako�czona
        if (gameState == GameState::Menu) {
            // Rysowanie element�w menu...
        }
        else if (gameState == GameState::Playing) {
            // Rysowanie element�w rozgrywki...
            if (bombaVisible) {
                window.draw(bombaSprite); // Rysuj bomb�, je�li jest widoczna
            }
        }
        if (bomba2Visible) {
            window.draw(bomba2Sprite); // Rysuj drug� bomb�, je�li jest widoczna
        }
        if (gwiazdaActive) {
            window.draw(gwiazdaSprite);
        }

    }

        window.display();
    }
    return 0;
}