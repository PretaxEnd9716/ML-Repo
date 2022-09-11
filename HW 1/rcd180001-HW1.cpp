#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

void printStats(vector<double> v);
double sum(vector<double> v);
double mean(vector<double> v);
double median(vector<double> v);
double range(vector<double> v);
double covar(vector<double> rm, vector<double> medv);
double cor(vector<double> rm, vector<double> medv);
double standardDev(vector<double> v);

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

    cout << "Length " << rm.size() << endl;

    cout << "Closing file Boston.csv" << endl;
    filestream.close();

    //Print Stats
    cout << "\nRM Stats\n";
    printStats(rm);
    

    cout << "\nMEDV Stats\n";
    printStats(medv);

    cout << "\nCovariance: " << covar(rm, medv) << endl;
    cout << "Correlation: " << cor(rm, medv) << endl;

    return 0;
}

void printStats(vector<double> v)
{
    //Sorts Vector
    sort(v.begin(), v.end());

    //Obtains all needed stats
    double s = sum(v);
    double m = mean(v);
    double med = median(v);
    double r = range(v);

    //Print stats
    cout 
    << "Sum: " << s << endl
    << "Mean: " << m << endl
    << "Median: " << med << endl
    << "Range: " << r << endl;
}

double sum(vector<double> v)
{
    int s = 0;

    for(int i = 0; i < v.size(); i++)
    {
        s += v.at(i);
    }

    return s;
}

double mean(vector<double> v)
{
    return sum(v) / v.size();
}

double median(vector<double> v)
{
    return v.at(v.size() / 2);
}

double range(vector<double> v)
{
    return v.at(v.size() - 1) - v.at(0) ;
}

double covar(vector<double> rm, vector<double> medv)
{
     double rm_mean = mean(rm);
     double medv_mean = mean(medv);

     double sum = 0;
     for(int i = 0; i < rm.size(); i++)
     {
        double r = rm.at(i) - rm_mean;
        double m = medv.at(i) - medv_mean;

        sum += (r * m);
     }

    return sum / (rm.size() - 1);
}

double cor(vector<double> rm, vector<double> medv)
{
    double c = covar(rm, medv);
    double rmStd = standardDev(rm);
    double medStd = standardDev(medv);
    double mult = rmStd * medStd;

    return (c/mult);
}

double standardDev(vector<double> v)
{
    double sum = 0;
    double m = mean(v);
    for(int i = 0; i < v.size(); i++)
    {
        double diff = (v.at(i) - m);
        sum += (diff * diff);
    }

    return sqrt(sum / v.size());
}