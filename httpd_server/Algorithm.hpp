#pragma once
#ifndef ALGORITHM
#define ALGORITHM

#include <vector>
#include <string>
using namespace std;

// 算法
class Algorithm
{
public:
	Algorithm();
	~Algorithm();
public:
	// KMP算法查找子串
	static size_t findSubstring(vector<char> s, string p) {
		size_t n = s.size(), m = p.size();
		if (m == 0) return 0;
		// 设置哨兵
		s.insert(s.begin(), ' ');
		p.insert(p.begin(), ' ');
		vector<int> next(m + 1);
		//预处理next数组
		for (size_t i = 2, j = 0; i <= m; i++) {
			while (j && p[i] != p[j+1])
			{
				j = next[j];
			}
		}
		// 匹配过程
		for (size_t i = 1, j = 0; i <= n; i++) {
			while (j && s[i] != p[j + 1]) {
				j = next[j];
			}

			if (s[i] == p[j + 1]) {
				j++;
			}

			if (j == m) {
				return i - m;
			}
		}
		return -1;
	}

};

Algorithm::Algorithm()
{
}

Algorithm::~Algorithm()
{
}

#endif // !ALGORITHM