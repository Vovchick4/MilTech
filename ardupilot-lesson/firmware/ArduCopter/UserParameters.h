// Замінити ~/ardupilot/ArduCopter/UserParameters.h
#pragma once

#include <AP_Param/AP_Param.h>

class UserParameters {

public:
    UserParameters();
    static const struct AP_Param::GroupInfo var_info[];

    // доступ з UserCode.cpp: g2.user_parameters.enabled()
    bool  enabled()     const { return _enable.get() != 0; }
    float max_dist_m()  const { return _max_dist.get(); }
    float square_m()    const { return _square_m.get(); }

private:
    AP_Int8  _enable;
    AP_Float _max_dist;
    AP_Float _square_m;
};
