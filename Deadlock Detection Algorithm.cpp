//Operating system project 


//C++ Program to detect deadlock in a given state

#include <iostream>
#include <cstdlib>

using namespace std;

#define BOOL_STR(b) (b?"true":"false")

// Matrix Class
class GraphMatrix
{
private:
	int iProcesses;
	int iResources;
	int **Matrix;
	bool *visited;

public:
	GraphMatrix(int iProcesses, int iResources)
	{
		this->iProcesses = iProcesses;
		this->iResources = iResources;
		visited = new bool[iProcesses];
		Matrix = new int*[iProcesses];
		for (int i = 0; i < iProcesses; i++)
		{
			Matrix[i] = new int[iResources];
			for (int j = 0; j < iResources; j++)
			{
				Matrix[i][j] = 0;
			}
		}
	}

	// Adding Edge to Graph
	void add_edge(int origin, int destin, int count)
	{
		if (origin > iProcesses || destin > iResources || origin < 0 || destin < 0 || count < 0)
		{
			cout << "Invalid edge!\n";
		}
		else
		{
			Matrix[origin - 1][destin - 1] = count;
		}
	}

	// Free resouces
	void free_edges(int process, int resource, int iunits)
	{
		Matrix[process - 1][resource - 1] += iunits;
	}

	// Print the graph
	void display()
	{
		int i, j;
		for (i = 0;i < iProcesses;i++)
		{
			for (j = 0; j < iResources; j++)
				cout << Matrix[i][j] << "  ";
			cout << endl;
		}
	}

	void remainResources(int* workingSet) {

		int sumResAll;

		for (int i = 0; i < iResources; i++) {

			sumResAll = 0;

			for (int j = 0; j < iProcesses; j++) {

				sumResAll += this->Matrix[j][i];
			}

			workingSet[i] -= sumResAll;

		}

	}

	void freeupResources(int* workingSet, int row) {

		for (int i = 0; i<iResources; i++) {
			workingSet[i] += this->Matrix[row][i];
			this->Matrix[row][i] = 0;
		}

	}

	int sumElementsRow(int row) {

		int sum = 0;

		for (int i = 0; i<iResources; i++)
			sum += this->Matrix[row][i];

		return sum;
	}

	bool checkifSatifies(int* workingSet, int row) {

		bool satisfies = true;
		for (int i = 0; i<iResources; i++) {
			cout << "Checking " << workingSet[i] << " and " << this->Matrix[row][i] << endl;
			if (workingSet[i] < this->Matrix[row][i]) {
				satisfies = false;
				break;
			}
		}

		return satisfies;
	}

};

bool reduceAndCheck(int* workingSet, int processes, int resources, bool* bComplete, GraphMatrix& allocmat, GraphMatrix& reqmat) {

	bool allTrue = true;

	for (int j = 0; j<processes; j++) {
		if (!bComplete[j]) {
			allTrue = false;
			break;
		}
	}

	cout << "****************************************************************************" << endl;
	cout << "reduceAndCheck : Have all processes completed? " << BOOL_STR(allTrue) << endl;

	if (allTrue)
		return true;
	else {

		//Loop over bComplete to eliminate processes

		for (int i = 0; i<processes; i++) {

			cout << "Checking process " << i + 1 << " - it is";

			if (!bComplete[i]) { //process not complete, reduce

				cout << " not complete" << endl << endl;

				if (reqmat.checkifSatifies(workingSet, i) == true) {

					cout << "Process " << i + 1 << " is satified " << endl << endl;
					//Free up resources and complete process
					allocmat.freeupResources(workingSet, i);
					bComplete[i] = true;

					cout << " >> Updated Resources Working Set : ";
					for (int tt = 0; tt<resources; tt++)
						cout << workingSet[tt] << " ";
					cout << endl << endl;

					return reduceAndCheck(workingSet, processes, resources, bComplete, allocmat, reqmat);
				}
				else {
					cout << "Process " << i + 1 << " is not satified " << endl << endl;
				}

			}
			else
				cout << " complete" << endl << endl;
		}

		return false;
	}

}

//Main
int main()
{
	int processes, resources;
	int max_edges, origin, destin, countres;

	int *iResAvail;
	bool *bComplete;

	//set_the_stage();

	cout << "Enter number of processes: ";
	cin >> processes;

	cout << "Enter number of resources: ";
	cin >> resources;

	//Process status list
	bComplete = new bool[processes];

	for (int i = 0; i<processes; i++)
		bComplete[i] = false;


	cout << "Update allocation matrix: ";
	GraphMatrix allocmat(processes, resources);
	max_edges = processes * resources;

	for (int i = 0; i < max_edges; i++)
	{
		cout << "Enter edge (process, resource, count) ";
		cin >> origin >> destin >> countres;
		if ((origin == -1) && (destin == -1))
			break;
		allocmat.add_edge(origin, destin, countres);
	}
	allocmat.display();


	cout << "Update request matrix: ";
	//Obtain the request matrix
	GraphMatrix reqmat(processes, resources);
	for (int i = 0; i < max_edges; i++)
	{
		cout << "Enter edge (process, resource, count)  ";
		cin >> origin >> destin >> countres;
		if ((origin == -1) && (destin == -1))
			break;
		reqmat.add_edge(origin, destin, countres);
	}
	reqmat.display();

	//Resource capacity
	iResAvail = new int[resources];
	for (int i = 0; i < resources; i++)
	{
		cout << "Enter number of units of the resource " << i + 1 << endl;
		cin >> iResAvail[i];
	}

	int* workingSet = new int[resources];
	for (int i = 0; i<resources; i++) {
		workingSet[i] = iResAvail[i];
	}

	cout << "----------------------------- BEGIN PROCESSING -----------------------------" << endl << endl;

	allocmat.remainResources(workingSet);

	//Eliminate sink nodes
	for (int i = 0; i<processes; i++) {
		if (!reqmat.sumElementsRow(i)) {
			allocmat.freeupResources(workingSet, i);
			bComplete[i] = true;
		}
	}

	cout << "After allocating resources and eliminating sink nodes, Resources Working Set available currently: " << endl;

	for (int i = 0; i<resources; i++) {
		cout << workingSet[i] << " ";
	}
	cout << endl << endl;

	//Recursively reduce and check if a process can complete

	bool result = reduceAndCheck(workingSet, processes, resources, bComplete, allocmat, reqmat);

	cout << "-----------------------------------------------------------------------------" << endl << endl;

	if (result)
		cout << "DEADLOCK FREE!" << endl;
	else {
		cout << "DEADLOCK DETECTED! BETWEEN PROCESSES ";
		for (int i = 0; i<processes; i++) {
			if (!bComplete[i])
				cout << "P" << i << " ";
		}
		cout << endl;
	}

	//Printing everything!!

	cout << endl;
	cout << "Printing matrices at the end of processing : " << endl << endl;

	cout << "Complete : ";
	for (int i = 0; i<processes; i++)
		cout << bComplete[i] << " ";
	cout << endl << endl;

	cout << "Workingset : ";
	for (int i = 0; i<resources; i++) {
		cout << workingSet[i] << " ";
	}
	cout << endl << endl;

	cout << "AllocMat : " << endl;
	allocmat.display();

	return 0;
}