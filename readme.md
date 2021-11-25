**Задача:** Разработать драйвер приема/передачи байтовых пакетов переменной длины.

**Стек технологий:**
- STM32CubeMX ver.4.27.0
- Atollic TrueSTUDIO® for STM32 ver.9.3.0
- Git

**Выполненные задачи:**
- реализован механизм упаковки/распаковки для однозначного разграничения одного пакета от другого.
- обеспечен механизм проверки целостности данных в пакете.
- реализована отправка и прием пакетов через UART

**Планы развития:**
- обеспечение безопасной работы в многопоточной среде.
- реализация отправки и приема пакетов через CAN
- переход на современную версию STM32CubeMX и как следствие -> современную версию HAL.

**Сторонние библиотеки**
- [minihdlc](https://github.com/mengguang/minihdlc)

**Используемое оборудование**
1. [Отладочная плата STM32F103Cx DEMO BOARD v2.1](https://amperkot.ru/msk/catalog/plata_stm32f103c8t6_arm_cortex_m3-25058812.html) - 2 шт.
2. [Модуль CAN шины TJA1050](https://amperkot.ru/msk/catalog/modul_can_shinyi_tja1050-24327391.html) - 2 шт.
3. [Программатор ST-Link V2 для STM8, STM32](https://amperkot.ru/msk/catalog/programmator_stlink_v2_dlya_stm8_stm32-24309549.html) - 2 шт.
4. [USB-TTL модуль CP2102](https://amperkot.ru/msk/catalog/usbttl_modul_cp2102-25061120.html) - 1 шт.

* * *
