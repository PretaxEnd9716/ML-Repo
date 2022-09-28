#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

ifstream openFile();
vector<double> sigmoid(vector<double> x);
vector<vector<double>> dataMatrix(vector<double> predictor);
vector<double> matrixMult(vector<double> matrixOne, vector<vector<double>> matrixTwo);
vector<double> matrixMult(vector<vector<double>> matrixOne, vector<double> matrixTwo);
vector<double> error(vector<double> probabilityVector, vector<double> target);
vector<double> newWeights(double learningRate, vector<double> weight, vector<vector<double>> dm, vector<double> e);

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



    //Setup Gradient Descent
    vector<double> weights {1, 1};
    vector<vector<double>> dm = dataMatrix(sexTrain);
    double learning_rate = .001;

    for(int i = 0; i < 500; i++)
    {
        vector<double> pv = sigmoid(matrixMult(weights, dm));
        vector<double> e = error(pv, survivedTrain);
        weights = newWeights(learning_rate, weights, dm, e);
        cout << i << endl;
    }

    cout << weights[0] << endl;
    return 0;
}

//Calculates new weights
vector<double> newWeights(double learningRate, vector<double> weight, vector<vector<double>> dm, vector<double> e)
{
    vector<vector<double>> stepOne(dm.size());
    vector<double> stepTwo;
    vector<double> output(2);

    //Multiply data matrix with learning rate
    for(int i = 0; i < dm.size(); i++)
    {
        vector<double> row = dm.at(i);
        stepOne.at(i) = {row.at(0) * learningRate, row.at(1) * learningRate};
    }
    
    //Matrix multiplication with error
    stepTwo = matrixMult(stepOne, e);

    //Add weight to matrix
    output.at(0) = stepTwo.at(0) + weight.at(0);
    output.at(1) = stepTwo.at(1) + weight.at(1);

    return output;
}

//Calculates error vector
vector<double> error(vector<double> probabilityVector, vector<double> target)
{
    vector<double> e(target.size());

    for(int i = 0; i < e.size(); i++)
    {
        e.at(i) = target.at(i) - probabilityVector.at(i);
    }

    return e;
}

//Matrix multiplication for weight vector
vector<double> matrixMult(vector<vector<double>> matrixOne, vector<double> matrixTwo)
{
    vector<double> m(2);
    vector<double> colOne(matrixOne.size());
    vector<double> colTwo(matrixOne.size());
    int r = 0;
    
    //Get each column
    for(vector<double> row : matrixOne)
    {
        colOne.at(r) = row.at(0);
        colTwo.at(r) = row.at(1);
        r++;
    }

    //Perform matrix multiplication
    for(int i = 0; i < matrixTwo.size(); i++)
    {
        double numOne = colOne.at(i) * matrixTwo.at(i);
        double numTwo = colTwo.at(i) * matrixTwo.at(i);

        m.at(0) += numOne;
        m.at(1) += numTwo;
    }

    return m;
}

//Matrix multiplication for probability vector
vector<double> matrixMult(vector<double> matrixOne, vector<vector<double>> matrixTwo)
{
    vector<double> m(matrixTwo.size());

    //Perform matrix multiplication
    for(int i = 0; i < matrixTwo.size(); i++)
    {
        vector<double> row = matrixTwo.at(i);

        double numOne = matrixOne.at(0) * row.at(0);
        double numTwo = matrixOne.at(1) * row.at(1);

        m.at(i) = numOne + numTwo;
    }

    return m;
}

//Returns the data matrix for logistic regression
vector<vector<double>> dataMatrix(vector<double> predictor)
{
    vector<vector<double>> DM(predictor.size());

    //Populate data matrix
    for(int i = 0; i < predictor.size(); i++)
    {
        //Create the row
        vector<double> entry {1, predictor.at(i)};
        DM.at(i) = entry;
    }

    return DM;
}

//Returns a vector of sigmoid values
vector<double> sigmoid(vector<double> x)
{
    vector<double> newSig(x.size());
    int index = 0;

    //Go through each element
    for(double element: x)
    {
        //Sigmoid equation
        double s = 1 / (1 + exp(-element));
        newSig.at(index) = s;

        index++;
    }

    return newSig;
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