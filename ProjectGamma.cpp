// ProjectGamma.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <assert.h>
#include <random>
#include <time.h>
#include <vector>
#include <math.h>

using namespace std;

#define xmax 100;
#define ymax 100;

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
	void update(city newCity);
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

void salesman::update(city newCity)
{
	//pythagorean theorem for distance to next city
	//keeping running total of distance until policy is completed
	distance = distance + sqrt(pow((xpos + newCity.xpos), 2) + pow((ypos + newCity.ypos), 2));
	fitness = distance;
	//reset salesman's positions to chosen city
	xpos = newCity.xpos;
	ypos = newCity.ypos;
}

int main()
{
	srand(time(NULL));

	int numCities = 25;

	// creat vector of cities
	vector<city> citySet;

	// initialize cities and put them into vector
	for (int i = 0; i < numCities; i++)
	{
		city c;
		c.init();
		citySet.push_back(c);
	}

	// check if size of vector is the same as the number of cities needed
	assert(citySet.size() == numCities);

	// Create vector of policies
	vector<vector<int>> policyPopulation;
	for (int j = 0; j < 10; j++)
	{
		vector<int> policy;

		// Create one policy which is a vector of integers. Each integer represents index in city vector
		for (int k = 0; k < numCities; k++)
		{
			policy.push_back(rand() % numCities);
		}
		policyPopulation.push_back(policy);
	}

	// EA_init()
	// EA_replicate()
	// EA_eval()
	// EA_downselect

    return 0;
}

