#include "../../misc/TEST.hpp"
#include "../../misc/ConsoleLogger.hpp"

#ifdef TEST
#include "test_TBox.hpp"
#include "test_TColorPair.hpp"
#include "test_TColorPairPalette.hpp"
#include "test_TEventHandler.hpp"
#include "test_TInput.hpp"
#include "test_TScrollbar.hpp"
#include "test_TTheme.hpp"
#include "test_TWindow.hpp"
#endif // TEST

int main(int argc, char** argv) {
    createLogger<ConsoleLogger>();
    Arguments args(argc, argv);
    tester.run(args);
}
