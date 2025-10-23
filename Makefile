DED_FLAGS =	-ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat \
		    -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported \
		    -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
		    -Wformat-signedness -Wformat=2 -Winline -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual \
		    -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-promo \
		    -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods \
		    -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef \
		    -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers \
		    -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation \
		    -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
		    -Wstack-usage=8192 -pie -fPIE -Werror=vla \
		    -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

MODE?=

CPP_FILES = $(wildcard *.cpp)
OBJECT_FILES = $(CPP_FILES:.cpp=.o)
HEADER_FILES = $(wildcard *.h)

# Исполняемые фалы
all: soft_processor assambler converter hash_maker

soft_processor: SoftProcessor.o commands.o my_stack.o ReadFile.o
	g++ $(DED_FLAGS) $(MODE) commands.o SoftProcessor.o my_stack.o ReadFile.o -o soft_processor

assambler: assambler.o commands.o my_stack.o ReadFile.o
	g++ $(DED_FLAGS) $(MODE) assambler.o commands.o my_stack.o ReadFile.o -o assambler

converter: converter.o
	g++ $(DED_FLAGS) $(MODE) converter.o -o converter

hash_maker: hash_maker.o
	g++ $(DED_FLAGS) $(MODE) hash_maker.o commands.o my_stack.o -o hash_maker

# Объектные файлы
assambler.o: assambler.cpp $(HEADER_FILES)
	g++ $(DED_FLAGS) $(MODE) -c assambler.cpp -o assambler.o

SoftProcessor.o: SoftProcessor.cpp $(HEADER_FILES)
	g++ $(DED_FLAGS) $(MODE) -c SoftProcessor.cpp -o SoftProcessor.o

commands.o: commands.cpp $(HEADER_FILES)
	g++ $(DED_FLAGS) $(MODE) -c commands.cpp -o commands.o

ReadFile.o: ReadFile.cpp $(HEADER_FILES)
	g++ $(DED_FLAGS) $(MODE) -c ReadFile.cpp -o ReadFile.o

my_stack.o: my_stack.cpp $(HEADER_FILES)
	g++ $(DED_FLAGS) $(MODE) -c my_stack.cpp -o my_stack.o

converter.o: CONVERTER.cpp $(HEADER_FILES)
	g++ $(DED_FLAGS) $(MODE) -c CONVERTER.cpp -o converter.o

hash_maker.o: hash_maker.cpp $(HEADER_FILES)
	g++ $(DED_FLAGS) $(MODE) -c hash_maker.cpp -o hash_maker.o

# очистка
clean:

	rm *.o
