/**
 * @file
 * @author Marek Bel
 */

#ifndef MENUSTACK_H
#define MENUSTACK_H

#include <stdint.h>

/** Pointer to function implementing menu.*/
typedef void (*menuFunc_t)();
/**
 * @brief Stack implementation for navigating menu structure
 */
class MenuStack
{
public:
    struct Record
    {
        menuFunc_t menu;
        int8_t position;
    };
    MenuStack():m_stack(),m_index(0) {}
    void push(menuFunc_t menu, int8_t position);
    Record pop();
    void reset(){m_index = 0;}
private:
    static const int max_depth = 4;
    Record m_stack[max_depth];
    uint8_t m_index;
};

#endif /* FIRMWARE_MENUSTACK_H_ */
