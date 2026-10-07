#include <iostream>
#include "BigInt.hpp"

int main() {
	BigInt one = BigInt::one();
	BigInt a = BigInt::random();
	BigInt b = BigInt::random();
	BigInt c = BigInt::random();

	if (a.cmp(b) < 0) {
		 std::swap(a, b);
	}

	std::cout << "a = " << a.to_hex() << "\n\n";
	std::cout << "b = " << b.to_hex() << "\n\n";
	std::cout << "c = " << c.to_hex() << "\n";

	BigInt dist_left  = (a + b) * c;
	BigInt dist_right = (a * c) + (b * c);
	std::cout << "(a + b) * c       = " << dist_left.to_hex() << "\n";
	std::cout << "(a * c) + (b * c) = " << dist_right.to_hex() << "\n";

	if (dist_left == c * (a + b) && dist_left == dist_right) {
		std::cout << "Distributivity test passed!\n\n";
	} else {
		std::cout << "Distributivity test failed!\n\n";
	}


	BigInt sum = BigInt::zero();
	for (int i = 0; i < 1000; ++i) {
		sum += a;
	}
	BigInt mul = a * BigInt(1000);
	std::cout << "1000 * a = " << mul.to_hex() << "\n";
    	std::cout << "sum = " << sum.to_hex() << "\n";
	if (mul == sum) {
		std::cout << "Multiple addition test passed!\n\n";
	} else {
		std::cout << "Multiple addition test failed!\n\n";
	}


	BigInt q = a / b;
	BigInt r = a % b;
	BigInt recon = (b * q) + r;
	std::cout << "q         = " << q.to_hex() << "\n";
	std::cout << "r         = " << r.to_hex() << "\n";
	std::cout << "b * q + r = " << recon.to_hex() << "\n";
	if (r.cmp(b) < 0 && recon == a) {
		std::cout << "Division invariant test passed!\n\n";
	} else {
	std::cout << "Division invariant test failed!\n\n";
	}


	BigInt a2, b2;
	BigInt::power(a, BigInt(2), a2);
	BigInt::power(b, BigInt(2), b2);
	BigInt diff_left  = (a + b) * (a - b);
	BigInt diff_right = a2 - b2;
	std::cout << "(a + b) * (a - b) = " << diff_left.to_hex() << "\n";
	std::cout << "a^2 - b^2 = " << diff_right.to_hex() << "\n";
	if (diff_left == diff_right) {
		std::cout << "Difference of squares test passed!\n\n";
	} else {
		std::cout << "Difference of squares test failed!\n\n";
	}


	BigInt ax, ay, a_xy;
	BigInt::power(a, BigInt(3), ax);
	BigInt::power(a, BigInt(4), ay);
	BigInt::power(a, BigInt(7), a_xy);
	BigInt pow_mul = ax * ay;
	std::cout << "a^3 * a^4 = " << pow_mul.to_hex() << "\n";
	std::cout << "a^7 = " << a_xy.to_hex() << "\n";
	if (a_xy == pow_mul) {
		std::cout << "Power rule test passed!\n\n";
	} else {
		std::cout << "Power rule test failed!\n\n";
	}


	BigInt a_sh = a;
	for (size_t i = WORDS - 4; i < WORDS; ++i) {
		a_sh.digits[i] = 0;
	}
	BigInt shift_res = (a_sh << 100) >> 100;
	std::cout << "a_masked = " << a_sh.to_hex() << "\n";
	std::cout << "(a_sh << 100) >> 100 = " << shift_res.to_hex() << "\n";
	if (shift_res == a_sh) {
		std::cout << "Shift roundtrip test passed!\n\n";
	} else {
		std::cout << "Shift roundtrip test failed!\n\n";
	}


	size_t k = 77;
	BigInt pow2 = one << k;
	BigInt div_pow2 = a / pow2;
	BigInt shift_k  = a >> k;
	std::cout << "a / (1 << 77) = " << div_pow2.to_hex() << "\n";
 	std::cout << "a >> 77 = " << shift_k.to_hex() << "\n";
	if ((a / pow2) == (a >> k)) {
		std::cout << "Div by 2^k vs shift test passed!\n\n";
	} else {
		std::cout << "Div by 2^k vs shift test failed!\n\n";
	}

    return 0;
}
