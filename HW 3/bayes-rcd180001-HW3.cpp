#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <chrono>
using namespace std;

double sensitivity(vector<double> predictions, vector<double> test);
double specificity(vector<double> predictions, vector<double> test);
double accuracy(vector<double> predictions, vector<double> test);
ifstream openFile();
vector<vector<double>> qualitative(vector<double> target, vector<double> predictor);
vector<vector<double>> quantitative(vector<double> target, vector<double> predictor);

int main(int argc, char** argv)
{
    const int MAX_LEN = 1500;
    vector<double> pclass(MAX_LEN);
    vector<double> survived(MAX_LEN);
    vector<double> sex(MAX_LEN);
    vector<double> age(MAX_LEN);
    ifstream filestream;
    string line, class_in, survived_in, sex_in, age_in;
    int numObservations = 0;

    filestream = openFile();
    
    //Reading Headings
    getline(filestream, line);
    cout << "Heading: " << line << endl;

    //Read Rows
    while(filestream.good())
    {
        //Get each column
        getline(filestream, line, ',');
        getline(filestream, class_in, ',');
        getline(filestream, survived_in, ',');
        getline(filestream, sex_in, ',');
        getline(filestream, age_in, '\n');

        //Store each column
        pclass.at(numObservations) = stoi(class_in);
        survived.at(numObservations) = stoi(survived_in);
        sex.at(numObservations) = stoi(sex_in);
        age.at(numObservations) = stoi(age_in);

        numObservations++;
    }

    //Resize Vectors
    pclass.resize(numObservations);
    survived.resize(numObservations);
    sex.resize(numObservations);
    age.resize(numObservations);

    //Closing File
    filestream.close();

    //Splits sex and survived for train and test vectors
    vector<double> sexTrain(sex.begin(), sex.begin() + 800);
    vector<double> sexTest(sex.begin() + 801, sex.end());

    vector<double> survivedTrain(survived.begin(), survived.begin() + 800);
    vector<double> survivedTest(survived.begin() + 801, survived.end());

    vector<double> ageTrain(age.begin(), age.begin() + 800);
    vector<double> ageTest(age.begin() + 801, age.end());

    vector<double> pclass(pclass.begin(), pclass.begin() + 800);
    vector<double> pclass(pclass.begin() + 801, pclass.end());
}

vector<double> targetCount 

vector<vector<double>> qualitative(vector<double> target, vector<double> predictor)
{
    
}

vector<vector<double>> quantitative(vector<double> target, vector<double> predictor)
{

}

//Compute Sensitivity
double sensitivity(vector<double> predictions, vector<double> test)
{
    int truePositive = 0;
    int falseNegative = 0;

    //Count the number of number of true positives and false negatives
    for(int i = 0; i < predictions.size(); i++)
    {
        if(predictions.at(i) == 1 && test.at(i) == 1)
            truePositive++;

        if(predictions.at(i) == 0 && test.at(i) == 1)
            falseNegative++;
    }

    return truePositive / (double)(truePositive + falseNegative);
}

//Compute Specificity
double specificity(vector<double> predictions, vector<double> test)
{
    int trueNegative = 0;
    int falsePositive = 0;

    //Count the number of number of true negatives and false positives
    for(int i = 0; i < predictions.size(); i++)
    {
        if(predictions.at(i) == 0 && test.at(i) == 0)
            trueNegative++;

        if(predictions.at(i) == 1 && test.at(i) == 0)
            falsePositive++;
    }

    return trueNegative / (double)(trueNegative + falsePositive);
}

//Compute Accuracy
double accuracy(vector<double> predictions, vector<double> test)
{
    int correctPredictions = 0;

    //Count the number of correct predictions
    for(int i = 0; i < predictions.size(); i++)
    {
        if(predictions.at(i) == test.at(i))
            correctPredictions++;
    }

    return correctPredictions / (double)predictions.size();
}

//Opens file or exits if the
ifstream openFile()
{
    ifstream filestream;

    //Open File
    filestream.open("Data/titanic_project.csv");

    if(!filestream.is_open())
    {
        cout << "Cannot open file" << endl;
        exit(0);
    }

    return filestream;
}