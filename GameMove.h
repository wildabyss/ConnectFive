#ifndef GAME_MOVE_H
#define GAME_MOVE_H

/**
 * This class is to be used to score a particular move on the board
 * It is designed to serves as a node in the decision tree
 */
class GameMove
{
public:
  GameMove(GameMove* _prev, short _row, short _col, char _side);
  ~GameMove();

  void SetScore(int _score);
  int GetScore();
  bool IsScoreAssigned();
  void ClearScore();

  // parent move
  GameMove* parent;

  // the move placed
  short row, col;
  
  char GetSide();
private:
  bool scoreAssigned;
  int score;

  // either 'W' or 'B'
  char side;
};

#endif