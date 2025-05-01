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

#include <iostream>
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
std::vector<std::string> text_buffer;     // Stores terminal lines
std::vector<SDL_Texture *> texture_cache; // Cached textures for each line
std::vector<bool> dirty_lines;            // Flags for lines needing texture updates
int char_width = 0, char_height = 0;      // Font character dimensions
int term_cols = 80;                       // Current terminal columns
int term_rows = 24;                       // Current terminal rows

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

        std::cerr << "Updated slave size to " << ws.ws_col << "x" << ws.ws_row << std::endl;

        // Notify child process
        if (child_pid > 0) {
            kill(child_pid, SIGWINCH);
        }

        // Adjust text buffer and texture cache
        for (auto &line : text_buffer) {
            if (line.size() > static_cast<size_t>(term_cols)) {
                line.resize(term_cols);
            }
        }
        while (text_buffer.size() > static_cast<size_t>(term_rows)) {
            text_buffer.erase(text_buffer.begin());
            if (!texture_cache.empty()) {
                SDL_DestroyTexture(texture_cache.front());
                texture_cache.erase(texture_cache.begin());
                dirty_lines.erase(dirty_lines.begin());
            }
        }
        // Resize texture cache and mark all lines dirty
        texture_cache.resize(text_buffer.size(), nullptr);
        dirty_lines.resize(text_buffer.size(), true);
    }
}

// Signal handler for SIGWINCH
void handle_sigwinch(int sig)
{
    update_terminal_size();
}

// Render text buffer to SDL window with double buffering and texture caching
void render_text()
{
    // Update textures for dirty lines
    SDL_Color white = { 255, 255, 255, 255 }; // White text
    for (size_t i = 0; i < text_buffer.size(); ++i) {
        if (!dirty_lines[i] && texture_cache[i])
            continue; // Skip unchanged lines

        // Destroy old texture if it exists
        if (texture_cache[i]) {
            SDL_DestroyTexture(texture_cache[i]);
            texture_cache[i] = nullptr;
        }

        // Render new texture for non-empty lines
        if (!text_buffer[i].empty()) {
            SDL_Surface *surface = TTF_RenderText_Blended(font, text_buffer[i].c_str(), white);
            if (surface) {
                texture_cache[i] = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);
            }
        }
        dirty_lines[i] = false; // Mark as clean
    }

    // Render to back buffer
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);

    // Copy cached textures to back buffer
    for (size_t i = 0; i < text_buffer.size() && i < static_cast<size_t>(term_rows); ++i) {
        if (!texture_cache[i])
            continue; // Skip empty or invalid textures
        int w, h;
        SDL_QueryTexture(texture_cache[i], nullptr, nullptr, &w, &h);
        SDL_Rect dst = { 0, static_cast<int>(i * char_height), w, h };
        SDL_RenderCopy(renderer, texture_cache[i], nullptr, &dst);
    }

    // Present back buffer (swap with front buffer)
    SDL_RenderPresent(renderer);
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
    text_buffer.push_back(""); // Start with one empty line
    texture_cache.push_back(nullptr);
    dirty_lines.push_back(true);

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
    if (ioctl(master_fd, TIOCSWINSZ, &ws) == -1) {
        std::cerr << "Error setting initial slave window size: " << strerror(errno) << std::endl;
    }

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
                if (key.keysym.sym == SDLK_RETURN) {
                    input = "\n";
                } else if (key.keysym.sym == SDLK_BACKSPACE) {
                    input = "\b";
                } else if (key.keysym.mod & KMOD_CTRL) {
                    if (key.keysym.sym == SDLK_c) {
                        kill(child_pid, SIGINT);
                    } else if (key.keysym.sym == SDLK_d) {
                        running = false;
                    }
                } else if (key.keysym.sym >= 32 && key.keysym.sym <= 126) {
                    input = static_cast<char>(key.keysym.sym);
                }
                if (!input.empty()) {
                    write(master_fd, input.c_str(), input.size());
                }
            } else if (event.type == SDL_WINDOWEVENT &&
                       event.window.event == SDL_WINDOWEVENT_RESIZED) {
                update_terminal_size(); // Update slave size on window resize
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
                    for (ssize_t i = 0; i < bytes; ++i) {
                        char c = buffer[i];
                        if (c == '\n') {
                            text_buffer.push_back("");
                            texture_cache.push_back(nullptr);
                            dirty_lines.push_back(true);
                        } else if (c == '\r') {
                            if (i + 1 < bytes && buffer[i + 1] == '\n') {
                                ++i; // Skip \n in \r\n
                                text_buffer.push_back("");
                                texture_cache.push_back(nullptr);
                                dirty_lines.push_back(true);
                            } else {
                                text_buffer.back().clear();
                                dirty_lines.back() = true;
                            }
                        } else if (c == '\b') {
                            if (!text_buffer.back().empty()) {
                                text_buffer.back().pop_back();
                                dirty_lines.back() = true;
                            }
                        } else if (c >= 32 && c <= 126) {
                            if (text_buffer.back().size() < static_cast<size_t>(term_cols)) {
                                text_buffer.back() += c;
                                dirty_lines.back() = true;
                            }
                        }
                    }
                    // Trim buffer to term_rows
                    while (text_buffer.size() > static_cast<size_t>(term_rows)) {
                        text_buffer.erase(text_buffer.begin());
                        if (!texture_cache.empty()) {
                            SDL_DestroyTexture(texture_cache.front());
                            texture_cache.erase(texture_cache.begin());
                            dirty_lines.erase(dirty_lines.begin());
                        }
                    }
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
    for (auto texture : texture_cache) {
        if (texture)
            SDL_DestroyTexture(texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
