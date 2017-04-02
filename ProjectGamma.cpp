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
	for (int i = 0; i < policy.size(); i++)
	{
		city newCity;
		newCity = citySet.at(policy.at(i));
		distance = distance + sqrt(pow((xpos + newCity.xpos), 2) + pow((ypos + newCity.ypos), 2));
		assert(distance >= 0);
		fitness = distance;
		//reset salesman's positions to chosen city
		xpos = newCity.xpos;
		ypos = newCity.ypos;
	}
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

	for (int i = 0; i < P.size(); i++)
	{
		cout << P.at(i) << " ";
	}
	cout << endl;
	assert(P.size() == numCities); // check that policy has same size as total number of cities
	assert(*P.begin() == 0); // check that every policy starts at the first city
	// previous asserts fulfill test that agent has set of policies that represent its path
	
	fitness = -1.0;
}

vector<policy> EA_init(int numCities, int popSize);

vector<policy> EA_replicate(vector<policy> P, int popSize, int numCities, salesman S, vector<city> citySet);

vector<policy> EA_eval(vector<policy> P, int popSize, int numCities, salesman S, vector <city> citySet);

vector<policy> EA_downselect(vector<policy> P, int popSize);


int main()
{
	srand(time(NULL));

	int numCities = 25;
	int popSize = 10;

	// creat vector of cities
	vector<city> citySet;

	// initialize cities and put them into vector
	for (int i = 0; i < numCities; i++)
	{
		city c;
		c.init();
		citySet.push_back(c);
		assert(c.xpos >= 0 && c.xpos <= xmax); // ensures city is within bounds. 
		assert(c.ypos >= 0 && c.ypos <= ymax); //fulfills test that program can represent a city
	}

	// check if size of vector is the same as the number of cities needed
	assert(citySet.size() == numCities);

	// create salesman
	salesman S;
	S.init(citySet);

	assert(S.xpos == citySet[0].xpos && S.ypos == citySet[0].ypos); //checks to see if agent is in first city
	//fulfills test that program can represent an agent

	vector<policy> population;

	population = EA_init(numCities, popSize);

	population = EA_replicate(population, popSize, numCities, S, citySet);

	// EA_eval()
	population = EA_downselect(population, popSize);
	assert(population.size() == popSize / 2);

	for (int k = 0; k < popSize / 2; k++)
	{
		for (int l = 0; l < numCities; l++)
		{
			cout << population.at(k).P.at(l) << " ";
		}
		cout << endl;
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
	assert(population.size() == popSize/2);

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

		// mutation process
		// swap two cities in policy
		index2 = rand() % numCities;
		index3 = rand() % numCities;
		
		int temp;
		temp = pol.P.at(index2);
		pol.P.at(index2) = pol.P.at(index3);
		pol.P.at(index3) = temp;

		population.push_back(pol);
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
		S.update(population.at(i).P, citySet);
		population.at(i).fitness = S.fitness;
		assert(population.at(i).fitness != -1);
		S.fitness = 0;
		//cout << population.at(i).fitness << " ";
	}

	return population;
}

vector<policy> EA_downselect(vector<policy> P, int popSize)
{
	vector<policy> population;

	// binary tournament
	// halving size of vector
	// trying to minimize fitness
	// compare two random spots in population, send lower fitness to downselected population
	while (population.size() != popSize / 2)
	{
		int index = rand() % popSize;
		int index2 = rand() % popSize;

		// assure that index2 is not the same as index
		while (index = index2)
		{
			index2 = rand() % popSize;
		}

		if (P.at(index).fitness < P.at(index2).fitness)
		{
			population.push_back(P.at(index));
		}
		else
		{
			population.push_back(P.at(index2));
		}
	}
	assert(population.size() == popSize/2);

	return population;
}