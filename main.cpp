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

	uint32_t sub(const BigInt& num, BigInt& res) const{
		int64_t borrow = 0;

		for (size_t i = 0; i < WORDS; ++i) {
                        int64_t temp = static_cast<int64_t>(digits[i]) - static_cast<int64_t>(num.digits[i]) - borrow;

			if (temp < 0) {
				temp += (1LL << 32);
				borrow = 1;
			} else {
				borrow = 0;
			}

			res.digits[i] = static_cast<uint32_t>(temp);
                }

                return static_cast<uint32_t>(borrow);
	}


	int cmp(const BigInt& num) const {
		for (int i = static_cast<int>(WORDS) - 1; i>=0; --i) {
			if (digits[i] > num.digits[i]) {
				return 1;
			}
			if (digits[i] < num.digits[i]) {
				return -1;
			}
		}

		return 0;
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
	BigInt sum, diff;
	uint32_t carry = a.add(b, sum);
	uint32_t borrow = a.sub(b, diff);

	std::cout << "\na = " << a.to_hex() << "\n";
        std::cout << "b = " << b.to_hex() << "\n";
	std::cout << " a + b (hex) = " << sum.to_hex() << " (carry: " << carry << ")\n";
	std::cout << " a - b (hex) = " << diff.to_hex() << " (borrow: " << borrow << ")\n";

	std::cout << "\na cmp a: " << a.cmp(a) << "\n";
    	std::cout << "0 cmp 1: " << z.cmp(o) << "\n";
    	std::cout << "1 cmp 0: " << o.cmp(z) << "\n";
	std::cout << "a cmp b: " << a.cmp(b) << "\n";

	return 0;
}
