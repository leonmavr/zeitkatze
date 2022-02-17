#ifndef TERMINAL_SETTER_HPP
#define TERMINAL_SETTER_HPP 
#include <termios.h> // struct termios, tcgetattr(), tcsetattr()

/**
 * @brief Sets and resets terminal properties to accept input
 */
class TerminalSetter
{
public:
    TerminalSetter () {};
/**
 * @brief   Set terminal properties to allow terminal to accept keys
 *          Implementation details: https://linux.die.net/man/3/tcgetattr
 */
    void SetTerminal();
    // Restores terminal properties as in before SetTerminal was called
    inline void ResetTerminal() {
        tcsetattr(1, TCSANOW, &tio_before_);
    };

private:
    struct termios tio_before_{}, tio_after_{};
};
#endif /* TERMINAL_SETTER_HPP */
