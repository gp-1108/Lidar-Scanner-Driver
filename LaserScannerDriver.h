#include <vector>
#include <iostream>
class LaserScannerDriver
{
private:
	int bufferIndex;
	double** scans;
	double resolution;
public:
	
	const int BUFFER_DIM = 3;

	LaserScannerDriver(const double sens);
	
	~LaserScannerDriver();
	
	void new_scan(const std::vector<double>& vec);

	void clear_buffer();

	/*
	std::vector<double> get_scan() const; //ritorno il vector o vector&? perchè?

	

	double get_distance(const double angle) const;
	*/

	friend std::ostream& operator<<(std::ostream& os, LaserScannerDriver& const driver);
};


