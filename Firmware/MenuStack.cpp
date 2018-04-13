/**
 * @file
 * @author Marek Bel
 */

#include "MenuStack.h"
/**
 * @brief Push menu on stack
 * @param menu
 * @param position selected position in menu being pushed
 */
void MenuStack::push(menuFunc_t menu, int8_t position)
{
    if (m_index >= max_depth) return;
    m_stack[m_index].menu = menu;
    m_stack[m_index].position = position;
    ++m_index;
}

/**
 * @brief Pop menu from stack
 * @return Record containing menu function pointer and previously selected line number
 */
MenuStack::Record MenuStack::pop()
{
    if (m_index != 0) m_index--;

    return m_stack[m_index];
}
