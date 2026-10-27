#include <iostream>
#include <vector>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>
using namespace std;

const string SNAKE_HEAD = "👀";
const string SNAKE_BODY = "🩷";
const string FOOD_ICON  = "🪱";
const string TILE       = "⬜";
const string BORDER     = "🟫";

const int WIDTH = 20;
const int HEIGHT = 12;

struct Position {
    int x, y;
    bool operator==(const Position &o) const { return x == o.x && y == o.y; }
};

class Snake {
public:
    enum Dir { UP, DOWN, LEFT, RIGHT };
private:
    vector<Position> body;
    Dir dir;
public:
    Snake() { reset(); }
    void reset() {
        body.clear();
        int midX = WIDTH / 2, midY = HEIGHT / 2;
        body.push_back({midX, midY});
        body.push_back({midX - 1, midY});
        body.push_back({midX - 2, midY});
        dir = RIGHT;
    }
    Position head() const { return body.front(); }
    const vector<Position>& getBody() const { return body; }
    void setDir(Dir d) {
        if ((dir == UP && d == DOWN) || (dir == DOWN && d == UP) ||
            (dir == LEFT && d == RIGHT) || (dir == RIGHT && d == LEFT))
            return;
        dir = d;
    }
    void setDirFromKey(char k) {
        if (k == 'w' || k == 'W') setDir(UP);
        else if (k == 's' || k == 'S') setDir(DOWN);
        else if (k == 'a' || k == 'A') setDir(LEFT);
        else if (k == 'd' || k == 'D') setDir(RIGHT);
    }
    void move(bool grow = false) {
        Position nh = head();
        if (dir == UP) nh.y--;
        else if (dir == DOWN) nh.y++;
        else if (dir == LEFT) nh.x--;
        else if (dir == RIGHT) nh.x++;
        body.insert(body.begin(), nh);
        if (!grow) body.pop_back();
    }
    bool collidesWithSelf() const {
        Position h = head();
        for (size_t i = 1; i < body.size(); ++i)
            if (body[i] == h) return true;
        return false;
    }
    bool occupies(const Position &p) const {
        for (auto &b : body)
            if (b == p) return true;
        return false;
    }
};

class Food {
    Position pos;
public:
    Position get() const { return pos; }
    void spawn(const Snake &s) {
        do {
            pos.x = rand() % WIDTH;
            pos.y = rand() % HEIGHT;
        } while (s.occupies(pos));
    }
};

class Game {
    Snake snake;
    Food food;
    int score = 0;
    int highScore = 0;
    bool running = true;
    bool gameOver = false;
    double speedFactor = 1.0;
    const int baseDelay = 180000; // start slow (microseconds)

    struct termios oldt;

    void hideCursor() { cout << "\033[?25l"; }
    void showCursor() { cout << "\033[?25h"; }
    void clearScreen() { cout << "\033[2J"; }
    void moveCursorTopLeft() { cout << "\033[H"; }

    void enableNonBlocking() {
        tcgetattr(STDIN_FILENO, &oldt);
        struct termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    }
    void disableNonBlocking() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    char readKey() {
        char ch = 0;
        ssize_t r = read(STDIN_FILENO, &ch, 1);
        if (r <= 0) return 0;
        return ch;
    }

    char readControl() {
        char ch = readKey();
        if (!ch) return 0;
        if (ch == '\033') {
            char ch1 = readKey();
            if (!ch1) return 0;
            if (ch1 == '[') {
                char ch2 = readKey();
                if (!ch2) return 0;
                if (ch2 == 'A') return 'U';
                if (ch2 == 'B') return 'D';
                if (ch2 == 'C') return 'R';
                if (ch2 == 'D') return 'L';
            }
            return 0;
        } else return ch;
    }

    void drawBoard() {
        moveCursorTopLeft();
        for (int i = 0; i < WIDTH + 2; i++) cout << BORDER;
        cout << '\n';
        for (int y = 0; y < HEIGHT; y++) {
            cout << BORDER;
            for (int x = 0; x < WIDTH; x++) {
                Position p{x, y};
                if (p == snake.head()) cout << SNAKE_HEAD;
                else if (snake.occupies(p)) cout << SNAKE_BODY;
                else if (p == food.get()) cout << FOOD_ICON;
                else cout << TILE;
            }
            cout << BORDER << '\n';
        }
        for (int i = 0; i < WIDTH + 2; i++) cout << BORDER;
        cout << "\nScore: " << score << "   High Score: " << highScore << '\n';
    }

    void processInput() {
        char control = readControl();
        if (!control) return;
        if (control == 'U') snake.setDir(Snake::UP);
        else if (control == 'D') snake.setDir(Snake::DOWN);
        else if (control == 'L') snake.setDir(Snake::LEFT);
        else if (control == 'R') snake.setDir(Snake::RIGHT);
        else if (control == 'w' || control == 'W') snake.setDir(Snake::UP);
        else if (control == 's' || control == 'S') snake.setDir(Snake::DOWN);
        else if (control == 'a' || control == 'A') snake.setDir(Snake::LEFT);
        else if (control == 'd' || control == 'D') snake.setDir(Snake::RIGHT);
        else if (control == 'q' || control == 'Q') running = false;
        else if (control == 'r' || control == 'R') restart();
    }

    void update() {
        snake.move();
        Position h = snake.head();

        if (h.x < 0 || h.x >= WIDTH || h.y < 0 || h.y >= HEIGHT || snake.collidesWithSelf()) {
            gameOver = true;
            return;
        }

        if (h == food.get()) {
            score++;
            snake.move(true);
            food.spawn(snake);
            if (score > highScore) highScore = score;
            if (speedFactor > 0.3) speedFactor *= 0.95; // speed increases gradually
        }
    }

    void restart() {
        snake.reset();
        score = 0;
        gameOver = false;
        speedFactor = 1.0;
        food.spawn(snake);
    }

public:
    Game() { srand(time(0)); }
    void run() {
        enableNonBlocking();
        hideCursor();
        clearScreen();
        restart();

        while (running) {
            while (!gameOver && running) {
                processInput(); // instant response
                update();
                drawBoard();
                usleep((int)(baseDelay * speedFactor));
            }

            if (!running) break;

            // Show Game Over only once
            cout << "\nGAME OVER \n";
            cout << "Final Score: " << score << "   High Score: " << highScore << '\n';
            cout << "Press R to restart or Q to quit.\n";

            // Wait for player choice
            bool choiceMade = false;
            while (!choiceMade && running) {
                char c = readControl();
                if (!c) { usleep(250000); continue; }
                if (c == 'r' || c == 'R') { restart(); choiceMade = true; }
                else if (c == 'q' || c == 'Q') { running = false; choiceMade = true; }
            }
        }

        showCursor();
        disableNonBlocking();
        cout << "\nThanks for playing! Final Score: " << score << "\n";
    }
};

int main() {
    Game g;
    g.run();
    return 0;
}
