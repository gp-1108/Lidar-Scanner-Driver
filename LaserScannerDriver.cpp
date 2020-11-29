#include "LaserScannerDriver.h"

LaserScannerDriver::LaserScannerDriver(const double sens) {
	if (sens < 0 || sens > 1) {
		throw std::out_of_range("Invalid angle resolution, angle must belong to ]0,1]");
	}
	resolution = sens;
	/*
		The next two index will be initialized to an invalid index for two reasons:
		-being able to tell if an object is being ispected by any means directly after initialization
		-to set their values to appropriate number as in new_scan() they get increased before usage
	*/
	bufferIndex = -1; 
	firstScanIndex = -1;
	

	/*
		creating a pointer to an array of pointers to save data
		no need to save angles because they can be retrieved as index*resolution
	*/
	scans = new double* [BUFFER_DIM];
	for (int i = 0; i < BUFFER_DIM; i++) {
		scans[i] = nullptr; //nullptr initalization to have a distinct invalid value without needing to allocate memory
	}
}

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
		scanNumber represents the number of different measures in a single scan, therefore it's 180/resolution. The int explicit conversion
		is used to "Math.floor" the number obtained in case 180 is not multiple of resolution. It's consistent with the class usage as the 
		last decimal in the division would mean we only got to measure just part of the distance. this is not only counter-intuitive but also
		very imprecise, hence the decision to only save complete measurements.
	*/
	int scanNumber = (int)(180 / resolution);

	for (int i = 0; i < BUFFER_DIM; i++) {
		if (drive.scans[i] != nullptr) {
			scans[i] = new double[scanNumber];
			for (int j = 0; j < scanNumber; j++) {
				scans[i][j] = drive.scans[i][j];
			}
		}
	}
}

LaserScannerDriver::LaserScannerDriver(LaserScannerDriver&& drive) {
	scans = new double* [BUFFER_DIM];
	for (int i = 0; i < BUFFER_DIM; i++) {
		scans[i] = nullptr;
	}
	resolution = drive.resolution;
	bufferIndex = drive.bufferIndex;
	firstScanIndex = drive.firstScanIndex;
	int scanNumber = (int)(180 / resolution);
	for (int i = 0; i < BUFFER_DIM; i++) {
		if (drive.scans[i] != nullptr) {
			scans[i] = new double[scanNumber];
			for (int j = 0; j < scanNumber; j++) {
				scans[i][j] = drive.scans[i][j];
			}
		}
	}
	drive.clear_buffer();
}

LaserScannerDriver::~LaserScannerDriver() {
	for (int i = 0; i < BUFFER_DIM; i++) {
		delete[] scans[i];
	}
	delete[] scans;
}

void LaserScannerDriver::new_scan(const std::vector<double>& vec) {
	bufferIndex++;
	if (firstScanIndex == -1) {
		firstScanIndex = 0;
	}
	if (bufferIndex >= BUFFER_DIM) {
		bufferIndex = 0;
	}

	if (bufferIndex == firstScanIndex) {
		if (scans[firstScanIndex] == nullptr) {

		}
		else {
			firstScanIndex++;
			if (firstScanIndex >= BUFFER_DIM) {
				firstScanIndex = 0;
			}
		}
	}


	int vecSize = vec.size();
	int scanNumber = (int)(180 / resolution); //using int explicit conversion to round-down the number of measures per scan to an integer
	
	//checking whether the array has already been initialized or not
	if (scans[bufferIndex] == nullptr) {
		scans[bufferIndex] = new double[scanNumber];
	}

	for (int i = 0; i < scanNumber; i++) {
		while (i < vecSize && i<scanNumber) {
			scans[bufferIndex][i] = vec[i];
			i++;
		}
		if (i >= scanNumber) {
			break;
		}
		scans[bufferIndex][i] = 0;
	}
}

std::ostream& operator<<(std::ostream& os, LaserScannerDriver& driver) {
	int scanNumber = (int)(180 / driver.resolution);
	for (int i = 0; i < scanNumber; i++) {
		os << driver.scans[driver.bufferIndex][i] << " ";
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

	if (scans[bufferIndex] == nullptr) {
		throw std::runtime_error("Trying to recover data from empty driver");
	}

	int scanNumber = (int)(180 / resolution);

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
	int flooredIndex = (int)rawIndex;

	if (flooredIndex - rawIndex >= 0.5) {
		return scans[bufferIndex][flooredIndex + 1];
	}

	return scans[bufferIndex][flooredIndex];
}

LaserScannerDriver& LaserScannerDriver::operator=(const LaserScannerDriver& drive) {
	double** newScans = new double* [BUFFER_DIM];
	resolution = drive.resolution;
	int scanNumber = (int)(180 / resolution);
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

	clear_buffer();
	bufferIndex = drive.bufferIndex;
	firstScanIndex = drive.firstScanIndex;

	scans = newScans;

	return *this;
}

LaserScannerDriver& LaserScannerDriver::operator=(LaserScannerDriver&& drive) {
	double** newScans = new double* [BUFFER_DIM];
	resolution = drive.resolution;
	int scanNumber = (int)(180 / resolution);
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

	clear_buffer();
	bufferIndex = drive.bufferIndex;
	firstScanIndex = drive.firstScanIndex;

	scans = newScans;

	drive.clear_buffer();

	return *this;
}

