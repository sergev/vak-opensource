//
// Build as: c++ -std=c++17 arcanoid.cpp -o arcanoid
//
#include <array>
#include <iostream>
#include <string_view>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

namespace {

constexpr int kRows = 8;
constexpr int kCols = 8;
constexpr int kPaddleWidth = 4;

constexpr int kTickMs = 250;

constexpr std::array<std::string_view, 5> kBlocks = {
    "\u2591",
    "\u2592",
    "\u2593",
    "\u2594",
    "\u25CF",
};

struct TerminalSize {
    int rows;
    int cols;
};

TerminalSize get_terminal_size() {
    winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
        return {24, 80};
    }
    return {w.ws_row, w.ws_col};
}

std::string_view get_background(int row, int col) {
    const bool even_cell = (row + col) % 2 == 0;
    return even_cell ? kBlocks[1] : kBlocks[0];
}

bool is_paddle(int row, int col, int paddle_x) {
    return row == kRows - 1 &&
           col >= paddle_x &&
           col < paddle_x + kPaddleWidth;
}

struct Ball {
    int row;
    int col;
    int dr;
    int dc;
};

void draw_scene(int paddle_x, const Ball& ball, bool game_over) {
    std::cout << "\x1b[2J\x1b[H";

    for (int row = 0; row < kRows; ++row) {
        for (int col = 0; col < kCols; ++col) {
            if (row == ball.row && col == ball.col) {
                std::cout << kBlocks[4];                 // ball
            } else if (is_paddle(row, col, paddle_x)) {
                std::cout << kBlocks[2];                 // paddle
            } else {
                std::cout << get_background(row, col);   // backdrop
            }
        }
        std::cout << '\n';
    }

    if (game_over) {
        std::cout << "\nGAME OVER - press q to quit.\n";
    } else {
        std::cout << "\nUse \u2190 / \u2192 to move, q to quit.\n";
    }
    std::cout.flush();
}

class RawMode {
public:
    RawMode() {
        if (tcgetattr(STDIN_FILENO, &original_) == 0) {
            termios raw = original_;
            raw.c_lflag &= ~(ICANON | ECHO);
            raw.c_cc[VMIN] = 0;
            raw.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &raw);
            active_ = true;
        }
    }
    ~RawMode() {
        if (active_) {
            tcsetattr(STDIN_FILENO, TCSANOW, &original_);
        }
    }
    RawMode(const RawMode&) = delete;
    RawMode& operator=(const RawMode&) = delete;

private:
    termios original_{};
    bool active_ = false;
};

enum class Key { None, Left, Right, Quit, Other };

Key read_key_nonblocking() {
    char c = 0;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n <= 0) return Key::None;

    if (c == 'q' || c == 'Q') return Key::Quit;

    if (c == '\x1b') {
        char seq[2] = {0, 0};
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return Key::Other;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return Key::Other;
        if (seq[0] == '[') {
            if (seq[1] == 'D') return Key::Left;
            if (seq[1] == 'C') return Key::Right;
        }
        return Key::Other;
    }
    return Key::Other;
}

bool wait_for_input(int timeout_ms) {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    timeval tv{};
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    int rv = select(STDIN_FILENO + 1, &rfds, nullptr, nullptr, &tv);
    return rv > 0;
}

long now_ms() {
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000L + ts.tv_nsec / 1'000'000L;
}

bool step_ball(Ball& ball, int paddle_x) {
    int next_row = ball.row + ball.dr;
    int next_col = ball.col + ball.dc;

    if (next_col < 0) {
        next_col = 0;
        ball.dc = 1;
    } else if (next_col >= kCols) {
        next_col = kCols - 1;
        ball.dc = -1;
    }

    if (next_row < 0) {
        next_row = 0;
        ball.dr = 1;
    }

    if (next_row == kRows - 1) {
        const bool on_paddle =
            next_col >= paddle_x && next_col < paddle_x + kPaddleWidth;
        if (on_paddle) {
            next_row = kRows - 2;
            ball.dr = -1;
        }
    }

    ball.row = next_row;
    ball.col = next_col;

    return ball.row < kRows;
}

}  // namespace

int main() {
    const TerminalSize ts = get_terminal_size();
    if (ts.cols < kCols || ts.rows < kRows + 2) {
        std::cerr << "Terminal is a bit small (" << ts.rows << "x"
                  << ts.cols << "), but we'll try anyway.\n";
    }

    RawMode raw;

    int paddle_x = (kCols - kPaddleWidth) / 2;
    Ball ball{0, kCols / 2, 1, 1};

    bool game_over = false;
    draw_scene(paddle_x, ball, game_over);

    long last_tick = now_ms();

    while (true) {
        const long now = now_ms();
        const long elapsed = now - last_tick;
        const int wait_ms = static_cast<int>(
            elapsed >= kTickMs ? 0 : (kTickMs - elapsed));

        const bool input_ready =
            wait_ms > 0 ? wait_for_input(wait_ms) : false;

        bool need_redraw = false;

        if (input_ready) {
            while (true) {
                Key k = read_key_nonblocking();
                if (k == Key::None) break;
                if (k == Key::Quit) {
                                    std::cout << "\x1b[2J\x1b[H";
                    std::cout.flush();
                    return 0;
                }
                if (game_over) continue;
                if (k == Key::Left && paddle_x > 0) {
                    --paddle_x;
                    need_redraw = true;
                } else if (k == Key::Right &&
                           paddle_x + kPaddleWidth < kCols) {
                    ++paddle_x;
                    need_redraw = true;
                }
            }
        }

        if (!game_over && now_ms() - last_tick >= kTickMs) {
            last_tick = now_ms();
            if (!step_ball(ball, paddle_x)) {
                game_over = true;
            }
            need_redraw = true;
        }

        if (need_redraw) {
            draw_scene(paddle_x, ball, game_over);
        }
    }
}
