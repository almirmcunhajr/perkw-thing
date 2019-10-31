# perkw-thing

## Description

Perkw is an IoT solution intended to manage and control electrical energy consumption by monitoring and controlling electrical current of electrical outlets. The project was a proposal for the Software and System Engineering discipline, taught in the first semester of 2018, at CIn-UFPE. We investigated a social problem under a specific theme, and tried to propose and develop a solution using an IoT platform. The agile methodology Scrum was used in the development process.

## Table of Contents

#### [thing/SensorsInfos.h](https://github.com/almdac/perkw-thing/blob/master/thing/SensorsInfos.h) - File with the defines to the thing;
#### [thing/thing.ino](https://github.com/almdac/perkw-thing/blob/master/thing/thing.ino) - File with the thing's code in C++;
#### [src/EmonLib](https://github.com/almdac/perkw-thing/tree/master/src/EmonLib) - Folder with the library used ([more about EmonLib](https://www.arduinolibraries.info/libraries/emon-lib));
#### [machine/Comm.h](https://github.com/almdac/perkw-thing/blob/master/machine/Comm.h) - File with the communication treatment;
#### [machine/MessagesTypes.h](https://github.com/almdac/perkw-thing/blob/master/machine/MessagesTypes.h) - File with types and sizes of the messages;
#### [machine/PerkwMachine.h](https://github.com/almdac/perkw-thing/blob/master/machine/PerkwMachine.h) - File with the state machine of the project;
#### [machine/States.h](https://github.com/almdac/perkw-thing/blob/master/machine/States.h) - File with the machine's list of states;
#### [interface](https://github.com/almdac/perkw-thing/tree/master/interface) - Folder with the project's interface in JavaScript;
#### [build/arduino-env.sh](https://github.com/almdac/perkw-thing/blob/master/build/arduino-env.sh) - File with the right Setup for this project.
