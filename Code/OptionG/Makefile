CC  = gcc
CXX = g++

all : matmul matmul.s matmul.out matmult matmult.s matmult.out

matmul.out : matmul
	perf stat -d -d -d numactl -C 8 ./matmul 2>&1 | tee -a matmul.out

matmul_v2.out : matmul_v2
	./$< 2>&1 | tee -a $@

matmul_v3.out : matmul_v3
	./$< 2>&1 | tee -a $@

% : %.o
	$(CXX) $< -o $@

memory : memory.o
	$(CXX) $< -o $@

%.o : %.cc
	$(CXX) -O2 -fno-unroll-loops -c $<
	$(CXX) -g -c $<

%.s : %.o
	objdump -D $< > $@

clean :
	rm -rf matmul.out matmul.s matmul.o matmul.trace matmul
	rm -rf matmul_v2.out matmul_v2.s matmul_v2.o matmul_v2.trace matmul_v2
	rm -rf matmul_v3.out matmul_v3.s matmul_v3.o matmul_v3.trace matmul_v3
