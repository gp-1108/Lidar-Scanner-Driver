#include <iostream>
#include "LaserScannerDriver.h"

using namespace std;

int main()
{
	//questa prova testa anche il metodo di troncamento valori di new scan

	cout << "Utilizzo costruttore: " << endl;
	LaserScannerDriver prova(1.0);
	
	cout << "Utilizzo new_scan() con vettore partenza 0" << endl;
	vector<double> zero;
	for(int i = 0; i < 190; i++) {
		zero.push_back(i);
	}
	prova.new_scan(zero);

	cout << "Utilizzo new_scan() con vettore partenza 200" << endl;
	vector<double> duecento;
	for (int i = 200; i < 390; i++) {
		duecento.push_back(i);
	}
	prova.new_scan(duecento);

	cout << "Utilizzo new_scan() con vettore partenza 400" << endl;
	vector<double> quattrocento;
	for (int i = 400; i < 590; i++) {
		quattrocento.push_back(i);
	}
	prova.new_scan(quattrocento);


	cout << "Utilizzo new_scan() !sovrascrittura del vettore 0! con partenza 600" << endl;
	vector<double> seicento;
	for (int i = 600; i < 790; i++) {
		seicento.push_back(i);
	}
	prova.new_scan(seicento);

	cout << "Utilizzo dell'operatore<< (dovrebbe uscire il seicento)" << endl;
	cout << prova << endl;

	LaserScannerDriver prova1 = prova;

	cout << "Utilizzo di get_scan() primo tentativo (dovrebbe uscire 200)" << endl;
	vector<double> firstGetScan = prova1.get_scan();
	for (int i = 0; i < firstGetScan.size(); i++) {
		cout << firstGetScan[i] << " ";
	}

	cout << endl << "Utilizzo di get_scan() secondo tentativo(dovrebbe uscire 400)" << endl;
	vector<double> secondGetScan = prova1.get_scan();
	for (int i = 0; i < secondGetScan.size(); i++) {
		cout << secondGetScan[i] << " ";
	}
	
	cout << endl << "Utilizzo di get_scan() terzo tentativo (dovrebbe uscire 600)" << endl;
	vector<double> thirdGetScan = prova1.get_scan();
	for (int i = 0; i < thirdGetScan.size(); i++) {
		cout << thirdGetScan[i] << " ";
	}

	/*
	cout << endl << "Utilizzo metodo clear_buffer()" << endl;
	prova.clear_buffer();
	*/

	cout << "Reinserimento degli array zero -> duecento -> quattrocento" << endl;
	prova1.new_scan(zero);
	prova1.new_scan(duecento);
	prova1.new_scan(quattrocento);

	cout << "Utilizzo di get_distance(90) dovrebbe dare 490 :" << prova1.get_distance(90) << endl;
	
	cout << "Utilizzo di get_scan() dovrebbe dare zero" << endl;
	vector<double> temp = prova1.get_scan();
	for (int i = 0; i < temp.size(); i++) {
		cout << temp[i] << " ";
	}
	
}
