#include <vector>
#include <iostream>
class LaserScannerDriver
{
private:
	int bufferIndex;
	double** scans;
public:

	LaserScannerDriver(const double sens);

	virtual ~LaserScannerDriver();

	const int BUFFER_DIM = 10;

	void new_scan(const std::vector<double>& vec);

	std::vector<double>& get_scan() const; //ritorno il vector o vector&? perchè?

	void clear_buffer();

	double get_distance(const double angle) const;

	std::ostream& operator<<(std::ostream& os) const;


};

