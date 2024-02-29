################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AMBDevice.cpp \
../src/DLLMain.cpp \
../src/SWVersion.cpp \
../src/Sequencer.cpp 

CPP_DEPS += \
./src/AMBDevice.d \
./src/DLLMain.d \
./src/SWVersion.d \
./src/Sequencer.d 

OBJS += \
./src/AMBDevice.o \
./src/DLLMain.o \
./src/SWVersion.o \
./src/Sequencer.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DBUILD_FRONTENDAMB -I"L:\ALMA-FEControl\CppUtilityLib\include" -I"L:\ALMA-FEControl\3rdparty\drmingw\include" -I"L:\ALMA-FEControl\FrontEndAMBLib\include" -O3 -Wall -c -fmessage-length=0 -Wno-dangling-else -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/AMBDevice.d ./src/AMBDevice.o ./src/DLLMain.d ./src/DLLMain.o ./src/SWVersion.d ./src/SWVersion.o ./src/Sequencer.d ./src/Sequencer.o

.PHONY: clean-src

