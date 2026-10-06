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

	static std::string to_hex_adv(const uint32_t* words, size_t n) {
		int i = static_cast<int>(n) - 1;
		while (i > 0 && words[i]==0) {
			--i;
		}

		char buf[16];
		std::snprintf(buf, sizeof(buf), "%x", words[i]);
		std::string s = buf;

		for (int j = i-1; j>= 0; --j) {
			std::snprintf(buf, sizeof(buf), "%08x", words[j]);
			s += buf;
		}

		return s;
	}

	std::string to_hex() const {
		return to_hex_adv(digits, WORDS);
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

	uint32_t mul_dig(uint32_t val, BigInt& res) const {
		uint64_t carry = 0;

		for (size_t i = 0; i < WORDS; i++) {
			uint64_t prod = static_cast<uint64_t>(digits[i]) * val + carry;
			res.digits[i] = static_cast<uint32_t>(prod);
			carry = prod >> 32;
		}

		return static_cast<uint32_t>(carry);
	}

	void mul(const BigInt& num, uint32_t res_words[2*WORDS]) const {
		for(size_t k = 0; k < 2*WORDS; ++k) {
			res_words[k] = 0;
		}

		for (size_t i = 0; i < WORDS; ++i) {
			if (num.digits[i] == 0) continue;

			BigInt temp;
			uint32_t carry_dig = mul_dig(num.digits[i], temp);

			uint64_t carry = 0;
			for (size_t j = 0; j < WORDS; ++j) {
				uint64_t sum = static_cast<uint64_t>(res_words[i+j]) + static_cast<uint64_t>(temp.digits[j]) + carry;

				res_words[i+j] = static_cast<uint32_t>(sum);
				carry = sum >> 32;
			}

			res_words[i+WORDS] = static_cast<uint32_t>(carry + carry_dig);
		}
	}

	void square(uint32_t res_words[2 * WORDS]) const {
		mul(*this, res_words);
	}

	size_t bit_length() const {
		int i = static_cast<int>(WORDS) - 1;
		while (i >= 0 && digits[i] == 0) {
			--i;
		}

		if (i < 0) {
			return 0;
		}

		uint32_t top = digits[i];
		size_t bits_in_top = 0;
		while (top > 0) {
			++bits_in_top;
			top >>= 1;
		}

		return static_cast<size_t>(i)*32 + bits_in_top;
	}

	void shift_l(size_t k, BigInt& res) const {
		res = BigInt::zero();

		if (k >= WORDS * 32) {
			return;
		}

		size_t word_shift = k / 32;
		size_t bit_shift  = k % 32;

		if (bit_shift == 0) {
			for (size_t i = 0; i + word_shift < WORDS; ++i) {
				res.digits[i + word_shift] = digits[i];
			}
		} else {
			for (size_t i = 0; i + word_shift < WORDS; ++i) {
				res.digits[i + word_shift] |= (digits[i] << bit_shift);

				if (i + word_shift + 1 < WORDS) {
					res.digits[i + word_shift + 1] |= (digits[i] >> (32 - bit_shift));
				}
			}
		}
	}

	void shift_r(size_t k, BigInt& res) const {
		res = BigInt::zero();

		if (k >= WORDS * 32) {
			return;
		}

		size_t word_shift = k / 32;
                size_t bit_shift  = k % 32;

		if (bit_shift == 0) {
                        for (size_t i = word_shift; i < WORDS; ++i) {
                                res.digits[i - word_shift] = digits[i];
                        }
                } else {
                        for (size_t i = word_shift; i < WORDS; ++i) {
                                res.digits[i - word_shift] |= (digits[i] >> bit_shift);

                                if (i + 1 < WORDS) {
                                        res.digits[i - word_shift] |= (digits[i+1] << (32 - bit_shift));
                                }
                        }
                }
        }

	void div(const BigInt& b, BigInt& q, BigInt& r) const {
		if (b.bit_length() == 0) {
			throw std::invalid_argument("Division by zero");
		}

		q = BigInt::zero();
		r = *this;

		if (r.cmp(b) < 0) {
			return;
		}

		BigInt one = BigInt::one();
		size_t k = b.bit_length();

		while (r.cmp(b) >= 0) {
			size_t t = r.bit_length();
			size_t shift = t - k;

			BigInt b_shifted;
			b.shift_l(shift, b_shifted);

			if (r.cmp(b_shifted) < 0) {
				--shift;
				b.shift_l(shift, b_shifted);
			}

			BigInt next_r;
			r.sub(b_shifted, next_r);
			r = next_r;

			BigInt q_term;
			one.shift_l(shift, q_term);
			BigInt next_q;
			q.add(q_term, next_q);
			q = next_q;
		}
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
	uint32_t mul_res[2 * WORDS];
	a.mul(b, mul_res);
	uint32_t sq_res[2*WORDS];
	a.square(sq_res);

	std::cout << "\na = " << a.to_hex() << "\n";
        std::cout << "b = " << b.to_hex() << "\n";
	std::cout << " a + b (hex) = " << sum.to_hex() << " (carry: " << carry << ")\n";
	std::cout << " a - b (hex) = " << diff.to_hex() << " (borrow: " << borrow << ")\n";
	std::cout << "a * b = " << BigInt::to_hex_adv(mul_res, 2 * WORDS) << "\n";
	std::cout << "a^2 = " << BigInt::to_hex_adv(sq_res, 2 * WORDS) << "\n";

	std::cout << "\na cmp a: " << a.cmp(a) << "\n";
    	std::cout << "0 cmp 1: " << z.cmp(o) << "\n";
    	std::cout << "1 cmp 0: " << o.cmp(z) << "\n";
	std::cout << "a cmp b: " << a.cmp(b) << "\n";

	BigInt max;
	o.shift_l(2047, max);
	std::cout << "\n1 << 2047 = " << max.bit_length() << "\n";

	BigInt of;
	o.shift_l(2048, of);
	std::cout << "1 << 2048 = " << of.to_hex() << "\n";

	BigInt sh32, res;
	o.shift_l(32, sh32);
	sh32.shift_r(32, res);
	std::cout << "100000000 >> 32 = " << res.to_hex() << "\n";

	BigInt q, r;
	a.div(b, q, r);
	std::cout << "\na / b (hex) = " << q.to_hex() << "\n";
	std::cout << "a mod b (hex) = " << r.to_hex() << "\n";

	BigInt q1, r1;
        a.div(a, q1, r1);
	std::cout << "a / a (hex) = " << q1.to_hex() << "\n";

	return 0;
}
