BIN_DIR = ./bin/
BIN_SUF = .o
make_bin_path = $(addprefix $(BIN_DIR), $(addsuffix $(BIN_SUF), $(1)))

SRC_DIR = ./src/
SRC_SUF = .cpp
make_src_path = $(addprefix $(SRC_DIR), $(addsuffix $(SRC_SUF), $(1)))

H_DIR = ./include/

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

TARGET = $(addprefix $(BIN_DIR), Assembler.exe)

OBJ = Common Option_manager User_error_handler main

make_object = $(call make_bin_path, $(1)) : $(call make_src_path, $(1)); \
@$(CXX) $(CXX_FLAGS) -c $$< -I $(H_DIR) -o $$@

.PHONY : all prepare clean

all : prepare $(call make_bin_path, $(OBJ))
	@$(CXX) $(CXX_FLAGS) $(call make_bin_path, $(OBJ)) -L$(LIB_DIR) $(addprefix -l, $(LIBS)) -o $(TARGET)
	@echo Compilation end
	@$(TARGET)

prepare :
	@mkdir -p bin

$(call make_object, Common)

$(call make_object, Option_manager)

$(call make_object, User_error_handler)

$(call make_object, main)

clean:
	@rm -rf bin
