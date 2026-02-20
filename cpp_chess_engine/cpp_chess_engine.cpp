#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

enum Piece {
    EMPTY = 0,
    WPAWN = 1, WKNIGHT, WBISHOP, WROOK, WQUEEN, WKING,
    BPAWN = -1, BKNIGHT = -2, BBISHOP = -3, BROOK = -4, BQUEEN = -5, BKING = -6
};

struct Move {
    int fromX, fromY;
    int toX, toY;
};

class Board {
public:
    int board[8][8];
    bool whiteTurn = true;

    Board() { initialize(); }

    void initialize() {
        int initial[8][8] = {
            {BROOK, BKNIGHT, BBISHOP, BQUEEN, BKING, BBISHOP, BKNIGHT, BROOK},
            {BPAWN, BPAWN, BPAWN, BPAWN, BPAWN, BPAWN, BPAWN, BPAWN},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {WPAWN, WPAWN, WPAWN, WPAWN, WPAWN, WPAWN, WPAWN, WPAWN},
            {WROOK, WKNIGHT, WBISHOP, WQUEEN, WKING, WBISHOP, WKNIGHT, WROOK}
        };
        copy(&initial[0][0], &initial[0][0] + 64, &board[0][0]);
    }

    void print() {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                cout << pieceToChar(board[i][j]) << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    char pieceToChar(int p) {
        switch (p) {
            case WPAWN: return 'P';
            case WKNIGHT: return 'N';
            case WBISHOP: return 'B';
            case WROOK: return 'R';
            case WQUEEN: return 'Q';
            case WKING: return 'K';
            case BPAWN: return 'p';
            case BKNIGHT: return 'n';
            case BBISHOP: return 'b';
            case BROOK: return 'r';
            case BQUEEN: return 'q';
            case BKING: return 'k';
            default: return '.';
        }
    }

    vector<Move> generateMoves(bool white) {
        vector<Move> moves;

        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                int piece = board[x][y];
                if (piece == EMPTY) continue;
                if (white && piece < 0) continue;
                if (!white && piece > 0) continue;

                generatePieceMoves(x, y, moves);
            }
        }
        return moves;
    }

    void generatePieceMoves(int x, int y, vector<Move>& moves) {
        int piece = board[x][y];
        int dir = (piece > 0) ? -1 : 1;

        if (abs(piece) == WPAWN) {
            int nx = x + dir;
            if (inBounds(nx, y) && board[nx][y] == EMPTY)
                moves.push_back({x,y,nx,y});
        }

        if (abs(piece) == WKNIGHT) {
            int dx[] = {-2,-2,-1,-1,1,1,2,2};
            int dy[] = {-1,1,-2,2,-2,2,-1,1};
            for (int i=0;i<8;i++) {
                int nx = x+dx[i], ny=y+dy[i];
                if (inBounds(nx,ny) && isOpponent(piece,board[nx][ny]))
                    moves.push_back({x,y,nx,ny});
            }
        }

        if (abs(piece) == WROOK || abs(piece)==WBISHOP || abs(piece)==WQUEEN) {
            int directions[8][2] = {
                {-1,0},{1,0},{0,-1},{0,1},
                {-1,-1},{-1,1},{1,-1},{1,1}
            };
            int start=0,end=8;
            if (abs(piece)==WROOK) { start=0; end=4; }
            if (abs(piece)==WBISHOP) { start=4; end=8; }

            for (int d=start; d<end; d++) {
                int nx=x, ny=y;
                while (true) {
                    nx+=directions[d][0];
                    ny+=directions[d][1];
                    if (!inBounds(nx,ny)) break;
                    if (board[nx][ny]==EMPTY)
                        moves.push_back({x,y,nx,ny});
                    else {
                        if (isOpponent(piece,board[nx][ny]))
                            moves.push_back({x,y,nx,ny});
                        break;
                    }
                }
            }
        }

        if (abs(piece)==WKING) {
            for(int dx=-1;dx<=1;dx++)
                for(int dy=-1;dy<=1;dy++) {
                    if(dx==0 && dy==0) continue;
                    int nx=x+dx, ny=y+dy;
                    if(inBounds(nx,ny) && isOpponent(piece,board[nx][ny]))
                        moves.push_back({x,y,nx,ny});
                }
        }
    }

    bool isOpponent(int p1,int p2){
        if(p2==EMPTY) return true;
        return (p1>0 && p2<0)||(p1<0 && p2>0);
    }

    bool inBounds(int x,int y){
        return x>=0 && x<8 && y>=0 && y<8;
    }

    void makeMove(Move m){
        board[m.toX][m.toY]=board[m.fromX][m.fromY];
        board[m.fromX][m.fromY]=EMPTY;
        whiteTurn=!whiteTurn;
    }

    int evaluate(){
        int score=0;
        for(int i=0;i<8;i++)
            for(int j=0;j<8;j++)
                score+=pieceValue(board[i][j]);
        return score;
    }

    int pieceValue(int p){
        switch(abs(p)){
            case WPAWN: return (p>0)?100:-100;
            case WKNIGHT: return (p>0)?320:-320;
            case WBISHOP: return (p>0)?330:-330;
            case WROOK: return (p>0)?500:-500;
            case WQUEEN: return (p>0)?900:-900;
            case WKING: return (p>0)?20000:-20000;
            default: return 0;
        }
    }
};

int minimax(Board board, int depth, bool maximizing){
    if(depth==0) return board.evaluate();

    auto moves=board.generateMoves(board.whiteTurn);
    if(moves.empty()) return board.evaluate();

    if(maximizing){
        int maxEval=numeric_limits<int>::min();
        for(auto m:moves){
            Board copy=board;
            copy.makeMove(m);
            int eval=minimax(copy,depth-1,false);
            maxEval=max(maxEval,eval);
        }
        return maxEval;
    }else{
        int minEval=numeric_limits<int>::max();
        for(auto m:moves){
            Board copy=board;
            copy.makeMove(m);
            int eval=minimax(copy,depth-1,true);
            minEval=min(minEval,eval);
        }
        return minEval;
    }
}

int main(){
    Board board;
    board.print();

    while(true){
        auto moves=board.generateMoves(board.whiteTurn);
        if(moves.empty()){
            cout<<"Game Over\n";
            break;
        }

        Move bestMove=moves[0];
        int bestScore=numeric_limits<int>::min();

        for(auto m:moves){
            Board copy=board;
            copy.makeMove(m);
            int score=minimax(copy,2,false);
            if(score>bestScore){
                bestScore=score;
                bestMove=m;
            }
        }

        board.makeMove(bestMove);
        board.print();
    }
}
