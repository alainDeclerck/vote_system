################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./comm.c \
./dynTable.c \
./hashTable.c \
./server.c \
./server_options.c \
./server_politiques.c \
./server_sema.c \
./server_shm.c \
./server_utils.c \
./server_vote.c 

OBJS += \
./comm.o \
./dynTable.o \
./hashTable.o \
./server.o \
./server_options.o \
./server_politiques.o \
./server_sema.o \
./server_shm.o \
./server_utils.o \
./server_vote.o 

C_DEPS += \
./comm.d \
./dynTable.d \
./hashTable.d \
./server.d \
./server_options.d \
./server_politiques.d \
./server_sema.d \
./server_shm.d \
./server_utils.d \
./server_vote.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ./%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O1 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


