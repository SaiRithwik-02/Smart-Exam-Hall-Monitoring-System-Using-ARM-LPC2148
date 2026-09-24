# Smart Exam Hall Monitoring and Management System

## 📌 Project Overview

The **Smart Exam Hall Monitoring and Management System** is an embedded system developed using the **LPC2148 ARM7 microcontroller** and **Embedded C**. The system automates examination timing, monitoring, and status indication to reduce manual intervention and timing errors during examinations.

The system uses an **RTC for time management, LM35 temperature sensor with ADC for room-temperature monitoring, 4x4 keypad for user input, LCD for information display, multiplexed 7-segment displays for countdown timing, LEDs for exam-status indication, and external interrupts for administrator access and pause/resume control.**

---

## 🎯 Objectives

* Automate examination start and end timing.
* Display real-time **date, time, and room temperature**.
* Allow the administrator to configure the **exam start time and duration**.
* Provide a countdown display using multiplexed 7-segment displays.
* Provide visual exam-status indication using LEDs.
* Implement password-protected administrator access.
* Provide pause/resume functionality using an external interrupt.
* Record exam start and end information.
* Reduce manual intervention and timing errors.

---

## 🛠️ Hardware Requirements

| Component                       | Purpose                                      |
| ------------------------------- | -------------------------------------------- |
| **LPC2148 ARM7**                | Main microcontroller                         |
| **16x2 LCD**                    | Displays RTC, temperature, menus and status  |
| **4x4 Matrix Keypad**           | User/admin input                             |
| **RTC**                         | Maintains date and time                      |
| **LM35 Temperature Sensor**     | Measures room temperature                    |
| **ADC**                         | Converts LM35 analog output to digital value |
| **2 × 7-Segment Displays**      | Displays remaining exam time                 |
| **Green LED**                   | Normal exam status                           |
| **Yellow LED**                  | Final 10-minute warning                      |
| **Red LED**                     | Final 1-minute warning                       |
| **Buzzer**                      | Exam completion alert                        |
| **External Interrupt Switches** | Admin access and pause/resume                |
| **USB-UART / Serial Interface** | Serial communication/debugging               |

---

## 💻 Software Requirements

* **Embedded C**
* **Keil µVision**
* **LPC2148 ARM7**
* **Flash Magic** for microcontroller programming
* **Proteus** *(optional, for simulation/testing)*

---

## 🔧 Technologies & Concepts Used

```text
Embedded C
ARM7 LPC2148
GPIO
RTC
ADC
LM35
16x2 LCD
4x4 Matrix Keypad
External Interrupts
7-Segment Multiplexing
LED Control
Timer/Countdown Logic
Modular Programming
Register-Level Programming
```

---

## ⚙️ System Working

### 1. System Initialization

When the system is powered ON, the LPC2148 initializes:

* LCD
* RTC
* Keypad
* ADC
* 7-segment displays
* GPIO
* External interrupts

---

### 2. Normal Monitoring Mode

During normal operation, the LCD continuously displays:

```text
Current Time
Current Date
Room Temperature
```

The LM35 generates an analog voltage corresponding to the room temperature. The LPC2148 ADC converts this analog signal into a digital value, which is then converted into temperature and displayed on the LCD.

---

### 3. Administrator Authentication

The administrator can press the configured external interrupt switch to request administrator access.

The interrupt service routine sets a flag:

```c
password_request = 1;
```

The main program detects the flag and requests a password through the keypad.

```text
External Interrupt
       ↓
Password Request
       ↓
Password Entry
       ↓
Password Verification
       ↓
 ┌─────┴─────┐
 ↓           ↓
Wrong       Correct
 ↓           ↓
Denied     Admin Menu
```

---

### 4. Administrator Menu

After successful authentication, the administrator can access functions such as:

```text
1. RTC Edit
2. Start Time
3. Duration
4. View Log
5. Exit
```

The RTC date/time, examination start time, and examination duration can be configured using the keypad.

---

### 5. Exam Start

The system continuously compares the current RTC time with the configured examination start time.

When the configured time is reached:

```text
Exam Started
     ↓
Countdown = Configured Duration
     ↓
Start Time Recorded
```

The exam countdown begins automatically.

---

### 6. Countdown Display

The remaining examination time is displayed using **two multiplexed 7-segment displays**.

For example:

```text
60 → 59 → 58 → 57 → ... → 03 → 02 → 01 → 00
```

Multiplexing is used to control both 7-segment displays using the microcontroller.

---

### 7. LED Status Indication

The system provides visual warnings according to the remaining examination time.

| Remaining Time       | LED Status   |
| -------------------- | ------------ |
| More than 10 minutes | 🟢 Green     |
| Final 10 minutes     | 🟡 Yellow    |
| Final 1 minute       | 🔴 Red       |
| Exam completed       | Buzzer alert |

---

### 8. Pause / Resume

An external interrupt is used to pause and resume the examination countdown.

```text
Running
   ↓
Interrupt Switch
   ↓
Paused
   ↓
Interrupt Switch
   ↓
Running
```

A flag is used to maintain the pause/resume state.

```c
pause_flag = 0;   // Running
pause_flag = 1;   // Paused
```

---

### 9. Exam Completion

When the countdown reaches zero:

* Exam status is changed to completed.
* End time is recorded.
* LED indications are cleared.
* Buzzer is activated.
* Exam completion message is displayed.

Example:

```text
----------------
Exam Completed
----------------
```

---

## 🧩 Project Flow

```text
                  POWER ON
                     |
                     ↓
              Initialize System
                     |
                     ↓
            Display RTC + Temperature
                     |
             External Interrupt
                     |
                     ↓
             Password Verification
                     |
             +-------+-------+
             |               |
          Incorrect         Correct
             |               |
             ↓               ↓
       Access Denied     Admin Menu
                             |
            +----------------+----------------+
            |        |        |       |       |
            ↓        ↓        ↓       ↓       ↓
         RTC Edit  Start   Duration  Log     Exit
                       Time
                         |
                         ↓
                  Wait for Start Time
                         |
                         ↓
                    Exam Starts
                         |
                         ↓
                  Countdown Timer
                         |
              +----------+----------+
              |                     |
              ↓                     ↓
        7-Segment Display      Pause/Resume
              |
              ↓
         LED Indication
              |
              ↓
       Countdown reaches 00
              |
              ↓
        Exam Completed
              |
              ↓
          Buzzer Alert
              |
              ↓
        Store/View Log
```

---

## 📁 Suggested Project Structure

```text
Smart-Exam-Hall-Monitoring/
│
├── README.md
│
├── src/
│   ├── main.c
│   ├── lcd.c
│   ├── kpm.c
│   ├── rtc.c
│   ├── adc.c
│   ├── 7seg.c
│   ├── eint.c
│   ├── pcb.c
│   ├── Functions.c
│   └── delay.c
│
├── include/
│   ├── header.h
│   └── defines.h
│
├── docs/
│   └── Project_Documentation.pdf
│
└── simulation/
    └── Proteus/
```

---

## 🔑 Key Features

* **RTC-based examination scheduling**
* **Automatic countdown timer**
* **Password-protected administrator access**
* **Room-temperature monitoring**
* **LCD-based user interface**
* **4x4 keypad-based configuration**
* **Multiplexed 7-segment countdown display**
* **Three-level LED status indication**
* **External interrupt-based control**
* **Pause/resume functionality**
* **Exam start/end logging**
* **Buzzer alert after exam completion**
* **Modular Embedded C firmware**

---

## 📚 Embedded Concepts Demonstrated

This project provides practical implementation of:

* ARM7 microcontroller programming
* GPIO configuration
* Register-level Embedded C
* LCD interfacing
* Matrix keypad interfacing
* ADC interfacing
* Temperature sensor interfacing
* RTC programming
* External interrupt configuration
* Interrupt Service Routines (ISR)
* Flag-based interrupt handling
* 7-segment multiplexing
* Embedded application state management
* Modular firmware development

---

## 🚀 How to Build and Run

### Step 1 — Open the Project

Open the LPC2148 project in **Keil µVision**.

### Step 2 — Add Source Files

Add the required `.c` files and header files to the Keil project.

### Step 3 — Configure the Target

Select the appropriate **LPC2148 ARM7** target device.

### Step 4 — Build

Build the project and verify that there are no compilation or linking errors.

### Step 5 — Generate HEX File

Configure the Keil project to generate the HEX file.

### Step 6 — Program the Microcontroller

Use **Flash Magic** to program the generated HEX file into the LPC2148.

### Step 7 — Test the System

Verify:

* LCD display
* RTC
* Temperature measurement
* Keypad input
* Administrator authentication
* Exam start time
* Countdown
* LED indication
* Pause/resume
* Exam completion buzzer
* Exam logging

---

## 🧪 Testing

| Test Case                | Expected Result                           |
| ------------------------ | ----------------------------------------- |
| Power ON                 | System initializes successfully           |
| LCD test                 | LCD displays system information           |
| Keypad input             | Correct key is detected                   |
| RTC                      | Correct date/time displayed               |
| LM35                     | Room temperature displayed                |
| Wrong password           | Access denied                             |
| Correct password         | Admin menu displayed                      |
| Start time configuration | Exam starts at configured time            |
| Duration configuration   | Countdown starts with configured duration |
| Countdown > 10 min       | Green LED                                 |
| Final 10 min             | Yellow LED                                |
| Final 1 min              | Red LED                                   |
| Pause switch             | Countdown pauses                          |
| Resume switch            | Countdown resumes                         |
| Countdown = 0            | Exam completed and buzzer activated       |
| Log request              | Exam log information displayed            |

---

## 🎓 Learning Outcomes

Through this project, I gained practical experience in **ARM7 LPC2148 programming, Embedded C, peripheral interfacing, ADC, RTC, GPIO, external interrupts, LCD/keypad interfacing, multiplexed 7-segment displays, and embedded system integration**.

---

## 👨‍💻 Author

**Sai Rithwik**

### Project Type

**B.Tech / Embedded Systems Project**

### Domain

**Embedded Systems | ARM7 | Embedded C | Microcontroller Programming**

