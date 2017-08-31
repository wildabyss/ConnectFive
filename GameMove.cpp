#include "GameMove.h"


GameMove::GameMove(GameMove* _prev, short _row, short _col, char _side)
{
  parent = _prev;
  row = _row;
  col = _col;
  side = _side;

  scoreAssigned = false;
  score = 0;
}


GameMove::~GameMove(void)
{
}


char GameMove::GetSide(){
  return side;
}


void GameMove::SetScore(int _score){
  score = _score;
  scoreAssigned = true;
}


bool GameMove::IsScoreAssigned(){
  return scoreAssigned;
}


int GameMove::GetScore(){
  return score;
}


void GameMove::ClearScore(){
  scoreAssigned = false;
  score = 0;
}