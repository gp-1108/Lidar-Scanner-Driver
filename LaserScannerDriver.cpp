#include "LaserScannerDriver.h"


/*
 *  Main constructor with sensor resolution as argument. It's the main constructor used as any kind of other constructor of this classs relies
 *  indirectly onto this one. The angle resolution must belong to [0,1], the internal buffer get initialized to an array of nullptr.
 */
LaserScannerDriver::LaserScannerDriver(const double sens) {
	if (sens < 0.1 || sens > 1) {
		throw std::out_of_range("Invalid angle resolution, angle must belong to [0.1,1]");
	}
	resolution = sens;
    /*
		The next two index will be initialized to an invalid index for two reasons:
		-being able to tell if an object is being ispected by any means directly after initialization
		-to set their values to appropriate number as in new_scan() they get increased before usage
	*/
	bufferIndex = -1; 
	firstScanIndex = -1;
	

	scans = new double* [BUFFER_DIM];
	for (int i = 0; i < BUFFER_DIM; i++) {
		scans[i] = nullptr; //nullptr initalization to have a distinct invalid value without needing to allocate memory
	}
}

/*
	Copy constructor and assignment follow the deep copy philosophy as far as copying the buffer indexes to ensure an
	identical behavior in the object copied compared to the original one.
*/
LaserScannerDriver::LaserScannerDriver(const LaserScannerDriver& drive) {
	
	/*
		scans initialization for later usage of scans.
	*/
	scans = new double* [BUFFER_DIM];
	for (int i = 0; i < BUFFER_DIM; i++) {
		scans[i] = nullptr;
	}


	resolution = drive.resolution;
	bufferIndex = drive.bufferIndex;
	firstScanIndex = drive.firstScanIndex;


	/*
		As "scanNumber" is widely used in this class it will be commented only once (here):
		scanNumber represents the number of different measures in a single scan including angle 180, therefore it's approx 180/resolution. 
		The int explicit conversion is used to "Math.floor" the number obtained in case 180 is not multiple of 
		resolution and the +1 to "Math.ceil" 180/resolution to allow for the measurement at angle 180. 
	*/
	int scanNumber = (static_cast<int>(180 / resolution))+1;


	for (int i = 0; i < BUFFER_DIM; i++) {
		if (drive.scans[i] != nullptr) {
			scans[i] = new double[scanNumber];
			for (int j = 0; j < scanNumber; j++) {
				scans[i][j] = drive.scans[i][j];
			}
		}
	}
}

/*
	Move Constructor: As it's a constructor there's no need to de-allocate scans or scans elements. The indexes get moved as well.
  
 */
LaserScannerDriver::LaserScannerDriver(LaserScannerDriver&& drive) {

	resolution = drive.resolution;
	bufferIndex = drive.bufferIndex;
	firstScanIndex = drive.firstScanIndex;
	scans = drive.scans;

	drive.scans = nullptr;
	drive.bufferIndex = -1;
	drive.firstScanIndex = -1;
}


/*
 * Object Distructor: it ensures to clear all data allocated with new[] by constructor and/or assignment.
 * 
 */
LaserScannerDriver::~LaserScannerDriver() {

	for (int i = 0; i < BUFFER_DIM && scans!=nullptr; i++) {
		delete[] scans[i];
	}
	delete[] scans;
}

/*
 * new_scan() inserts a vector<double> of measures into the main memory (scans) onto the index bufferIndex + 1 (updated on the firs line).
 * If the data are not enoguh to fill the entire space allocated in the array (scanNuber) then they get initialized to zero, on the contrary 
 * if there are more data then needed last measures don't get saved.
 * 
 */
void LaserScannerDriver::new_scan(const std::vector<double>& vec) {
	/*
		bufferIndex is used as the last position where data had been inserted. This behavior makes the get_distance() method much
		more intuitive. It requires the index to be incremented immediately before insertion.
	*/
	bufferIndex++;
	
	/*
		Checking wether the buffer has ever been used, if not then sets firstScan Index to the correct position.
	*/
	if (firstScanIndex == -1) {
		firstScanIndex = 0;
	}

	
	if (bufferIndex >= BUFFER_DIM) {        //circular-buffer
		bufferIndex = 0;
	}

	/*
		Check if the most-recent (bufferIndex) and least-recent (firstScanIndex) indexes are the same -> buffer full or empty
	*/
	if (bufferIndex == firstScanIndex) {
		/*
			There are two possibilities:
			1. Buffer is full (scans[firstScanIndex] it's not empty (=nullptr)) then we need to move firstScanIndex as we are updating those data
			soon with another scan
			2. Buffer is empty then it's correct to have most-recent and least-recent points to the new scan insertion. No need to change anything
		*/
		if(scans[firstScanIndex] != nullptr) {	
			firstScanIndex++;
			if (firstScanIndex >= BUFFER_DIM) {
				firstScanIndex = 0;
			}
		}
	}


	int vecSize = vec.size();   //called once as it will be later used multiple times.
	int scanNumber = (static_cast<int>(180 / resolution))+1; //using int explicit conversion to round-down the number of measures per scan to an integer
	
	//checking whether the array has already been created or not
	if (scans[bufferIndex] == nullptr) {
		scans[bufferIndex] = new double[scanNumber];
	}

	for (int i = 0; i < scanNumber; i++) {
		while (i < vecSize && i<scanNumber) {
			scans[bufferIndex][i] = vec[i];
			i++;
		}
		if (i >= scanNumber) {  //to much data
			break;
		}
		scans[bufferIndex][i] = 0; //not enough data
	}
}


/* 
 *  operator<< has been implemented with the aid of two different public function members to avoid friend.
 *  Rather than using ger_resolution() and get_distance() and saving some performance (not on an asymptotic scale) here is used a 
 *  get_recent_scan() which returns a vector<double> of the most recent scan. This choice has been made to improve class 
 * usage by implementing potentially useful function member without any asymptotic penalty.
 * 
 */
std::ostream& operator<<(std::ostream& os,const LaserScannerDriver& driver) {
    
	if (driver.is_empty()) {
		os << "Empty Buffer";
		return os;
	}
	double angle = 0;
	double res = driver.get_resolution();
	for (int i = 0; angle <= 180; i++) {
		angle = i * res;
		os << driver.get_distance(angle) << " ";
	}
	return os;
}



void LaserScannerDriver::clear_buffer() {
	for (int i = 0; i < BUFFER_DIM; i++) {
		delete[] scans[i];
		scans[i] = nullptr;
	}

	bufferIndex = -1;
	firstScanIndex = -1;
}

std::vector<double> LaserScannerDriver::get_scan() {

	if (bufferIndex == -1 || scans[bufferIndex] == nullptr) {
		throw std::runtime_error("Trying to recover data from empty driver");
	}

	int scanNumber = (static_cast<int>(180 / resolution))+1;

	std::vector<double> firstScan;

	for (int i = 0; i < scanNumber; i++) {
		firstScan.push_back(scans[firstScanIndex][i]);
	}

	delete[] scans[firstScanIndex];
	scans[firstScanIndex] = nullptr;
	firstScanIndex++;
	if (firstScanIndex >= BUFFER_DIM) {
		firstScanIndex = 0;
	}

	return firstScan; //essendo gia gestita la classe vector se la fa lei di come le pare
}

double LaserScannerDriver::get_distance(const double angle) const {
	if (scans[bufferIndex] == nullptr) {
		throw std::runtime_error("Trying to access to empty Driver");
	}

	if (angle < 0 || angle > 180) {
		throw std::out_of_range("Angle request exceed [0,180] valid interval");
	}

	double rawIndex = angle / resolution;
	int flooredIndex = static_cast<int>(rawIndex);

    int scanNumber = (static_cast<int>(180/resolution))+1;
    
	if (rawIndex - flooredIndex >= 0.5 && rawIndex<=(scanNumber-1)) {
		return scans[bufferIndex][flooredIndex + 1];
	}

	return scans[bufferIndex][flooredIndex];
}

LaserScannerDriver& LaserScannerDriver::operator=(const LaserScannerDriver& drive) {
	double** newScans = new double* [BUFFER_DIM];
	resolution = drive.resolution;
	int scanNumber = (static_cast<int>(180 / resolution))+1;
	for (int i = 0; i < BUFFER_DIM; i++) {
		if (drive.scans[i] == nullptr) {
			newScans[i] = nullptr;
		}
		else {
			newScans[i] = new double[scanNumber];
			for (int j = 0; j < scanNumber; j++) {
				newScans[i][j] = drive.scans[i][j];
			}
		}
	}

	/*
		Freeing memory before giving new values to prevent memory leaks
	*/
	for (int i = 0; i < BUFFER_DIM; i++) {
		delete[] scans[i];
	}
	delete[] scans;


	bufferIndex = drive.bufferIndex;
	firstScanIndex = drive.firstScanIndex;

	scans = newScans;

	return *this;
}

LaserScannerDriver& LaserScannerDriver::operator=(LaserScannerDriver&& drive) {

	for (int i = 0; i < BUFFER_DIM; i++) {
		delete[] scans[i];
	}
	delete[] scans;

	scans = drive.scans;
	resolution = drive.resolution;
	bufferIndex = drive.bufferIndex;
	firstScanIndex = drive.firstScanIndex;

	drive.bufferIndex = -1;
	drive.firstScanIndex = -1;
	drive.scans = nullptr;

	return *this;
}

double LaserScannerDriver::get_resolution() const{
	return resolution;
}

bool LaserScannerDriver::is_empty() const {
	return false;
}