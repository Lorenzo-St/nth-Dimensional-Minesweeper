/*********************************************************************
 * @file   nth Dimensional MineSweeper.cpp
 * @brief  Main file
 * 
 * @author Lorenzo St. Luce(lorenzo.stluce)
 * @date   November 2023
 *********************************************************************/
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <iomanip>
#include "cexprString.h"
#include "board.h"
#define SDL_MAIN_HANDLED
#include "RenderBackend.h"

void EndState(board& b, bool& ended, bool& lost, int& flags, int& count, Renderer& r)
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
      b = board(x, y, z, count, r);
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

  Renderer r = Renderer();
  std::cout << "Welcome\n Please State you board size" << std::endl;
  std::cout << "Syntax: \"X Y Z #\"" << std::endl;
  std::cout << "-----------------------" << std::endl;
  int x = 10, y = 10, z = 10, count = 200;
  //std::cin >> x;
  //std::cin >> y;
  //std::cin >> z;
  //std::cin >> count;
  x = (x < 1 ? 1 : x);
  y = (y < 1 ? 1 : y);
  z = (z < 1 ? 1 : z);
  board b = board(x, y, z, count, r);
  b.generate();
  b.Hide();
  bool ended = false;
  bool lost = false;
  int flags = 0;
  std::string lastCommand = "";
  while (r.running)
  {
    r.Update();

    //system("clear");
    std::cout << b;
    //std::cout << "Possible Commands:" << std::endl;
    //std::cout << "------------------" << std::endl;
    //std::cout << "* = parameter is optional" << std::endl;
    //std::cout << "Check[x][y][z*]" << std::endl;
    //std::cout << "Flag[x][y][z*]" << std::endl;
    //std::cout << "Unflag[x][y][z*]" << std::endl;
    //std::cout << "Depth[z]" << std::endl;
    //std::cout << "------------------" << std::endl;
    //std::cout << "Your current depth is: " << b.QuerryDepth() << " of " << z-1 << std::endl;
    //std::cout << "Flags: " << flags << "/" << count << std::endl;
    //std::cout << "Last Command: " << lastCommand << std::endl;
    //std::cout << "------------------" << std::endl;
    //std::cout << std::endl;
    std::string command = "";


    //std::cin >> command;
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
      EndState(b, ended, lost, flags, count, r);
    }
    if (lost)
    {
      system("clear");
      b.Reveal();
      std::cout << b << std::endl;
      std::cout << "You lose\n Unfortunate soul\n" << std::endl;
      EndState(b, ended, lost, flags, count, r);

    }
    if (ended)
      r.running = false;

  }
  return 0;
}

