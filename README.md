# NXP Application Code Hub
[<img src="https://mcuxpresso.nxp.com/static/icon/nxp-logo-color.svg" width="100"/>](https://www.nxp.com)

## PWM-Based Steering Control for FRDM-A-S32K312
This demo showcases a steering angle control system on the FRDM‑A‑S32K312 development board. The application reads an analog input from a potentiometer to simulate a steering wheel position and translates it into a corresponding servo motor angle. By adjusting the potentiometer, the user can observe real-time steering movements, demonstrating how driver input is acquired, processed, and converted into actuator control. The system highlights key concepts such as ADC signal acquisition, signal scaling, and PWM-based servo control, commonly used in electronic power steering and steer-by-wire automotive applications.
<p align="center"><img src="images/demo.png" width="500"/></p>

#### Boards: FRDM-A-S32K312
#### Categories: Sensor
#### Peripherals: I2C, PWM, ADC
#### Toolchains: S32 Design Studio IDE

## Table of Contents
1. [Software and Tools](#step1)
2. [Hardware](#step2)
3. [Setup](#step3)
4. [Results](#step4)
5. [Support](#step5)
6. [Release Notes](#step6)

## 1. Software and Tools<a name="step1"></a>
This example was developed using the FRDM Automotive Bundle for S32K3 + S32M27. To download and install the complete software and tools ecosystem, use the following link:
- [ FRDM Automotive S32K3 + S32M27 Board Installation Package](https://www.nxp.com/app-autopackagemgr/automotive-software-package-manager:AUTO-SW-PACKAGE-MANAGER?currentTab=0&selectedDevices=S32K3&applicationVersionID=203)

## 2. Hardware<a name="step2"></a>
### 2.1 Required Hardware
- Personal Computer
- 12V Power Supply
- Type-C USB cable
  
| Boards | Images |
| ----------- | ------- |
| - [FRDM-A-S32K312](https://www.nxp.com/design/design-center/development-boards-and-designs/S32K312MINI-EVB) | <img src="images/FRDM-A-S32K312.png" width="600"> |
| - [FRDM K64 click shield](https://www.mikroe.com/frdm-k64-click-shield) | <p align="center"><img src="images/frdm-k64-click.png" width="400"/> |
| - [Servo Click](https://www.mikroe.com/servo-click) <br> - [POT Click](https://www.mikroe.com/pot-click) <br> | <p align="center"><img src="images/servo-click.png" height="300"/> <img src="images/pot-click.png" height="300"/> |
|- [Micro Servo motor SG 180 degree](https://www.mikroe.com/micro-servo-motor-sg-180-degree) | <p align="center"><img src="images/micro-servo-motor-sg-180-degree.png" height="300"/> |

### 2.2 Hardware Connections
| FRDM-A-S32K312   | Header Pin |I/O| FRDM Shield  | Click Board   | Click Pin | Description  |
|------------------|------------|---|--------------|---------------|-----------|--------------|
| PTB17 GPIO, 49   | J2 pin 5   | → | D10          | Servo Click   | OE        | Output Enable|
| PTC6 LPI2C1_SDA  | J2 pin 17  | → | SDA          | Servo Click   | SDA       | I2C SDA Pin  |
| PTC7 LPI2C1_SCL  | J2 pin 19  | → | SCL          | Servo Click   | SCL       | I2C SCL Pin  |
| P5V0 power 5V    | JA3 pin 11 | → | 5V power     | Servo Click   | 5V        | VCC MOT move 0ohm to 5V |
| PTD0 ADC0_P1     | J4 pin 3   | ← | A1           | POT Click     | AN        | Analog Output|
| GND              | JA3 pin 11 | → | GND          | POT Click     | GND       | Ground       |
| VDD_PERH         | JA3 pin 7  | → | 3.3V         | POT Click     | 3V3       | 3.3V Power   |

**Note:** The Servo Click motor can be powered in two ways:

- **External Power (default)**: Connect an external 5V power supply to the Servo Click's VCC MOT terminal — no hardware modification is needed.
- **Board Power**: Move the VCC MOT 0Ω resistor from the EXT position to the 5V position on the Servo Click board. This allows the servo motor to be powered directly from the FRDM-A-S32K312 board's 5V rail, eliminating the need for an external power supply.

The Servo motor is connected to the Servo Click on the channel 1, folowing table:

| Servo Click | Servo Motor |
|-------------|-------------|
| GND         | Brown Wire  |
| VCC         | Red Wire    |
| PWM         | Orange Wire |

### 2.3 Debugger Connection
- Connect 12V DC power supply to the board via the 12V power connector.
- Connect the Type-C USB cable to PC and FRDM-A-S32K312 board for power supply and debugging

## 3. Setup<a name="step3"></a>

### 3.1 Import the Project into S32 Design Studio IDE
1. Open S32 Design Studio IDE, in the Dashboard Panel, choose **Import project from Application Code Hub**.
[<p align="center"><img src="images/import_project_1.png" width="400"/></p>](./images/import_project_1.png)

2. You can find the demo you need by searching for the name directly.
 Open the project, click the **GitHub link** from this window, S32 Design Studio IDE will automatically retrieve project attributes then click **Next>**.
[<p align="center"><img src="images/import_project_3.png" width="600"/></p>](./images/import_project_3.png)

3. Select **main** branch and then click **Next>**.
4. Select your local path for the repo in **Destination->Directory** window. The S32 Design Studio IDE will clone the repo into this path, click **Next>**.

5. Select **Import existing Eclipse projects** then click **Next>**.

6. Select the project in this repo (only one project in this repo) then click **Finish**.
### 3.2 Generating, Building and Running the Example
1. In Project Explorer, right-click the project and select **Update Code and Build Project**. This will generate the configuration (Pins, Clocks, Peripherals), update the source code and build the project using the active configuration (e.g. Debug_FLASH).
Make sure the build completes successfully and the *.elf file is generated without errors.
[<p align="center"><img src="images/update_and_build.png" width="200"/></p>](./images/update_and_build.png)
Press **Yes** in the **SDK Component Management** pop-up window to continue.

2. Go to **Debug** and select **Debug Configurations**. There will be a debug configuration for this project:
[<p align="center"><img src="images/Debug_config.png" width="200"/></p>](./images/Debug_config.png)

        Configuration Name                  Description
        -------------------------------     -----------------------
        $(example)_debug_flash_pemicro      Debug the FLASH configuration using PEmicro probe

    Select the desired debug configuration and click on **Debug**. Now the perspective will change to the **Debug Perspective**.
    Use the controls to control the program flow.

## 4. Results<a name="step4"></a>
The servo motor follows the potentiometer position in real-time, simulating a steering control system. Upon startup, the servo centers to its neutral position. As the user rotates the potentiometer, the ADC reads the analog input and maps it to a corresponding PWM duty cycle, causing the servo to smoothly track the potentiometer angle. This demonstrates a basic steer-by-wire concept where driver input (potentiometer) is translated into actuator movement (servo).
[<p align="center"><img src="images/results.gif" width=400/></p>](./images/results.gif)

## 5. Support<a name="step5"></a>
For general technical questions related to NXP microcontrollers, please use the *NXP Community Forum*.
#### Project Metadata

<!----- Boards ----->
[![Board badge](https://img.shields.io/badge/Board-FRDM&ndash;A&ndash;S32K312-blue)]()

<!----- Categories ----->
[![Category badge](https://img.shields.io/badge/Category-SENSOR-yellowgreen)](https://mcuxpresso.nxp.com/appcodehub?category=sensor)

<!----- Peripherals ----->
[![Peripheral badge](https://img.shields.io/badge/Peripheral-I2C-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=i2c)
[![Peripheral badge](https://img.shields.io/badge/Peripheral-PWM-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=pwm)
[![Peripheral badge](https://img.shields.io/badge/Peripheral-ADC-yellow)](https://mcuxpresso.nxp.com/appcodehub?peripheral=adc)

<!----- Toolchains ----->
[![Toolchain badge](https://img.shields.io/badge/Toolchain-S32%20DESIGN%20STUDIO%20IDE-orange)](https://mcuxpresso.nxp.com/appcodehub?toolchain=s32_design_studio_ide)

Questions regarding the content/correctness of this example can be entered as Issues within this GitHub repository.

>**Warning**: For more general technical questions regarding NXP Microcontrollers and the difference in expected functionality, enter your questions on the [NXP Community Forum](https://community.nxp.com/)

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/NXP_Semiconductors)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/nxp-semiconductors)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/nxpsemi/)
[![Follow us on Twitter](https://img.shields.io/badge/X-Follow%20us%20on%20X-black.svg)](https://x.com/NXP)

## 6. Release Notes<a name="step6"></a>
| Version | Description / Update                           | Date                        |
|:-------:|------------------------------------------------|----------------------------:|
| 1.0     | Initial release on Application Code Hub        | May 28<sup>th</sup> 2026     |
| 1.1     | Updated to FRDM Automotive S32K3 + S32M27 (RTD 7.0.1)        |August 24<sup>th</sup> 2026|