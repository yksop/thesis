################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/msp430-gcc

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./accel.o" \
"./alpaca.o" \
"./main_alpaca.o" \
"./watchdog.o" \
"./libPF/PF_sim.o" \
"./libfvad/src/fvad.o" \
"./libfvad/src/signal_processing/division_operations.o" \
"./libfvad/src/signal_processing/energy.o" \
"./libfvad/src/signal_processing/get_scaling_square.o" \
"./libfvad/src/signal_processing/resample_48khz.o" \
"./libfvad/src/signal_processing/resample_by_2_internal.o" \
"./libfvad/src/signal_processing/resample_fractional.o" \
"./libfvad/src/signal_processing/spl_inl.o" \
"./libfvad/src/vad/vad_core.o" \
"./libfvad/src/vad/vad_filterbank.o" \
"./libfvad/src/vad/vad_gmm.o" \
"./libfvad/src/vad/vad_sp.o" \
-T"../msp430fr5994.ld" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lgcc \
-lc \
-Wl,--end-group \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include libPF/subdir_vars.mk
-include libfvad/src/subdir_vars.mk
-include libfvad/src/signal_processing/subdir_vars.mk
-include libfvad/src/vad/subdir_vars.mk
-include subdir_rules.mk
-include libPF/subdir_rules.mk
-include libfvad/src/subdir_rules.mk
-include libfvad/src/signal_processing/subdir_rules.mk
-include libfvad/src/vad/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
alpaca.out \

EXE_OUTPUTS__QUOTED += \
"alpaca.out" \


# All Target
all: $(OBJS) $(LD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "alpaca.out"

# Tool invocations
alpaca.out: $(OBJS) $(LD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"C:/ti/msp430-gcc/bin/msp430-elf-gcc-9.3.1.exe" -mhwmult=f5series -Og -g -gdwarf-3 -gstrict-dwarf -Wall -mcode-region=none -mlarge -Wl,-Map,"alpaca.map" -Wl,--gc-sections -L"C:/ti/ccs1250/ccs/ccs_base/msp430/include_gcc" -mmcu=msp430fr5994 -o"alpaca.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "accel.o" "alpaca.o" "main_alpaca.o" "watchdog.o" "libPF\PF_sim.o" "libfvad\src\fvad.o" "libfvad\src\signal_processing\division_operations.o" "libfvad\src\signal_processing\energy.o" "libfvad\src\signal_processing\get_scaling_square.o" "libfvad\src\signal_processing\resample_48khz.o" "libfvad\src\signal_processing\resample_by_2_internal.o" "libfvad\src\signal_processing\resample_fractional.o" "libfvad\src\signal_processing\spl_inl.o" "libfvad\src\vad\vad_core.o" "libfvad\src\vad\vad_filterbank.o" "libfvad\src\vad\vad_gmm.o" "libfvad\src\vad\vad_sp.o" 
	-$(RM) "accel.d" "alpaca.d" "main_alpaca.d" "watchdog.d" "libPF\PF_sim.d" "libfvad\src\fvad.d" "libfvad\src\signal_processing\division_operations.d" "libfvad\src\signal_processing\energy.d" "libfvad\src\signal_processing\get_scaling_square.d" "libfvad\src\signal_processing\resample_48khz.d" "libfvad\src\signal_processing\resample_by_2_internal.d" "libfvad\src\signal_processing\resample_fractional.d" "libfvad\src\signal_processing\spl_inl.d" "libfvad\src\vad\vad_core.d" "libfvad\src\vad\vad_filterbank.d" "libfvad\src\vad\vad_gmm.d" "libfvad\src\vad\vad_sp.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

