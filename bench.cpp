#include <iostream>
#include <chrono>
#include <x86intrin.h>
#include "BigInt.hpp"

int main() {
	BigInt a = BigInt::random();
	BigInt b = BigInt::random();

	if (a < b) {
		std::swap(a, b);
	}

	BigInt b_1024 = BigInt::random();
	for (size_t i = 32; i < WORDS; ++i) { 
		b_1024.digits[i] = 0;
	}
	b_1024.digits[31] |= 0x80000000;

	BigInt res, q, r, exp(17);
	uint32_t buf[2 * WORDS];

	uint64_t c1 = __rdtsc();
	auto t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 100000; ++i) a.add(b, res);
	auto t2 = std::chrono::high_resolution_clock::now();
	uint64_t c2 = __rdtsc();
	std::cout << "Add (+): " << (c2 - c1) / 100000 << " cycles | " << std::chrono::duration<double, std::micro>(t2 - t1).count() / 100000 << " us\n";

	c1 = __rdtsc();
	t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 100000; ++i) a.sub(b, res);
	t2 = std::chrono::high_resolution_clock::now();
	c2 = __rdtsc();
	std::cout << "Sub (-): " << (c2 - c1) / 100000 << " cycles | " << std::chrono::duration<double, std::micro>(t2 - t1).count() / 100000 << " us\n";

	c1 = __rdtsc();
	t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 100000; ++i) a.shift_l(37, res);
	t2 = std::chrono::high_resolution_clock::now();
	c2 = __rdtsc();
	std::cout << "Shift (<<): " << (c2 - c1) / 100000 << " cycles | " << std::chrono::duration<double, std::micro>(t2 - t1).count() / 100000 << " us\n";

	c1 = __rdtsc();
	t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 10000; ++i) a.mul(b, buf);
	t2 = std::chrono::high_resolution_clock::now();
	c2 = __rdtsc();
	std::cout << "Mul (*): " << (c2 - c1) / 10000 << " cycles | " << std::chrono::duration<double, std::micro>(t2 - t1).count() / 10000 << " us\n";

	c1 = __rdtsc();
	t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 10000; ++i) a.square(buf);
	t2 = std::chrono::high_resolution_clock::now();
	c2 = __rdtsc();
	std::cout << "Sqr: " << (c2 - c1) / 10000 << " cycles | " << std::chrono::duration<double, std::micro>(t2 - t1).count() / 10000 << " us\n";

	c1 = __rdtsc();
	t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 100; ++i) a.div(b, q, r);
	t2 = std::chrono::high_resolution_clock::now();
	c2 = __rdtsc();
	std::cout << "Div: " << (c2 - c1) / 100 << " cycles | " << std::chrono::duration<double, std::micro>(t2 - t1).count() / 100 << " us\n";

	c1 = __rdtsc();
	t1 = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 100; ++i) BigInt::power(a, exp, res);
	t2 = std::chrono::high_resolution_clock::now();
	c2 = __rdtsc();
	std::cout << "Power: " << (c2 - c1) / 100 << " cycles | " << std::chrono::duration<double, std::micro>(t2 - t1).count() / 100 << " us\n";

    return 0;
}
