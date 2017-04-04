// ProjectGamma.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <assert.h>
#include <random>
#include <time.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <fstream>

using namespace std;

int xmax = 100;
int ymax = 100;

/*class grid {
public:
	int xmax;
	int ymax;

	void init();
};

void grid::init()
{
	xmax = 25;
	ymax = 25;
}*/

class city {
public:
	double xpos;
	double ypos;

	void init();
};

void city::init()
{
	//all cities will be initialized within the bounds of the grid
	xpos = rand() % xmax;
	ypos = rand() % ymax;
	assert(xpos >= 0 && xpos <=100);
	assert(ypos >= 0 && ypos <= 100);
}

class salesman {
public:
	double distance;
	double xpos;
	double ypos;
	double fitness;

	void init(vector<city> citySet);
	void update(vector<int> policy, vector<city> citySet);
};

void salesman::init(vector<city> citySet)
{
	//initialize salesman at first city with no distance covered
	distance = 0.0;
	xpos = citySet.at(0).xpos;
	ypos = citySet.at(0).ypos;
	//using same value as distance for fitness
	//aiming to minimize distance and fitness through multiple episodes
	fitness = distance;
}

void salesman::update(vector<int> policy, vector<city> citySet)
{
	//pythagorean theorem for distance to next city
	//keeping running total of distance until policy is completed
	distance = 0;
	for (int i = 0; i < policy.size(); i++)
	{
		city newCity;
		newCity = citySet.at(policy.at(i));
		distance = distance + sqrt(pow((xpos - newCity.xpos), 2) + pow((ypos - newCity.ypos), 2));
		//cout << distance << endl;
		assert(distance >= 0); // fulfills test that program is calculating distance between two cities (LR.7)
		fitness = distance;
		//reset salesman's positions to chosen city
		xpos = newCity.xpos;
		ypos = newCity.ypos;
	}
	xpos = citySet.at(0).xpos;
	ypos = citySet.at(0).ypos;
}

class policy {
public:
	vector<int> P;
	double fitness;

	void init(int numCities);
};

void policy::init(int numCities)
{
	// Create one policy which is a vector of integers. Each integer represents index in city vector
	for (int k = 0; k< numCities; k++)
	{
		P.push_back(k);
	}
	
	// randomly shuffle all cities but keep first one in same position
	random_shuffle(P.begin() + 1, P.end());

	/*for (int i = 0; i < P.size(); i++)
	{
		cout << P.at(i) << " ";
	}
	cout << endl;*/
	assert(P.size() == numCities); // check that policy has same size as total number of cities
	assert(*P.begin() == 0); // check that every policy starts at the first city
	// previous asserts fulfill test that agent has set of policies that represent its path (LR.3)
	// previous asserts also partially fulfill test that agent always starts at same city (LR.5)
	
	fitness = -1.0;
}

vector<policy> EA_init(int numCities, int popSize);

vector<policy> EA_replicate(vector<policy> P, int popSize, int numCities, salesman S, vector<city> citySet);

vector<policy> EA_eval(vector<policy> P, int popSize, int numCities, salesman S, vector <city> citySet);

vector<policy> EA_downselect(vector<policy> P, int popSize);


int main()
{
	srand(time(NULL));
	ofstream fout;

	int numCities = 10;
	int popSize = 150;

	// creat vector of cities
	vector<city> citySet;

	// initialize cities and put them into vector 
	// for only ten cities, hardcode where cities are in order to verify that EA works
	
	if (numCities == 10)
	{
		for (int i = 0; i < numCities; i++)
		{
			city c;
			c.xpos = 5*i;
			c.ypos = 5*i;
			citySet.push_back(c); //would make optimal distance 9*sqrt(50) ~ 63.6
		}
	}

	else
	{
		for (int i = 0; i < numCities; i++)
		{
			city c;
			c.init();
			citySet.push_back(c);
			assert(c.xpos >= 0 && c.xpos <= xmax); // ensures city is within bounds. 
			assert(c.ypos >= 0 && c.ypos <= ymax); //fulfills test that program can represent a city (LR.1)
		}
	}

	// check if size of vector is the same as the number of cities needed
	assert(citySet.size() == numCities);

	// create salesman
	salesman S;
	S.init(citySet);
	assert(S.xpos == citySet.at(0).xpos && S.ypos == citySet.at(0).ypos);

	assert(S.xpos == citySet[0].xpos && S.ypos == citySet[0].ypos); //checks to see if agent is in first city
	//fulfills test that program can represent an agent (LR.2)

	vector<policy> population;

	population = EA_init(numCities, popSize);
	assert(population.size() == popSize / 2); //fulfills test that program can initiate population of policies (MR.1)

	int numGenerations = 1000;

	fout.clear();
	fout.open("LearningCurve.txt");

	for (int gen = 0; gen < numGenerations; gen++)
	{
		population = EA_replicate(population, popSize, numCities, S, citySet);
		assert(population.size() == popSize); // fulfills test that population is back at max capacity with mutated copies (MR.5)

		for (int i = 0; i < popSize; i++)
		{
			assert(population.at(i).P.at(0) == 0); // fulfills test that agent always starts in same city (LR.5)

			for (int j = 0; j < numCities; j++)
			{
				for (int k = j+1; k < numCities; k++)
				{
					assert(population.at(i).P.at(j) != population.at(i).P.at(k)); // fullfills test that agent never visits same city twice (LR_6)
					// Sierra Gonzales' idea/help
				}
			}
		}

		population = EA_eval(population, popSize, numCities, S, citySet);
		assert(population.size() == popSize);

		population = EA_downselect(population, popSize);
		assert(population.size() == popSize / 2);

		for (int i = 0; i < popSize/2; i++)
		{
			assert(population.at(i).P.at(0) == 0); // fulfills test that agent always starts in same city (LR.5)
		}

		fout << "Generation:" << "\t" << gen + 1 << "\t" << "Fitnesses: " << "\t";
		for (int policy = 0; policy < popSize / 2; policy++)
		{
			fout << population.at(policy).fitness << "\t";
		}
		fout << endl;

	//reset fitness 
		for (int polIndex = 0; polIndex < popSize / 2; polIndex++)
		{
			population.at(polIndex).fitness = 0;
		}
	}

	system("pause");
    return 0;
}

vector<policy> EA_init(int numCities, int popSize)
{
	vector<policy> population;

	for (int i = 0; i < popSize/2; i++)
	{
		policy p;
		p.init(numCities);
		population.push_back(p);
	}
	assert(population.size() == popSize/2); // fulfills test that a population of policies is created (MR.1)

	return population;
}



vector<policy> EA_replicate(vector<policy> P, int popSize, int numCities, salesman S, vector<city> citySet)
{
	vector<policy> population;
	population = P;
	assert(population.size() == popSize / 2);

	while (population.size() != popSize)
	{
		int index;
		index = rand() % (popSize / 2);
		policy pol;
		pol = population.at(index);
		
		int index2;
		int index3;
		int index4;

		// mutation process
		// swap three cities in policy
		// chose to swap three instead of two cities for faster learning curve only if the number of cities is larger than ten
		// for fewer number of cities, more swaps leads to too much randomness
		index2 = rand() % (numCities-1) + 1; //gets number between one and the maximum number of cities
		index3 = rand() % (numCities-1) + 1; //ensures that the first city is never mutated, so salesman always starts at same city 
		index4 = rand() % (numCities - 1) + 1;
		
		while (index2 == index3)
		{
			index3 = rand() % (numCities - 1) + 1;
		}

		assert(index2 != 0 && index3 != 0); // fulfills test that agent always starts in same city (LR.5)
		
		int temp;
		temp = pol.P.at(index2);
		pol.P.at(index2) = pol.P.at(index3);
		pol.P.at(index3) = temp;

		if (numCities > 10)
		{
			while (index3 == index4 || index2 == index4)
			{
				index4 = rand() % (numCities - 1) + 1;
			}

			assert(index3 != 0 && index4 != 0); // fulfills test that agent always starts in same city (LR.5)

			temp = pol.P.at(index3);
			pol.P.at(index3) = pol.P.at(index4);
			pol.P.at(index4) = temp;
		}

		population.push_back(pol);
		assert(population.at(index).P != pol.P); //fulfills test that a policy is mutated by the program
	}
	assert(population.size() == popSize);

	

	return population;
}

vector<policy> EA_eval(vector<policy> P, int popSize, int numCities, salesman S, vector <city> citySet)
{
	vector<policy> population;
	population = P;
	assert(population.size() == popSize);

	// update the distance travelled 
	// update the fitness
	// assert that the fitness does not equal -1 (the default value)
	for (int i = 0; i < population.size(); i++)
	{
		//assert(population.at(i).fitness == -1); //ensures that initial fitness is at default value (-1)

		S.update(population.at(i).P, citySet);
		assert(S.distance >= numCities); //fulfills test that program can calculate total distance in policy (minimum dist is the number of cities (i.e. all cities are next to each other) (LR.8)

		population.at(i).fitness = S.fitness;
		assert(population.at(i).fitness != -1); //fulfills test that program can set fitness of policy based on distance (MR.2)
		// doing it in loop fulfills test that program can set fitness of each policy in population (MR.3)
		S.fitness = 0;
		//cout << population.at(i).fitness << " ";
	}

	return population;
}

vector<policy> EA_downselect(vector<policy> P, int popSize)
{
	vector<policy> population;
	assert(population.size() == 0);

	// binary tournament
	// halving size of vector
	// trying to minimize fitness
	// compare two random spots in population, send lower fitness to downselected population
	while (population.size() != popSize / 2)
	{
		int index = rand() % (popSize);
		int index2 = rand() % (popSize);
		
		// assure that index2 is not the same as index
		while (index == index2)
		{
			index2 = rand() % popSize;
		}

		//cout << "Index 1: " << index << " Index 2: " << index2 << endl;

		if (P.at(index).fitness < P.at(index2).fitness)
		{
			population.push_back(P.at(index));
		}
		else
		{
			population.push_back(P.at(index2));
		}
	}
	assert(population.size() == popSize/2); //fulfills test that population can be downselected - size of vector halved (MR.4)

	return population;
}