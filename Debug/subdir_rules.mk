################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/packages" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/products/bios_6_45_00_20/packages" --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/products/tidrivers_cc32xx_2_15_00_26/packages" --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/products/uia_2_00_02_39/packages" --include_path="C:/Users/a0505179/Documents/Activities/TIEM/IR_HUB_2025_2/smart_ir_hub" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink/" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink/include" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink/source" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/oslib" --include_path="C:/ti/ccsv8/ccs_base" --include_path="C:/ti/xdctools_3_32_00_06_core" --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/packages" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/example/common" --define=ccs --define=SL_PLATFORM_MULTI_THREADED --define=USE_TIRTOS --define=cc3220 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_ccs.obj: C:/ti/CC3200SDK_1.5.0/cc3200-sdk/example/common/startup_ccs.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/packages" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink_extlib/provisioninglib" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/products/bios_6_45_00_20/packages" --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/products/tidrivers_cc32xx_2_15_00_26/packages" --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/products/uia_2_00_02_39/packages" --include_path="C:/Users/a0505179/Documents/Activities/TIEM/IR_HUB_2025_2/smart_ir_hub" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink/" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink/include" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/simplelink/source" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/driverlib" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/inc" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/oslib" --include_path="C:/ti/ccsv8/ccs_base" --include_path="C:/ti/xdctools_3_32_00_06_core" --include_path="C:/ti/tirtos_cc32xx_2_15_00_17/packages" --include_path="C:/ti/CC3200SDK_1.5.0/cc3200-sdk/example/common" --define=ccs --define=SL_PLATFORM_MULTI_THREADED --define=USE_TIRTOS --define=cc3220 -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


