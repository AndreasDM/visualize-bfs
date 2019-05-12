#include "Node.h"

node::node(int w, int h) noexcept
  : shape{ { static_cast<float>(w), static_cast<float>(h) } }
{
  shape.setFillColor(sf::Color::Black);
}

void node::setPosition(int x, int y) noexcept
{
  shape.setPosition(x, y);
}
