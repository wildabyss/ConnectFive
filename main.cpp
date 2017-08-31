#include <iostream>
#include <sstream>
#include <string>
#include "GameLogic.h"

using namespace std;


/**
 * Print main menu
 * Quit: 0
 * New game: 1
 * Set board: 2
 */
short MainMenu(){
  while (1){
    cout << "\nWelcome to Connect Five\n-------------------\n";
    cout << "1. New game\n";
    cout << "2. Set board size\n";
    cout << "3. Set difficulty\n";
    cout << "q. Quit\n";
    cout << "\nChoice: ";

    string a;
    cin >> a;

    if (a=="q")
      return 0;
    else if (a=="1")
      return 1;
    else if (a=="2")
      return 2;
    else if (a=="3")
      return 3;
  }
}


/**
 * Prompt user to set board size
 */
short PromptBoard(){
  short ret = 0;
  const short SMALLEST_SIZE = 6;

  do {
    cout << endl << "Dimension size (min " << SMALLEST_SIZE+1 << "): ";
  } while (!(cin >> ret) || ret <= SMALLEST_SIZE);

  return ret;
}


/**
 * Set AI difficulty
 */
short PromptDifficulty(){
  short ret = 0;

  do {
    cout << endl << "Set AI difficulty (1=easy, 2=hard): ";
  } while (!(cin >> ret) || (ret < 1 || ret > 2));

  return ret;
}


/**
 * Return 1 on regular move
 * Return -1 on failure
 * Return 0 on quit
 */
short ReadMove(short *row, short *col){
  string a;
  do {
    cout << endl << "Your move (row, col): ";
    cin >> a;

    if (a=="q"){
      return 0;
    } else {
      // attempt to parse "row,col"
      char sep = 0x00;
      stringstream ss(a);
      if ((ss >> skipws >> *row >> sep >> *col) && sep == ',')
        return 1;
    }
  } while (1);

  return 0;
}


int main(){
  // board size;
  short dimSize = 15;
  // AI difficulty, 0 - 3
  short difficulty = 2;

  //- outside loop for overall game control
  while (1){
    // main menu selection
    short res = MainMenu();

    if (res == 0){
      // exit
      return 0;

    } else if (res == 2){
      // choose board size
      dimSize = PromptBoard();

    } else if (res == 3){
      // choose difficulty
      difficulty = PromptDifficulty();

    } else {

      // play game
      GameLogic game(dimSize, difficulty);

      //- inner loop for game play
      while (1){
        string in;

        // print board
        game.PrintBoard();

        // get human's move
        short row = -1, col = -1;
        do {
          res = ReadMove(&row, &col);

          if (res == 0)
            goto END_MAIN_MENU;  // exit current game
        } while (!game.SetMove(row, col));

        // determine winning condition
        GameLogic::Arbitration state = game.Arbitrate(HUMAN_COLOR);
        if (state==GameLogic::WIN){
          game.PrintBoard();
          cout << endl << "You've won!" << endl;
          goto END_MAIN_MENU;
        } else if (state == GameLogic::DRAW){
          game.PrintBoard();
          cout << endl << "Game is a draw." << endl;
          goto END_MAIN_MENU;
        }

        // AI's turn
        game.AIMakeMove(&row, &col);

        // determine winning condition
        state = game.Arbitrate(AI_COLOR);
        if (state==GameLogic::WIN){
          game.PrintBoard();
          cout << endl << "You lost!" << endl;
          goto END_MAIN_MENU;
        } else if (state == GameLogic::DRAW){
          game.PrintBoard();
          cout << endl << "Game is a draw." << endl;
          goto END_MAIN_MENU;
        }
      }

    }


END_MAIN_MENU:
    continue;
  }

  return 0;
}