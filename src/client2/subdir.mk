################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./clientJournaliste.c \
./comm.c \


OBJS += \
./clientJournaliste.o \
./comm.o \


C_DEPS += \
./clientJournaliste.d \
./comm.d \



# Each subdirectory must supply rules for building sources it contributes
./%.o: ./%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


