#ifndef K_NEAREST_NEIGHBOUR_H
#define K_NEAREST_NEIGHBOUR_H

#include "ClassificationData.h"
#include <vector>
#include <algorithm>
#include <set>
#include <map>

#define K_NEAREST_NEIGHBOUR_TEST

using namespace std;

template <typename T>
class NearestNeighbourEntry
{
public:
	int test_pattern_index;
	int classification;
	T distance; // distance is, dependent on the data-type e.g. either float or int...
	bool operator<(const NearestNeighbourEntry<T>& val) const 
	{
		return distance < val.distance;
	}
};

template <typename T>
class CKNearestNeighbourClassifier
{
private:
	int m_K;	// K-in k-nearest-neighbours
	CClassificationData<T>* m_p_training_data;
	int num_training_data_set;
public:
	void SetK(int K){m_K=K;}
	void SetPointerTrainingData(CClassificationData<T>* p_training_data){m_p_training_data=p_training_data;}
	int ClassifyPattern(CPattern<T> pattern_to_classify);
};


template <typename T> int CKNearestNeighbourClassifier<T>::ClassifyPattern(CPattern<T> pattern_to_classify)
{
	int num_training_sets = m_p_training_data->m_num_data_sets;
	int num_dimensions = m_p_training_data->m_data_sets[0].m_pattern_data.size();

	cout << "number of training sets: " << num_training_sets << endl;
	cout << "number of dimensions: " << num_dimensions << endl;

	// create a vector of Nearest-Neighbour-Entries...
	vector<NearestNeighbourEntry<T>> nearest_neighbour_entries;

	NearestNeighbourEntry<T> temp_entry;
	
	for(int index_pattern=0;index_pattern<num_training_sets;index_pattern++)
	{
		T curr_distance(0);
		temp_entry.classification=m_p_training_data->m_data_sets[index_pattern].m_classification;
		temp_entry.test_pattern_index = index_pattern;
		// loop over all dimensions of the data....
		for(int index_dimension=0;index_dimension<num_dimensions;index_dimension++)
		{
			curr_distance+=(pattern_to_classify.m_pattern_data[index_dimension]-m_p_training_data->m_data_sets[index_pattern].m_pattern_data[index_dimension])*(pattern_to_classify.m_pattern_data[index_dimension]-m_p_training_data->m_data_sets[index_pattern].m_pattern_data[index_dimension]);
		}
		temp_entry.distance = curr_distance;
		nearest_neighbour_entries.push_back(temp_entry);
	}

	// sort the entries by their distance..
	sort(nearest_neighbour_entries.begin(),nearest_neighbour_entries.end());

	// create set (unique list) of classifications in the top-K-entries....
	set<int> set_of_classifications;

	for(int i=0;i<m_K;++i)
	{
		set_of_classifications.insert(nearest_neighbour_entries[i].classification);
	}

	// for test-purposes: print-out the classifications:
	set<int>::iterator it_set;
#ifdef K_NEAREST_NEIGHBOUR_TEST
	
	cout << "set of classifications under the K-nearest neighbours..." << endl;
	for(it_set=set_of_classifications.begin();it_set!=set_of_classifications.end();it_set++)
	{
		cout << *it_set << " ";
	}
	cout << endl;

#endif

// now create a histogram of the frequencies of these classifications....
	map<int,int> histogram_of_classifications;
	
	// initialize with zero...

	for(it_set=set_of_classifications.begin();it_set!=set_of_classifications.end();it_set++)
		histogram_of_classifications[*it_set]=0;

	// now count (make histogram essentially)
	for(int i=0;i<m_K;++i)
		histogram_of_classifications[nearest_neighbour_entries[i].classification]++;

	

	map<int,int>::iterator it_map;
// print-out the shit for test-purposes
#ifdef K_NEAREST_NEIGHBOUR_TEST
	for(it_map=histogram_of_classifications.begin();it_map!=histogram_of_classifications.end();it_map++)
		cout << "classification: " << it_map->first << "  frequency: " << it_map->second << endl;
	cout << endl;
#endif
	int arg_max=0;
	int maximum=0;
	for(it_map=histogram_of_classifications.begin();it_map!=histogram_of_classifications.end();it_map++)
	{
		if(it_map->second>maximum)
		{
			maximum=it_map->second;
			arg_max=it_map->first;
		}
	}
	return arg_max;
}

#endif
