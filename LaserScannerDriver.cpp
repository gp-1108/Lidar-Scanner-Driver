#include "LaserScannerDriver.h"

/*
	  Main constructor with sensor resolution as argument. It's the "main" constructor as any kind of other constructor of this classs relies
	  indirectly onto this one. The angle resolution must belong to [0.1,1] otherwise a out_of_range exception is thrown. The internal buffer get initialized to an array of nullptr.
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

	newScanIndex = -1; 
	oldScanIndex = -1;
	

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
	newScanIndex = drive.newScanIndex;
	oldScanIndex = drive.oldScanIndex;


	/*
		As "dataNumber" is widely used in this class it will be commented only once (here):
		dataNumber represents the number of different measures in a single scan including angle 180, therefore it's approx 180/resolution. 
		The int explicit conversion is used to "Math.floor" the number obtained in case 180 is not multiple of 
		resolution and the +1 to "Math.ceil" 180/resolution to allow for the measurement at angle 180. 
	*/
	int dataNumber = (static_cast<int>(180 / resolution))+1;


	for (int i = 0; i < BUFFER_DIM; i++) {
		if (drive.scans[i] != nullptr) {
			scans[i] = new double[dataNumber];
			for (int j = 0; j < dataNumber; j++) {
				scans[i][j] = drive.scans[i][j];
			}
		}
	}
}

/*
	Move Constructor: As it's a constructor there's no need to de-allocate scans or scans elements. The indexes get moved as well.
	The invalidation of the object moved is done by assigning to nullptr scans
 */
LaserScannerDriver::LaserScannerDriver(LaserScannerDriver&& drive) {

	resolution = drive.resolution;
	newScanIndex = drive.newScanIndex;
	oldScanIndex = drive.oldScanIndex;
	scans = drive.scans;

	drive.scans = nullptr;
	drive.newScanIndex = -1;
	drive.oldScanIndex = -1;
}


/*
 * Object Distructor: it ensures to clear all data allocated with new[] by constructor and/or assignment.
 * 
 */
LaserScannerDriver::~LaserScannerDriver() {

	for (int i = 0; i < BUFFER_DIM && scans!=nullptr; i++) { //checking scans!=nullptr in case distructor gets called on "moved" objects
		delete[] scans[i];
	}
	delete[] scans;
}

/*
 * new_scan() inserts a vector<double> of measures into the main memory (scans) onto the index newScanIndex + 1 (updated on the firs line).
 * If the data are not enoguh to fill the entire space allocated in the array (scanNuber) then they get initialized to zero, on the contrary 
 * if there are more data then needed the last measures don't get saved.
 * 
 */
void LaserScannerDriver::new_scan(const std::vector<double>& vec) {
	/*
		Checking wether the buffer has ever been used, if not then sets firstScan Index to the correct position.
	*/
	if (oldScanIndex == -1) {
		oldScanIndex = 0;
	}

	/*
		newScanIndex is used as the last position where data had been inserted. This behavior makes the get_distance() method much
		more intuitive. It requires the index to be incremented immediately before insertion.
	*/
	newScanIndex++;
	
	if (newScanIndex >= BUFFER_DIM) {        //circular-buffer
		newScanIndex = 0;
	}

	/*
		Check if the most-recent (newScanIndex) and least-recent (oldScanIndex) indexes are the same -> buffer full or empty
	*/
	if (newScanIndex == oldScanIndex) {
		/*
			There are two possibilities:
			1. Buffer is full (scans[oldScanIndex] it's not empty (=nullptr)) then we need to move oldScanIndex as we are updating those data
			soon with another scan
			2. Buffer is empty then it's correct to have most-recent and least-recent points to the new scan insertion. No need to change anything
		*/
		if(scans[oldScanIndex] != nullptr) {	
			oldScanIndex++;
			if (oldScanIndex >= BUFFER_DIM) {
				oldScanIndex = 0;
			}
		}
	}


	int vecSize = vec.size();   //called once as it will be later used multiple times.
	int dataNumber = (static_cast<int>(180 / resolution))+1; 
	
	//checking whether the array has already been created or not
	if (scans[newScanIndex] == nullptr) {
		scans[newScanIndex] = new double[dataNumber];
	}

	for (int i = 0; i < dataNumber; i++) {			//SISTEMARE chiarezza del codice
		while (i < vecSize && i < dataNumber) {
			scans[newScanIndex][i] = vec[i];
			i++;
		}
		
		if (i >= dataNumber) {  //too much data
			break;
		}
		scans[newScanIndex][i] = 0; //not enough data
	}
}


/* 
 *  operator<< has been implemented with the aid of two different public function members to avoid friend: is_empty() and get_resolution(). 
 *	By using get_distance() it retrieves all datas of the last scan and returns them with the ostream. If the buffer is empty then it retrieves
 * a simple string saying so.
 * 
 */
std::ostream& operator<<(std::ostream& os,const LaserScannerDriver& driver) {
    
	if (driver.is_empty()) {
		os << "Empty Buffer";
		return os;
	}

	double angle = 0;
	double resolution = driver.get_resolution();
	for (int i = 0; angle <= 180; i++) {
		angle = i * resolution;
		if (angle > 180) {
			break;
		}
		os << driver.get_distance(angle) << " ";
	}
	return os;
}


/*
	Deletes all data stored and sets the Driver state as if it had just been initialized.
*/
void LaserScannerDriver::clear_buffer() {
	for (int i = 0; i < BUFFER_DIM; i++) {
		delete[] scans[i];
		scans[i] = nullptr;
	}

	newScanIndex = -1;
	oldScanIndex = -1;
}

/*
	Returns a vector<double> filled with the last scan inserted by new_scan(). If empty it throws an runtime_error
	As vector-class is already defined the choice of returning reference or copy is left to the vector-class implementation.
*/
std::vector<double> LaserScannerDriver::get_scan() {

	if (is_empty()) {
		throw std::runtime_error("Trying to recover data from empty driver");
	}

	int dataNumber = (static_cast<int>(180 / resolution))+1;

	std::vector<double> firstScan;

	for (int i = 0; i < dataNumber; i++) {
		firstScan.push_back(scans[oldScanIndex][i]);
	}

	delete[] scans[oldScanIndex];
	scans[oldScanIndex] = nullptr;

	oldScanIndex++;
	if (oldScanIndex >= BUFFER_DIM) {
		oldScanIndex = 0;
	}

	return firstScan;
}

/*
	get_distance(angle) returns the most similar valued associated to angle in the last scan inserted. If the driver is empty then a runtime_error 
	is thrown. In case angle doesnt belong to [0,180] interval an out_of_range exception is thrown. This choice had been made to avoid confusion
	and overall consistency as we don't accept any measurement above 180°
*/
double LaserScannerDriver::get_distance(const double angle) const {
	if (is_empty()) {
		throw std::runtime_error("Trying to get distance from empty Driver");
	}

	if (angle < 0 || angle > 180) {
		throw std::out_of_range("Angle requested exceeds [0,180] valid interval");
	}

	double rawIndex = angle / resolution;
	int flooredIndex = static_cast<int>(rawIndex);

    int dataNumber = (static_cast<int>(180/resolution))+1;
    
	if (rawIndex - flooredIndex >= 0.5 && rawIndex<=((static_cast<double>(dataNumber))-1)) { //the max index allowed for upper-rounding is scanNuber-1
		return scans[newScanIndex][flooredIndex + 1];											
	}

	return scans[newScanIndex][flooredIndex];
}

/*
	Copy-assignment follows the deep copy philosophy as far as copying the indexes to ensure identical behavior in both the copied object and the copy
*/
LaserScannerDriver& LaserScannerDriver::operator=(const LaserScannerDriver& drive) {
	double** newScans = new double* [BUFFER_DIM];
	resolution = drive.resolution;
	int dataNumber = (static_cast<int>(180 / resolution))+1;
	for (int i = 0; i < BUFFER_DIM; i++) {
		if (drive.scans[i] == nullptr) {
			newScans[i] = nullptr;
		}
		else {
			newScans[i] = new double[dataNumber];
			for (int j = 0; j < dataNumber; j++) {
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


	newScanIndex = drive.newScanIndex;
	oldScanIndex = drive.oldScanIndex;

	scans = newScans;

	return *this;
}


/*
	Move-assignment
*/
LaserScannerDriver& LaserScannerDriver::operator=(LaserScannerDriver&& drive) {

	for (int i = 0; i < BUFFER_DIM; i++) {
		delete[] scans[i];
	}
	delete[] scans;

	scans = drive.scans;
	resolution = drive.resolution;
	newScanIndex = drive.newScanIndex;
	oldScanIndex = drive.oldScanIndex;

	drive.newScanIndex = -1;
	drive.oldScanIndex = -1;
	drive.scans = nullptr;

	return *this;
}

/*
	Returns the resolution of this particular driver
*/
double LaserScannerDriver::get_resolution() const{
	return resolution;
}


/*
	Checks wheter the Driver is empty or not
*/
bool LaserScannerDriver::is_empty() const {
	if (newScanIndex == -1 || scans[newScanIndex] == nullptr) {
		return true;
	}
	return false;
}