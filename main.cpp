/*
g++ -o main.exe main.cpp -Isfml-ui/sfml/include -Lsfml-ui/sfml/lib "-Wl,--start-group" -lfreetype -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-main -lgdi32 -lopengl32 -lwinmm "-Wl,--end-group"
*/

#include "sfml-ui/main.hpp"
#include <stdlib.h>
#include <time.h>
#include <random>

const sf::Vector2u matcherSize = {8u, 8u};
const std::size_t letterMax = 182303;
const std::vector<std::pair<char, std::size_t>> letterFreq{
    {'e', 21912},
    {'t', 16587},
    {'a', 14810},
    {'o', 14003},
    {'i', 13318},
    {'n', 12666},
    {'s', 11450},
    {'r', 10977},
    {'h', 10795},
    {'d', 7874},
    {'l', 7253},
    {'u', 5246},
    {'c', 4943},
    {'m', 4761},
    {'f', 4200},
    {'y', 3853},
    {'w', 3819},
    {'g', 3693},
    {'p', 3316},
    {'v', 2715},
    {'b', 2019},
    {'k', 1257},
    {'x', 315},
    {'q', 205},
    {'j', 188},
    {'z', 128}
};

int main(int argCount, char* args[]) {
    std::mt19937_64 myRand;
    myRand.seed(std::time(NULL));
    fstream words;
    words.open("./data/words_alpha.txt");
    std::vector<std::string> wordList, wordsUsed;
    std::size_t score = 0;
    init(argCount, args, [&](sf::RenderWindow& window, sf::Font& font, sf::Cursor& normalCursor, sf::Cursor& textboxCursor, sf::Clock& clock, UI::Focus*& focus, UI::Hold*& hold, UI::Hover*& hover, UI::Scene*& currentScene) -> std::function<void(sf::Time& time)> {
        std::string temp;
        while (words >> temp) wordList.push_back(temp);
        UI::Scene* mainScene = new UI::Scene();
        UI::PopBackground* background = new UI::PopBackground({0.f, 0.f}, {UI::winW, UI::winH});
        sf::Text
        *   textDisplay = new sf::Text("Your string will appear here!", font, 16),
        *   scoreDisplay = new sf::Text("Score: 0", font, 16);
        UI::Table<UI::Button<>>* matcher = new UI::Table<UI::Button<>>();
        UI::Button<>* submit = new UI::Button<>("Submit", font, {8.f, UI::winH - 24.f});
        mainScene->addElem(background);
        mainScene->addElem(matcher);
        mainScene->addElem(textDisplay);
        mainScene->addElem(submit);
        mainScene->addElem(scoreDisplay);
        matcher->setPosition({8.f, 8.f});
        matcher->setSize({UI::winW - 16.f, UI::winH - 16.f});

        background->invert();

        UI::Table<UI::Button<>>::ArrayType initArray;
        for (std::size_t i = 0u; i < matcherSize.x; i++) {
            initArray.push_back({});
            for (std::size_t j = 0u; j < matcherSize.y; j++) {
                std::size_t ticket = myRand() % letterMax, current = 0u;
                char choice = 'e';
                for (std::pair<char, std::size_t> letter : letterFreq) {
                    current += letter.second;
                    if (ticket < current) {
                        choice = letter.first;
                        break;
                    }
                }
                UI::Button<> newButton{(std::string)("") + choice, font};
                newButton.setHoverAction([=](UI::Button<>* button) -> void {
                    if (button->getFillColor().toInteger() != sf::Color::Cyan.toInteger() && button->getFillColor().toInteger() != sf::Color::Green.toInteger()) button->setFillColor(UI::sWhite, true);
                });
                newButton.setUnHoverAction([=](UI::Button<>* button) -> void {
                    if (button->getFillColor().toInteger() != sf::Color::Cyan.toInteger() && button->getFillColor().toInteger() != sf::Color::Green.toInteger()) button->revert();
                });
                newButton.setClickAction([=](UI::Button<>* button) -> void {
                    button->defaultClickEffect();
                    if (button->getFillColor().toInteger() == sf::Color::Cyan.toInteger() || button->getFillColor().toInteger() == sf::Color::Green.toInteger()) {
                        button->setFillColor(sf::Color::Red, true);
                        return;
                    }
                    UI::Table<UI::Button<>>::ArrayType* elements = matcher->getElements();
                    UI::Button<>* lastGreen;
                    std::size_t beginX = i, beginY = j;
                    if (beginX > 0u) beginX--;
                    if (beginY > 0u) beginY--;
                    bool valid = false;
                    std::string word = textDisplay->getString();
                    if (word == "Your string will appear here!" || word == "Correct!" || word == "Wrong!") valid = true, word = "";
                    else for (std::size_t t_i = beginX; t_i <= i + 1 && t_i < elements->size(); t_i++) {
                        if (valid) break;
                        else for (std::size_t t_j = beginY; t_j <= j + 1 && t_j < elements->at(0).size(); t_j++) {
                            if (elements->at(t_i).at(t_j).getFillColor().toInteger() == sf::Color::Green.toInteger()) {
                                lastGreen = &elements->at(t_i).at(t_j);
                                valid = true;
                                break;
                            }
                        }
                    }
                    if (valid) {
                        button->setFillColor(sf::Color::Green);
                        if (word.size()) lastGreen->setFillColor(sf::Color::Cyan);
                        textDisplay->setString(word + button->getString());
                    } else button->setFillColor(sf::Color::Red, true);
                });
                newButton.setUnClickAction([=](UI::Button<>* button) -> void {
                    button->revert();
                });
                initArray.back().push_back(std::move(newButton));
            }
        }
        matcher->setAllElements(initArray);
        UI::Table<UI::Button<>>::ArrayType* elementReference = matcher->getElements();
        for (std::size_t i = 0u; i < elementReference->size(); i++) for (std::size_t j = 0u; j < elementReference->at(i).size(); j++) mainScene->addElem(&elementReference->at(i).at(j));
        matcher->setSize({UI::winW - 16.f, UI::winH - 32.f});

        textDisplay->setPosition({submit->getSize().x + 16.f, UI::winH - 24.f});
        textDisplay->setFillColor(UI::sBlack);

        submit->setHoverAction([=](UI::Button<>* button) -> void {
            button->setFillColor(UI::sWhite, true);
        });
        submit->setUnHoverAction([=](UI::Button<>* button) -> void {
            button->revert();
        });
        submit->setClickAction([=, &wordList, &wordsUsed, &score](UI::Button<>* button) -> void {
            std::string word = textDisplay->getString();
            if (std::find(wordList.begin(), wordList.end(), word) != wordList.end() && std::find(wordsUsed.begin(), wordsUsed.end(), word) == wordsUsed.end()) {
                score += word.size();
                wordsUsed.push_back(word);
                textDisplay->setString("Correct!");
            } else {
                if (score >= word.size()) score -= word.size();
                else score = 0u;
                textDisplay->setString("Wrong!");
            }
            UI::Table<UI::Button<>>::ArrayType* elements = matcher->getElements();
            for (std::size_t i = 0u; i < elements->size(); i++) for (std::size_t j = 0u; j < elements->at(i).size(); j++) elements->at(i).at(j).setFillColor(UI::lGrey);
            scoreDisplay->setString("Score: " + to_string(score));
            scoreDisplay->setPosition({UI::winW - (scoreDisplay->getLocalBounds().width + 8.f), UI::winH - 24.f});
        });
        submit->setUnClickAction([=](UI::Button<>* button) -> void {
            button->revert();
        });

        scoreDisplay->setFillColor(UI::sBlack);
        scoreDisplay->setPosition({UI::winW - (scoreDisplay->getLocalBounds().width + 8.f), UI::winH - 24.f});

        currentScene = mainScene;

        return [&](const sf::Time& time) -> void {
            if (time.asSeconds() == time.asMicroseconds() / 1000000) myRand.seed(std::time(NULL));
        };
    }, "./sfml-ui");
    words.close();
    return 0;
}