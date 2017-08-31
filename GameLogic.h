#ifndef RULES_H
#define RULES_H

#include "GameMove.h"

#define UNOCCUPIED    '\0'
#define HUMAN_COLOR   'B'
#define AI_COLOR      'W'

class GameLogic
{
public:
  GameLogic(short _dim, short _difficulty);
  ~GameLogic(void);

  /**
   * Print board to stdio
   */
  void PrintBoard();
  /**
   * Human player set move
   * i = row, j = col
   * Return true if it's a valid move
   */
  bool SetMove(short i, short j);
  /**
   * AI player set move
   * The move made is stored in row and col
   */
  void AIMakeMove(short *row, short *col);
  /**
   * Arbitrate whether a side has won depending on the moveRow and moveCol provided
   */
  enum Arbitration {
    WIN,
    DRAW,
    NONE
  };
  Arbitration Arbitrate(char side);

  void SetDifficulty(short _diff);
  void SetBoardSize(short _dim);

private:
  short dimSize;
  short difficulty;

  /**
   * Storage for board moves
   * B = human
   * W = AI
   */
  char *board;
  void deleteBoard(char* _board);
  void newBoard(char** _board);

  /**
   * Starting from startRow and startCol, check in the direction of dirRow and dirCol
   * If there are five consecutive pieces of color side, return true
   */
  bool isFiveConnected(short startRow, short startCol, short dirRow, short dirCol, char side);

  /**
   * From rowStart and colBegin, search in dirRow and dirCol for length same color
   */
  enum Boundedness{
    BOUNDED,
    UNBOUNDED,
    ONE_SIDED
  };
  Boundedness isBounded(short rowStart, short colBegin, short dirRow, short dirCol, short* length);

  /**
   * Assign a score of the current combination based on boundedness, length of continuous colors and
   * the color of the player (human or AI)
   */
  int scoreFunction(short length, Boundedness boundedness, char side);

  /**
   * Assess a line in the board from [rowStart, colStart] in the direction of dirRow and dirCol
   * Return a score based on scoreFunction
   */
  int assessLine(short rowStart, short colStart, short dirRow, short dirCol);
  /**
   * Assess the current board
   * Return a score based on scoreFunction
   */
  int assessBoard();

  /**
   * Evaluate whether the move is admissible
   */
  bool isMoveAdmissible(short row, short col);

  /**
   * Apply minimax to levels number of moves beneath move
   * Return the score based on whether it's maximize or minimize
   * alphaBetaExtremum = the min or max value from the parent, to be used in alpha-beta
   */
  int assessMove(GameMove* move, short levels, bool alphaBeta = false, short alphaBetaExtremum = 0);

};

#endif