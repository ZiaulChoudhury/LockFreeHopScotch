################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Hopscotch.cpp \
../src/LFHash.cpp \
../src/workload.cpp 

OBJS += \
./src/Hopscotch.o \
./src/LFHash.o \
./src/workload.o 

CPP_DEPS += \
./src/Hopscotch.d \
./src/LFHash.d \
./src/workload.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -std=c++11 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


