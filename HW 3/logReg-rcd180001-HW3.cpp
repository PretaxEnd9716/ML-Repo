#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <chrono>
using namespace std;

ifstream openFile();
vector<double> sigmoid(vector<double> x);
vector<vector<double>> dataMatrix(vector<double> predictor);
vector<double> matrixMult(vector<double> matrixOne, vector<vector<double>> matrixTwo);
vector<double> matrixMult(vector<vector<double>> matrixOne, vector<double> matrixTwo);
vector<double> error(vector<double> probabilityVector, vector<double> target);
vector<double> newWeights(double learningRate, vector<double> weight, vector<vector<double>> dm, vector<double> e);
vector<double> probability(vector<double> predicted);
vector<double> prediction(vector<double> probilities);
double accuracy(vector<double> predictions, vector<double> test);
double specificity(vector<double> predictions, vector<double> test);
double sensitivity(vector<double> predictions, vector<double> test);

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

    //Start Time
    chrono::time_point<chrono::system_clock> start, stop;
    start = chrono::system_clock::now();

    //Gradient Descent
    for(int i = 0; i < 500; i++)
    {
        vector<double> pv = sigmoid(matrixMult(weights, dm));
        vector<double> e = error(pv, survivedTrain);
        weights = newWeights(learning_rate, weights, dm, e);
    }

    //End Time
    stop = chrono::system_clock::now();
    chrono::duration<double> difference = (stop - start);

    //Testing
    vector<vector<double>> tm = dataMatrix(sexTest);
    vector<double> predicted = matrixMult(weights, tm);
    vector<double> probabilities = probability(predicted);
    vector<double> predictions = prediction(probabilities);

    //Calculate and print metrics
    double a = accuracy(predictions, survivedTest);
    double sens = sensitivity(predictions, survivedTest);
    double spec = specificity(predictions, survivedTest);

    cout << "\n--Metrics--\n";
    cout << "Accuracy: " << a << endl;
    cout << "Sensitivity: " << sens << endl;
    cout << "Specificity: " << spec << endl;
    cout << "Algorithm Run Time: " << difference.count() << " seconds" << endl;

    return 0;
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

//From probabilities obtain the predictions
vector<double> prediction(vector<double> probabilities)
{
    vector<double> pred(probabilities.size());

    //Based on the probabilties predict the value of survive
    for(int i = 0; i < pred.size(); i++)
    {
        if(probabilities.at(i) > .5)
        {
            pred.at(i) = 1;
        }
        else
        {
            pred.at(i) = 0;
        }
    }

    return pred;
}

//Calculate probabilities vector
vector<double> probability(vector<double> predicted)
{
    vector<double> p(predicted.size());

    //Calculate the probabilties for each instance
    for(int i = 0; i < p.size(); i++)
    {
        p.at(i) = exp(predicted.at(i)) / (1 + exp(predicted.at(i)));
    }

    return p;
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