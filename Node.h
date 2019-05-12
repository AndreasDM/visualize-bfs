#ifndef NODE_H
#define NODE_H

#include <SFML/Graphics.hpp>
#include <vector>

struct node {
  sf::RectangleShape shape;
  std::vector<node*> adj{};

  bool  marked   = false;
  bool  obstacle = false;
  node* parent   = nullptr;

  explicit node(int w, int h)    noexcept;
  void setPosition(int x, int y) noexcept;
};

#endif
