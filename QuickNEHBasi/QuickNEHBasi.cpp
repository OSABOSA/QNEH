#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>
#include <chrono>

class Task
{
public:
	Task();
	Task(int m);
	Task(const Task& task);
	~Task();
	void fill(int *machines);
	void fill();
	int* getMachines() { return machines; }
	int& operator[](size_t index) {
		if (index < 0 || index >= m) {
			return defaultValue;
		}
		else {
			return machines[index];
		}
	}

private:
	int* machines;
	int m;
	int defaultValue = 0;
};

Task::Task()
{
	machines = nullptr;
	m = 0;
}

Task::Task(int m)
{
	machines = new int[m];
	this->m = m;
}

Task::Task(const Task& task)
{
	machines = new int[task.m];
	m = task.m;
	for (int i = 0; i < m; i++) {
		machines[i] = task.machines[i];
	}
}

Task::~Task()
{
}

void Task::fill(int* machines)
{
	for (int i = 0; i < m; i++) {
		this->machines[i] = machines[i];
	}
}

void Task::fill()
{
	for (int i = 0; i < m; i++) {
		this->machines[i] = 0;
	}
}



class Graf
{
public:
	Graf();
	~Graf();
	Graf(int j, int m);
	Task& operator[](size_t index) {
		if (index < 0 || index >= j) {
			return defaultValue;
		}
		else {
			return tasks[index];
		}
	}
	void insert(int index, Task task);
	void remove(int index);
	int getJ() { return j; }
	int getM() { return m; }
private:
	Task* tasks;
	int j;
	int m;
	Task defaultValue;

};

Graf::Graf()
{
	tasks = nullptr;
	j = 0;
	m = 0;
	defaultValue = Task();
}

Graf::~Graf()
{
}

Graf::Graf(int j, int m)
{
	tasks = new Task[j];
	for (int i = 0; i < j; i++) {
		tasks[i] = Task(m);
	}
	this->j = j;
	this->m = m;
	defaultValue = Task();
}

void Graf::insert(int index, Task task)
{
	// dodaj zadanie na pozycje index wpierw przesuwajac wszystkie zadania o 1
	if (index >= 0 && index <= j) {
		Task* newTasks = new Task[j + 1];
		for (int i = 0; i < j; i++) {
			if (i < index) {
				newTasks[i] = tasks[i];
			}
			else {
				newTasks[i + 1] = tasks[i];
			}
		}
		newTasks[index] = task;
		delete[] tasks;
		tasks = newTasks;
		j++;
	}
}

void Graf::remove(int index)
{
	// usun zadanie z pozycji index przesuwajac wszystkie zadania o 1
	if (index >= 0 && index < j) {
		Task* newTasks = new Task[j - 1];
		for (int i = 0; i < j; i++) {
			if (i < index) {
				newTasks[i] = tasks[i];
			}
			else if (i > index) {
				newTasks[i - 1] = tasks[i];
			}
			else{
				continue;
			}
		}
		delete[] tasks;
		tasks = newTasks;
		j--;
	}
}


void NEH(Graf forward, Graf current, Graf backwrd, int starting_task, int ending_task){
	int j = forward.getJ();
	int m = forward.getM();
	for (int i = starting_task; i < ending_task; i++) {
		for (int k = 0; k < m; k++) {
			forward[i][k] = current[i][k] + std::max(forward[i - 1][k], forward[i][k - 1]);
			backwrd[j - i - 1][m - k - 1] = current[j - i - 1][m - k - 1] + std::max(backwrd[j - i][m - k - 1], backwrd[j - i - 1][m - k]);
		}
	}
}

void NEH(Graf forward, Graf current, Graf backwrd) {
	NEH(forward, current, backwrd, 0, forward.getJ());
}

void NEH(Graf forward, Graf current, Graf backwrd, int position) {
	NEH(forward, current, backwrd, position, position + 1);
}

void print(Graf graf) {
	int j = graf.getJ();
	int m = graf.getM();
	std::cout << "Graf: " << j << " x " << m << std::endl;
	for (int i = 0; i < j; i++) {
		for (int k = 0; k < m; k++) {
			std::cout << graf[i][k] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "------------------" << std::endl;
}

int main()
{
	std::ifstream file("data.txt");
	if (!file.is_open()) {
		std::cerr << "Failed to open file. Aborting ;((" << std::endl;
		return 1;
	}
	int j, m;
	std::string line;
	file >> line;
	std::string TargetLine = "data.120:";
	while (line != TargetLine)
	{
		file >> line;
	}

	file >> j >> m;

	Graf forward(0, m);
	Graf current(0, m);
	Graf backwrd(0, m);
	Graf p(j, m);
	int* task = new int[m];
	int* w = new int[j];
	for (int i = 0; i < j; i++) {
		w[i] = 0;
		for (int k = 0; k < m; k++) {
			file >> task[k];
			w[i] += task[k];
		}
		p[i].fill(task);
	}

	int* weights = new int[j];
	for (int i = 0; i < j; i++) {
		weights[i] = i;
	}
	std::sort(weights, weights + j,
		[&w](int a, int b) { return w[a] > w[b]; });
	

	file.close();

	auto start = std::chrono::high_resolution_clock::now();


	int cmax, best_pos;
	std::list<int> order;
	for (int width = 0; width < j; width++) {
		cmax = INT_MAX;
		Task current_task = p[weights[width]];
		// basic NEH
		NEH(forward, current, backwrd);
		//print(current);

		for (int position = 0; position <= width; position++) {

			forward.insert(position, Task(current_task));
			current.insert(position, Task(current_task));
			backwrd.insert(position, Task(current_task));

			
			NEH(forward, current, backwrd, position);

			//print(current);

			int row = INT_MIN;
			for (int k = 0; k < m; k++) {
				if (row < forward[position][k] + backwrd[position + 1][k]) {
					row = forward[position][k] + backwrd[position + 1][k];
				}
			}
			if (cmax > row) {
				cmax = row;
				best_pos = position;
			}

			forward.remove(position);
			current.remove(position);
			backwrd.remove(position);
		}
		forward.insert(best_pos, Task(current_task));
		current.insert(best_pos, Task(current_task));
		backwrd.insert(best_pos, Task(current_task));
		auto it = std::next(order.begin(), best_pos);
		order.insert(it, weights[width] + 1);
	}

	std::cout << "Cmax: " << cmax << std::endl;
	std::cout << "Order:";
	for (int i : order) {
		std::cout << i << " ";
	}

	auto end = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << std::endl << "Elapsed time: " << duration.count() << " milliseconds" << std::endl;
	
}

