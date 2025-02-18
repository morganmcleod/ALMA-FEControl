################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/iniFile.cpp \
../src/listDir.cpp \
../src/logger.cpp \
../src/setTimeStamp.cpp \
../src/splitPath.cpp \
../src/stringConvert.cpp 

CPP_DEPS += \
./src/iniFile.d \
./src/listDir.d \
./src/logger.d \
./src/setTimeStamp.d \
./src/splitPath.d \
./src/stringConvert.d 

OBJS += \
./src/iniFile.o \
./src/listDir.o \
./src/logger.o \
./src/setTimeStamp.o \
./src/splitPath.o \
./src/stringConvert.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"L:\ALMA-FEControl\CppUtilityLib\include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/iniFile.d ./src/iniFile.o ./src/listDir.d ./src/listDir.o ./src/logger.d ./src/logger.o ./src/setTimeStamp.d ./src/setTimeStamp.o ./src/splitPath.d ./src/splitPath.o ./src/stringConvert.d ./src/stringConvert.o

.PHONY: clean-src

