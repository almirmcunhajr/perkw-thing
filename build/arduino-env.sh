#!/bin/bash

# Setup PerkwMachine
rm -rf "${HOME}/Arduino/libraries/PerkwMachine"
cp -r "machine" "${HOME}/Arduino/libraries/PerkwMachine"

# Setup EmonLib
rm -rf "${HOME}/Arduino/libraries/EmonLib"
cp -r "src/EmonLib" "${HOME}/Arduino/libraries/EmonLib"

# Setup SensorsInfos
rm -rf "${HOME}/Arduino/libraries/SensorsInfos"
mkdir "${HOME}/Arduino/libraries/SensorsInfos"
cp "thing/SensorsInfos.h" "${HOME}/Arduino/libraries/SensorsInfos/SensorsInfos.h"
