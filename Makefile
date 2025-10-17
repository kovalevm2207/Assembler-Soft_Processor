DED_FLAGS = -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat \
		    -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported \
		    -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
		    -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual \
		    -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-promo \
		    -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
		    -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef \
		    -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
		    -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation \
		    -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
		    -Wstack-usage=8192 -pie -fPIE -Werror=vla \
		    -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

MODE?=

CPP_FILES = $(wildcard *.cpp)
OBJECT_FILES = $(CPP_FILES:.cpp=.o)
HEADER_FILES = $(wildcard *.h)

# Исполняемые файлы
all: soft_processor assambler

soft_processor: SoftProcessor.o commands.o my_stack.o
	@g++ $(DED_FLAGS) $(MODE) commands.o SoftProcessor.o my_stack.o -o soft_processor

assambler: assambler.o
	@g++ $(DED_FLAGS) $(MODE) assambler.o -o assambler

# Объектные файлы
assambler.o: assambler.cpp $(HEADER_FILES)
	@g++ $(DED_FLAGS) $(MODE) -c assambler.cpp -o assambler.o

SoftProcessor.o: SoftProcessor.cpp $(HEADER_FILES)
	@g++ $(DED_FLAGS) $(MODE) -c SoftProcessor.cpp -o SoftProcessor.o

commands.o: commands.cpp $(HEADER_FILES)
	@g++ $(DED_FLAGS) $(MODE) -c commands.cpp -o commands.o

my_stack.o: my_stack.cpp $(HEADER_FILES)
	@g++ $(DED_FLAGS) $(MODE) -c my_stack.cpp -o my_stack.o

clean:

	rm *.o
