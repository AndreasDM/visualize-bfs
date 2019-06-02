#include "Game.h"
#include "Node.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

Game::Game(int w, int h, int bw, int bh, int cubeW, int cubeH) noexcept
  : window{ sf::VideoMode(w, h), "Game" }
  , bw{bw}
  , bh{bh}
  , cw{cubeW}
  , ch{cubeH}
{
  window.setPosition({ 0, 0 });

  for (int j{}, y{ ch }; j != bh; ++j, y += ch + 2)
    for (int i{}, x{ cw }; i != bw; ++i, x += cw + 2) {
      node model{ cw, ch };
      model.setPosition(x, y);
      nodes.push_back(model);
    }
}

inline void Game::update() noexcept
{
  if (startClicked) { pickStart(); startClicked = false; }
  if (endClicked)   { pickEnd(); endClicked     = false; }
  if (click)        { markObstacle(); click     = false; }

  if (fClicked) {
    findNeighbors();
    search();
    fClicked = false;

    if (!goal->marked) showPath = false;

    // perhaps unessessary to
    // do this here
    for (auto & i : nodes) {
      i.marked = false;
      if (i.shape.getFillColor() == sf::Color::Green)
        i.shape.setFillColor(sf::Color::Black);
    }
  }

  if (showPath) {
    calculatePath();
    showPath = false;
  }
}

inline void Game::render() noexcept
{
  for (auto const& n : nodes)
    window.draw(n.shape);
}

inline void Game::display() noexcept
{
  window.display();
}

inline void Game::handleEvents() noexcept
{
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.key.code == sf::Keyboard::Escape) window.close();
    if (event.type == sf::Event::MouseButtonPressed) click = true;
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::F) {
        fClicked = true;
        showPath = true;
      }
      if (event.key.code == sf::Keyboard::S) startClicked = true;
      if (event.key.code == sf::Keyboard::E) endClicked   = true;
    }
  }
}

inline void Game::clear() noexcept
{
  window.clear(sf::Color::White);
}

void Game::run() noexcept
{
  while (window.isOpen()) {
    handleEvents();
    clear();
    update();
    render();
    display();
  }
}

inline void Game::pickStart() noexcept
{
  for (size_t i{}; i != nodes.size(); ++i) {
    if (nodes[i].shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
      start = &nodes[i];
      nodes[i].shape.setFillColor(sf::Color::Red);
      return;
    }
  }
}

inline void Game::pickEnd() noexcept
{
  for (size_t i{}; i != nodes.size(); ++i) {
    if (nodes[i].shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
      goal = &nodes[i];
      nodes[i].shape.setFillColor(sf::Color::Red);
      return;
    }
  }
}

inline void Game::markObstacle() noexcept
{
  for (auto &i : nodes) {
    if (i.shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
      i.obstacle = true;
      if (i.obstacle) i.shape.setFillColor(sf::Color::Blue);
    }
  }
}

inline void Game::addIfNotObstacle(int const& current_index, int const& neighbor_index) noexcept
{
  if (!nodes[neighbor_index].obstacle)
    nodes[current_index].adj.push_back(&nodes[neighbor_index]);
}

inline void Game::findNeighbors() noexcept
{
  for (int i{}; i != nodes.size(); ++i) {
    int x{ i % bw }, y { i / bh };

    nodes[i].adj.clear();

    auto const left           = x      -  1;
    auto const right          = x      +  1;
    auto const top            = y      -  1;
    auto const bottom         = y      +  1;
    auto const outside_bottom = bottom >= bh;
    auto const outside_top    = top    <  0;
    auto const outside_left   = left   <  0;
    auto const outside_right  = right  >= bw;

    if (!outside_bottom)                   addIfNotObstacle(i, i + bw);
    if (!outside_top)                      addIfNotObstacle(i, i - bw);
    if (!outside_left)                     addIfNotObstacle(i, i - 1);
    if (!outside_right)                    addIfNotObstacle(i, i + 1);
    if (!outside_left  && !outside_top)    addIfNotObstacle(i, i - bw - 1);
    if (!outside_left  && !outside_bottom) addIfNotObstacle(i, i + bw - 1);
    if (!outside_right && !outside_top)    addIfNotObstacle(i, i - bw + 1);
    if (!outside_right && !outside_bottom) addIfNotObstacle(i, i + bw + 1);
  }
}

inline void Game::search() noexcept
{
  std::deque<node*> queue;
  queue.push_back(start);

  while (!queue.empty()) {
    node* v   = queue.front();
    v->marked = true;
    queue.pop_front();
    if (v == goal) break;

    for (auto & w : v->adj) {
      if (!w->marked) {
        w->marked = true;
        w->parent = v;
        queue.push_back(w);
        w->shape.setFillColor(sf::Color::Magenta);
        std::this_thread::sleep_for(20ms);
        clear();
        render();
        display();
      }
    }
  }
}

inline void Game::calculatePath() noexcept
{
  for (node* ptr = goal; ptr != nullptr; ptr = ptr->parent)
    stack.push_front(ptr);

  while (!stack.empty()) {
    node* n = stack.front();
    n->shape.setFillColor(sf::Color::Green);
    stack.pop_front();
  }
}
