# Pin Functional Specification (ATmega128A)

> Аудитория: разработчики, тестировщики и инженеры сопровождения, работающие с прошивкой.
> Документ описывает проект в эксплуатационном формате и используется как рабочий артефакт в MR/релизном процессе.

## 1. Назначение документа

Документ фиксирует функциональную роль пинов МК в данной прошивке:
- входы клавиш;
- выходы подсветки (LED);
- UART интерфейс;
- служебные выводы, влияющие на работу системы.

## 2. Общие допущения

- МК: **ATmega128A**.
- Логика кнопок: **active-low** (нажатие = уровень 0 на входе).
- Для входов кнопок включены внутренние pull-up резисторы.
- Подсветка управляется 12 независимыми каналами (PA0..PA5, PC0..PC5).

## 3. Таблица функциональности пинов

## 3.1 Входы клавиш

| Логическая клавиша | Пин МК | Порт/бит | Направление | Режим | Примечание |
|---|---|---|---|---|---|
| KEY1  | PF7 | PORTF7 | Input | Pull-up ON | active-low |
| KEY2  | PF6 | PORTF6 | Input | Pull-up ON | active-low |
| KEY3  | PF5 | PORTF5 | Input | Pull-up ON | active-low |
| KEY4  | PF4 | PORTF4 | Input | Pull-up ON | active-low |
| KEY5  | PF3 | PORTF3 | Input | Pull-up ON | active-low |
| KEY6  | PF2 | PORTF2 | Input | Pull-up ON | active-low |
| KEY7  | PD0 | PORTD0 | Input | Pull-up ON | active-low |
| KEY8  | PD1 | PORTD1 | Input | Pull-up ON | active-low |
| KEY9  | PD2 | PORTD2 | Input | Pull-up ON | active-low |
| KEY10 | PD3 | PORTD3 | Input | Pull-up ON | active-low |
| KEY11 | PD4 | PORTD4 | Input | Pull-up ON | active-low |
| KEY12 | PD5 | PORTD5 | Input | Pull-up ON | active-low |

## 3.2 Выходы LED

| Логический LED | Пин МК | Порт/бит | Направление | Активный уровень | Примечание |
|---|---|---|---|---|---|
| LED1  | PA0 | PORTA0 | Output | High=ON | Управление из RX-команд |
| LED2  | PA1 | PORTA1 | Output | High=ON |  |
| LED3  | PA2 | PORTA2 | Output | High=ON |  |
| LED4  | PA3 | PORTA3 | Output | High=ON |  |
| LED5  | PA4 | PORTA4 | Output | High=ON |  |
| LED6  | PA5 | PORTA5 | Output | High=ON |  |
| LED7  | PC0 | PORTC0 | Output | High=ON |  |
| LED8  | PC1 | PORTC1 | Output | High=ON |  |
| LED9  | PC2 | PORTC2 | Output | High=ON |  |
| LED10 | PC3 | PORTC3 | Output | High=ON |  |
| LED11 | PC4 | PORTC4 | Output | High=ON |  |
| LED12 | PC5 | PORTC5 | Output | High=ON |  |

## 3.3 UART0

| Функция | Пин МК | Направление | Параметры |
|---|---|---|---|
| UART0 RXD | PE0 (RXD0) | Input  | 115200, 8N1 |
| UART0 TXD | PE1 (TXD0) | Output | 115200, 8N1 |

Примечание: в текущей реализации используется UART0 с прерыванием RX и поллингом TX готовности.

## 3.4 Таймер

| Блок | Роль |
|---|---|
| Timer0 Compare Match | Формирование системного тика ~1 ms (`g_ms`) |

## 4. Нефункциональные требования к разводке/схеме

1. Линии кнопок должны обеспечивать стабильный уровень лог.1 в отпущенном состоянии.
2. Для длинных трасс кнопок рекомендуется внешняя RC-фильтрация/защита от наводок.
3. Токи LED-каналов должны соответствовать допустимым ограничениям портов ATmega128A.
4. UART-линии должны быть согласованы по уровням с внешним устройством.

## 5. Связь с кодом

- Настройка GPIO кнопок/LED: `src/platform/avr/hal_avr.c`.
- Чтение кнопок: `avr_read_key(...)`.
- Управление LED: `avr_set_led(...)`.
- UART init/RX/TX: `uart0_init(...)`, `USART0_RX_vect`, `avr_uart_send(...)`.
