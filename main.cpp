#include <SFML/Graphics.hpp>
#include <cstdio>
#include <deque>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

#define WIDTH 50
#define HEIGHT 50
#define BW 10
#define BH 10

struct node {
  sf::RectangleShape shape;
  std::vector<node*> adj{};
  bool               marked   = false;
  bool               obstacle = false;
  node*              parent   = nullptr;

  node(int w, int h)
    : shape{ { static_cast<float>(w), static_cast<float>(h) } }
  {
    shape.setFillColor(sf::Color::Black);
  }

  void setPosition(int x, int y)
  {
    shape.setPosition(x, y);
  }
};

auto findNeighbors(std::vector<node>& nodes)
{
  for (int i{}; i != nodes.size(); ++i) {
    int x{ i % BW }, y{ i / BH };

    nodes[i].adj.clear();

    if (!(x - 1 < 0))
      if (!nodes[i - 1].obstacle)
        nodes[i].adj.push_back(&nodes[i - 1]);
    if (!(x + 1 >= BW))
      if (!nodes[i + 1].obstacle)
        nodes[i].adj.push_back(&nodes[i + 1]);
    if (!(((float(i) + BW) / BH) > BH))
      if (!nodes[i + BW].obstacle)
        nodes[i].adj.push_back(&nodes[i + BW]);
    if (!(((float(i) - BW) / BH) < 0))
      if (!nodes[i-BW].obstacle)
        nodes[i].adj.push_back(&nodes[i - BW]);
  }
}

void markObstacle(sf::RenderWindow& window, node* start, node* goal, std::vector<node> & nodes)
{
  for (auto &i : nodes) {
    if (i.shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
      i.obstacle = true;
      if (i.obstacle) i.shape.setFillColor(sf::Color::Blue);
    }
  }
}

auto search(node* start, node* goal, std::vector<node> & nodes) {
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

void calculatePath(std::deque<node*>& stack, node* goal)
{
  for (node* ptr = goal; ptr != nullptr; ptr = ptr->parent)
    stack.push_front(ptr);

  while (!stack.empty()) {
    node* n = stack.front();
    n->shape.setFillColor(sf::Color::Green);
    stack.pop_front();
  }
}

void pickStart(sf::RenderWindow& window, node*& start, std::vector<node>& nodes)
{
  for (int i{}; i != nodes.size(); ++i) {
    if (nodes[i].shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
      start = &nodes[i];
      nodes[i].shape.setFillColor(sf::Color::Red);
      return;
    }
  }
}

void pickEnd(sf::RenderWindow& window, node*& goal, std::vector<node>& nodes)
{
  for (int i{}; i != nodes.size(); ++i) {
    if (nodes[i].shape.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
      goal = &nodes[i];
      nodes[i].shape.setFillColor(sf::Color::Red);
      return;
    }
  }
}

int main()
{
  sf::RenderWindow window{ { 800, 800 }, "Game" };
  window.setPosition({0,0});

  std::vector<node> nodes;

  for (int j{}, y{ HEIGHT }; j != BH; ++j, y += HEIGHT + 2)
    for (int i{}, x{ WIDTH }; i != BW; ++i, x += WIDTH + 2) {
      node model{WIDTH,HEIGHT};
      model.setPosition(x, y);
      nodes.push_back(model);
    }

  // findNeighbors(nodes);

  // for (auto * i : nodes[0].adj)
  //   std::printf("x: %f y: %f\n", i->shape.getPosition().x, i->shape.getPosition().y);

  // bfs
  // std::deque<node*> queue;
  // queue.push_back(&nodes[0]);
  // node* goal = &nodes[nodes.size()/2 + 5];
  //
  // while (!queue.empty()) {
  //   node* v = queue.front();
  //   v->marked = true;
  //   queue.pop_front();
  //   if (v == goal) break;
  //
  //   for (auto & w : v->adj) {
  //     if (!w->marked) {
  //       w->marked = true;
  //       w->parent = v;
  //       queue.push_back(w);
  //     }
  //   }
  // }


  // lets find the path to goal
  node* goal = nullptr;
  node* start = nullptr;
  std::deque<node*> stack;
  // for (node* ptr = goal; ptr != nullptr; ptr = ptr->parent)
  //   stack.push_front(ptr);

  auto click        = false;
  auto fClicked     = false;
  auto showPath     = false;
  auto endClicked   = false;
  auto startClicked = false;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.key.code == sf::Keyboard::Escape) window.close();
      if (event.type == sf::Event::MouseButtonPressed) click = true;
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F) { fClicked = true; showPath = true; }
        if (event.key.code == sf::Keyboard::S) startClicked = true;
        if (event.key.code == sf::Keyboard::E) endClicked = true;
      }
      // if (event.type == sf::Event::KeyReleased)
      //   if (event.key.code == sf::Keyboard::F) fClicked = false;
    }

    window.clear(sf::Color::White);
    for (auto& n : nodes)
      window.draw(n.shape);

    // click on squares to make them obstacles
    // for (auto & i : nodes) {
    // }

    if (startClicked) {
      pickStart(window, start, nodes);
      startClicked = false;
    }

    if (endClicked) {
      pickEnd(window, goal, nodes);
      endClicked = false;
    }

    if (click) {
      markObstacle(window, start, goal, nodes);
      click = false;
    }

    if (fClicked) {
      findNeighbors(nodes);
      search(start, goal, nodes);
      fClicked = false;
      for (auto & i : nodes) {
        i.marked = false;
        if (i.shape.getFillColor() == sf::Color::Green)
          i.shape.setFillColor(sf::Color::Black);
      }
    }

    if (showPath) {
      calculatePath(stack, goal);
      showPath = false;

      // for (auto * i : nodes[1].adj)
      //   std::printf("x: %f y: %f\n", i->shape.getPosition().x, i->shape.getPosition().y);

      // if (!stack.empty()) {
      //   node* n = stack.front();
      //   n->shape.setFillColor(sf::Color::Green);
      //   stack.pop_front();
      // }
    }

    window.display();
    // std::this_thread::sleep_for(500ms);
  }
}
