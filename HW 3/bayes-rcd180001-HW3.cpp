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
vector<vector<double>> qualitativeSex(vector<double> target, vector<double> predictor, vector<double> surviveCount);
vector<vector<double>> qualitativeClass(vector<double> target, vector<double> predictor, vector<double> surviveCount);
vector<double> mean(vector<double> target, vector<double> predictor, vector<double> surviveCount);
vector<double> variance(vector<double> target, vector<double> predictor, vector<double> surviveCount, vector<double> mean);
double ageLikelihood(double age, double meanAge, double varAge);
vector<double> countTarget(vector<double> target);
vector<vector<double>> survProb(vector<double> survivedTrain, vector<double> sexTrain, vector<double> pclassTrain, vector<double> ageTrain, vector<double> sexTest, vector<double> pclassTest, vector<double> ageTest);
vector<double> predict(vector<vector<double>> probabilities);

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
        pclass.at(numObservations) = stof(class_in);
        survived.at(numObservations) = stof(survived_in);
        sex.at(numObservations) = stof(sex_in);
        age.at(numObservations) = stof(age_in);

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
    vector<double> sexTest(sex.begin() + 800, sex.end());

    vector<double> survivedTrain(survived.begin(), survived.begin() + 800);
    vector<double> survivedTest(survived.begin() + 800, survived.end());

    vector<double> ageTrain(age.begin(), age.begin() + 800);
    vector<double> ageTest(age.begin() + 800, age.end());

    vector<double> pclassTrain(pclass.begin(), pclass.begin() + 800);
    vector<double> pclassTest(pclass.begin() + 800, pclass.end());

    //Start Time
    chrono::time_point<chrono::system_clock> start, stop;
    start = chrono::system_clock::now();

    //Calculates all probabilities
    vector<vector<double>> probabilities = survProb(survivedTrain, sexTrain, pclassTrain, ageTrain, sexTest, pclassTest, ageTest); 

    //End Time
    stop = chrono::system_clock::now();
    chrono::duration<double> difference = (stop - start);

    //Gets predictions
    vector<double> predictions = predict(probabilities);

    //Calculate Metrics
    double a = accuracy(predictions, survivedTest);
    double sens = sensitivity(predictions, survivedTest);
    double spec = specificity(predictions, survivedTest);

    //Print Metrics
    cout << "\n--Naive Bayes Metrics--\n";
    
    cout << "Accuracy: " << a << endl;
    cout << "Sensitivity: " << sens << endl;
    cout << "Specificity: " << spec << endl;
    cout << "Algorithm Run Time: " << difference.count() << " seconds" << endl;
}

//Predicts with the probabilties
vector<double> predict(vector<vector<double>> probabilities)
{
    vector<double> predictions(probabilities.size());

    for(int i = 0; i < probabilities.size(); i++)
    {
        if(probabilities.at(i).at(0) > probabilities.at(i).at(1))
        {
            predictions.at(i) = 0;
        }
        else
        {
            predictions.at(i) = 1;
        }
    }

    return predictions;
}

//Calculates probability of survival given all the predictors
vector<vector<double>> survProb(vector<double> survivedTrain, vector<double> sexTrain, vector<double> pclassTrain, vector<double> ageTrain, vector<double> sexTest, vector<double> pclassTest, vector<double> ageTest)
{
    //Count the number of survived and precentage of each survival levels
    vector<double> sc = countTarget(survivedTrain);
    vector<double> survivalPerc = {sc.at(0)/survivedTrain.size(), sc.at(1)/survivedTrain.size()};

    //Calculate likelihood for sex and class
    vector<vector<double>> sexLikelihood = qualitativeSex(survivedTrain, sexTrain, sc);
    vector<vector<double>> classLikelihood = qualitativeClass(survivedTrain, pclassTrain, sc);

    //Calculate mean and variance for age
    vector<double> meanAge = mean(survivedTrain, ageTrain, sc);
    vector<double> varAge = variance(survivedTrain, ageTrain, sc, meanAge);

    //Go through each row and calculate each probability of survival
    vector<vector<double>> probabilities;
    for(int i = 0; i < ageTest.size(); i++)
    {
        //double survived = classLikelihood.at(1).at(pclassTest.at(i) - 1) survivalPerc.at(1) * ageLikelihood(ageTest.at(i), meanAge.at(1), varAge.at(1));
        //double dead = classLikelihood.at(0).at(pclassTest.at(i) - 1) * sexLikelihood.at(0).at(sexTest.at(i)) * survivalPerc.at(0) * ageLikelihood(ageTest.at(i), meanAge.at(0), varAge.at(0));
        double dead = classLikelihood.at(1).at(pclassTest.at(i) - 1) * sexLikelihood.at(1).at(sexTest.at(i)) *  survivalPerc.at(1) * ageLikelihood(ageTest.at(i), meanAge.at(1), varAge.at(1));
        double survived = classLikelihood.at(0).at(pclassTest.at(i) - 1) * sexLikelihood.at(0).at(sexTest.at(i)) * survivalPerc.at(0) * ageLikelihood(ageTest.at(i), meanAge.at(0), varAge.at(0));
        double denom = survived + dead;

        vector<double> instance {survived/denom, dead/denom};
        probabilities.push_back(instance);
    }

    cout << "\n--Summary--\n";
    cout << "A-Priori Probabilities: " << survivalPerc.at(0) << " " << survivalPerc.at(1) << endl;
    cout << "\n--Conditional Probabilities--";

    cout << "\nClass\n";
    cout << "0: " << classLikelihood.at(0).at(0) << " " << classLikelihood.at(0).at(1) << " " << classLikelihood.at(0).at(1) << endl;
    cout << "1: " << classLikelihood.at(1).at(0) << " " << classLikelihood.at(1).at(1) << " " << classLikelihood.at(1).at(1) << endl;

    cout << "\nSex\n";
    cout << "0: " << sexLikelihood.at(0).at(0) << " " << sexLikelihood.at(0).at(1) << endl;
    cout << "1: " << sexLikelihood.at(1).at(0) << " " << sexLikelihood.at(1).at(1) << endl;

    cout << "\nAge\n";
    cout << "Mean: " << meanAge.at(0) << " " << meanAge.at(1) << endl;
    cout << "Variance: " << varAge.at(0) << " " << varAge.at(1) << endl;

    return probabilities;
}

//Calculates the probabilty of survival of a given age
double ageLikelihood(double age, double meanAge, double varAge)
{
    //return exp(-(pow(age - meanAge,2)) / (2 * varAge));
    return 1 / sqrt(2 * M_PI * varAge) * exp(-(pow(age - meanAge,2)) / (2 * varAge));
}

//Calculate counts for survived
vector<double> countTarget(vector<double> target)
{
    vector<double> counts = {0, 0};

    //Count each value
    for(int i = 0; i < target.size(); i++)
    {
        counts.at(target.at(i))++;
    }

    return counts;
}

//Calculate likelihood for sex
vector<vector<double>> qualitativeSex(vector<double> target, vector<double> predictor, vector<double> surviveCount)
{
    vector<vector<double>> sexLikelihood = {{0,0},{0,0}};

    //Count each sex and whether they survived
    for(int i = 0; i < predictor.size(); i++)
    {
        double currentSurvived = target.at(i);
        double currentSex = predictor.at(i);

        sexLikelihood.at(currentSurvived).at(currentSex)++;
    }

    //Calcuate likelihood
    for(int survived = 0; survived < sexLikelihood.size(); survived++)
    {
        vector<double> currentSurvived = sexLikelihood.at(survived);

        for(int sex = 0; sex < currentSurvived.size(); sex++)
        {
            sexLikelihood.at(survived).at(sex) = currentSurvived.at(sex) / surviveCount.at(survived); 
        }
    }

    return sexLikelihood;
}

//Calculate likelihood for class
vector<vector<double>> qualitativeClass(vector<double> target, vector<double> predictor, vector<double> surviveCount)
{
    vector<vector<double>> classLikelihood = {{0,0,0},{0,0,0}};

    //Count each class and whether they survived
    for(int i = 0; i < predictor.size(); i++)
    {
        double currentSurvived = target.at(i);
        double currentClass = predictor.at(i) - 1;

        classLikelihood.at(currentSurvived).at(currentClass)++;
    }

    //Calculate Likelihood
    for(int survived = 0; survived < classLikelihood.size(); survived++)
    {
        vector<double> currentSurvived = classLikelihood.at(survived);

        for(int pclass = 0; pclass < currentSurvived.size(); pclass++)
        {
            classLikelihood.at(survived).at(pclass) = currentSurvived.at(pclass) / surviveCount.at(survived); 
        }
    }

    return classLikelihood;
}

//Calculate mean for quantitative data
vector<double> mean(vector<double> target, vector<double> predictor, vector<double> surviveCount)
{
    vector<double> meanPredictors = {0,0};

    //Calculate mean age for each survived level
    for(int i = 0; i < target.size(); i++)
        meanPredictors.at(target.at(i)) += predictor.at(i);
    
    meanPredictors.at(0) = meanPredictors.at(0) / surviveCount.at(0);
    meanPredictors.at(1) = meanPredictors.at(1) / surviveCount.at(1);

    return meanPredictors;
}

//Calculate variance for quantitative data
vector<double> variance(vector<double> target, vector<double> predictor, vector<double> surviveCount, vector<double> mean)
{
    vector<double> varPredictor = {0,0};

    //Calculate variance for each survive level
    for(int i = 0; i < predictor.size(); i++)
        varPredictor.at(target.at(i)) += ((predictor.at(i) - (mean.at(target.at(i))))*((predictor.at(i) - (mean.at(target.at(i))))));

    varPredictor.at(0) = varPredictor.at(0) / (surviveCount.at(0) - 1);
    varPredictor.at(1) = varPredictor.at(1) / (surviveCount.at(1) - 1);

    return varPredictor;
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