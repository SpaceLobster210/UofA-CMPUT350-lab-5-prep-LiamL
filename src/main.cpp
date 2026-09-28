#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <cmath>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;
const float FRAMES_PER_ANIM = 100;
const int GRAPH_BORDER = 50;

float curFrame = 0;


// global tween function
std::function<float(float, float, float)> tween = [](float a, float b, float t) {
    return (1 - t) * a + t * b;
};

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        //  (Q2)
        //  implement key presses (1-9) that replace the tween function
        //  with different alternate tween functions.
        //  Credit to https://easings.net/#
        // ====== ====== ======
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) { // Normal lerp
            tween = [](float a, float b, float t) {
                return (1 - t) * a + t * b;
            };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) { // t^2 ease in
            tween = [](float a, float b, float t) {
                float mix = t*t;
                return (1 - mix) * a + mix * b;
            };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) { // t^3 ease in
            tween = [](float a, float b, float t) {
                float mix = t*t*t;
                return (1 - mix) * a + mix * b;
            };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4)) { // Sin ease in
            tween = [](float a, float b, float t) {
                float mix = 1 - std::cos((t * M_PI) / 2);
                return (1 - mix) * a + mix * b;
            };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5)) { // Sin ease out
            tween = [](float a, float b, float t) {
                float mix = std::sin((t * M_PI) / 2);
                return (1 - mix) * a + mix * b;
            };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num6)) { // Sin ease in/out
            tween = [](float a, float b, float t) {
                float mix = (1 - std::cos(t * M_PI)) / 2;
                return (1 - mix) * a + mix * b;
            };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num7)) { // t^2 ease in/out
            tween = [](float a, float b, float t) {
                float mix = ((1 - t) * (t*t)) + (t * (1 - ((1-t)*(1-t))));
                return (1 - mix) * a + mix * b;
            };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num8)) { // t^3 ease in/out
            tween = [](float a, float b, float t) {
                float mix = ((1 - t) * (t*t*t)) + (t * (1 - ((1-t)*(1-t)*(1-t))));
                return (1 - mix) * a + mix * b;
            };
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num9)) { // ease in back
            tween = [](float a, float b, float t) {
                float mix = 2.70158f * t*t*t - 1.70158f * t*t;
                return (1 - mix) * a + mix * b;
            };
        }
    }
}

void render(sf::RenderWindow& window) {
    // Clear with blue background (sky)
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // (Q1) Draw circle that moves between
    // the left/right half of the screen.
    // Movement should be governed by the tween function.
    // ====== ====== ======
    sf::CircleShape circleMain;
    circleMain.setPosition({tween(0, WINDOW_WIDTH, curFrame / FRAMES_PER_ANIM), WINDOW_HEIGHT / 3});
    circleMain.setFillColor(sf::Color(0, 255, 255));
    window.draw(circleMain);


    // ====== ====== ======
    // (Q3) Draw tween function graph with a dot
    // on the current portion of the curve
    // ====== ====== ======

    // Draw graph bounds
    float graphWidth = WINDOW_WIDTH - 100;
    float graphHeight = (WINDOW_HEIGHT / 3) - GRAPH_BORDER;

    sf::RectangleShape boundingLine1({graphWidth, 2});
    sf::RectangleShape boundingLine2({2, graphHeight});
    boundingLine1.setPosition(sf::Vector2f{ WINDOW_WIDTH / 2.0f, static_cast<float>(WINDOW_HEIGHT) - GRAPH_BORDER });
    boundingLine2.setPosition(sf::Vector2f{ static_cast<float>(GRAPH_BORDER), WINDOW_HEIGHT - ((WINDOW_HEIGHT / 6.0f) + (GRAPH_BORDER / 2.0f)) });
    window.draw(boundingLine1);
    window.draw(boundingLine2);

    // Draw tween function line
    float lineUnit = FRAMES_PER_ANIM / 2 * (graphWidth); // The length of a line segment in the x axis
    float prevVal = tween(0, graphHeight, 0);

    sf::CircleShape graphDot;
    if (curFrame == 0) { // Cover edge case of dot where frame is 0
        graphDot.setPosition(sf::Vector2f{ static_cast<float>(GRAPH_BORDER), (WINDOW_HEIGHT - (GRAPH_BORDER) + prevVal) });
    }

    for (int fr = 1; fr <= FRAMES_PER_ANIM; fr++) {
        // Get info about the line segment
        float curVal = tween(0, graphHeight, fr / FRAMES_PER_ANIM);
        float dist = std::hypot(lineUnit, std::abs(curVal - prevVal));
        float angle = atan2(curVal - prevVal, lineUnit);

        // Draw the line segment
        sf::RectangleShape lineSegment({dist, 1});
        lineSegment.setPosition(sf::Vector2f{ (GRAPH_BORDER + (((2 * fr) - 1) * (lineUnit / 2))), (WINDOW_HEIGHT - (GRAPH_BORDER + prevVal + ((curVal - prevVal) / 2))) });
        lineSegment.setRotation(sf::degrees(angle * 180 / M_PI));
        lineSegment.setFillColor(sf::Color(0, 255, 255));
        window.draw(lineSegment);

        // mark position of dot on line
        if (curFrame == fr) {
            graphDot.setPosition(sf::Vector2f{ (GRAPH_BORDER + fr * lineUnit), (WINDOW_HEIGHT - (GRAPH_BORDER) + curVal) });
        }

        prevVal = curVal;
    }
    // Draw dot
    graphDot.setFillColor(sf::Color(255, 216, 0));
    window.draw(graphDot);
    
    curFrame += 1;
    if (curFrame > FRAMES_PER_ANIM) {
        curFrame = 0;
    }

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
