#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#ifdef PLATFORM_WINDOWS
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

namespace Platform {

    inline void sleep(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    inline void clearScreen() {
        std::cout << "\033[2J\033[1;1H";
    }

    inline void hideCursor() {
        std::cout << "\033[?25l";
    }

    inline void showCursor() {
        std::cout << "\033[?25h";
    }

    inline void setCursorPosition(int x, int y) {
        std::cout << "\033[" << y << ";" << x << "H";
    }

#ifdef PLATFORM_WINDOWS
    inline void init() {
        // Enable ANSI escape codes on Windows 10+
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
        SetConsoleOutputCP(CP_UTF8);
    }

    inline void cleanup() {
        showCursor();
    }

    inline bool kbhit() {
        return _kbhit();
    }

    inline char getch() {
        return _getch();
    }

    inline bool isKeyDown(int vKey) {
        return (GetAsyncKeyState(vKey) & 0x8000) != 0;
    }

    inline void getTerminalSize(int& width, int& height) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#else
    inline struct termios orig_termios;

    inline void cleanup() {
        showCursor();
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    }

    inline void init() {
        tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(cleanup);
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ECHO | ICANON);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    inline bool kbhit() {
        struct timeval tv = { 0L, 0L };
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
    }

    inline char getch() {
        char c;
        if (read(STDIN_FILENO, &c, 1) < 0) return 0;
        return c;
    }

    inline bool isKeyDown(int vKey) {
        // Not implemented for Linux in this simple version
        return false;
    }

    inline void getTerminalSize(int& width, int& height) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        width = w.ws_col;
        height = w.ws_row;
    }
#endif

    // Colors
    enum Color {
        BLACK = 0,
        RED = 1,
        GREEN = 2,
        YELLOW = 3,
        BLUE = 4,
        MAGENTA = 5,
        CYAN = 6,
        WHITE = 7,
        RESET = 9
    };

    inline std::string getFgColorCode(Color c) {
        if (c == RESET) return "\033[0m";
        return "\033[3" + std::to_string((int)c) + "m";
    }

    inline std::string getBgColorCode(Color c) {
        if (c == RESET) return "\033[0m";
        return "\033[4" + std::to_string((int)c) + "m";
    }
}
