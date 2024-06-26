#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <algorithm>
#include <exception>

using namespace std;

void fill_matrix(int matrix[8][8]);

void print_matrix(int matrix[8][8]);

int letterToIndex(char letter);

void make_turn(int player, int board[8][8], string from, string to);

vector<int> parse_turn(string turn);

vector<char> board_alp{'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'Z'};

vector<vector<string>> historyOfMoves{};

void fill_matrix(int matrix[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i == 0 || i == 7) && (j > 0 && j < 7)) {
                matrix[i][j] = -1;
            } else if ((j == 0 || j == 7) && (i > 0 && i < 7)) {
                matrix[i][j] = 1;
            } else {
                matrix[i][j] = 0;
            }
        }
    }
}

/*
void print_matrix(int matrix[8][8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << setw(2) << matrix[i][j];
        }
        cout << endl;
    }
}
*/


// void print_matrix(int matrix[8][8]) {
//     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//     for (int i = 0; i < 8; i++) {
//         for (int j = 0; j < 8; j++) {
//             if (matrix[i][j] == 1) {
//                 // Red color for 1
//                 SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
//             } else if (matrix[i][j] == -1) {
//                 // Blue color for -1
//                 SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
//             } else {
//                 // Reset color to default
//                 SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
//             }
//             cout << setw(2) << matrix[i][j];
//         }
//         cout << endl;
//     }
//     // Reset color to default at the end
//     SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
// }

int letterToIndex(char letter) {
    auto it = std::find(board_alp.begin(), board_alp.end(), letter);
    if (it != board_alp.end()) {
        // Найденный элемент находится в it, возвращаем его индекс
        return std::distance(board_alp.begin(), it);
    } else {
        // Элемент не найден, возвращаем какой-то код ошибки или -1
        return -1; // или что-то другое, что сигнализирует об ошибке
    }
}

void make_turn(int player, int board[8][8], string from, string to) {
    vector<int> start_cordinate = parse_turn(from);
    vector<int> end_cordianate = parse_turn(to);

    board[start_cordinate[1]][start_cordinate[0]] = 0;
    board[end_cordianate[1]][end_cordianate[0]] = player;
}

vector<int> parse_turn(string turn) {
    vector<int> result;
    result.push_back(letterToIndex(turn.at(0)));
    result.push_back(8 - (turn.at(1) - '0'));

    return result;
}

struct Result {
    //{координаты точки: [посещяли или нет, конечная или нет, кордината один, координата два]}
    map<vector<int>, vector<int> > pointsOfPile{};
    //{[координата один, координата два]}
    vector<vector<int> > allPlayersPoints{};
};

class Checker {
private:
    /*
     * Move Check
    */

    //ПРОВЕРЯЕМ, ЕСЛИ ХОДИМ ПО ВЕРТИКАЛЕ
    static bool moveIsVertically(int player, int board[8][8], vector<int> from, vector<int> to) {
        //количество союзников на линии
        int countAllies = 0;

        //кол-во клеток, которые нужно пройти
        int countFreeCells = 0;

        //найде клетка откуда ходим
        bool fl1 = false;

        //найде клетка куда ходим
        bool fl2 = false;

        //на пути есть противник
        bool enemy = false;

        if (from[0] != to[0] || board[to[1]][to[0]] == player || board[from[1]][from[0]] != player) {
            return false;
        }

        for (int i = 0; i < 8; i++) {
            if (i == from[1]) {
                fl1 = true;
            }

            if (i == to[1]) {
                fl2 = true;
            }

            if (board[i][from[0]] != 0) {
                countAllies++;
            }

            if (fl1 && fl2) {
                continue;
            }

            if (fl1 || fl2) {
                countFreeCells++;

                if (board[i][to[0]] != 0 && board[i][to[0]] != player) {
                    enemy = true;
                    break;
                }
            }
        }

        if (enemy || (countFreeCells) != countAllies) {
            return false;
        }

        if ((countFreeCells) == countAllies) {
            return true;
        }

        return false;
    }

    //ПРОВЕРЯЕМ, ЕСЛИ ХОДИМ ПО ГОРИЗОНТАЛЕ
    static bool moveIsHorizontal(int player, int board[8][8], vector<int> from, vector<int> to) {
        //количество союзников на линии
        int countAllies = 0;

        //кол-во клеток, которые нужно пройти
        int countFreeCells = 0;

        //найде клетка откуда ходим
        bool fl1 = false;

        //найде клетка куда ходим
        bool fl2 = false;

        //на пути есть противник
        bool enemy = false;

        if (from[1] != to[1] || board[to[1]][to[0]] == player || board[from[1]][from[0]] != player) {
            return false;
        }

        for (int i = 0; i < 8; i++) {
            if (i == from[0]) {
                fl1 = true;
            }

            if (i == to[0]) {
                fl2 = true;
            }

            if (board[from[1]][i] != 0) {
                countAllies++;
            }

            if (fl1 && fl2) {
                continue;
            }

            if (fl1 || fl2) {
                countFreeCells++;

                if (board[to[1]][i] != 0 && board[to[1]][i] != player) {
                    enemy = true;
                    break;
                }
            }
        }

        if (enemy) {
            return false;
        }

        if ((countFreeCells) == countAllies) {
            return true;
        }

        return false;
    }

    //ПРОВЕРЯЕМ ДИАГОНАЛЬ КОРОЧЕ ТИПО С ВЕРХУ ВНИЗ ЛИНИЮ ПОНЯЛИ ДА!
    static bool moveIsDiagonalRight(int player, int board[8][8], vector<int> from, vector<int> to) {
        //количество союзников на линии
        int countAllies = 0;

        //кол-во клеток, которые нужно пройти
        int countFreeCells = 0;

        //найде клетка откуда ходим
        bool fl1 = false;

        //найде клетка куда ходим
        bool fl2 = false;

        //на пути есть противник
        bool enemy = false;

        if (from[0] != from[1] || to[0] != to[1] || board[to[1]][to[0]] == player || board[from[1]][from[0]] !=
                                                                                     player) {
            return false;
        }

        for (int i = 0; i < 8; i++) {
            if (i == from[0] && i == from[1]) {
                fl1 = true;
            }

            if (i == to[0] && i == to[1]) {
                fl2 = true;
            }

            if (board[i][i] != 0) {
                countAllies++;
            }

            if (fl1 && fl2) {
                continue;
            }

            if (fl1 || fl2) {
                countFreeCells++;

                if (board[i][i] != 0 && board[i][i] != player) {
                    enemy = true;
                    break;
                }
            }
        }

        if (enemy || (countFreeCells) != countAllies) {
            return false;
        }

        if ((countFreeCells) == countAllies) {
            return true;
        }

        return false;
    }

    //КОРОЧЕ ПРОВЕРЯЕМЯ ВСЕ ЧТО ВЫШИ ПРАВОЙ ДИАГОНАЛИ ПО ДИАГОНАЛИ
    static bool moveIsDiagonalRightUp(int player, int board[8][8], vector<int> from, vector<int> to) {
        //количество союзников на линии
        int countAllies = 0;

        //кол-во клеток, которые нужно пройти
        int countFreeCells = 0;

        //найде клетка откуда ходим
        bool fl1 = false;

        //найде клетка куда ходим
        bool fl2 = false;

        //на пути есть противник
        bool enemy = false;

        int number = 1;
        bool fl = false;

        if (board[to[1]][to[0]] == player || board[from[1]][from[0]] != player) {
            return false;
        }

        for (int k = 0; k < 8; k++) {
            fl1 = false;
            fl2 = false;
            enemy = false;
            countAllies = 0;
            countFreeCells = 0;

            for (int i = 0; i < 8; i++) {
                int j = i + number;
                if (j < 8) {
                    if (i == from[1] && j == from[0]) {
                        fl1 = true;
                    }

                    if (i == to[1] && j == to[0]) {
                        fl2 = true;
                    }

                    if (board[i][j] != 0) {
                        countAllies++;
                    }

                    if (fl1 && fl2) {
                        fl = true;
                        continue;
                    }

                    if (fl1 || fl2) {
                        countFreeCells++;

                        if (board[i][j] != 0 && board[i][j] != player) {
                            enemy = true;
                            break;
                        }
                    }
                }
            }
            if (fl) {
                break;
            }

            number++;
        }

        if (enemy || (countFreeCells) != countAllies) {
            return false;
        }

        if (fl1 && fl2 && (countFreeCells) == countAllies) {
            return true;
        }

        return false;
    }

    //ТОЖЕ САМОЕ НО ТОЛЬКО ВНИЗ СПРАВА ЕС
    static bool moveIsDiagonalRightDown(int player, int board[8][8], vector<int> from, vector<int> to) {
        //количество союзников на линии
        int countAllies = 0;

        //кол-во клеток, которые нужно пройти
        int countFreeCells = 0;

        //найде клетка откуда ходим
        bool fl1 = false;

        //найде клетка куда ходим
        bool fl2 = false;

        //на пути есть противник
        bool enemy = false;

        int number = 1;
        bool fl = false;

        if (board[to[1]][to[0]] == player || board[from[1]][from[0]] != player) {
            return false;
        }

        for (int k = 0; k < 8; k++) {
            fl1 = false;
            fl2 = false;
            enemy = false;
            countAllies = 0;
            countFreeCells = 0;

            for (int i = 0; i < 8; i++) {
                int j = i + number;
                if (j < 8) {
                    if (i == from[0] && j == from[1]) {
                        fl1 = true;
                    }

                    if (i == to[0] && j == to[1]) {
                        fl2 = true;
                    }

                    if (board[j][i] != 0) {
                        countAllies++;
                    }

                    if (fl1 && fl2) {
                        fl = true;
                        continue;
                    }

                    if (fl1 || fl2) {
                        countFreeCells++;

                        if (board[j][i] != 0 && board[j][i] != player) {
                            enemy = true;
                            break;
                        }
                    }
                }
            }
            if (fl) {
                break;
            }

            number++;
        }

        if (enemy || (countFreeCells) != countAllies) {
            return false;
        }

        if (fl1 && fl2 && (countFreeCells) == countAllies) {
            return true;
        }

        return false;
    }

    //ДИАГОНАЛЬ СНИЗУ ВВЕРХ КОРОЧЕ ЛЕВО
    static bool moveIsDiagonalLeft(int player, int board[8][8], vector<int> from, vector<int> to) {
        //количество союзников на линии
        int countAllies = 0;

        //кол-во клеток, которые нужно пройти
        int countFreeCells = 0;

        //найде клетка откуда ходим
        bool fl1 = false;

        //найде клетка куда ходим
        bool fl2 = false;

        //на пути есть противник
        bool enemy = false;

        if (board[to[1]][to[0]] == player || board[from[1]][from[0]] != player) {
            return false;
        }

        for (int i = 0; i < 8; i++) {
            int col = 7 - i;
            if (i == from[1] && col == from[0]) {
                fl1 = true;
            }

            if (i == to[1] && col == to[0]) {
                fl2 = true;
            }

            if (board[i][col] != 0) {
                countAllies++;
            }

            if (fl1 && fl2) {
                continue;
            }

            if (fl1 || fl2) {
                countFreeCells++;

                if (board[i][col] != 0 && board[i][col] != player) {
                    enemy = true;
                    break;
                }
            }
        }

        if (enemy || (countFreeCells) != countAllies) {
            return false;
        }

        if (fl1 && fl2 && (countFreeCells) == countAllies) {
            return true;
        }

        return false;
    }

    static bool moveIsDiagonaLeftUp(int player, int board[8][8], vector<int> from, vector<int> to) {
        //количество союзников на линии
        int countAllies = 0;

        //кол-во клеток, которые нужно пройти
        int countFreeCells = 0;

        //найде клетка откуда ходим
        bool fl1 = false;

        //найде клетка куда ходим
        bool fl2 = false;

        //на пути есть противник
        bool enemy = false;

        int number = 1;
        bool fl = false;

        if (board[to[1]][to[0]] == player || board[from[1]][from[0]] != player) {
            return false;
        }

        for (int k = 0; k < 8; k++) {
            fl1 = false;
            fl2 = false;
            enemy = false;
            countAllies = 0;
            countFreeCells = 0;

            for (int i = 0; i < 8; i++) {
                int j = 7 - i - number;
                if (j >= 0) {
                    if (i == from[1] && j == from[0]) {
                        fl1 = true;
                    }

                    if (i == to[1] && j == to[0]) {
                        fl2 = true;
                    }

                    if (board[i][j] != 0) {
                        countAllies++;
                    }

                    if (fl1 && fl2) {
                        fl = true;
                        continue;
                    }

                    if (fl1 || fl2) {
                        countFreeCells++;

                        if (board[i][j] != 0 && board[i][j] != player) {
                            enemy = true;
                            break;
                        }
                    }
                }
            }
            if (fl) {
                break;
            }

            number++;
        }

        if (enemy || (countFreeCells) != countAllies) {
            return false;
        }

        if (fl1 && fl2 && (countFreeCells) == countAllies) {
            return true;
        }

        return false;
    }

    static bool moveIsDiagonalLeftDown(int player, int board[8][8], vector<int> from, vector<int> to) {
        //количество союзников на линии
        int countAllies = 0;

        //кол-во клеток, которые нужно пройти
        int countFreeCells = 0;

        //найде клетка откуда ходим
        bool fl1 = false;

        //найде клетка куда ходим
        bool fl2 = false;

        //на пути есть противник
        bool enemy = false;

        int number = 1;
        bool fl = false;

        if (board[to[1]][to[0]] == player || board[from[1]][from[0]] != player) {
            return false;
        }

        for (int k = 0; k < 8; k++) {
            fl1 = false;
            fl2 = false;
            enemy = false;
            countAllies = 0;
            countFreeCells = 0;

            for (int i = 0; i < 8; i++) {
                int col = 7 - i;
                int j = i + number;
                if (j < 8) {
                    if (col == from[0] && j == from[1]) {
                        fl1 = true;
                    }

                    if (col == to[0] && j == to[1]) {
                        fl2 = true;
                    }

                    if (board[j][col] != 0) {
                        countAllies++;
                    }

                    if (fl1 && fl2) {
                        fl = true;
                        continue;
                    }

                    if (fl1 || fl2) {
                        countFreeCells++;

                        if (board[j][col] != 0 && board[j][col] != player) {
                            enemy = true;
                            break;
                        }
                    }
                }
            }
            if (fl) {
                break;
            }

            number++;
        }

        if (enemy || (countFreeCells) != countAllies) {
            return false;
        }

        if (fl1 && fl2 && (countFreeCells) == countAllies) {
            return true;
        }

        return false;
    }

    static int countPlayers(int player, int board[8][8]) {
        int count = 0;

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (player == board[i][j]) {
                    count++;
                }
            }
        }

        return count;
    }

    static Result getWeightPile(int player, int board[8][8]) {
        vector<Result> vectorResults{};
        Result res;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (!isPointInPiles(vectorResults, vector<int>{i, j}) &&
                    board[i][j] == player) {
                    vectorResults.push_back(getPile(player, board, vector<int>{i, j}));
                }
            }
        }

        for (Result i: vectorResults) {
            if (i.pointsOfPile.size() >= res.pointsOfPile.size()) {
                res = i;
            }
        }

        return res;
    }

    static Result getPile(int player, int board[8][8], vector<int> cord) {
        vector<int> tmpCord = cord;
        vector<int> tmpCord2 = cord;
        Result res;
        map<vector<int>, vector<int> > tempVector;

        do {
            if (res.pointsOfPile.find(tmpCord2) == res.pointsOfPile.end()) {
                tempVector[tmpCord2] = vector<int>{0, tmpCord[0], tmpCord[1]};
            }

            res.pointsOfPile = tempVector;
            tmpCord = getNextPoint(tmpCord2, res, board, player);

            if (tmpCord2 == tmpCord) {
                tempVector.at(tmpCord2)[0] = 1;
                tmpCord[0] = tempVector.at(tmpCord2)[1];
                tmpCord[1] = tempVector.at(tmpCord2)[2];
            }

            if (res.pointsOfPile.find(tmpCord) == res.pointsOfPile.end()) {
                tempVector[tmpCord] = vector<int>{0, tmpCord2[0], tmpCord2[1]};
            }

            res.pointsOfPile = tempVector;
            tmpCord2 = getNextPoint(tmpCord, res, board, player);

            if (tmpCord2 == tmpCord) {
                tempVector.at(tmpCord)[0] = 1;
                tmpCord2[0] = tempVector.at(tmpCord)[1];
                tmpCord2[1] = tempVector.at(tmpCord)[2];
            }
        } while (res.pointsOfPile.at(cord)[0] != 1);

        res.pointsOfPile = tempVector;
        return res;
    }

    static vector<int> getNextPoint(vector<int> cord, Result result, int board[8][8], int player) {
        vector<int> tmpCord{};

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                tmpCord = {cord[0] + i, cord[1] + j};

                if (board[tmpCord[0]][tmpCord[1]] == player &&
                    result.pointsOfPile.find(tmpCord) == result.pointsOfPile.end() &&
                    !isOutOfBounds(tmpCord)) {
                    return tmpCord;
                }
            }
        }

        return cord;
    }

    static bool isPointInPiles(vector<Result> piles, vector<int> cord) {
        for (Result i: piles) {
            if (i.pointsOfPile.find(cord) != i.pointsOfPile.end()) {
                return true;
            }
        }

        return false;
    }

    static bool isOutOfBounds(vector<int> cord) {
        if ((cord[0] >= 0 && cord[0] < 8) &&
            (cord[1] >= 0 && cord[1] < 8)) {
            return false;
        }

        return true;
    }

    static vector<vector<int>> getPerfectMove(int player, int board[8][8], Result result) {
        vector<vector<int>> points = result.allPlayersPoints;
        vector<vector<int>> pile;
        map<vector<int>, vector<int>> goodPoints;

        vector<int> tmp;

        for (const auto& key : result.pointsOfPile) {
            pile.push_back(key.first);
        }

        for (int index = 0; index < 8; index++) {
            for (const vector<int>& point : points) {
                if (!isPointInPile(point, pile)) {
                    tmp = getDataPoint(point, pile, board, player, index);

                    if (tmp.size() == 2 && !etoZaciklyvanie(vector<vector<int>> {point, tmp})) {
                        return vector<vector<int>> {point, tmp};
                    }
                }
            }
        }

        for (const vector<int>& point : points) {
            if (!isPointInPile(point, pile)) {
                tmp = getDataPoint(point, pile, board, player, 9);

                if (tmp.size() == 2 && !etoZaciklyvanie(vector<vector<int>> {point, tmp})) {
                    return vector<vector<int>> {point, tmp};
                }
            }
        }
        
        for (const vector<int>& point : points) {
            if (!isPointInPile(point, pile)) {
                tmp = lastChance(player, board, point);

                if (tmp.size() == 2 && !etoZaciklyvanie(vector<vector<int>> {point, tmp})) {
                    return vector<vector<int>> {point, tmp};
                }
            }
        }

        return vector<vector<int>> {{8, 8}, {8, 8}};
    }

    static vector<int> getDataPoint(vector<int> point, vector<vector<int>> pile, int board[8][8], int player, int indexOfRad) {

        for (vector<int> pointInPile : pile) {
            vector<vector<int>> radius {
                {-1, 0, 1}, {-2, -1, 0, 1, 2}, {-3, -2, -1, 0, 1, 2, 3}, {-4, -3, -2, -1, 0, 1, 2, 3, 4},
                {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5}, {-6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6},
                {-7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7},
                {-8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8}
            };

            if (indexOfRad == 9){
                for (const vector<int>& rad : radius) {
                    return getTmpCord(player, rad, pointInPile, point, board);
                }
            } 
            else {
                return getTmpCord(player, radius[indexOfRad], pointInPile, point, board);
            }
        }

        return vector<int> {};
    }

    static vector<int> getTmpCord(int player, vector<int> rad, vector<int> pointInPile, vector<int> point, int board[8][8]) {
        vector<int> tmpCord{};

        for (const auto& i : rad) {
            for (const auto& j : rad) {
                tmpCord = {pointInPile[0] + i, pointInPile[1] + j};

                if (board[tmpCord[0]][tmpCord[1]] != player &&
                    !isOutOfBounds(tmpCord) && moveIsCorrect(player, board, vector<int> {point[1], point[0]}, vector<int> {tmpCord[1], tmpCord[0]})) {
                    return tmpCord;
                }
            }
        }

        return vector<int> {};
    }

    static vector<int> lastChance(int player, int board[8][8], vector<int> point) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                 if (board[i][j] != player &&
                    moveIsCorrect(player, board, vector<int> {point[1], point[0]}, vector<int> {j, i})) {
                    return vector<int> {i, j};
                }
            }
        }
        
        return vector<int> {};
    }

    static bool isPointInPile(vector<int> point, vector<vector<int>> pile) {
        for (const auto& pointInPile : pile) {
            if (point == pointInPile) {
                return true;
            }
        }

        return false;
    }

    static string cordsToMove(vector<int> from, vector<int> to) {
        std::ostringstream oss;

        oss << board_alp[from[1]] << 8 - from[0] << "-" << board_alp[to[1]] << 8 - to[0];

        return oss.str();
    }

    static bool etoZaciklyvanie(vector<vector<int>> move) {
        if (historyOfMoves.size() == 0)
            return false;

        vector<int> tmpFrom = parse_turn(historyOfMoves[historyOfMoves.size() - 1][0]);
        vector<int> tmpTo = parse_turn(historyOfMoves[historyOfMoves.size() - 1][1]);

        std::reverse(tmpFrom.begin(), tmpFrom.end());
        std::reverse(tmpTo.begin(), tmpTo.end());

        if (tmpFrom == move[1] && tmpTo == move[0]) {
            return true;
        }

        return false;
    }

public:
    static bool moveIsCorrect(int player, int board[8][8], vector<int> from, vector<int> to) {
        vector<bool> tests{
                moveIsHorizontal(player, board, from, to),
                moveIsVertically(player, board, from, to),
                moveIsDiagonalLeft(player, board, from, to),
                moveIsDiagonalRight(player, board, from, to),
                moveIsDiagonaLeftUp(player, board, from, to),
                moveIsDiagonalLeftDown(player, board, from, to),
                moveIsDiagonalRightDown(player, board, from, to),
                moveIsDiagonalRightUp(player, board, from, to),
        };

        return std::any_of(tests.begin(), tests.end(), [](bool val) { return val; });
    }

    static int getMaxWeightOfPile(int player, int board[8][8]) {
        return getWeightPile(player, board).pointsOfPile.size();
    }

    static int getMaxWeight(int player, int board[8][8]) {
        return countPlayers(player, board);
    }

    static bool isFinal(int player, int board[8][8]) {
        return countPlayers(player, board) == getWeightPile(player, board).pointsOfPile.size() ||
               countPlayers(0 - player, board) == getWeightPile(0 - player, board).pointsOfPile.size();
    }

    static string getMove(int player, int board[8][8], Result result) {
        vector<vector<int>> cords = getPerfectMove(player, board, result);
        return cordsToMove(cords[0], cords[1]);
    }

    static Result getResult(int player, int board[8][8]) {
        return getWeightPile(player, board);
    }

    static Result addAllPlayerPointInResult(int player, int board[8][8], Result result) {
        Result res = std::move(result);

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (player == 1) {
                    if (board[j][i] == player) {
                        res.allPlayersPoints.push_back(vector<int> {j, i});
                    }
                }
                else {
                    if (board[i][j] == player) {
                        res.allPlayersPoints.push_back(vector<int> {i, j});
                    }
                }
            }
        }

        return res;
    }
};

int main() {
    int board[8][8];
    fill_matrix(board);

    int player;
    string hod;
    int max_hod;

    cin >> max_hod;
    cin >> player;
    bool we;

    if (player == 0) {
        player = 1;
        we = true;
    }
    else {
        player = 1;
        we = false;
    }


    while (true) {
        Result res = Checker::getResult(player, board);
        res = Checker::addAllPlayerPointInResult(player, board, res);

        if (we) {
            hod = Checker::getMove(player, board, res);
            if (Checker::moveIsCorrect(player, board, parse_turn(hod.substr(0, 2)), parse_turn(hod.substr(3, 2)))) {
                make_turn(player, board, hod.substr(0, 2), hod.substr(3, 2));

                //{{"A2", "B2"}, {"A2", "B2"}}
                historyOfMoves.push_back(vector<string> {hod.substr(0, 2), hod.substr(3, 2)});
            }

            cout << hod << endl;
        }
        else {
            hod = "";
            cin >> hod;
            make_turn(player, board, hod.substr(0, 2), hod.substr(3, 2));
        }

        if (player == 1) {
            player = -1;
        }
        else {
            player = 1;
        }

        we = !we;
    }

    return 0;
}
