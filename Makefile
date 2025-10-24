BIN_DIR = ./bin/
BIN_SUF = .o
make_bin_path = $(addprefix $(BIN_DIR), $(addsuffix $(BIN_SUF), $(1)))
make_dir_bin_path = $(addprefix $(1), $(call make_bin_path, $(2)))

SRC_DIR = ./src/
SRC_SUF = .cpp
make_src_path = $(addprefix $(SRC_DIR), $(addsuffix $(SRC_SUF), $(1)))
make_dir_src_path = $(addprefix $(1), $(call make_src_path, $(2)))

H_DIR = ./Assembler/include/ ./SPU/include/ ./include/

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

TARGET = $(addprefix ./bin/, Test.exe)

Assembler_OBJ = Assembler_basic Assembler_functions
SPU_OBJ = SPU_basic SPU_functions
OBJ = My_functions File_input Option_manager main

make_object = $(call make_bin_path, $(1)) : $(call make_src_path, $(1)); \
@$(CXX) $(CXX_FLAGS) -c $$< $(addprefix -I, $(H_DIR)) -o $$@
make_dir_object = $(call make_dir_bin_path, $(1), $(2)) : $(call make_dir_src_path, $(1), $(2)); \
@$(CXX) $(CXX_FLAGS) -c $$< $(addprefix -I, $(H_DIR)) -o $$@

.PHONY : all prepare clean Fact Square Circle

all : prepare $(call make_bin_path, $(OBJ))
	@$(CXX) $(CXX_FLAGS) $(call make_dir_bin_path, ./Assembler/, $(Assembler_OBJ))	\
						 $(call make_dir_bin_path, ./SPU/, $(SPU_OBJ))				\
			  			 $(call make_bin_path, $(OBJ))								\
            -L$(LIB_DIR) $(addprefix -l, $(LIBS)) -o $(TARGET)
	@echo Compilation end

Fact : all
	@$(TARGET) --in Solutions/Fact.txt

Square : all
	@$(TARGET) --in Solutions/Square.txt

Circle : all
	@$(TARGET) --in Solutions/Circle.txt

prepare :
	@make -C ./Assembler/
	@make -C ./SPU/
	@mkdir -p ./bin/

$(call make_object, My_functions)

$(call make_object, File_input)

$(call make_object, Option_manager)

$(call make_object, main)

clean:
	@rm -rf ./bin/ Byte_code Text_byte_code.txt
