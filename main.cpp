#include <iostream>
#include "BigInt.hpp"

int main() {
	BigInt z = BigInt::zero();
	BigInt o = BigInt::one();
	BigInt val(42);

	std::cout << "Zero: " << z.to_hex() << "\n";
	std::cout << "One: " << o.to_hex() << "\n";
	std::cout << "Value: " << val.to_hex() << "\n";

	std::string test = "3130313031303130313031303130313031303130";
	BigInt test_num = BigInt::from_hex(test);
	std::cout << "Test number: " << test_num.to_hex() << "\n";

	BigInt a = BigInt::random();
	BigInt b = BigInt::random();

	std::cout << "\na = " << a.to_hex() << "\n";
	std::cout << "b = " << b.to_hex() << "\n";

	BigInt sum = a + b;
	BigInt diff = a - b;
	BigInt mul = a * b; 

	BigInt sum_raw, diff_raw;
	uint32_t carry = a.add(b, sum_raw);
	uint32_t borrow = a.sub(b, diff_raw);
	uint32_t mul_full[2 * WORDS];
 	a.mul(b, mul_full);
	uint32_t sq_full[2 * WORDS];
	a.square(sq_full);

	std::cout << " a + b (hex) = " << sum.to_hex() << " (carry: " << carry << ")\n";
	std::cout << " a - b (hex) = " << diff.to_hex() << " (borrow: " << borrow << ")\n";
	std::cout << " a * b (mod) = " << mul.to_hex() << "\n";
	std::cout << "a * b (full) = " << BigInt::to_hex_adv(mul_full, 2 * WORDS) << "\n";
	std::cout << "a^2   (full) = " << BigInt::to_hex_adv(sq_full, 2 * WORDS) << "\n";

	std::cout << "\na cmp a: " << a.cmp(a) << "\n";
 	std::cout << "0 cmp 1: " << z.cmp(o) << "\n";
  	std::cout << "1 cmp 0: " << o.cmp(z) << "\n";
	std::cout << "a cmp b: " << a.cmp(b) << "\n";

	BigInt max = o << 2047;
	std::cout << "\n1 << 2047 bit_length = " << max.bit_length() << "\n";

	BigInt of = o << 2048;
	std::cout << "1 << 2048 = " << of.to_hex() << "\n";

	BigInt sh32 = o << 32;
	BigInt res = sh32 >> 32;
	std::cout << "1 << 32 = " << sh32.to_hex() << "\n";
	std::cout << "100000000 >> 32 = " << res.to_hex() << "\n";

	BigInt q = a / b;
	BigInt r = a % b;
	std::cout << "\na / b (hex) = " << q.to_hex() << "\n";
	std::cout << "a mod b (hex) = " << r.to_hex() << "\n";

	BigInt q1 = a / a;
	std::cout << "a / a (hex) = " << q1.to_hex() << "\n";

	BigInt p0, p1, p2;
	BigInt::power(a, z, p0);
	BigInt::power(a, o, p1);
	BigInt::power(BigInt(2), BigInt(10), p2);

	std::cout << "\na^0 (hex) = " << p0.to_hex() << "\n";
	std::cout << "a^1 (hex) = " << p1.to_hex() << "\n";
	std::cout << "2^10 (hex) = " << p2.to_hex() << "\n";

    return 0;
}
