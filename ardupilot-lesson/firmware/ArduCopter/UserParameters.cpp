// Замінити ~/ardupilot/ArduCopter/UserParameters.cpp
#include "UserParameters.h"
#include "config.h"

#if USER_PARAMS_ENABLED
// Префікс "USR" додається автоматично. Ім'я параметра — максимум 16 символів разом з префіксом.
const AP_Param::GroupInfo UserParameters::var_info[] = {

    // @Param: _ENABLE
    // @DisplayName: Enable user distance guard
    // @Description: 1 = надсилати HOME_DIST і вмикати RTL при перевищенні USR_MAX_DIST
    // @Values: 0:Disabled,1:Enabled
    // @User: Standard
    AP_GROUPINFO("_ENABLE", 0, UserParameters, _enable, 1),

    // @Param: _MAX_DIST
    // @DisplayName: Max distance from home
    // @Description: Якщо дрон далі від home — RTL. 0 = вимкнено
    // @Units: m
    // @Range: 0 200000
    // @User: Standard
    AP_GROUPINFO("_MAX_DIST", 1, UserParameters, _max_dist, 0),

    // @Param: _SQUARE_M
    // @DisplayName: Square mode side length
    // @Description: Довжина сторони квадрата для режиму SQUARE
    // @Units: m
    // @Range: 10 50000
    // @User: Standard
    AP_GROUPINFO("_SQUARE_M", 2, UserParameters, _square_m, 200),

    AP_GROUPEND
};

UserParameters::UserParameters()
{
    AP_Param::setup_object_defaults(this, var_info);
}
#endif // USER_PARAMS_ENABLED
