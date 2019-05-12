#include "Game.h"
#include "Node.h"

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

    // prepare for next run
    // TODO: improve this
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

inline void Game::clear() noexcept {
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
  for (int i{}; i != nodes.size(); ++i) {
    if (nodes[i].shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
      start = &nodes[i];
      nodes[i].shape.setFillColor(sf::Color::Red);
      return;
    }
  }
}

inline void Game::pickEnd() noexcept
{
  for (int i{}; i != nodes.size(); ++i) {
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

inline void Game::findNeighbors() noexcept
{
  for (int i{}; i != nodes.size(); ++i) {
    int x{ i % bw }, y{ i / bh };

    nodes[i].adj.clear();

    if (!(x - 1 < 0))
      if (!nodes[i - 1].obstacle) nodes[i].adj.push_back(&nodes[i - 1]);
    if (!(x + 1 >= bw))
      if (!nodes[i + 1].obstacle) nodes[i].adj.push_back(&nodes[i + 1]);
    if (!(((float(i) + bw) / bh) > bh))
      if (!nodes[i + bw].obstacle) nodes[i].adj.push_back(&nodes[i + bw]);
    if (!(((float(i) - bw) / bh) < 0))
      if (!nodes[i - bw].obstacle) nodes[i].adj.push_back(&nodes[i - bw]);
  }
}

inline void Game::search() noexcept
{
  std::deque<node*> queue;
  queue.push_back(start);

  while (!queue.empty()) {
    node* v = queue.front();
    v->marked = true;
    queue.pop_front();
    if (v == goal) break;

    for (auto & w : v->adj) {
      if (!w->marked) {
        w->marked = true;
        w->parent = v;
        queue.push_back(w);
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
