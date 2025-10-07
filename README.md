# 🧭 UART Channel Manager (Qt Application)

**English version below ↓**

---

## 🇷🇺 Описание проекта

Графическое приложение на **Qt**, предназначенное для обмена данными по **UART** с несколькими устройствами (каналами).  
Позволяет задавать команды для каждого канала, отправлять их последовательно и сохранять ответы в лог-файл.

> ⚙️ Проект разработан в первую очередь для взаимодействия с осциллографами **RIGOL** и другими устройствами, поддерживающими управление по **RS-232 / UART**.  
> Интерфейс и логика обмена адаптированы под типичные команды SCPI, используемые приборами RIGOL.

### 🔧 Основные возможности
- Работа с **COM-портами** (автоматическое определение доступных портов)
- Настройка скорости, бит данных, четности и стоп-битов
- Поддержка нескольких **каналов команд**
- Циклический опрос устройств с заданным интервалом
- Запись логов в CSV-файл с временной меткой
- Интерактивная **консоль вывода** для контроля обмена

---

## 🧩 Структура проекта

| Файл | Назначение |
|------|-------------|
| `main.cpp` | Точка входа в приложение |
| `mainwindow.h / .cpp` | Главное окно, логика UI |
| `uart.h / .cpp` | Класс для работы с UART (QSerialPort) |
| `channel.h / .cpp` | Класс, представляющий отдельный канал команд |
| `channelmanager.h / .cpp` | Менеджер, управляющий опросом и обменом |
| `ui_mainwindow.h` | Сгенерированный Qt Designer интерфейс |

---

## 🚀 Сборка и запуск

### Требования
- **Qt 6.x** (или Qt 5.15+)
- **CMake ≥ 3.10**
- **C++17**

### Инструкция по сборке
```bash
git clone https://github.com/SeegmaEpsilon/QT_RIGOL_RS232.git
cd uart-channel-manager
mkdir build && cd build
cmake ..
make
./uart-channel-manager
```

> 💡 Приложение можно собрать и запустить напрямую из **Qt Creator**, выбрав `.pro` или `CMakeLists.txt`.

---

## 🧠 Принцип работы

1. Пользователь выбирает COM-порт и параметры UART.
2. В полях `channel_1`, `channel_2`, ... вводятся команды.
3. При нажатии **«Отправить»** — команды выполняются по очереди.
4. Ответы отображаются в консоли и сохраняются в CSV-файл вида  
   `log_DD.MM.YYYY.csv`.

---

## 📂 Пример лога
```
date time,channel1,channel2
07.10.2025 16:20:32,OK,READY
07.10.2025 16:20:37,OK,READY
```

---

## ⚖️ Лицензия
Проект распространяется под лицензией **Apache-2.0 license**.

---

# 🇬🇧 UART Channel Manager (Qt Application)

## 🧭 Overview
A **Qt-based desktop app** for communicating with multiple devices via **UART**.  
Each channel can send its own command and receive a response sequentially, with results saved to a CSV log file.

> ⚙️ This project was primarily designed for communication with **RIGOL oscilloscopes** and other instruments supporting **RS-232 / UART** control.  
> The interface and command handling are tailored for typical SCPI commands used by RIGOL devices.

### 🔧 Key Features
- Auto-detection of available COM ports  
- Full UART parameter configuration  
- Multiple command channels  
- Periodic polling with adjustable interval  
- CSV logging with timestamps  
- Interactive output console

---

## 🧩 Project Structure

| File | Description |
|------|--------------|
| `main.cpp` | Application entry point |
| `mainwindow.h / .cpp` | Main window and UI logic |
| `uart.h / .cpp` | UART communication handler |
| `channel.h / .cpp` | Represents a command channel |
| `channelmanager.h / .cpp` | Controls sequential command processing |
| `ui_mainwindow.h` | Auto-generated UI file from Qt Designer |

---

## 🚀 Build & Run

### Requirements
- **Qt 6.x** (or Qt 5.15+)
- **CMake ≥ 3.10**
- **C++17**

### Build steps
```bash
git clone https://github.com/SeegmaEpsilon/QT_RIGOL_RS232.git
cd uart-channel-manager
mkdir build && cd build
cmake ..
make
./uart-channel-manager
```

> 💡 You can also open and run the project directly in **Qt Creator**.

---

## 🧠 How it works
1. User selects the COM port and UART parameters.
2. Commands are entered into the channel fields.
3. On **“Send”**, each channel command is sent sequentially.
4. Responses are displayed in the console and saved to  
   a CSV log file named `log_DD.MM.YYYY.csv`.

---

## 📂 Example log
```
date time,channel1,channel2
07.10.2025 16:20:32,OK,READY
07.10.2025 16:20:37,OK,READY
```

---

## ⚖️ License
Released under the **Apache-2.0 license**.

---

## 👤 Author
Developed by me.
💬 Contributions, pull requests, and suggestions are welcome!
