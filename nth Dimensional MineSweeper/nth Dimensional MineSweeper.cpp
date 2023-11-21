#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <iomanip>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define BRIGHT_BLACK   "\033[90m"      /* Bright Black */
#define BRIGHT_RED     "\033[91m"      /* Bright Red */
#define BRIGHT_GREEN   "\033[92m"      /* Bright Green */
#define BRIGHT_YELLOW  "\033[93m"      /* Bright Yellow */
#define BRIGHT_BLUE    "\033[94m"      /* Bright Blue */
#define BRIGHT_MAGENTA "\033[95m"      /* Bright Magenta */
#define BRIGHT_CYAN    "\033[96m"      /* Bright Cyan */
#define WHITE          "\033[97m"      /* White */

#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

class cexprString
{
public:
  constexpr cexprString(const char* s) : string(s)
  {
    const char* l = s;
    while (*l != 0)
    {
      ++len;
      ++l;
    }
  }
  constexpr int Length() { return len; };
private:
  const char* string;
  int len = 0;
};



class board
{
public:
  board(unsigned int x, unsigned int y, unsigned int z, unsigned int count)
    : width(x)
    , height(y)
    , depth(z)
    , bombCount(count)
    , hidden(false)
    , hasWon(false)
    , power(0)
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
  board(unsigned int x, unsigned count) : board(x, 1, 1, count) {};
  board(unsigned int x, unsigned int y, unsigned count) : board(x, y, 1, count) {};




  void generate()
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

  void populate()
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
  void Clear()
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
  bool Won() { return hasWon; }

  void UnFlag(size_t x, size_t y) { UnFlag(x, y, currentSlice); }
  void UnFlag(size_t x, size_t y, size_t z)
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
  void Flag(size_t x, size_t y) { Flag(x, y, currentSlice); }
  void Flag(size_t x, size_t y, size_t z)
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
  bool Check(size_t x, size_t y) { return Check(x, y, currentSlice); }
  bool Check(size_t x, size_t y, size_t z)
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
  void Hide() { hidden = true; }
  void Reveal() { hidden = false; }
  unsigned int QuerryDepth() { return currentSlice; }
  void SetSlice(size_t z)
  {
    if (z > depth)
      currentSlice = static_cast<unsigned int>(depth);
    currentSlice = static_cast<unsigned int>(z);
  }
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
};

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
  for (size_t i = 0; i < b.power; ++i)
    os << " ";
  for (unsigned int i = 0; i < b.width; ++i)
    os << std::setw(b.power + 2) << i;
  os << std::endl;
  for (size_t i = 0; i < b.power; ++i)
    os << " ";
  os << "+";
  for (unsigned int i = 0; i < b.width; ++i)
    for (int j = 0; j < b.power + 2; ++j)
      os << "-";
  os << std::endl;
  auto z = b.currentSlice;
  for (unsigned int y = 0; y < b.height; ++y)
  {
    os << std::setw(b.power) << y << "|";
    for (unsigned int x = 0; x < b.width; ++x)
    {
      unsigned int bm = b.bombs[z][y][x];
      unsigned int revealed = b.Revealedbombs[z][y][x];
      if (b.hidden == false)
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
      os << RESET;
    }
    os << "\n";
  }
  return os;
}

void EndState(board& b, bool& ended, bool& lost, int& flags, int& count)
{
  int x, y, z;
  std::cout << "Play Again?\n-------------------\ntype Y for yes\ntype N : no" << std::endl;
  char again = 0;
  while (again != 'y' && again != 'Y' && again != 'n' && again != 'N')
    std::cin >> again;
  if (again == 'y' || again == 'Y')
  {
    char Resize = 0;
    std::cout << "Would you like to resize the board?" << std::endl;
    std::cout << "-----------------------------------\ntype Y for yes\ntype N : no" << std::endl;
    while (Resize != 'y' && Resize != 'Y' && Resize != 'n' && Resize != 'N')
      std::cin >> Resize;
    if (Resize == 'y' || Resize == 'Y')
    {
      std::cout << "Please State you board size" << std::endl;
      std::cout << "Syntax: \"X Y Z #\"" << std::endl;
      std::cout << "-----------------------" << std::endl;
      std::cin >> x;
      std::cin >> y;
      std::cin >> z;
      std::cin >> count;
      x = (x < 1 ? 1 : x);
      y = (y < 1 ? 1 : y);
      z = (z < 1 ? 1 : z);
      b = board(x, y, z, count);
    }
    b.Clear();
    b.generate();
    b.Hide();
    flags = 0;
    lost = false;
  }
  else
  {
    ended = true;
  }
}



int main()
{
  std::cout << "Welcome\n Please State you board size" << std::endl;
  std::cout << "Syntax: \"X Y Z #\"" << std::endl;
  std::cout << "-----------------------" << std::endl;
  int x, y, z, count;
  std::cin >> x;
  std::cin >> y;
  std::cin >> z;
  std::cin >> count;
  x = (x < 1 ? 1 : x);
  y = (y < 1 ? 1 : y);
  z = (z < 1 ? 1 : z);
  board b = board(x, y, z, count);
  b.generate();
  b.Hide();
  bool ended = false;
  bool lost = false;
  int flags = 0;
  std::string lastCommand = "";
  while (!ended)
  {
    system("clear");
    std::cout << b << std::endl;
    std::cout << "Possible Commands:" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "* = parameter is optional" << std::endl;
    std::cout << "Check[x][y][z*]" << std::endl;
    std::cout << "Flag[x][y][z*]" << std::endl;
    std::cout << "Unflag[x][y][z*]" << std::endl;
    std::cout << "Depth[z]" << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << "Your current depth is: " << b.QuerryDepth() << std::endl;
    std::cout << "Flags: " << flags << "/" << count << std::endl;
    std::cout << "Last Command: " << lastCommand << std::endl;
    std::cout << "------------------" << std::endl;
    std::cout << std::endl;
    std::string command;
    std::cin >> command;
    lastCommand = command;
    if (command.find("[") != std::string::npos)
    {
      std::string type = command.substr(0, command.find('['));
      command.erase(command.begin(), command.begin() + command.find('[') + 1);


      size_t index1 = SIZE_MAX;

      size_t index2 = SIZE_MAX;
      size_t index3 = SIZE_MAX;
      size_t pos = command.find('[');
      try
      {
        index1 = std::stoi(command);
        if (pos != std::string::npos)
        {
          command.erase(command.begin(), command.begin() + pos + 1);
          index2 = std::stoi(command);
        }
        pos = command.find('[');
        if (pos != std::string::npos)
        {
          command.erase(command.begin(), command.begin() + pos + 1);
          index3 = std::stoi(command);
        }
      }
      catch (...)
      {
        goto end;
      }

      if (type == "Check" && index3 == SIZE_MAX)
      {
        std::cout << "Checking position: " << index1 << " , " << index2 << std::endl;
        lost = b.Check(index1, index2);
      }
      else if (type == "Check")
      {
        std::cout << "Checking position: " << index1 << " , " << index2 << " , " << index3 << std::endl;
        lost = b.Check(index1, index2, index3);
      }
      if (type == "Flag" && index3 == SIZE_MAX)
      {
        std::cout << "Flagging position: " << index1 << " , " << index2 << std::endl;
        b.Flag(index1, index2);
        ++flags;
      }
      else if (type == "Flag")
      {
        std::cout << "Flagging position: " << index1 << " , " << index2 << " , " << index3 << std::endl;
        b.Flag(index1, index2, index3);
        ++flags;
      }
      if (type == "Unflag" && index3 == SIZE_MAX)
      {
        std::cout << "Unflagging position: " << index1 << " , " << index2 << std::endl;
        b.UnFlag(index1, index2);
        --flags;
      }
      else if (type == "Unflag")
      {
        std::cout << "Unflagging position: " << index1 << " , " << index2 << " , " << index3 << std::endl;
        b.UnFlag(index1, index2, index3);
        --flags;
      }
      if (type == "Depth")
      {
        std::cout << "Setting Depth to: " << index1 << std::endl;
        b.SetSlice(index1);
      }
    }
    end:
    if (command == "Reveal")
      b.Reveal();
    if (command == "Hide")
      b.Hide();

    if (b.Won())
    {
      system("clear");
      b.Reveal();
      std::cout << b << std::endl;
      std::cout << "All bombs found\n Congrats\n" << std::endl;
      EndState(b, ended, lost, flags, count);
    }
    if (lost)
    {
      system("clear");
      b.Reveal();
      std::cout << b << std::endl;
      std::cout << "You lose\n Unfortunate soul\n" << std::endl;
      EndState(b, ended, lost, flags, count);

    }
  }

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
