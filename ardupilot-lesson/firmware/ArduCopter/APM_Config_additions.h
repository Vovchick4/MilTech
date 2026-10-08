// ===== Додати в КІНЕЦЬ ~/ardupilot/ArduCopter/APM_Config.h =====
// (задачі 8–11 і 12 з 09_zadachi_proshyvka.md)

#define USERHOOK_SUPERSLOWLOOP userhook_SuperSlowLoop();   // наш код 1 раз на секунду
#define USER_PARAMS_ENABLED 1                               // параметри USR_*
#define MODE_SQUARE_ENABLED 1                               // наш режим SQUARE (задача 12)

// УВАГА: для USER_PARAMS_ENABLED у master потрібні 2 виправлення (Copter.h, Parameters.cpp) —
// див. 09_zadachi_proshyvka.md, задача 9.
