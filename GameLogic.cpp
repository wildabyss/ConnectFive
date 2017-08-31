#include <iostream>
#include <time.h>
#include "GameLogic.h"
#include "GameMove.h"

using namespace std;

#define PRINT_TOTAL_NODES
#ifdef PRINT_TOTAL_NODES
unsigned int totalNodes = 0;
#endif

/**
 * Constructor
 */
GameLogic::GameLogic(short dim, short _difficulty)
{
  dimSize = dim;
  difficulty = _difficulty;
  newBoard(&board);
}


/**
 * Destructor
 */
GameLogic::~GameLogic(void)
{
  deleteBoard(board);
}


/**
 * Print board to stdio
 */
void GameLogic::PrintBoard(){
  cout << std::endl;

  for (short i=0; i<dimSize; i++){
    // first row
    if (i==0){
      // print col id
      for (short j=0; j<dimSize; j++){
        if (j==0)
          cout << "  ";
        cout << j%10 << " ";
      }
      cout << endl;

      // print top boundary
      for (short j=0; j<dimSize; j++){
        if (j==0)
          cout << " -";
        cout << "--";
      }
      cout << endl;
    }

    // print middle layer
    for (short j=0; j<dimSize; j++){
      if (j==0)
        cout << i%10 << "|";

      if (board[i*dimSize+j] != '\0')
        cout << board[i*dimSize+j] << "|";
      else
        cout << " " << "|";
    }
    cout << endl;

    // print bottom boundary
    for (short j=0; j<dimSize; j++){
      if (j==0)
        cout << " -";

      cout << "--";
    }
    cout << endl;
  }
}


/**
 * Human player set move
 * Return true if it's a valid move
 */
bool GameLogic::SetMove(short i, short j){
  if (i>=0 && i<dimSize && j>=0 && j<dimSize && board[i*dimSize+j] == UNOCCUPIED){
    board[i*dimSize+j] = HUMAN_COLOR;
    return true;
  }

  return false;
}


/**
 * AI player set move
 */
void GameLogic::AIMakeMove(short *row, short *col){
  short max_move_row = -1, max_move_col = -1;
  int max_score = 0;

  if (difficulty == 1){
    // find the highest score and make the move. Greedy algorithm

    // find empty moves
    for (short i=0; i<dimSize; i++){
      for (short j=0; j<dimSize; j++){
        short ind = i*dimSize+j;
        if (isMoveAdmissible(i,j)){
          // temporarily put a move there
          board[ind] = AI_COLOR;

          // assess board and get score
          int score = assessBoard();

          if ((max_move_row < 0 || max_move_col < 0) || score > max_score){
            max_move_row = i;
            max_move_col = j;
            max_score = score;
          }

          // remove the move
          board[ind] = UNOCCUPIED;
        }
      }
    }

    // make the permanent move
    board[max_move_row*dimSize + max_move_col] = AI_COLOR;
    (*row) = max_move_row;
    (*col) = max_move_col;

  }
  else if (difficulty == 2){
    // apply minimax to N levels

    short N = 4; 
#ifdef PRINT_TOTAL_NODES
    totalNodes = 0;
#endif

    // find empty moves
    for (short i=0; i<dimSize; i++){
      for (short j=0; j<dimSize; j++){
        if (isMoveAdmissible(i,j)){
          GameMove* move = new GameMove(nullptr, i, j, AI_COLOR);
          // note: move is deleted from within assessMove
          int score = assessMove(move, N);
          if (max_move_row < 0 || max_move_col < 0 || max_score < score){
            max_score = score;
            max_move_row = i;
            max_move_col = j;
          }
        }
      }
    }

    // make the permanent move
    board[max_move_row*dimSize + max_move_col] = AI_COLOR;
    (*row) = max_move_row;
    (*col) = max_move_col;

#ifdef PRINT_TOTAL_NODES
    cout << "Total nodes traversed: " << totalNodes << endl;
#endif
  }
}


/**
 * Apply minimax to levels number of moves beneath move
 * Return the score based on whether it's maximize or minimize
 * Minimize if move == AI_COLOR
 * Maximize if move == HUMAN_COLOR
 */
int GameLogic::assessMove(GameMove* move, short levels, bool isAlphaBeta, short alphaBetaExtremum){
  if (levels <= 0){
    // Get the actual scores

#ifdef PRINT_TOTAL_NODES
    totalNodes++;
#endif

    // Set the board
    board[move->row*dimSize+move->col] = move->GetSide();
    for (GameMove* up=move->parent; up!=nullptr; up=up->parent){
      board[up->row*dimSize+up->col] = up->GetSide();
    }

    int score = assessBoard();

    // Unset the board
    board[move->row*dimSize+move->col] = UNOCCUPIED;
    for (GameMove* up=move->parent; up!=nullptr; up=up->parent){
      board[up->row*dimSize+up->col] = UNOCCUPIED;
    }

    delete move;

    return score;

  } else {
    // Not at the deepest level, branch down

    bool allBreak = false;

    for (int i=0; i<dimSize && !allBreak; i++){
      for (int j=0; j<dimSize && !allBreak; j++){
        // Set the board
        board[move->row*dimSize+move->col] = move->GetSide();
        for (GameMove* up=move->parent; up!=nullptr; up=up->parent){
          board[up->row*dimSize+up->col] = up->GetSide();
        }

        // determine admissibility
        bool adm = isMoveAdmissible(i,j);

        // Unset the board
        board[move->row*dimSize+move->col] = UNOCCUPIED;
        for (GameMove* up=move->parent; up!=nullptr; up=up->parent){
          board[up->row*dimSize+up->col] = UNOCCUPIED;
        }


        // only assess the cells that are unoccupied
        if (adm){
          // child color should be the opposite of the parent color
          char childSide;
          if (move->GetSide() == AI_COLOR)
            childSide = HUMAN_COLOR;
          else
            childSide = AI_COLOR;

          GameMove* child = new GameMove(move, i, j, childSide);
          int score = assessMove(child, levels-1, move->IsScoreAssigned(), move->GetScore());

          if (move->GetSide() == AI_COLOR){
            // minimizer

            if (!move->IsScoreAssigned() || move->GetScore() > score){
              move->SetScore(score);

              // try alpha-beta, parent = maximizer
              if (isAlphaBeta && alphaBetaExtremum >= move->GetScore()){
                allBreak = true;
              }
            }
          } else {
            // maximizer
            // parent= maximizer
            if (!move->IsScoreAssigned() || move->GetScore() < score){
              move->SetScore(score);

              // try alpha-beta, parent = minimizer
              if (isAlphaBeta && alphaBetaExtremum <= move->GetScore()){
                allBreak = true;
              }
            }
          }
        }
      }
    }

    int max_score = move->GetScore();
    delete move;
    return max_score;
  }
}


/**
 * Assess a line in the board from [rowStart, colStart] in the direction of dirRow and dirCol
 * Return a score based on scoreFunction
 */
int GameLogic::assessLine(short rowStart, short colStart, short dirRow, short dirCol){
  int score = 0;
  int i = rowStart, j = colStart;

  while (i<dimSize && i>=0 && j<dimSize && j>=0){
    short ind = i*dimSize+j;

    if (board[ind] != UNOCCUPIED){
      // occupied cell

      short length = 0;
      Boundedness boundedness = isBounded(i, j, dirRow, dirCol, &length);
      score += scoreFunction(length, boundedness, board[ind]);

      // advance cursor by length
      i += dirRow*length;
      j += dirCol*length;
    } else {
      // blank space

      // advance cursor by 1
      i += dirRow;
      j += dirCol;
    }
  }

  return score;
}


/**
 * Assess the current board
 * Return a score based on scoreFunction
 */
int GameLogic::assessBoard(){
  int score = 0;

  // go through all rows, at col=0
  for (int i=0; i<dimSize; i++){
    score += assessLine(i, 0, 0, 1);
  }

  // go through all columns
  for (int j=0; j<dimSize; j++){
    // do columns
    score += assessLine(0, j, 1, 0);

    // do negative diagonals
    score += assessLine(0, j, 1, 1);
    // do positive diagonals
    score += assessLine(0, j, 1, -1);
    
    // bottom row diagonals
    if (j>0 && j<dimSize-1){
      // do negative diagonal
      score += assessLine(dimSize-1, j, -1, -1);
      // do positive diagonal
      score += assessLine(dimSize-1, j, -1, 1);
    }
  }

  return score;
}


/**
 * From rowStart and colBegin, search in dirRow and dirCol for length same color
 * dirRow = -1, 0, 1
 * dirCol = -1, 0, 1
 */
//FIXME: what about XX_XX
GameLogic::Boundedness GameLogic::isBounded(short rowStart, short colStart, short dirRow, short dirCol, short* length){
  short spaceBefore = 0, spaceAfter = 0;

  // march in dirRow and dirCol
  short i=rowStart, j=colStart;
  bool flipped = false;
  for (; i>=0 && i<dimSize && j>=0 && j<dimSize; i+=dirRow, j+=dirCol){
    if (!flipped && board[i*dimSize+j] == board[rowStart*dimSize+colStart])
      (*length)++;
    else {
      flipped = true;

      if (board[i*dimSize+j] == UNOCCUPIED)
        spaceAfter++;
      else
        break;
    }
  }

  // march in opposite direction
  i=rowStart-dirRow; j=colStart-dirCol;
  flipped = false;
  for (; i>=0 && i<dimSize && j>=0 && j<dimSize; i-=dirRow, j-=dirCol){
    if (!flipped && board[i*dimSize+j] == board[rowStart*dimSize+colStart])
      (*length)++;
    else {
      flipped = true;

      if (board[i*dimSize+j] == UNOCCUPIED)
        spaceBefore++;
      else
        break;
    }
  }

  // determine boundedness
  if ((*length)+spaceBefore+spaceAfter < 5)
    return BOUNDED;
  else if (spaceBefore > 0 && spaceAfter > 0)
    return UNBOUNDED;
  else
    return ONE_SIDED;
}


/**
 * Arbitrate whether a side has won depending on the moveRow and moveCol provided
 */
GameLogic::Arbitration GameLogic::Arbitrate(char mySide){
  // go through all rows, at col=0
  for (int i=0; i<dimSize; i++){
    if (isFiveConnected(i,0,0,1, mySide))
      return WIN;
  }

  // go through all columns
  for (int j=0; j<dimSize; j++){
    // do columns at row=0
    if (isFiveConnected(0,j,1,0,mySide))
      return WIN;

    // do negative diagonals
    if (isFiveConnected(0,j,1,1,mySide))
      return WIN;
    // do positive diagonals
    if (isFiveConnected(0,j,1,-1,mySide))
      return WIN;
    
    // bottom row diagonals
    if (j>0 && j<dimSize-1){
      // do negative diagonal
      if (isFiveConnected(dimSize-1,j,-1,-1,mySide))
        return WIN;
      // do positive diagonal
      if (isFiveConnected(dimSize-1,j,-1,1,mySide))
        return WIN;
    }
  }

  // check for empty cell
  for (int i=0; i<dimSize; i++){
    for (int j=0; j<dimSize; j++){
      if (board[i*dimSize+j] == UNOCCUPIED)
        return NONE;
    }
  }

  return DRAW;
}


/**
 * Evaluate whether the move is admissible
 * It's admissible only if it's not more than EXCEEDANCE moves away from other pieces AND the cell is unoccupied
 */
bool GameLogic::isMoveAdmissible(short row, short col){
  const short EXCEEDANCE = 1;

  if (board[row*dimSize + col] != UNOCCUPIED)
    return false;

  for (int i=row-EXCEEDANCE; i<=row+EXCEEDANCE; i++)
    if (i>=0 && i<dimSize)
      for (int j=col-EXCEEDANCE; j<=col+EXCEEDANCE; j++)
        if (j>=0 && j<dimSize && board[i*dimSize+j]!=UNOCCUPIED)
          return true;
  

  return false;
}


/**
 * Starting from startRow and startCol, check in the direction of dirRow and dirCol
 * If there are five consecutive pieces of color side, return true
 */
bool GameLogic::isFiveConnected(short startRow, short startCol, short dirRow, short dirCol, char side){
  short i=startRow, j=startCol;
  short count = 0;

  for (; i >= 0 && i < dimSize && j >= 0 && j < dimSize; i+=dirRow, j+=dirCol){
    if (board[i*dimSize+j] != side)
      count = 0;
    else
      count++;

    // winning condition: consecutive 5 pieces
    if (count >= 5)
      return true;
  }

  return false;
}


/**
 * Assign a score of the current combination based on boundedness, length of continuous colors and
 * the color of the player (human or AI)
 */
int GameLogic::scoreFunction(short length, Boundedness boundedness, char side){
  // guard against non-sensical inputs
  if (side == UNOCCUPIED)
    return 0;

  // guard against negative length
  length = (length>0)?length:0;

  if (boundedness == BOUNDED){
    // both sides bounded
    return 0;

  } else if (boundedness == UNBOUNDED){
    // both sides unbounded

    if (side == HUMAN_COLOR){
      // human color
      if (length < 2)
        return -length*3;
      else if (length == 2)
        return -50;
      else if (length == 3)
        return -150;
      else if (length == 4)
        return -500;
      else
        return -4000;
    } else {
      // AI color
      if (length < 2)
        return length*2;
      else if (length == 2)
        return 40;
      else if (length == 3)
        return 90;
      else if (length == 4)
        return 300;
      else
        return 4000;
    }
  }

  else {
    // one side bounded

    if (side == HUMAN_COLOR){
      // human color
      if (length <= 2)
        return -length;
      else if (length == 3)
        return -20;
      else if (length == 4)
        return -400;
      else
        return -4000;
    } else {
      // AI color
      if (length <= 2)
        return length;
      else if (length == 3)
        return 15;
      else if (length == 4)
        return 150;
      else
        return 4000;
    }
  }
}


void GameLogic::SetBoardSize(short _dim){
  dimSize = _dim;

  deleteBoard(board);
  newBoard(&board);
}

void GameLogic::SetDifficulty(short _diff){
  difficulty = _diff;
}


/**
 * Create a brand new board based on dimSize
 * Remember to call deleteBoard prior to using newBoard
 */
void GameLogic::newBoard(char** _board){
  (*_board) = new char[dimSize*dimSize];
  for(short i = 0; i < dimSize; i++) {
      for (short j=0; j<dimSize; j++)
        (*_board)[i*dimSize+j] = UNOCCUPIED;
  }
}


/**
 * Perform memory deletion of the board
 * Remember to call this method every time SetBoardSize
 */
void GameLogic::deleteBoard(char* _board){
  delete [] _board;
}