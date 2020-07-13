#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <array>
#include <string>
#include <map>

using namespace std;

enum RuType {
	tone_26 = 0,
	tone_52,
	tone_106,
	tone_242,
	tone_484,
	tone_996,
	tone_2x996
};

struct staInfo {
	int MCSIndex;
	double buffer;
};

typedef pair<string, double> staPair;

bool comp(const staInfo& ls, const staInfo& rs) {
	return ls.buffer > rs.buffer;
}

class RuAllocation {

	typedef vector<vector<RuType>> ChannelMechanism;
	
public:
	ChannelMechanism channelMechanism;
	vector<vector<double>> rateTable{
		{0.9, 1.8, 3.8, 8.6},
		{1.8, 3.5, 7.5, 17.2},
		{2.6, 5.3, 11.3, 25.8},
		{3.5, 7.1, 15.0, 34.4},
		{5.3, 10.6, 22.5, 51.6},
		{7.1, 14.1, 30.0, 68.8},
		{7.9, 15.9, 33.8, 77.4},
		{8.8, 17.6, 37.5, 86.0},
		{10.6, 21.2, 45.0, 103.2},
		{11.8, 23.5, 50.0, 114.7}
	};


	void getChannelMechanism(int channelWidth);
	void getTheta(vector<staInfo> staList);
	double getRate(int MCSIndex, RuType ruType);

	RuType switchIntToRuType(int n);

};

void RuAllocation::getChannelMechanism(int channelWidth) {


	vector<vector<int>> vec_cm;
	
	if (channelWidth == 20) {
		stack<vector<int>> stack1;
		stack<vector<int>> stack2;
		vector<int> arr = {1,0,0,1};
		vec_cm.push_back(arr);
		stack1.push(arr);
		while (!stack1.empty()) {
			vector<int> temp = stack1.top();
			stack1.pop();
			while (temp[0] > 0) {
				temp[0] -= 1;
				temp[1] += 2;
				stack2.push(temp);
			}

		}

		while (!stack2.empty()) {
			vector<int> temp = stack2.top();
			vec_cm.push_back(temp);
			stack2.pop();
			while (temp[1] > 0) {
				temp[1] -= 1;
				temp[2] += 2;
				stack1.push(temp);
			}
		}

		while (!stack1.empty()) {
			vector<int> temp = stack1.top();
			vec_cm.push_back(temp);
			stack1.pop();
			while (temp[2] > 0) {
				temp[2] -= 1;
				temp[3] += 2;
				stack2.push(temp);
			}
		}

		while (!stack2.empty()) {
			vec_cm.push_back(stack2.top());
			stack2.pop();
		}
		
	}
	else if (channelWidth == 40) {
		stack<vector<int>> stack1;
		stack<vector<int>> stack2;
		vector<int> arr = { 1,0,0,0,0 };
		vec_cm.push_back(arr);
		stack1.push(arr);
		while (!stack1.empty()) {
			vector<int> temp = stack1.top();
			stack1.pop();
			while (temp[0] > 0) {
				temp[0] -= 1;
				temp[1] += 2;
				stack2.push(temp);
			}

		}

		while (!stack2.empty()) {
			vector<int> temp = stack2.top();
			vec_cm.push_back(temp);
			stack2.pop();
			while (temp[1] > 0) {
				temp[1] -= 1;
				temp[2] += 2;
				temp[4] += 1;
				stack1.push(temp);
			}
		}

		while (!stack1.empty()) {
			vector<int> temp = stack1.top();
			vec_cm.push_back(temp);
			stack1.pop();
			while (temp[2] > 0) {
				temp[2] -= 1;
				temp[3] += 2;
				stack2.push(temp);
			}
		}

		while (!stack2.empty()) {
			vector<int> temp = stack2.top();
			vec_cm.push_back(temp);
			stack2.pop();
			while (temp[3] > 0) {
				temp[3] -= 1;
				temp[4] += 2;
				stack1.push(temp);
			}
		}

		while (!stack1.empty()) {
			vec_cm.push_back(stack1.top());
			stack1.pop();
		}
	}
	//else if (channelWidth == 80) {
	//	
	//}

	for (auto vec : vec_cm) {

		for (auto num : vec) {
			cout << num << " ";
		}
		cout << endl;



		vector<RuType> temp;
		for (int i = 0; i < vec.size(); i++) {
			RuType ruType = switchIntToRuType(vec.size() - i - 1);
			for (int j = vec[i]; j > 0; j--) {
				temp.push_back(ruType);
			}
		}
		channelMechanism.push_back(temp);
	}
	cout << "共有" << vec_cm.size() << "种信道机制" << endl;

}

RuType RuAllocation::switchIntToRuType(int n) {
	switch (n) {
	
	case 0:
		return tone_26;
		break;
	case 1:
		return tone_52;
		break;
	case 2:
		return tone_106;
		break;
	case 3:
		return tone_242;
		break;
	case 4:
		return tone_484;
		break;
	case 5:
		return tone_996;
		break;
	case 6:
		return tone_2x996;
		break;
	}
}

double RuAllocation::getRate(int MCSIndex, RuType ruType) {
	//ruType强转int
	return rateTable[MCSIndex][ruType];
}


void RuAllocation::getTheta(vector<staInfo> staList) {
	int staN = staList.size();
	vector<RuType> res;
	double maxTheta = 0.0;
	for (auto cm : channelMechanism) {
		int ruN = cm.size();
		int minN = (staN <= ruN) ? staN : ruN;
		double maxT = 0.0;
		double totalBuffer = 0.0;
		for (int i = 0; i < minN; i++) {
			double rate = getRate(staList[i].MCSIndex, cm[i]);
			double T = staList[i].buffer / rate;
			maxT = (T > maxT) ? T : maxT;
			totalBuffer += staList[i].buffer;
		}
		double theta = totalBuffer / (maxT + 100);
		if (theta > maxTheta) {
			maxTheta = theta;
			res = cm;
		}
	}
	for (auto c : res) {
		cout << c << " ";
	}
	cout << endl;

}



int main() {

	staInfo Sta1 {3, 80.4};
	staInfo Sta2 {4, 10.5};
	staInfo Sta3 {5, 30.2};
	staInfo Sta4 {4, 10.3};
	staInfo Sta5 {6, 50.1};
	vector<staInfo> staList{ Sta1, Sta2, Sta3, Sta4, Sta5 };
	sort(staList.begin(), staList.end(), comp);

	RuAllocation example;
	example.getChannelMechanism(20);
	example.getTheta(staList);
	return 0;
}
