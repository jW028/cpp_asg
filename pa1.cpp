#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
	int block_length, block_width, house_length, house_width;
	int block_area, house_area, lawn_area, mowing_time;
	const int SPEED = 2;

	cout << "\tCalculate Mowing Time" << endl;
	cout << "\t----------------------" << endl;
	cout << "\tEnter house block Measurement:" << endl;
	cout << "\tEnter block length:";
	cin >> block_length;
	cout << "\tEnter block width:";
	cin >> block_width;

	cout << "\n\tEnter House Measurement:" << endl;
	cout << "\tEnter house length:";
	cin >> house_length;
	cout << "\tEnter house width:";
	cin >> house_width;

	block_area = block_length * block_width;
	house_area = house_length * house_width;
	lawn_area = block_area - house_area;
	mowing_time = lawn_area / SPEED;

	cout << "\n\t------Actual Output------" << endl;
	cout << left;
	cout << setw(16) << "\tBlock Length: " << block_length << "\n"
		<< setw(16) << "\tBlock Width: " << block_width << "\n"
		<< setw(16) << "\tHouse Length: " << house_length << "\n"
		<< setw(16) << "\tHouse Width: "  << house_width
		<< "\n\n\tMowing time: " << mowing_time << " minutes" << endl;
	cout << "\t-----------------------------";

	return 0;
}


