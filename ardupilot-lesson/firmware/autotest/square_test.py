# Задача 14: автотест для режиму SQUARE.
# Вставити як метод класу AutoTestCopter у ~/ardupilot/Tools/autotest/arducopter.py
# і додати self.SquareMode у список, який повертає tests1a() (або інший tests*()).
#
# Запуск (WSL Ubuntu, з активованим venv якщо є):
#   cd ~/ardupilot
#   Tools/autotest/autotest.py build.Copter test.Copter.SquareMode
#
# Каркас — назви допоміжних методів можуть трохи відрізнятися між версіями ArduPilot;
# дивись сусідні тести (наприклад GuidedSubModeChange).

    def SquareMode(self):
        '''Test student SQUARE mode (29)'''
        side = 100
        self.set_parameters({
            "USR_SQUARE_M": side,
            "USR_MAX_DIST": 0,        # наш failsafe не заважає
            "FS_GCS_ENABLE": 0,
        })
        self.change_mode("GUIDED")
        self.wait_ready_to_arm()
        self.arm_vehicle()
        self.user_takeoff(alt_min=20)

        self.change_mode(29)                       # SQUARE
        self.wait_statustext("SQUARE: corner 1", timeout=10)
        self.wait_distance_to_home(side - 10, side + 10, timeout=60)        # P1
        self.wait_statustext("SQUARE: corner 2", timeout=60)
        self.wait_distance_to_home(side * 1.41 - 10, side * 1.41 + 10, timeout=60)  # P2 (діагональ)
        self.wait_statustext("SQUARE: done", timeout=180)
        self.wait_distance_to_home(0, 10, timeout=10)

        self.do_RTL()
