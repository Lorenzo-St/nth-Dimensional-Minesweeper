#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <iomanip>

class Renderer;
class board
{
public:
  board(unsigned int x, unsigned int y, unsigned int z, unsigned int count, Renderer& r);
  board(unsigned int x, unsigned int y, unsigned count, Renderer& r) : board(x, y, 1, count, r) {};
  board(unsigned int x, unsigned count, Renderer& r) : board(x, 1, 1, count, r) {};
  board& operator=(board const& b);

  void generate();
  void populate();
  void Clear();

  bool Won() { return hasWon; }

  void UnFlag(size_t x, size_t y) { UnFlag(x, y, currentSlice); }
  void UnFlag(size_t x, size_t y, size_t z);

  void Flag(size_t x, size_t y) { Flag(x, y, currentSlice); }
  void Flag(size_t x, size_t y, size_t z);
  
  bool Check(size_t x, size_t y) { return Check(x, y, currentSlice); }
  bool Check(size_t x, size_t y, size_t z);
  
  void Hide() { hidden = true; }
  void Reveal() { hidden = false; }
  
  unsigned int QuerryDepth() { return currentSlice; }
  void SetSlice(size_t z);
  
  friend std::ostream& operator<<(std::ostream& os, board& b);

private:
  std::vector<std::vector<std::vector<unsigned int>>> bombs;
  std::vector<std::vector<std::vector<unsigned int>>> Revealedbombs;

  unsigned bombCount;
  unsigned int width;
  unsigned int height;
  unsigned int depth;
  unsigned int currentSlice = 0;
  size_t power;
  bool hidden;
  bool hasWon;
  Renderer& renderer;
};