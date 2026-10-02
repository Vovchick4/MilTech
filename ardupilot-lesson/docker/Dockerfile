# ArduPilot SITL у Docker (headless, без GUI).
# GCS (Mission Planner / QGroundControl) і C++ працюють на ХОСТІ
# та підключаються до портів 5760 / 5762, проброшених з контейнера.
#
# Збірка образу (займає 10–20 хв):
#   docker build -t ardupilot-sitl -f docker/Dockerfile .
FROM ardupilot/ardupilot-dev-base:latest

ARG AP_BRANCH=Copter-4.5   # або master
WORKDIR /ardupilot

RUN git clone --depth 1 --branch ${AP_BRANCH} --recurse-submodules --shallow-submodules \
        https://github.com/ArduPilot/ardupilot.git . \
 && python3 -m pip install --break-system-packages empy==3.3.4 pexpect future 2>/dev/null \
 || python3 -m pip install empy==3.3.4 pexpect future

RUN ./waf configure --board sitl && ./waf copter

EXPOSE 5760 5762 5763

# --home Canberra (дефолт ArduPilot). SPEEDUP можна передати через -e SPEEDUP=10
ENV SPEEDUP=1
CMD ["sh", "-c", "build/sitl/bin/arducopter --model quad --speedup ${SPEEDUP} --defaults Tools/autotest/default_params/copter.parm"]
