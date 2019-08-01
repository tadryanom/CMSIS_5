#include "Softmax.h"
#include "Error.h"
#include "arm_nnfunctions.h"
#include "Test.h"

#include <cstdio>

int16_t findMaxIndex(q7_t *vec_in, int length)
{
  int16_t currentIndex=0;
  int16_t i=1;
  q7_t currentMax=vec_in[0];

  while(i<length)
  {
    if (vec_in[i] > currentMax)
    {
       currentMax = vec_in[i];
       currentIndex = i;
    }
    i++;
  }

  return(currentIndex+1);
}

int16_t differences(int16_t *pa,int16_t *pb, int length)
{
  int16_t d=0;
  int i=0;
  while(i < length)
  {
     if (*pa != *pb)
     {
       d++;
     }

     pa++;
     pb++;
     i++;
  }
  return(d);
}


    void Softmax::test_softmax_q7()
    {
       const q7_t *vec_in = input.ptr();
       q7_t *pTmp = temp.ptr();
       int16_t *pOut = output.ptr();
       int16_t maxIndex;

       for(int i=0; i <this->nbSamples;i++)
       {
          arm_softmax_q7(vec_in, this->vecDim, pTmp );
          maxIndex=findMaxIndex(pTmp,this->vecDim);
          *pOut++ = maxIndex;

          vec_in += this->vecDim;
          pTmp += this->vecDim;
       }

       int diff = differences(ref.ptr(),output.ptr(),this->nbSamples);
       //printf("diffs = %d\n",diff);
       // 6% of errors are accepted for 20 entry samples
       ASSERT_TRUE(100.0*diff/this->nbSamples <= 6);
       
    } 

  
    void Softmax::setUp(Testing::testID_t id,std::vector<Testing::param_t>& paramsArgs,Client::PatternMgr *mgr)
    {

       switch(id)
       {
          case Softmax::TEST_SOFTMAX_Q7_1:
               ref.reload(Softmax::REF1_S16_ID,mgr);
               dims.reload(Softmax::DIMS1_S16_ID,mgr);
               input.reload(Softmax::INPUT1_Q7_ID,mgr);

               const int16_t *pDims=dims.ptr();

               this->nbSamples = pDims[0];
               this->vecDim = pDims[1];
          break; 

       }

        output.create(ref.nbSamples(),Softmax::OUTPUT_S16_ID,mgr);
        // Used to compare bit exactness of the reference C version
        // and the optimized version.
        temp.create(this->vecDim*this->nbSamples,Softmax::TEMP_Q7_ID,mgr);
       

    }

    void Softmax::tearDown(Testing::testID_t id,Client::PatternMgr *mgr)
    {
        // Array are big so by default they are not dumped and only
        // used for debug.
        //output.dump(mgr);
        //temp.dump(mgr);
    }
