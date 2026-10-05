#include <iostream>
#include <cstdint>
#include <string>
#include <cstdio>
#include <cctype>
#include <stdexcept>
#include <random>

constexpr size_t WORDS = 64;

struct BigInt { 
	uint32_t digits[WORDS];

	BigInt() {
		for (size_t i=0; i < WORDS; ++i) {
			digits[i] = 0;
		}
	}

	BigInt(uint32_t val) {
		digits[0] = val;
		for (size_t i=1; i< WORDS; ++i) {
			digits[i]=0;
		}
	}
	
	static BigInt zero() {
		return BigInt(0);
	}
	
	static BigInt one() {
		return BigInt(1);
	}

	std::string to_hex() const {
		int i = static_cast<int>(WORDS) - 1;
		while (i > 0 && digits[i]==0) {
			--i;
		}

		char buf[16];
		std::snprintf(buf, sizeof(buf), "%x", digits[i]);
		std::string s = buf;

		for (int j = i-1; j>= 0; --j) {
			std::snprintf(buf, sizeof(buf), "%08x", digits[j]);
			s += buf;
		}

		return s;
	}

	static BigInt from_hex(std::string hex) {
		BigInt res;

		if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
			hex = hex.substr(2);
		}

		for (char c: hex) {
			if (!std::isxdigit(static_cast<unsigned char>(c))) {
				throw std::invalid_argument("incorrect hex symbol");
			}
		}

		int len = static_cast<int>(hex.length());
		size_t idx = 0;

		for (int i=len; i>0; i-=8) {
			if (idx>= WORDS) {
				throw std::overflow_error("number >= 2048 bits");
			}

			int start = (i>= 8) ? i-8: 0;
			int count = i - start;
			std::string part = hex.substr(start, count);

			res.digits[idx] = static_cast<uint32_t>(std::stoul(part, nullptr, 16));
			idx++;
		}

		return res;
	}

	uint32_t add(const BigInt& num, BigInt& res) const{
		uint64_t carry = 0;

		for (size_t i = 0; i < WORDS; ++i) {
			uint64_t temp = static_cast<uint64_t>(digits[i]) + num.digits[i] + carry;
			res.digits[i] = static_cast<uint32_t>(temp);
			carry = temp >> 32;
		}

		return static_cast<uint32_t>(carry);
	}


	static BigInt random() {
    		static std::random_device rd;
    		static std::mt19937_64 gen(rd());
    		std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

    		BigInt res;
    		for (size_t i = 0; i < WORDS; ++i) {
        		res.digits[i] = dis(gen);
    		}

    		return res;
	}
};


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
	BigInt sum;
	uint32_t c = a.add(b, sum);
	std::cout << "a = " << a.to_hex() << "\n";
        std::cout << "b = " << b.to_hex() << "\n";
	std::cout << " a + b (hex) = " << sum.to_hex() << " (carry: " << c << ")\n";

	return 0;
}
