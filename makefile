#GCC MakeFile Version 1.0.0 			         

#Define Target 
TARGET	= MPC5643L_PROJECT
TARGET_ELF	= ./Debug/Exe/$(TARGET).elf
TARGET_MAP = ./Debug/Exe/$(TARGET).map

#Directorys of source to build
SRC_DIR_SRC								=	./src/
SRC_DIR_PROJECTSETING					=	./Project_Settings/Startup_Code/
SRC_DIR_INCLUDE 						= 	./include
NXP_BUILDTOOLS_EWLC_INCLUDE 			= 	$(NXP_ROOT)/build_tools/e200_ewl2/EWL_C/include
NXP_BUILDTOOLS_EWLC_INCLUDE_PA 			= 	$(NXP_ROOT)/build_tools/e200_ewl2/EWL_C/include/pa

DEBUG_DIR			= ./Debug
EXE_DIR 			= $(DEBUG_DIR)/Exe
OBJ_DIR				= $(DEBUG_DIR)/Obj

#Define include directorys to build
INCLUDE 			+= $(SRC_DIR_SRC)
INCLUDE 			+= $(SRC_DIR_PROJECTSETING)
INCLUDE 			+= $(SRC_DIR_INCLUDE)  
INCLUDE 			+= $(NXP_BUILDTOOLS_EWLC_INCLUDE)
INCLUDE 			+= $(NXP_BUILDTOOLS_EWLC_INCLUDE_PA)

#Source files to build
SOURCE				=	INTCISRVEC_table.c
SOURCE				+= 	Vector.c
SOURCE				+= 	flashrchw.c
SOURCE				+= 	main.c
SOURCE				+= 	intc_sw_handlers.c
SOURCE				+= 	startup.S
SOURCE				+= 	Init_Intc.c
SOURCE				+= 	Init_Watchdog.c
SOURCE				+= 	Init_ModeClock.c
SOURCE				+= 	Init_Pit.c
SOURCE				+= 	Drv_Pit.c


SOURCE_NAMES = $(notdir $(SOURCE))
BASE_NAMES	 = $(basename $(SOURCE_NAMES))
OBJECTS_NAMES = $(addsuffix .o, $(BASE_NAMES))
OBJECTS = $(addprefix $(OBJ_DIR)/, $(OBJECTS_NAMES))

#Set search path
vpath % $(INCLUDE)   

#Gcc compiler option
C_OPTION			= -DSTART_FROM_FLASH
C_OPTION 			+= -DMPC564xL
C_OPTION 			+= -DE200Z4
C_OPTION 			+= -O0
C_OPTION 			+= -g3
C_OPTION 			+= -Wall
C_OPTION 			+= -c
C_OPTION 			+= -fmessage-length=0
C_OPTION 			+= -fstrict-volatile-bitfields
C_OPTION 			+= -ffunction-sections
C_OPTION 			+= -fdata-sections
C_OPTION 			+= -mcpu=e200z4
C_OPTION 			+= -specs=ewl_c9x_noio.specs
C_OPTION 			+= -mbig
C_OPTION 			+= -mvle
C_OPTION 			+= -mregnames
C_OPTION 			+= -mhard-float
C_OPTION 			+= --sysroot=$(NXP_ROOT)/build_tools/e200_ewl2
C_OPTION 			+= $(patsubst %,-I%,$(INCLUDE)) 
           

#Gcc assembler option
A_OPTION 		= -c
A_OPTION 		+= -x assembler-with-cpp
A_OPTION 		+= -DSTART_FROM_FLASH
A_OPTION 		+= -DMPC564xL
A_OPTION 		+= -DDISABLE_SWT
A_OPTION 		+= -DMMU_CONFIG
A_OPTION 		+= -DE200Z4
A_OPTION 		+= -DI_CACHE
A_OPTION 		+= -DSPE_ENABLE
A_OPTION 		+= -g3
A_OPTION 		+= -mcpu=e200z4
A_OPTION 		+= -specs=ewl_c9x_noio.specs
A_OPTION 		+= -mbig
A_OPTION 		+= -mvle
A_OPTION 		+= -mregnames
A_OPTION 		+= --sysroot=$(NXP_ROOT)/build_tools/e200_ewl2
A_OPTION 		+= $(patsubst %,-I%,$(INCLUDE))   


#Gcc Linker option
L_OPTION		= -Wl,-Map,$(TARGET_MAP)
L_OPTION		+= -Xlinker
L_OPTION		+= --gc-sections
L_OPTION		+= -mcpu=e200z4 
L_OPTION		+= -specs=ewl_c9x_noio.specs 
L_OPTION		+= -mhard-float
L_OPTION		+= -T"./Project_Settings/Linker_Files/56xx_flash.ld"
L_OPTION		+= -T"./Project_Settings/Linker_Files/libs.ld"
L_OPTION		+= --sysroot=$(NXP_ROOT)/build_tools/e200_ewl2


#Define Rules (make objects)
$(OBJ_DIR)/%.o: %.c
	@echo 'Building file: $<'
	@powerpc-eabivle-gcc $(C_OPTION) -o $@ $<
	@echo 'Building Success: $@'
	@echo ' '

$(OBJ_DIR)/%.o: %.S
	@echo 'Building file: $<'
	@powerpc-eabivle-gcc $(A_OPTION) -o $@ $<
	@echo 'Building Success: $@'
	@echo ' '


#Build targets
.PHONY: dir all build clean 

all: dir build

dir: 
	@echo make dir...
	@if test ! -d $(DEBUG_DIR); then mkdir -p $(DEBUG_DIR);fi
	@if test ! -d $(EXE_DIR); then mkdir -p $(EXE_DIR);fi
	@if test ! -d $(OBJ_DIR); then mkdir -p $(OBJ_DIR);fi

build: $(OBJECTS)
	@echo 'Linking Start'
	powerpc-eabivle-gcc -o $(TARGET_ELF) $^ $(L_OPTION)
	@echo 'Linking Success : $(TARGET_ELF)'
	@echo ' '

clean:
	@rm	-rf	$(OBJ_DIR)/*.o $(TARGET_ELF) $(TARGET_MAP)
	@if test -d $(EXE_DIR); then rm -r $(EXE_DIR);fi
	@if test -d $(OBJ_DIR); then rm -r $(OBJ_DIR);fi
	@if test -d $(DEBUG_DIR); then rm -r $(DEBUG_DIR);fi