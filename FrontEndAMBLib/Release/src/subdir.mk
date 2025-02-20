################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CANBusInterface.cpp \
../src/ChannelNodeMap.cpp \
../src/NICANBusInterface.cpp \
../src/SocketClientBusInterface.cpp \
../src/ambDeviceImpl.cpp \
../src/ambDeviceInt.cpp \
../src/ambInterface.cpp \
../src/ambQueue.cpp \
../src/ds1820.cpp \
../src/messagePackUnpack.cpp 

CPP_DEPS += \
./src/CANBusInterface.d \
./src/ChannelNodeMap.d \
./src/NICANBusInterface.d \
./src/SocketClientBusInterface.d \
./src/ambDeviceImpl.d \
./src/ambDeviceInt.d \
./src/ambInterface.d \
./src/ambQueue.d \
./src/ds1820.d \
./src/messagePackUnpack.d 

OBJS += \
./src/CANBusInterface.o \
./src/ChannelNodeMap.o \
./src/NICANBusInterface.o \
./src/SocketClientBusInterface.o \
./src/ambDeviceImpl.o \
./src/ambDeviceInt.o \
./src/ambInterface.o \
./src/ambQueue.o \
./src/ds1820.o \
./src/messagePackUnpack.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"../include" -I"../include/FrontEndAMB" -I"../../3rdparty/Nican/include" -I"../../CppUtilityLib/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/CANBusInterface.d ./src/CANBusInterface.o ./src/ChannelNodeMap.d ./src/ChannelNodeMap.o ./src/NICANBusInterface.d ./src/NICANBusInterface.o ./src/SocketClientBusInterface.d ./src/SocketClientBusInterface.o ./src/ambDeviceImpl.d ./src/ambDeviceImpl.o ./src/ambDeviceInt.d ./src/ambDeviceInt.o ./src/ambInterface.d ./src/ambInterface.o ./src/ambQueue.d ./src/ambQueue.o ./src/ds1820.d ./src/ds1820.o ./src/messagePackUnpack.d ./src/messagePackUnpack.o

.PHONY: clean-src

