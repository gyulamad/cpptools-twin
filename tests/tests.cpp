#include <cstdio>
#include <cstdlib>
#include <termios.h>
#include <unistd.h>

#include "../../misc/TEST.hpp"
#include "../../misc/ConsoleLogger.hpp"

#ifdef TEST
#include "test_NcursesScreen.hpp"
#include "test_TBox.hpp"
#include "test_TColorPair.hpp"
#include "test_TColorPairPalette.hpp"
#include "test_TEventResult.hpp"
#include "test_TEventHandler.hpp"
#include "test_TInput.hpp"
#include "test_TScrollbar.hpp"
#include "test_TTheme.hpp"
#include "test_TWindow.hpp"

// ---------------------------------------------------------------
// ncurses hack:

// Global termios state saved before any ncurses initialization.
static struct termios g_originalTermios;
static bool g_termiosSaved = false;

static void restoreTerminal() {
    fprintf(stdout, "\n\n\n\n");
    if (g_termiosSaved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_originalTermios);
    }
    // Send ncurses terminal reset sequences to stdout.
    // These are the same codes endwin() sends when switching from alternate
    // screen back to normal and restoring cursor/keyboard/mouse state.
    fprintf(stdout, "\033[?1049l");  // Switch from alternate screen to normal
    // fprintf(stdout, "\033[2J\033[H"); // Clear screen and move cursor home
    fprintf(stdout, "\033[?7h");     // Re-enable line wrapping
    fprintf(stdout, "\033[?1l");     // Exit application keypad mode (keypad off)
    fprintf(stdout, "\033[?25h");    // Show cursor (curs_set(1))
    fflush(stdout);
}
#endif // TEST

int main(int argc, char** argv) {
#ifdef TEST
    g_termiosSaved = tcgetattr(STDIN_FILENO, &g_originalTermios) == 0;
    atexit(restoreTerminal);
#endif
    createLogger<ConsoleLogger>();
    Arguments args(argc, argv);
    tester.run(args);
}
