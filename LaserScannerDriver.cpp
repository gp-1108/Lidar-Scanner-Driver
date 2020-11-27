#include "LaserScannerDriver.h"

LaserScannerDriver::LaserScannerDriver(const double sens) {
	resolution = sens;
	bufferIndex = -1; //so when it get increased it's in the correct position

	


	//creating a pointer to an array of pointers to save data
	//no need to save angles because they can be retrieved as index*resolution
	scans = new double* [BUFFER_DIM];
	for (int i = 0; i < BUFFER_DIM; i++) {
		scans[i] = nullptr;
	}
}

LaserScannerDriver::~LaserScannerDriver() {
	for (int i = 0; i < BUFFER_DIM; i++) {
		delete scans[i];
	}
	delete scans;
}

void LaserScannerDriver::new_scan(const std::vector<double>& vec) {
	bufferIndex++;
	if (bufferIndex >= BUFFER_DIM) {
		bufferIndex = 0;
	}

	int vecSize = vec.size();
	int scanNumber = (int)(180 / resolution); //using int explicit conversion to round-down the number of measures per scan to an integer
	
	//checking whether the array has already been initialized
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

std::ostream& operator<<(std::ostream& os, LaserScannerDriver& driver){
	int scanNumber = (int)(180 / driver.resolution);
	for (int i = 0; i < scanNumber; i++) {
		os << driver.scans[driver.bufferIndex][i] << " ";
	}
	return os;
}

void LaserScannerDriver::clear_buffer() {
	for (int i = 0; i < BUFFER_DIM; i++) {
		delete scans[i];
	}

	for (int i = 0; i < BUFFER_DIM; i++) {
		scans[i] = nullptr;
	}
	bufferIndex = -1;
}