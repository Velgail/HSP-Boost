#pragma once
#include <boost/multiprecision/cpp_int.hpp>
extern int N;
typedef boost::multiprecision::cpp_int boost_longint;
#define repeat(rN) for(int for_cnt=0;for_cnt<rN;for_cnt++)
class boost_fixedpoint {
	boost_longint data;
public:
	boost_fixedpoint() = default;
	~boost_fixedpoint() = default;
	boost_fixedpoint(const std::string& ref) {
		int cnt = N;
		bool is_after_decimal_point = false;
		data = 0;
		//浮動小数点数表記("1.23e4"等)は対応していません。だって面倒……
		for (auto itr : ref) {
			if ((itr >= '0') && (itr <= '9')) {
				data *= 10;
				data += (itr - '0');
				if (is_after_decimal_point) {
					cnt--;
				}
			}
			else if ((itr == '.') && (!is_after_decimal_point)) {
				is_after_decimal_point = true;
			}
			if (cnt == 0) {
				break;
			}
		}
		while (cnt--) {
			data *= 10;
		}
	}
	boost_fixedpoint(const int ref) {
		data = ref;
		repeat(N) {
			data *= 10;
		}
	}
	boost_fixedpoint(const double ref) {
		auto tmp = ref;
		data = static_cast<boost_longint>(tmp);
		int lc = std::min(N, 16);
		if (data < boost_longint("10000000000000000")) {
			repeat(lc) {
				tmp *= 10;
			}
			data = static_cast<boost_longint>(tmp);
		}
		else {
			lc = 0;
		}
		repeat(N - lc) {
			data *= 10;
		}
	}
	std::string str() {
		std::string ret = data.str();
		int fill = N + 1 - ret.size();
		std::string fill0 = "";
		while (fill0.size() != fill) {
			fill0.push_back('0');
		}
		ret.insert(0, fill0);
		ret.insert(ret.size() - N, ".");
		return ret;
	}
	operator int() {
		boost_longint ret = data;
		repeat(N) {
			ret /= 10;
		}
		return static_cast<int>(ret);
	}
	operator double() {
		double ret = static_cast<double>(data);
		repeat(N) {
			ret /= 10;
		}
		return ret;
	}
	boost_fixedpoint& operator=(boost_fixedpoint rValue) {
		data = rValue.data;
		return *this;
	}
	boost_fixedpoint& operator+=(boost_fixedpoint rValue) {
		data += rValue.data;
		return *this;
	}
	boost_fixedpoint& operator-=(boost_fixedpoint rValue) {
		data -= rValue.data;
		return *this;
	}
	boost_fixedpoint& operator*=(boost_fixedpoint rValue) {
		data *= rValue.data;
		repeat(N) {
			data /= 10;
		}
		return *this;
	}
	boost_fixedpoint& operator/=(boost_fixedpoint rValue) {
		repeat(N) {
			data *= 10;
		}
		data /= rValue.data;
		return *this;
	}
	bool operator==(boost_fixedpoint rValue) {
		return data == rValue.data;
	}
	bool operator>=(boost_fixedpoint rValue) {
		return data >= rValue.data;
	}
	bool operator<=(boost_fixedpoint rValue) {
		return data <= rValue.data;
	}
	bool operator<(boost_fixedpoint rValue) {
		return data < rValue.data;
	}
	bool operator>(boost_fixedpoint rValue) {
		return data > rValue.data;
	}
	bool operator!=(boost_fixedpoint rValue) {
		return data != rValue.data;
	}
};
