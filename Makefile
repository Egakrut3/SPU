BIN_DIR = bin/
BIN_SUF = .o
make_bin_path = $(addprefix $(BIN_DIR), $(addsuffix $(BIN_SUF), $(1)))
make_dir_bin_path = $(addprefix $(1), $(call make_bin_path, $(2)))

SRC_DIR = src/
SRC_SUF = .cpp
make_src_path = $(addprefix $(SRC_DIR), $(addsuffix $(SRC_SUF), $(1)))
make_dir_src_path = $(addprefix $(1), $(call make_src_path, $(2)))

H_DIR = ./Assembler/include/ ./Disassembler/include ./SPU/include/ ./Others/include/

LIB_DIR = static_libs/
LIBS = Colored_printf My_stack

CXX = g++
CXX_FLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline   \
-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default  \
-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy \
-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op                      \
-Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith          \
-Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits           \
-Wwrite-strings -Werror=vla -D_EJUDGE_CLIENT_SIDE -D__USE_MINGW_ANSI_STDIO -D_DEBUG

TARGET = $(addprefix ./bin/, Assembler.exe)

Assembler_OBJ = Assembler_basic Assembler_functions
Disassembler_OBJ = Disassembler
SPU_OBJ = SPU_common SPU_basic SPU_functions
Others_OBJ = Common main

make_object = $(call make_bin_path, $(1)) : $(call make_src_path, $(1)); \
@$(CXX) $(CXX_FLAGS) -c $$< $(addprefix -I, $(H_DIR)) -o $$@
make_dir_object = $(call make_dir_bin_path, $(1), $(2)) : $(call make_dir_src_path, $(1), $(2)); \
@$(CXX) $(CXX_FLAGS) -c $$< $(addprefix -I, $(H_DIR)) -o $$@

.PHONY : all prepare clean

all : prepare $(call make_dir_bin_path, ./Assembler/, $(Assembler_OBJ))			\
			  $(call make_dir_bin_path, ./Others/, $(Others_OBJ))
	@$(CXX) $(CXX_FLAGS) $(call make_dir_bin_path, ./Assembler/, $(Assembler_OBJ))			\
			  			 $(call make_dir_bin_path, ./Others/, $(Others_OBJ))				\
            -L$(LIB_DIR) $(addprefix -l, $(LIBS)) -o $(TARGET)
	@echo Compilation end
	@$(TARGET)

prepare :
	@mkdir -p ./Assembler/bin/ ./Disassembler/bin/ ./SPU/bin/ ./Others/bin/ ./bin/

$(call make_dir_object, ./Assembler/, Assembler_basic)

$(call make_dir_object, ./Assembler/, Assembler_functions)

#$(call make_dir_object, ./Disassembler/, Disassembler)

#$(call make_dir_object, ./SPU/, SPU_common)

#$(call make_dir_object, ./SPU/, SPU_basic)

#$(call make_dir_object, ./SPU/, SPU_functions)

$(call make_dir_object, ./Others/, main)

$(call make_dir_object, ./Others/, Common)

clean:
	@rm -rf ./Assembler/bin/ ./Disassembler/bin/ ./SPU/bin/ ./Others/bin/ ./bin/	\
	Byte_code Text_byte_code.txt Dis_code.txt
