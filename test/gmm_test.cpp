#include "libcluster.h"
#include "probutils.h"
#include "testdata.h"

//
// Namespaces
//


using namespace std;
using namespace Eigen;
using namespace libcluster;
using namespace probutils;


//
// Functions
//

void printwj(const vector<SuffStat>& SSgroup)
{

  cout << endl;

  for (unsigned int j = 0; j < SSgroup.size(); ++j)
  {
    // Get number of Observations
    RowVectorXd wj = RowVectorXd::Zero(SSgroup[j].getK());

    for (unsigned int k = 0; k < SSgroup[j].getK(); ++k)
      wj(k) = SSgroup[j].getN_k(k);

    cout << "w_group(" << j << ") = " << wj/wj.sum() << endl;
  }
}


void printGMM (const SuffStat& SS)
{
  // Get number of Dimensions and Observations
  bool isdiag = SS.getSS2(0).rows() == 1;
  double N = 0;
  for (unsigned int k = 0; k < SS.getK(); ++k)
    N += SS.getN_k(k);

  // Print Mixture properties
  for (unsigned int k = 0; k < SS.getK(); ++k)
  {
    double w = SS.getN_k(k)/N;
    RowVectorXd mean = SS.getSS1(k)/SS.getN_k(k);
    MatrixXd cov;

    if (isdiag == true)
      cov = MatrixXd(RowVectorXd((SS.getSS2(k)/SS.getN_k(k)
                              - mean.array().square().matrix())).asDiagonal());
    else
      cov = SS.getSS2(k)/SS.getN_k(k) - mean.transpose()*mean;

    cout << endl << "w_k(" << k << ") = " << w << endl
         << "mu_k(" << k << ") = " << mean << endl
         << "sigma_k(" << k << ") = " << endl << cov << endl;
  }

  cout << endl;
}


// Main
int main()
{

  // Populate test data from testdata.h
  MatrixXd X1, X2, Xcat;
  vector<MatrixXd> X;
  makedata(X1, X2, Xcat, X);

  // GMC
  SuffStat SS;
  vector<SuffStat> SSgroup(2, SuffStat());
  vector<MatrixXd> qZgroup;
  clock_t start = clock();
  learnGMC (X, qZgroup, SSgroup, SS, true, false, true);

//  vector<MatrixXd> Xt;
//  Xt.push_back(X1);
//  vector<SuffStat> SSgroup1;
//  learnGMC (Xt, qZgroup, SSgroup1, SS, true, false, true);

//  Xt[0] = X2;
//  vector<SuffStat> SSgroup2;
//  learnGMC (Xt, qZgroup, SSgroup2, SS, true, false, true);

  double stop = (double)((clock() - start))/CLOCKS_PER_SEC;
  cout << "GMC Elapsed time = " << stop << " sec." << endl;
  printwj(SSgroup);
  printGMM(SS);


  // SGMC
  SS = SuffStat();
  SSgroup.clear();
  start = clock();
  learnSGMC (X, qZgroup, SSgroup, SS, true, false, true);

  stop = (double)((clock() - start))/CLOCKS_PER_SEC;
  cout << "Symmetric GMC Elapsed time = " << stop << " sec." << endl;
  printwj(SSgroup);
  printGMM(SS);


  // VDP
  MatrixXd qZ;
  SS = SuffStat();
  start = clock();
  learnVDP(Xcat, qZ, SS, false, true);

  stop = (double)((clock() - start))/CLOCKS_PER_SEC;
  cout << "VDP Elapsed time = " << stop << " sec." << endl;
  printGMM(SS);


  // GMM
  start = clock();
  SS = SuffStat();
  learnGMM(Xcat, qZ, SS, false, true);

  stop = (double)((clock() - start))/CLOCKS_PER_SEC;
  cout << "Bayesian GMM Elapsed time = " << stop << " sec." << endl;
  printGMM(SS);

  return 0;
}
