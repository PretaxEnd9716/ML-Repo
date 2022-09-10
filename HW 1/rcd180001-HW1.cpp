#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

int main(int argc, char** argv)
{
    ifstream filestream;
    string line, rm_in, medv_in;
    const int MAX_LEN = 1000;
    vector<double> rm(MAX_LEN);
    vector<double> medv(MAX_LEN);

    cout << "Opening Boston.csv" << endl;
    filestream.open("Data/Boston.csv");
    if(!filestream.is_open())
    {
        cout << "Cannot open Boston.csv" << endl;
        return 1;
    }

    cout << "Reading line 1" << endl;
    getline(filestream, line);

    cout << "Heading: " << line << endl;

    int numObservations = 0;
    while(filestream.good())
    {
        getline(filestream, rm_in, ',');
        getline(filestream, medv_in, '\n');

        rm.at(numObservations) = stof(rm_in);
        medv.at(numObservations) = stof(medv_in);

        numObservations++;
    }

    rm.resize(numObservations);
    medv.resize(numObservations);

    return 0;
}