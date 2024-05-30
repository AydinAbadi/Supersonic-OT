//
#include <iostream>
#include <math.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "gmp.h"
#include <gmpxx.h>
#include "Rand.h"
#include <chrono>
#include <random>
using namespace std::chrono;

//===============================
//KeyGen--- It generates (1) "number" random keys of size: bit_size and (2) "number" random values of size bit_size_.
// the random values generated in (2) will be used for secret sharing.
//It returns the random values of size bit_size. It also stores/amends random values of size bit_size_ in "random_val_"


bigint** GenKeys(int number, int* &random_val_, int bit_size, int bit_size_){


  mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
  uniform_int_distribution<int> distribution(0, 1);
  bigint  **random_val;
  random_val = (mpz_t**)malloc(number * sizeof(mpz_t));
  random_val_ = new int[number];
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, time(NULL)* 100); // Seed with current time
  for (int i = 0; i < number; ++i) {
    random_val[i] = (mpz_t*)malloc(2 * sizeof(mpz_t));
    mpz_urandomb(random_val[i][0], state, bit_size); // Generate a random 256-bit integer
    mpz_urandomb(random_val[i][1], state, bit_size);
    random_val_[i] = distribution(rng);
  }
  return random_val;}

//===============================
//SS--  XOR-based (2, 2) Secret Sharing function.
int* SS(int size, int* secret, int* random_val, int* &p_shares){
  //initiates the parameters
  int  *s_shares = new int[size];
  p_shares = new int[size];
  for (int i = 0; i < size ; i++){
    s_shares[i] = random_val[i] ^ secret[i];
    p_shares[i] = random_val[i];
  }
  return s_shares;
}

//============================


//===============================

bigint* oblivious_filter(int size, int* share, bigint** pair){

  bigint* result;
  result = (mpz_t*)malloc(size * sizeof(mpz_t));
  for (int i=0; i < size ; i++){
    //checks the value of the share
    //when the share equals 0, then it does not swap
    if (share[i]==0){
      mpz_init_set(result[i],pair[i][0]);
    }
    //when the share equals 1, then it swaps
    else if (share[i]==1){
      mpz_init_set(result[i],pair[i][1]);
    }
  }
  return result;
}
//===================
//it generates "size" pairs of random messages
bigint** gen_random_messages(int number, int bit_size){

  bigint  **random_val;
  random_val = (mpz_t**)malloc(number * sizeof(mpz_t));
  gmp_randstate_t state;
  gmp_randinit_default(state);
  gmp_randseed_ui(state, time(NULL)*1000); // Seed with current time
  for (int i = 0; i < number; ++i) {
    random_val[i] = (mpz_t*)malloc(2 * sizeof(mpz_t));
    mpz_init(random_val[i][0]);
    mpz_init(random_val[i][1]);
    mpz_urandomb(random_val[i][0], state, bit_size); // Generate a random 256-bit integer
    mpz_urandomb(random_val[i][1], state, bit_size);
  }
  return random_val;
}
//================


//======================
bigint** Enc_and_Swap(int size, int* share, bigint** pair, bigint **random_keys_ ){

  bigint** result, ** res;
  result = (mpz_t**)malloc(size * sizeof(mpz_t));
  res = (mpz_t**)malloc(size * sizeof(mpz_t));
  for (int i=0; i < size ; i++){
    result[i] = (mpz_t*)malloc(2 * sizeof(mpz_t));
    res[i] = (mpz_t*)malloc(2 * sizeof(mpz_t));
    mpz_xor(res[i][0], random_keys_[i][0], pair[i][0]);
    mpz_xor(res[i][1], random_keys_[i][1], pair[i][1]);
    //checks the value of the share
    //when the share equals 0, then it does not swap
    if (share[i]==0){
      mpz_init_set(result[i][0],res[i][0]);
      mpz_init_set(result[i][1],res[i][1]);
    }
    //when the share equals 1, then it swaps
    else if (share[i]==1){
      mpz_init_set(result[i][0],res[i][1]);
      mpz_init_set(result[i][1],res[i][0]);
    }
  }
  return result;
}
//=====================
bigint* extract_result(int size, bigint* p_response, int* secret_indices, bigint**keys){

  bigint* result;
  result = (mpz_t*)malloc(size * sizeof(mpz_t));
  for(int i=0; i<size; i++){
    mpz_init(result[i]);
    mpz_xor(result[i], p_response[i], keys[i][secret_indices[i]]);
  }
  return result;
}
//======================
//======================


int main() {

  int number_of_OT = 1000;
  int bit_size = 128; //security parameter
  int secret_bit_size = 1;
  int number_of_tests = 50;
  int* int_secrets = new int[number_of_OT];
  bigint  **random_keys_, **messages_;
  float counter = 0;
  bigint* p_shares;
  int* p_shares_;
  bigint* final_result;
  float phase1_, phase2_, phase3_, phase4_, phase5_;
  float phase1_total, phase2_total, phase3_total, phase4_total, phase5_total;

  int* random_val_x_;
  mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
  uniform_int_distribution<int> distribution(0, 1);
  //For the test, we have set the secret indices that the receiver R is interested in to random values.
  for(int i = 0; i < number_of_OT; i++){
    int_secrets[i] = distribution(rng);
  }
  //Generates "number_of_OT" pairs of random messages of size: "bit_size"
  messages_= gen_random_messages(number_of_OT, bit_size);

  //==========================================
  for(int j = 0;j < number_of_tests; j++){

  //cout<<"\n\n==================Runtime Breakdown========================"<<endl;
  double phase1;//time related variable
  double start_phase1 = clock();//time related variable
  //******* Phase 1: R-side key generation
  random_keys_ = GenKeys(number_of_OT, random_val_x_, bit_size, secret_bit_size);
  double end_phase1 = clock();//time related variable
  phase1 = end_phase1 - start_phase1;//time related variable
  phase1_ = phase1 / (double) CLOCKS_PER_SEC;//time related variable

  //==========================================

    double phase2;//time related variable
    double start_phase2 = clock();//time related variable
    //******* Phase 2: R-side Query Generation
    // splits each private index "int_secrets" into two shares: s_shares  and p_shares_.
    int* s_shares = SS(number_of_OT, int_secrets, random_val_x_, p_shares_);
    double end_phase2 = clock();//time related variable
    phase2 = end_phase2 - start_phase2;//time related variable
    phase2_ = phase2 / (double) CLOCKS_PER_SEC;//time related variable
    //cout<<"\n Phase 2 runtime: "<<phase2_<<endl;
    //==========================================

    //==========================================
    //******* Phase 3: S-side Response Generation:
    //step a & b) encrypts each messages using the secret keys
    // and permutes the encrypted elements
    double phase3;//time related variable
    double start_phase3 = clock();//time related variable
    bigint** s_response= Enc_and_Swap(number_of_OT, s_shares, messages_, random_keys_);
    double end_phase3 = clock();//time related variable
    phase3 = end_phase3 - start_phase3;//time related variable
    phase3_ = phase3 / (double) CLOCKS_PER_SEC;//time related variable
    //==========================================

    //==========================================
    double phase4;//time related variable
    double start_phase4 = clock();//time related variable
    //******* Phase 4: P-side Oblivious Filtering:
    bigint* p_permutation_ = oblivious_filter(number_of_OT, p_shares_, s_response);
    double end_phase4 = clock();//time related variable
    phase4 = end_phase4 - start_phase4;//time related variable
    phase4_ = phase4 / (double) CLOCKS_PER_SEC;//time related variable
    //==========================================

    //==========================================
    double phase5;//time related variable
    double start_phase5 = clock();//time related variable
    //******* Phase 5: R-side Message Extraction:
    //decrypts the result
    final_result = extract_result(number_of_OT, p_permutation_, int_secrets, random_keys_);
    double end_phase5 = clock();//time related variable
    phase5 = end_phase5 - start_phase5;//time related variable
    phase5_ = phase5 / (double) CLOCKS_PER_SEC;//time related variable
    counter += phase5_ + phase4_ + phase3_ + phase2_ + phase1_;
    phase1_total += phase1_;
    phase2_total += phase2_;
    phase3_total += phase3_;
    phase4_total += phase4_;
    phase5_total += phase5_;
  }
  cout<<"\n\n================== Correctness Test ========================"<<endl;

  // for(int i = 0; i<number_of_OT; i++){
  //   cout<<"\n Secret["<<i<<"]: "<<int_secrets[i]<<endl;
  //   cout<<"\n Original message["<< i<<",0]: "<<messages_[i][0]<<endl;
  //   cout<<"\n Original message["<< i<<",1]: "<<messages_[i][1]<<endl;
  //   cout<<"\n Result(the message R receives): "<<final_result[i]<<endl;
  //   cout<<"\n\n...................."<<endl;
  // }

    cout<<"\n\n================== Runtime Breakdown ========================"<<endl;
    cout<<"\n Phase 1 runtime: "<<phase1_total/number_of_tests<<endl;
    cout<<"\n Phase 2 runtime: "<<phase2_total/number_of_tests<<endl;
    cout<<"\n Phase 3 runtime: "<<phase3_total/number_of_tests<<endl;
    cout<<"\n Phase 4 runtime: "<<phase4_total/number_of_tests<<endl;
    cout<<"\n Phase 5 runtime: "<<phase5_total/number_of_tests<<endl;

  cout<<"\n\n================== Total Runtime========================"<<endl;
  //cout<<"\n Total cost for " <<number_of_OT<<" OT executions: "<<phase5_ + phase4_ + phase3_ + phase2_+phase1_<<endl;
  // cout<<"\n Original message[0]: "<<messages_[0][0]<<endl;
  // cout<<"\n Original messages[1]: "<<messages_[0][1]<<endl;

  cout<<"\n Total cost for " <<number_of_OT<<" OT executions: "<<counter/number_of_tests<<endl;
  cout<<"\n\n=======================================================\n"<<endl;
  return 0;
}

// g++ -c Rand.cpp
// g++  Rand.o  main.cpp  -o main -lgmpxx -lgmp
// ./main
