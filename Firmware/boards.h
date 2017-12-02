#ifndef BOARDS_H
#define BOARDS_H

#define BOARD_UNKNOWN -1

#define BOARD_RAMBO             100  // Rambo - 100 (orig 301)

#define BOARD_RAMBO_MINI_1_0    200  // Rambo-mini 1.0 - 200 (orig 102)
#define BOARD_RAMBO_MINI_1_3    203  // Rambo-mini 1.3 - 203 (orig 302)

#define BOARD_EISNY_0_3a         303  // EINY 0.3a - 303 (orig 300)
#define BOARD_EINSY_0_4a         304  // EINY 0.4a - 304 (orig 299)
#define BOARD_EINSY_0_5a         305  // EINY 0.5a - 305 (orig 298)

#define MB(board) (MOTHERBOARD==BOARD_##board)
#define IS_RAMPS (MB(RAMPS_OLD) || MB(RAMPS_13_EFB) || MB(RAMPS_13_EEB) || MB(RAMPS_13_EFF) || MB(RAMPS_13_EEF))

#endif //__BOARDS_H
