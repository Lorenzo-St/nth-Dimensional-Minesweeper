/*********************************************************************
 * @file   board.cpp
 * @brief
 *
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   November 2023
 *********************************************************************/
#include "board.h"
#include "consoleColors.hpp"
#include "RenderBackend.h"

board::board(unsigned int x, unsigned int y, unsigned int z, unsigned int count, Renderer& r)
  : width(x)
  , height(y)
  , depth(z)
  , bombCount(count)
  , hidden(false)
  , hasWon(false)
  , power(0)
  , renderer(r)
{
  if (x * y * z < count)
    throw std::invalid_argument("Too many bombs");
  bombs.resize(depth);
  Revealedbombs.resize(depth);
  for (auto& y : bombs)
  {
    y.resize(height);
    for (auto& x : y)
      x.resize(width);
  }
  for (auto& y : Revealedbombs)
  {
    y.resize(height);
    for (auto& x : y)
      x.resize(width);
  }
  std::string ex = std::to_string(x);
  std::string why = std::to_string(y);
  if (ex.length() > why.length())
    power = ex.length();
  else
    power = why.length();
}

board& board::operator=(board const& b)
{
  width = b.width;
  height = b.height;
  depth = b.depth;
  bombCount = b.bombCount;
  hidden = b.hidden;
  renderer = b.renderer;
  hasWon = b.hasWon;
  power = b.power;
  return *this;
}

void board::generate()
{
  std::random_device d;
  std::mt19937 generator = std::mt19937(d());
  std::uniform_int_distribution<> disX(0, width - 1);
  std::uniform_int_distribution<> disY(0, height - 1);
  std::uniform_int_distribution<> disZ(0, depth - 1);

  for (unsigned int i = 0; i < bombCount; ++i)
  {
    int y = disY(generator);
    int x = disX(generator);
    int z = disZ(generator);
    if (bombs[z][y][x] == INT_MAX)
    {
      --i;
      continue;
    }
    bombs[z][y][x] = INT_MAX;


  }
  populate();
}

void board::populate()
{
  for (unsigned int z = 0; z < depth; ++z)
    for (unsigned int y = 0; y < height; ++y)
      for (unsigned int x = 0; x < width; ++x)
        if (bombs[z][y][x] == INT_MAX)
        {
          int slice = static_cast<int>(z + 1);
          int top = static_cast<int>(y + 1);
          int right = static_cast<int>(x + 1);
          for (int k = static_cast<int>(z) - 1; k <= slice; ++k)
            for (int j = static_cast<int>(y) - 1; j <= top; ++j)
              for (int i = static_cast<int>(x) - 1; i <= right; ++i)
              {
                if (i == x && j == y && k == z)
                  continue;
                if (j < 0 || static_cast<unsigned int>(j) >= height)
                  continue;
                if (i < 0 || static_cast<unsigned int>(i) >= width)
                  continue;
                if (k < 0 || static_cast<unsigned int>(k) >= depth)
                  continue;
                if (bombs[k][j][i] == INT_MAX)
                  continue;
                if (i == 0)
                  i = i;
                ++bombs[k][j][i];
              }
        }


}

void board::Clear()
{
  for (auto& z : bombs)
    for (auto& y : z)
      for (auto& x : y)
        x = 0;
  for (auto& z : Revealedbombs)
    for (auto& y : z)
      for (auto& x : y)
        x = 0;
}

void board::UnFlag(size_t x, size_t y, size_t z)
{
  if (x < 0 || x >= width)
    return;
  if (y < 0 || y >= height)
    return;
  if (z < 0 || z >= depth)
    return;
  unsigned int& pos = Revealedbombs[z][y][x];
  if (pos == 2)
    pos = 0;

}

void board::Flag(size_t x, size_t y, size_t z)
{
  if (x < 0 || x >= width)
    return;
  if (y < 0 || y >= height)
    return;
  if (z < 0 || z >= depth)
    return;
  unsigned int& pos = Revealedbombs[z][y][x];
  if (pos == 0)
    pos = 2;


  int flagged = 0;
  for (unsigned int z = 0; z < depth; ++z)
    for (unsigned int y = 0; y < height; ++y)
      for (unsigned int x = 0; x < width; ++x)
        if (bombs[z][y][x] == INT_MAX)
          if (Revealedbombs[z][y][x] == 2)
            ++flagged;

  if (flagged == bombCount)
    hasWon = true;

}

bool board::Check(size_t x, size_t y, size_t z)
{
  if (x < 0 || x >= width)
    return false;
  if (y < 0 || y >= height)
    return false;
  if (z < 0 || z >= depth)
    return false;
  if (bombs[z][y][x] == INT_MAX)
  {
    bombs[z][y][x] = INT_MAX / 2;
    return true;
  }

  Revealedbombs[z][y][x] = 1;
  bool done = 0;
  while (!done)
  {
    done = true;
    long long  tDepth = static_cast<long long>(depth);

    long long  tHeight = static_cast<long long>(height);
    long long  tWidth = static_cast<long long>(width);

    for (long long z = 0; z < tDepth; ++z)
      for (long long y = 0; y < tHeight; ++y)
        for (long long x = 0; x < tWidth; ++x)
        {
          if (bombs[z][y][x] != 0)
            continue;
          if (Revealedbombs[z][y][x] == 0)
            continue;

          for (long long k = z - 1; k < z + 2; ++k)
            for (long long j = y - 1; j < y + 2; ++j)
              for (long long i = x - 1; i < x + 2; ++i)
              {
                if (i == x && j == y)
                  continue;
                if (j < 0 || static_cast<unsigned int>(j) >= height)
                  continue;
                if (i < 0 || static_cast<unsigned int>(i) >= width)
                  continue;
                if (k < 0 || static_cast<unsigned int>(k) >= depth)
                  continue;
                if (bombs[k][j][i] == INT_MAX)
                  continue;
                if (Revealedbombs[k][j][i] == 0)
                  Revealedbombs[k][j][i] = 1;
              }
        }
  }
  return false;
}

void board::SetSlice(size_t z)
{
  if (z > depth)
    currentSlice = static_cast<unsigned int>(depth);
  currentSlice = static_cast<unsigned int>(z);
}
const char* sendNumberColor(int number)
{
  switch (number)
  {
  case 0:
    return WHITE;
  case 1:
    return BRIGHT_BLACK;
  case 2:
    return BRIGHT_CYAN;
  case 3:
    return BRIGHT_BLUE;
  case 4:
    return BRIGHT_GREEN;
  case 5:
    return BRIGHT_MAGENTA;
  case 6:
    return MAGENTA;
  case 7:
    return GREEN;
  case 8:
    return BLUE;
  default:
    return WHITE;
  }
}

std::ostream& operator<<(std::ostream& os, board& b)
{
  //for (size_t i = 0; i < b.power; ++i)
  //  os << " ";
  //for (unsigned int i = 0; i < b.width; ++i)
  //  os << std::setw(b.power + 2) << i;
  //os << std::endl;
  //for (size_t i = 0; i < b.power; ++i)
  //  os << " ";
  //os << "+";
  //for (unsigned int i = 0; i < b.width; ++i)
  //  for (int j = 0; j < b.power + 2; ++j)
  //    os << "-";
  //os << std::endl;
  for (unsigned int z = b.currentSlice; z < b.depth; ++z)
  {

    for (unsigned int y = 0; y < b.height; ++y)
    {
      //os << std::setw(b.power) << y << "|";
      for (unsigned int x = 0; x < b.width; ++x)
      {
        unsigned int bm = b.bombs[z][y][x];
        unsigned int revealed = b.Revealedbombs[z][y][x];
        glm::vec3 local = { x * 2, y * 2, z};
        local += glm::vec3( -10, -5,  10 );
        b.renderer.SetColor({ .5, .5, .5, 1 });
        b.renderer.SetMatrix(local, { .7, .7, 1 });
        b.renderer.DrawRect();
        b.renderer.SetColor({ .8, .8, .8, 1 });
        b.renderer.SetMatrix(local, {.5, .5, 1});
        b.renderer.DrawRect();

          
        /*if (b.hidden == false)
        {

          if (revealed != 0)
            os
            << std::setw(1) << RESET"["
            << RESET << ((revealed == 1) ? sendNumberColor(bm) : BRIGHT_RED) << std::setw(b.power) << ((revealed == 1) ? std::to_string(bm) : "F")
            << std::setw(1) << RESET"]";
          else
            os
            << std::setw(1) << RESET"["
            << ((bm == INT_MAX) ? YELLOW : (bm == INT_MAX / 2) ? RED : sendNumberColor(bm)) << std::setw(b.power) << ((bm == INT_MAX) ? "b" : (bm == INT_MAX / 2) ? "x" : std::to_string(bm))
            << std::setw(1) << RESET"]";
          os << RESET;

        }
        else if (revealed != 0)
        {
          os
            << std::setw(1) << RESET"["
            << RESET << ((revealed == 1) ? sendNumberColor(bm) : BRIGHT_RED) << std::setw(b.power) << ((revealed == 1) ? std::to_string(bm) : "F")
            << std::setw(1) << RESET"]";
          os << RESET;
        }
        else
        {
          os
            << std::setw(1) << RESET"[";
          for (int i = 0; i < b.power; ++i)
            os << " ";
          os << std::setw(1) << RESET"]";
          os << RESET;

        }
        os << RESET;*/
      }
      //os << "\n";
    }
  }

  return os;
}