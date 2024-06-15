\mainpage ME 507 Strobe Tuner Documentation

Project Members: Jojo Penrose, Jared Sinasohn

Github: https://github.com/JaredSinasohn4159/ME507-TermProject

Welcome to the ME 507 Strobe Tuner Documentation!

This website provides an overview of the software side of our ME 507 Strobe Tuner project, including its features, setup, and usage instructions.

## Overview and Theory

A strobe tuner is a highly accurate device used for tuning musical instruments by measuring the frequency of sound waves. Unlike conventional tuners, which often use needle or LED indicators to show pitch deviation, a strobe tuner operates by comparing the sound wave of the instrument to a reference frequency. It uses a rotating disk with accurately patterned slits cut into it, illuminated by a strobe light, to visually represent the difference between the played note and the desired pitch. As the disk spins, patterns of light and dark bands appear to move. When the frequency of the played note matches the reference frequency, the bands appear stationary, indicating that the instrument is in tune. If the instrument is out of tune, the bands will appear to move, depending on whether the pitch is flat or sharp. 

However, in our research, we found that most strobe tuners can be thousands of dollars, which is an amount of money many musicians are unable to afford.  So, our team set out to produce a strobe tuner running off a low-cost microcontroller with low cost parts to eventually make a low cost tuner that could one day be turned into a full product.  We designed custom PCB's, created a full 3d-printed body, and coded in C to create an initial prototype.  All in all, even though the initial prototype had many bugs, we were able to create a proof of concept showing our project is viable.  If you would like to see more about the project, including information about the PCB Design, Analog filtering, mechanical design, and the trials and tribulations, visit [Final Report Here.](tuner_report.pdf)

## Features

Here are some of the features of our project:
- Asthetically pleasing exterior
- DC motor with full PIDF speed control
- I2C driven 16-segment display with sharp/flat LED's for display of the current tuning pitch
- Rotary knob encoder for custom pitch selection
Here are some things we couldn't quite implement right now but will implement in the future:
- Input capture on the Microphone to auto-select tuning pitch
- Current sense on the motor through an ADC for over-current shutdown
- Making the I2C driver work

## Getting Started in Software

All code for this project was completed in C using STM32CubeID (Cube).  Our project runs on the STM32L476RGT6 chip, and before the code can be run it must be configured correctly using Cube's pin configuration GUI.  The a table of the correct pin configurations for this project can be found below
<div style="text-align: center;">
    <img src="pinout_table.jpg" alt="Pinout Table" width="1000" />
</div>
## Software Implementation - Classes

Even though C does not support object oriented programming, an object oriented approact was taken when coding this project to streamline the coding process.  Classes for a DC motor, RC_controller (not used but would have been used for E-stop), a Display, a Generic Encoder, a Pitch Selection encoder, an a Closed Loop Controller with full PIDF were written for the purposes of this project, and can be found below.

- \ref CLController "CLController"
- \ref Display "Display"
- \ref Encoder "Encoder"
- \ref Motor "Motor"
- \ref PitchEncoder "PitchEncoder"
- \ref RC_Controller "RC_Controller"

## Software Implementation - Cooperative Multitasking

In order to run all peripherals at once, a Cooperative Multitasking approach was taken.  For this, different tasks for the microcontroller to execute were designed with each task handling a different function.  The two tasks that were written are for handling pitch selection and the LED display, and the handling of the motor.  Finite State machines were drawn for each task to explain how each task would operate, and images of these finite state machines can be found below.
<div style="text-align: center;">
    <img src="display_task.jpg" alt="Display Task" width="500" />
</div>
<div style="text-align: center;">
    <img src="motor_task.jpg" alt="Motor Task" width="500" />
</div>

Each task was run in a round-robin style, where each task assumes none of the other tasks will take a significant amount of time, allowing for each task to run simultaneously.  This was deemed satisfactory for this project since there are only two tasks and each task is not all that complicated.

## Conclusion

In this project, our team was able to create a proof of concept prototype for a mechanical strobe tuner.  We are excited to continue this project in the future to make a low-cost strobe tuner that we can be proud of.
Feel free to look through the documentation on this website.  Or, alternatively, you can also refer to the [User Manual here](uman_tuner.pdf) for details about the project.
