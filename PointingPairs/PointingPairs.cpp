#include <iostream>
#include <atomic>
#include <random>
#include <vector>
#include <algorithm>
#include <thread>
#include <iterator>
#include <tbb/task_group.h>
                  
struct Person
{
	const Person* pointsTo = nullptr;
};

int main(int argc, char* argv[])
{
	size_t numberOfPeople = std::stoi(argv[1]);
	size_t numberOfSamples = std::stoi(argv[2]);

	std::atomic<size_t> countPointingPairs(0);
	
	std::uniform_int_distribution<int> distribution(0, numberOfPeople - 1);
	std::mt19937 engine;
	auto rand = std::bind(distribution,engine);
	
	tbb::task_group tg;
    
  tg.run([&]{
	std::vector<Person> people(numberOfPeople);
	for ( size_t i=0; i<numberOfSamples; ++i )
	{
		for ( auto& person : people )
		{
			auto targetPerson = &people[rand()]; 
			while( targetPerson == &person )
			{     
				targetPerson = &people[rand()];  
			}
    	person.pointsTo = targetPerson;
		}
		
		for ( auto& person : people )
		{
    	if ( person.pointsTo->pointsTo == &person )
    	{
				++countPointingPairs;
				break;
			}
		}
	}});

	tg.wait();
	
	std::cout << (double)countPointingPairs / (double)numberOfSamples << std::endl;

	return 0;
}
