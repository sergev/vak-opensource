//
// Simple ANSI terminal emulator.
//
// Copyright (c) 2025 Serge Vakulenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#ifdef __linux__
#include <pty.h>
#else // Assume macOS or other POSIX systems
#include <util.h>
#endif

pid_t child_pid        = 0;  // Global to handle signals
int master_fd          = -1; // Global for SIGWINCH handler
SDL_Window *window     = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *font         = nullptr;
int char_width = 0, char_height = 0;       // Font character dimensions
int term_cols                      = 80;   // Current terminal columns
int term_rows                      = 24;   // Current terminal rows
bool cursor_visible                = true; // Cursor visibility for blinking
Uint32 last_cursor_toggle          = 0;    // Time of last cursor toggle
const Uint32 cursor_blink_interval = 500;  // Blink interval in ms

// Structure for character attributes
struct CharAttr {
    SDL_Color fg = { 255, 255, 255, 255 }; // Foreground color (default white)
    SDL_Color bg = { 0, 0, 0, 255 };       // Background color (default black)

    bool operator==(const CharAttr &other) const
    {
        return fg.r == other.fg.r && fg.g == other.fg.g && fg.b == other.fg.b &&
               fg.a == other.fg.a && bg.r == other.bg.r && bg.g == other.bg.g &&
               bg.b == other.bg.b && bg.a == other.bg.a;
    }
};

// Structure for a single character with attributes
struct Char {
    char ch = ' ';
    CharAttr attr;
};

// Structure for a span of characters with the same attributes
struct TextSpan {
    std::string text;
    CharAttr attr;
    int start_col;
    SDL_Texture *texture = nullptr;
};

// Text buffer: vector of lines, each a vector of Char
std::vector<std::vector<Char>> text_buffer;
std::vector<std::vector<TextSpan>> texture_cache; // Cached textures for each line (spans)
std::vector<bool> dirty_lines;                    // Flags for lines needing texture updates

// Cursor position
struct Cursor {
    int row = 0;
    int col = 0;
} cursor;

// Current text attributes
CharAttr current_attr;

// ANSI parsing states
enum class AnsiState {
    NORMAL,
    ESCAPE,
    CSI,
};

// ANSI colors
const SDL_Color ansi_colors[] = {
    { 0, 0, 0, 255 },       // Black
    { 255, 0, 0, 255 },     // Red
    { 0, 255, 0, 255 },     // Green
    { 255, 255, 0, 255 },   // Yellow
    { 0, 0, 255, 255 },     // Blue
    { 255, 0, 255, 255 },   // Magenta
    { 0, 255, 255, 255 },   // Cyan
    { 255, 255, 255, 255 }, // White
};

// Signal handler to forward signals to child
void forward_signal(int sig)
{
    if (child_pid > 0) {
        kill(child_pid, sig);
    }
}

// Update slave terminal size based on window dimensions
void update_terminal_size()
{
    if (master_fd == -1) {
        std::cerr << "update_terminal_size: master_fd invalid" << std::endl;
        return;
    }

    int win_width, win_height;
    SDL_GetWindowSize(window, &win_width, &win_height);

    // Calculate terminal dimensions in characters
    int new_cols = win_width / char_width;
    int new_rows = win_height / char_height;

    // Ensure minimum size
    new_cols = std::max(new_cols, 1);
    new_rows = std::max(new_rows, 1);

    // Update terminal size if changed
    if (new_cols != term_cols || new_rows != term_rows) {
        term_cols = new_cols;
        term_rows = new_rows;

        struct winsize ws;
        ws.ws_col    = term_cols;
        ws.ws_row    = term_rows;
        ws.ws_xpixel = term_cols * char_width;
        ws.ws_ypixel = term_rows * char_height;

        if (ioctl(master_fd, TIOCSWINSZ, &ws) == -1) {
            std::cerr << "Error setting slave window size: " << strerror(errno) << std::endl;
            return;
        }

        //std::cerr << "Updated slave size to " << ws.ws_col << "x" << ws.ws_row << std::endl;

        // Notify child process
        if (child_pid > 0) {
            kill(child_pid, SIGWINCH);
        }

        // Adjust text buffer and texture cache
        text_buffer.resize(term_rows, std::vector<Char>(term_cols, { ' ', current_attr }));
        for (auto &line : text_buffer) {
            line.resize(term_cols, { ' ', current_attr });
        }
        for (auto &line_spans : texture_cache) {
            for (auto &span : line_spans) {
                if (span.texture)
                    SDL_DestroyTexture(span.texture);
            }
        }
        texture_cache.resize(term_rows);
        dirty_lines.resize(term_rows, true);

        // Clamp cursor position
        cursor.row = std::min(cursor.row, term_rows - 1);
        cursor.col = std::min(cursor.col, term_cols - 1);
    }
}

// Signal handler for SIGWINCH
void handle_sigwinch(int sig)
{
    update_terminal_size();
}

// Render text buffer to SDL window with per-character colors and backgrounds
void render_text()
{
    // Update cursor blinking
    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_cursor_toggle >= cursor_blink_interval) {
        cursor_visible     = !cursor_visible;
        last_cursor_toggle = current_time;
    }

    // Update textures for dirty lines
    for (size_t i = 0; i < text_buffer.size(); ++i) {
        if (!dirty_lines[i])
            continue; // Skip unchanged lines

        // Destroy old textures for this line
        for (auto &span : texture_cache[i]) {
            if (span.texture)
                SDL_DestroyTexture(span.texture);
        }
        texture_cache[i].clear();

        // Build spans of characters with the same attributes
        std::string current_text;
        CharAttr current_span_attr = text_buffer[i][0].attr;
        int start_col              = 0;

        for (int j = 0; j < term_cols; ++j) {
            const auto &c = text_buffer[i][j];
            if (c.attr == current_span_attr && j < term_cols - 1) {
                current_text += c.ch;
            } else {
                if (!current_text.empty()) {
                    TextSpan span;
                    span.text      = current_text;
                    span.attr      = current_span_attr;
                    span.start_col = start_col;

                    // Render texture for the span
                    SDL_Surface *surface =
                        TTF_RenderText_Blended(font, current_text.c_str(), current_span_attr.fg);
                    if (surface) {
                        span.texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_FreeSurface(surface);
                    }
                    texture_cache[i].push_back(span);
                }
                current_text      = c.ch;
                current_span_attr = c.attr;
                start_col         = j;
            }
        }

        // Handle the last span
        if (!current_text.empty()) {
            TextSpan span;
            span.text      = current_text;
            span.attr      = current_span_attr;
            span.start_col = start_col;

            SDL_Surface *surface =
                TTF_RenderText_Blended(font, current_text.c_str(), current_span_attr.fg);
            if (surface) {
                span.texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
            }
            texture_cache[i].push_back(span);
        }

        dirty_lines[i] = false; // Mark as clean
    }

    // Render to back buffer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Copy cached textures with background rectangles
    for (size_t i = 0; i < text_buffer.size() && i < static_cast<size_t>(term_rows); ++i) {
        for (const auto &span : texture_cache[i]) {
            if (!span.texture)
                continue;

            // Draw background rectangle
            SDL_SetRenderDrawColor(renderer, span.attr.bg.r, span.attr.bg.g, span.attr.bg.b,
                                   span.attr.bg.a);
            SDL_Rect bg_rect = { span.start_col * char_width, static_cast<int>(i * char_height),
                                 static_cast<int>(span.text.length() * char_width), char_height };
            SDL_RenderFillRect(renderer, &bg_rect);

            // Draw text texture
            int w, h;
            SDL_QueryTexture(span.texture, nullptr, nullptr, &w, &h);
            SDL_Rect dst = { span.start_col * char_width, static_cast<int>(i * char_height), w, h };
            SDL_RenderCopy(renderer, span.texture, nullptr, &dst);
        }
    }

    // Draw blinking cursor if visible
    if (cursor_visible && cursor.row < term_rows && cursor.col < term_cols) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White cursor
        SDL_Rect cursor_rect = { cursor.col * char_width, cursor.row * char_height, char_width,
                                 char_height };
        SDL_RenderFillRect(renderer, &cursor_rect);
    }

    // Present back buffer (swap with front buffer)
    SDL_RenderPresent(renderer);
}

// Parse and process ANSI escape sequences
void process_ansi_sequence(const std::string &seq, char final_char)
{
    if (final_char == 'c') { // ESC c: Full reset and clear screen
        //std::cerr << "Processing ESC c: Resetting terminal state" << std::endl;
        // Reset attributes
        current_attr = CharAttr();
        // Reset cursor
        cursor.row = 0;
        cursor.col = 0;
        // Clear screen
        for (int r = 0; r < term_rows; ++r) {
            text_buffer[r] = std::vector<Char>(term_cols, { ' ', current_attr });
            dirty_lines[r] = true;
            // Clear texture cache for this line
            for (auto &span : texture_cache[r]) {
                if (span.texture)
                    SDL_DestroyTexture(span.texture);
            }
            texture_cache[r].clear();
        }
        return;
    }

    if (seq.empty() || seq[0] != '[') {
        std::cerr << "Invalid CSI sequence: " << seq << final_char << std::endl;
        return;
    }

    //std::cerr << "Processing CSI sequence: " << seq << final_char << std::endl;

    std::vector<int> params;
    std::string param_str;

    // Extract parameters safely
    for (size_t i = 1; i < seq.size() - 1; ++i) {
        if (std::isdigit(seq[i])) {
            param_str += seq[i];
        } else if (seq[i] == ';' || i == seq.size() - 2) {
            if (!param_str.empty()) {
                try {
                    params.push_back(std::stoi(param_str));
                } catch (const std::exception &e) {
                    std::cerr << "Error parsing parameter '" << param_str << "': " << e.what()
                              << std::endl;
                    params.push_back(0); // Default to 0 on error
                }
                param_str.clear();
            } else {
                params.push_back(0);
            }
        }
    }
    if (!param_str.empty()) {
        try {
            params.push_back(std::stoi(param_str));
        } catch (const std::exception &e) {
            std::cerr << "Error parsing final parameter '" << param_str << "': " << e.what()
                      << std::endl;
            params.push_back(0);
        }
    }

    // Handle CSI sequences
    switch (final_char) {
    case 'm': // SGR (Select Graphic Rendition)
        for (size_t i = 0; i < params.size(); ++i) {
            int p = params[i];
            if (p == 0) { // Reset
                current_attr = CharAttr();
            } else if (p >= 30 && p <= 37) { // Foreground color
                current_attr.fg = ansi_colors[p - 30];
            } else if (p >= 40 && p <= 47) { // Background color
                current_attr.bg = ansi_colors[p - 40];
            } else if (p >= 90 && p <= 97) { // Bright foreground
                current_attr.fg = ansi_colors[p - 90];
            } else if (p >= 100 && p <= 107) { // Bright background
                current_attr.bg = ansi_colors[p - 100];
            }
        }
        break;

    case 'H': // CUP (Cursor Position)
    {
        int row    = (params.size() > 0 ? params[0] : 1) - 1;
        int col    = (params.size() > 1 ? params[1] : 1) - 1;
        cursor.row = std::max(0, std::min(row, term_rows - 1));
        cursor.col = std::max(0, std::min(col, term_cols - 1));
        break;
    }

    case 'A': // CUU (Cursor Up)
        cursor.row = std::max(0, cursor.row - (params.empty() ? 1 : params[0]));
        break;

    case 'B': // CUD (Cursor Down)
        cursor.row = std::min(term_rows - 1, cursor.row + (params.empty() ? 1 : params[0]));
        break;

    case 'C': // CUF (Cursor Forward)
        cursor.col = std::min(term_cols - 1, cursor.col + (params.empty() ? 1 : params[0]));
        break;

    case 'D': // CUB (Cursor Backward)
        cursor.col = std::max(0, cursor.col - (params.empty() ? 1 : params[0]));
        break;

    case 'J': // ED (Erase in Display)
    {
        int mode = params.empty() ? 0 : params[0];
        if (mode == 0 || mode == 2) { // Clear from cursor to end or entire screen
            for (int r = cursor.row; r < term_rows; ++r) {
                for (int c = (r == cursor.row ? cursor.col : 0); c < term_cols; ++c) {
                    text_buffer[r][c] = { ' ', current_attr };
                }
                dirty_lines[r] = true;
            }
        }
        break;
    }

    case 'K': // EL (Erase in Line)
    {
        int mode = params.empty() ? 0 : params[0];
        //std::cerr << "Processing ESC [ " << mode << "K" << std::endl;
        switch (mode) {
        case 0: // Clear from cursor to end of line
            for (int c = cursor.col; c < term_cols; ++c) {
                text_buffer[cursor.row][c] = { ' ', current_attr };
            }
            break;
        case 1: // Clear from start of line to cursor
            for (int c = 0; c <= cursor.col; ++c) {
                text_buffer[cursor.row][c] = { ' ', current_attr };
            }
            break;
        case 2: // Clear entire line
            for (int c = 0; c < term_cols; ++c) {
                text_buffer[cursor.row][c] = { ' ', current_attr };
            }
            break;
        default:
            std::cerr << "Unknown EL mode: " << mode << std::endl;
            break;
        }
        dirty_lines[cursor.row] = true;
        break;
    }
    }
}

int main()
{
    // Initialize SDL2 and SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Load font (adjust path as needed)
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 16);
    if (!font) {
        // Fallback for macOS
        font = TTF_OpenFont("/System/Library/Fonts/Menlo.ttc", 16);
    }
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Get font metrics
    TTF_SizeText(font, "M", &char_width, &char_height);
    if (char_width == 0 || char_height == 0) {
        std::cerr << "Failed to get font metrics" << std::endl;
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create SDL window
    window = SDL_CreateWindow("Terminal Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              term_cols * char_width, term_rows * char_height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Initialize text buffer and texture cache
    text_buffer.resize(term_rows, std::vector<Char>(term_cols, { ' ', current_attr }));
    texture_cache.resize(term_rows);
    dirty_lines.resize(term_rows, true);

    // Pseudo-terminal setup
    char *slave_name;
    struct termios slave_termios;

    // Open pseudo-terminal
    master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (master_fd == -1) {
        std::cerr << "Error opening pseudo-terminal: " << strerror(errno) << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Grant access to slave
    if (grantpt(master_fd) == -1) {
        std::cerr << "Warning: grantpt failed: " << strerror(errno) << std::endl;
    }

    // Unlock pseudo-terminal
    if (unlockpt(master_fd) == -1) {
        std::cerr << "Warning: unlockpt failed: " << strerror(errno) << std::endl;
    }

    // Get slave name
    slave_name = ptsname(master_fd);
    if (!slave_name) {
        std::cerr << "Error getting slave name: " << strerror(errno) << std::endl;
        close(master_fd);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Set up terminal attributes for slave
    tcgetattr(STDIN_FILENO, &slave_termios);
    slave_termios.c_lflag |= ISIG;          // Enable signal handling
    slave_termios.c_iflag |= ICRNL;         // Map CR to NL on input
    slave_termios.c_oflag |= OPOST | ONLCR; // Enable output processing, map NL to CR-NL

    // Set initial slave window size
    struct winsize ws;
    ws.ws_col    = term_cols;
    ws.ws_row    = term_rows;
    ws.ws_xpixel = term_cols * char_width;
    ws.ws_ypixel = term_rows * char_height;

    // Fork to create child process
    child_pid = fork();
    if (child_pid == -1) {
        std::cerr << "Error forking: " << strerror(errno) << std::endl;
        close(master_fd);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    if (child_pid == 0) { // Child process
        close(master_fd);
        if (setsid() == -1) {
            std::cerr << "Error setting session: " << strerror(errno) << std::endl;
            _exit(1);
        }

        int slave_fd = open(slave_name, O_RDWR);
        if (slave_fd == -1) {
            std::cerr << "Error opening slave: " << strerror(errno) << std::endl;
            _exit(1);
        }

        if (ioctl(slave_fd, TIOCSCTTY, 0) == -1) {
            std::cerr << "Error setting controlling terminal: " << strerror(errno) << std::endl;
            _exit(1);
        }

        if (ioctl(slave_fd, TIOCSWINSZ, &ws) == -1) {
            std::cerr << "Error setting initial slave window size: " << strerror(errno)
                      << std::endl;
            _exit(1);
        }

        if (tcsetattr(slave_fd, TCSANOW, &slave_termios) == -1) {
            std::cerr << "Error setting slave terminal attributes: " << strerror(errno)
                      << std::endl;
            _exit(1);
        }

        dup2(slave_fd, STDIN_FILENO);
        dup2(slave_fd, STDOUT_FILENO);
        dup2(slave_fd, STDERR_FILENO);
        if (slave_fd > 2)
            close(slave_fd);

        execl("/bin/sh", "sh", nullptr);
        std::cerr << "Error executing shell: " << strerror(errno) << std::endl;
        _exit(1);
    }

    // Parent process
    struct sigaction sa;
    sa.sa_handler = forward_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGQUIT, &sa, nullptr);

    sa.sa_handler = handle_sigwinch;
    sigaction(SIGWINCH, &sa, nullptr);

    // Set non-blocking mode for master_fd
    fcntl(master_fd, F_SETFL, O_NONBLOCK);

    bool running = true;
    fd_set read_fds;
    char buffer[1024];
    int status;
    AnsiState state = AnsiState::NORMAL;
    std::string ansi_seq;

    while (running) {
        // Handle SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            } else if (event.type == SDL_KEYDOWN) {
                SDL_KeyboardEvent &key = event.key;
                std::string input;
                Uint16 mod = key.keysym.mod;

                // Log key event for debugging
                //std::cerr << "Key pressed: SDLKey=" << key.keysym.sym << ", Modifiers=" << mod << std::endl;

                switch (key.keysym.sym) {
                case SDLK_RETURN:
                    input = "\n";
                    break;
                case SDLK_BACKSPACE:
                    input = "\b";
                    break;
                case SDLK_TAB:
                    input = "\t";
                    break;
                case SDLK_UP:
                    input = "\033[A";
                    break;
                case SDLK_DOWN:
                    input = "\033[B";
                    break;
                case SDLK_RIGHT:
                    input = "\033[C";
                    break;
                case SDLK_LEFT:
                    input = "\033[D";
                    break;
                case SDLK_HOME:
                    input = "\033[H";
                    break;
                case SDLK_END:
                    input = "\033[F";
                    break;
                case SDLK_INSERT:
                    input = "\033[2~";
                    break;
                case SDLK_DELETE:
                    input = "\033[3~";
                    break;
                case SDLK_PAGEUP:
                    input = "\033[5~";
                    break;
                case SDLK_PAGEDOWN:
                    input = "\033[6~";
                    break;
                case SDLK_F1:
                    input = "\033[11~";
                    break;
                case SDLK_F2:
                    input = "\033[12~";
                    break;
                case SDLK_F3:
                    input = "\033[13~";
                    break;
                case SDLK_F4:
                    input = "\033[14~";
                    break;
                case SDLK_F5:
                    input = "\033[15~";
                    break;
                case SDLK_F6:
                    input = "\033[17~";
                    break;
                case SDLK_F7:
                    input = "\033[18~";
                    break;
                case SDLK_F8:
                    input = "\033[19~";
                    break;
                case SDLK_F9:
                    input = "\033[20~";
                    break;
                case SDLK_F10:
                    input = "\033[21~";
                    break;
                case SDLK_F11:
                    input = "\033[23~";
                    break;
                case SDLK_F12:
                    input = "\033[24~";
                    break;
                default:
                    if (mod & KMOD_CTRL) {
                        // Handle control characters for a-z
                        if (key.keysym.sym >= SDLK_a && key.keysym.sym <= SDLK_z) {
                            char ctrl_char =
                                (key.keysym.sym - SDLK_a) + 1; // Ctrl+A = 0x01, Ctrl+B = 0x02, etc.
                            input = std::string(1, ctrl_char);
                        } else if (key.keysym.sym == SDLK_c) {
                            kill(child_pid, SIGINT); // Ctrl+C sends SIGINT
                        } else if (key.keysym.sym == SDLK_d) {
                            running = false; // Ctrl+D exits
                        }
                    } else if (key.keysym.sym >= SDLK_a && key.keysym.sym <= SDLK_z) {
                        // Handle Shift for a-z
                        char base_char = key.keysym.sym - SDLK_a + 'a';
                        if (mod & KMOD_SHIFT) {
                            base_char = std::toupper(base_char); // Convert to uppercase
                        }
                        input = std::string(1, base_char);
                    } else if (key.keysym.sym >= 32 && key.keysym.sym <= 126) {
                        // Handle other printable characters with Shift
                        char base_char = key.keysym.sym;
                        if (mod & KMOD_SHIFT) {
                            // Map lowercase to uppercase for letters, or use shifted symbols
                            if (base_char >= 'a' && base_char <= 'z') {
                                base_char = std::toupper(base_char);
                            } else {
                                // Handle common shifted symbols (simplified)
                                static const std::map<char, char> shift_map = {
                                    { '1', '!' }, { '2', '@' }, { '3', '#' }, { '4', '$' },
                                    { '5', '%' }, { '6', '^' }, { '7', '&' }, { '8', '*' },
                                    { '9', '(' }, { '0', ')' }, { '-', '_' }, { '=', '+' },
                                    { '[', '{' }, { ']', '}' }, { ';', ':' }, { '\'', '"' },
                                    { ',', '<' }, { '.', '>' }, { '/', '?' }, { '`', '~' }
                                };
                                auto it = shift_map.find(base_char);
                                if (it != shift_map.end()) {
                                    base_char = it->second;
                                }
                            }
                        }
                        input = std::string(1, base_char);
                    }
                    break;
                }
                if (!input.empty()) {
                    //std::cerr << "Sending input: ";
                    //for (char c : input) {
                    //    std::cerr << (int)c << " ";
                    //}
                    //std::cerr << std::endl;
                    write(master_fd, input.c_str(), input.size());
                }
            } else if (event.type == SDL_WINDOWEVENT &&
                       event.window.event == SDL_WINDOWEVENT_RESIZED) {
                update_terminal_size();
            }
        }

        // Check pseudo-terminal for output
        FD_ZERO(&read_fds);
        FD_SET(master_fd, &read_fds);
        struct timeval tv = { 0, 10000 }; // 10ms timeout
        if (select(master_fd + 1, &read_fds, nullptr, nullptr, &tv) > 0) {
            if (FD_ISSET(master_fd, &read_fds)) {
                ssize_t bytes = read(master_fd, buffer, sizeof(buffer) - 1);
                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    //std::cerr << "Read " << bytes << " bytes: ";
                    //for (ssize_t j = 0; j < bytes; ++j) {
                    //    std::cerr << (int)buffer[j] << " ";
                    //}
                    //std::cerr << std::endl;
                    for (ssize_t i = 0; i < bytes; ++i) {
                        char c = buffer[i];
                        switch (state) {
                        case AnsiState::NORMAL:
                            if (c == '\033') {
                                state = AnsiState::ESCAPE;
                                ansi_seq.clear();
                                //std::cerr << "Received ESC, transitioning to ESCAPE state" << std::endl;
                            } else if (c == '\n') {
                                cursor.row++;
                                cursor.col = 0;
                                if (cursor.row >= term_rows) {
                                    text_buffer.erase(text_buffer.begin());
                                    text_buffer.push_back(
                                        std::vector<Char>(term_cols, { ' ', current_attr }));
                                    texture_cache.erase(texture_cache.begin());
                                    texture_cache.push_back({});
                                    dirty_lines.erase(dirty_lines.begin());
                                    dirty_lines.push_back(true);
                                    cursor.row--;
                                }
                                dirty_lines[cursor.row] = true;
                            } else if (c == '\r') {
                                cursor.col = 0;
                                if (i + 1 < bytes && buffer[i + 1] == '\n') {
                                    ++i; // Skip \n in \r\n
                                    cursor.row++;
                                    if (cursor.row >= term_rows) {
                                        text_buffer.erase(text_buffer.begin());
                                        text_buffer.push_back(
                                            std::vector<Char>(term_cols, { ' ', current_attr }));
                                        texture_cache.erase(texture_cache.begin());
                                        texture_cache.push_back({});
                                        dirty_lines.erase(dirty_lines.begin());
                                        dirty_lines.push_back(true);
                                        cursor.row--;
                                    }
                                }
                                dirty_lines[cursor.row] = true;
                            } else if (c == '\b') {
                                if (cursor.col > 0) {
                                    cursor.col--;
                                    text_buffer[cursor.row][cursor.col] = { ' ', current_attr };
                                    dirty_lines[cursor.row]             = true;
                                }
                            } else if (c >= 32 && c <= 126) {
                                if (cursor.col < term_cols && cursor.row < term_rows) {
                                    text_buffer[cursor.row][cursor.col] = { c, current_attr };
                                    cursor.col++;
                                    dirty_lines[cursor.row] = true;
                                }
                                if (cursor.col >= term_cols) {
                                    cursor.col = 0;
                                    cursor.row++;
                                    if (cursor.row >= term_rows) {
                                        text_buffer.erase(text_buffer.begin());
                                        text_buffer.push_back(
                                            std::vector<Char>(term_cols, { ' ', current_attr }));
                                        texture_cache.erase(texture_cache.begin());
                                        texture_cache.push_back({});
                                        dirty_lines.erase(dirty_lines.begin());
                                        dirty_lines.push_back(true);
                                        cursor.row--;
                                    }
                                }
                            }
                            break;

                        case AnsiState::ESCAPE:
                            if (c == '[') {
                                state = AnsiState::CSI;
                                ansi_seq.clear(); // Clear to start fresh CSI sequence
                                ansi_seq += c;
                                //std::cerr << "Received [, transitioning to CSI state" << std::endl;
                            } else if (c == 'c') {
                                //std::cerr << "Received ESC c, processing reset" << std::endl;
                                process_ansi_sequence("", c); // Handle ESC c
                                state = AnsiState::NORMAL;
                                ansi_seq.clear();
                            } else {
                                //std::cerr << "Unknown ESC sequence char: " << (int)c << ", resetting to NORMAL" << std::endl;
                                state = AnsiState::NORMAL;
                                ansi_seq.clear();
                            }
                            break;

                        case AnsiState::CSI:
                            ansi_seq += c;
                            if (std::isalpha(c)) {
                                //std::cerr << "Received CSI final char: " << c << std::endl;
                                process_ansi_sequence(ansi_seq, c);
                                state = AnsiState::NORMAL;
                                ansi_seq.clear();
                            }
                            break;
                        }
                    }
                } else if (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                    std::cerr << "Error reading from master_fd: " << strerror(errno) << std::endl;
                    running = false;
                }
            }
        }

        // Render text
        render_text();

        // Check if child has exited
        if (waitpid(child_pid, &status, WNOHANG) > 0) {
            running = false;
        }
    }

    // Cleanup
    kill(child_pid, SIGTERM);
    waitpid(child_pid, &status, 0);
    close(master_fd);
    for (auto &line_spans : texture_cache) {
        for (auto &span : line_spans) {
            if (span.texture)
                SDL_DestroyTexture(span.texture);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
