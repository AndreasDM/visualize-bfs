#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <deque>

struct node {
  sf::RectangleShape shape;
  std::vector<node*> adj{};
  bool               marked   = false;
  bool               obstacle = false;
  node*              parent   = nullptr;

  explicit node(int w, int h) noexcept
    : shape{ { static_cast<float>(w), static_cast<float>(h) } }
  {
    shape.setFillColor(sf::Color::Black);
  }

  inline void setPosition(int x, int y) noexcept
  {
    shape.setPosition(x, y);
  }
};

class Game {
private:
  sf::RenderWindow  window;
  int               bw;
  int               bh;
  int               cw;
  int               ch;
  std::vector<node> nodes;
  std::deque<node*> stack;

private:
  node* start        = nullptr;
  node* goal         = nullptr;
  bool  startClicked = false;
  bool  endClicked   = false;
  bool  showPath     = false;
  bool  fClicked     = false;
  bool  click        = false;

private:
  void update()        noexcept;
  void render()        noexcept;
  void display()       noexcept;
  void handleEvents()  noexcept;
  void clear()         noexcept;
  void pickStart()     noexcept;
  void pickEnd()       noexcept;
  void markObstacle()  noexcept;
  void findNeighbors() noexcept;
  void search()        noexcept;
  void calculatePath() noexcept;

public:
  explicit Game(int w, int h, int bw, int bh, int cubeW, int cubeH) noexcept;

public:
  void run() noexcept;
};

#endif
