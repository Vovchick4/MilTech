#pragma once
// T08. Контрольна сума MAVLink (CRC-16/MCRF4XX, вона ж X.25 у mavlink).
// Кожен MAVLink-пакет закінчується 2 байтами CRC. Реалізуй функцію з mavlink/checksum.h:
//
//   для кожного байта b:
//       tmp = b ^ (crc & 0xFF)
//       tmp ^= (tmp << 4)                (tmp — 8-бітний!)
//       crc = (crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4)
//   початкове значення crc = 0xFFFF
//
// Контрольне значення: CRC рядка "123456789" = 0x6F91.
// Тема: робота з бітами, типи фіксованого розміру (uint8_t / uint16_t), переповнення.
#include <cstddef>
#include <cstdint>

namespace tasks {

inline std::uint16_t crc16_mavlink(const std::uint8_t* data, std::size_t len, std::uint16_t crc = 0xFFFF)
{
    (void)data;
    (void)len;
    // TODO
    return crc;
}

} // namespace tasks
