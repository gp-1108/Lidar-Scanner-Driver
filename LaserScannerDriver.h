#include <vector>
#include <iostream>
class LaserScannerDriver
{
private:
	int bufferIndex;
	int firstScanIndex;
	double** scans;
	double resolution;
public:
	/*
		LaserScannerDriver class doesn't have a default constructor. That's due to a Developer choice: it doesn't make sense to 
		intialize a drive for a sensor without the main infos about the sensor itself. Also it's an error-prone behavior because
		it could lead to data insertions witout actually knowing how many data will be saved and to which angle they correspond.
		Therefore it is thought to have precise data when initalizing a Driver.
	*/

	/*
		BUFFER_DIM constant is declared in the public section for logic reasons. Any class-utilizer can see the buffer dimension and (if needed) 
		write re-usable code without the hassle of changing code whenever he/she modifies the buffer size in the class header.
	*/
	const int BUFFER_DIM = 3; 

	LaserScannerDriver& operator=(const LaserScannerDriver& drive);

	LaserScannerDriver& operator=(LaserScannerDriver&& drive);

	LaserScannerDriver(LaserScannerDriver&& drive);

	LaserScannerDriver(const double sens);

	LaserScannerDriver(const LaserScannerDriver& drive);
	
	~LaserScannerDriver();
	
	void new_scan(const std::vector<double>& vec);

	void clear_buffer();

	std::vector<double> get_scan(); 

	double get_distance(const double angle) const;
	
	friend std::ostream& operator<<(std::ostream& os, LaserScannerDriver& const driver);
};


