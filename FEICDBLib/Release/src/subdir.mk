################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FEICDataBase.cpp \
../src/MySQLConnection.cpp 

CPP_DEPS += \
./src/FEICDataBase.d \
./src/MySQLConnection.d 

OBJS += \
./src/FEICDataBase.o \
./src/MySQLConnection.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"L:\ALMA-FEControl\FEICDBLib\include" -I"L:\ALMA-FEControl\CppUtilityLib\include" -I"L:\ALMA-FEControl\3rdparty\MySQL41\include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/FEICDataBase.d ./src/FEICDataBase.o ./src/MySQLConnection.d ./src/MySQLConnection.o

.PHONY: clean-src

