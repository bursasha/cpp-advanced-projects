// The classes in this header are used in the example test. You are free to 
// modify these classes, add more test cases, and add more test sets.
// These classes do not exist in the progtest's testing environment.
#ifndef SAMPLE_TESTER_H_2983745628345129345
#define SAMPLE_TESTER_H_2983745628345129345

#include "common.h"

//=============================================================================================================================================================
/**
 * An example CProductionLine implementation suitable for testing. This implementation does not cause 
 * any delays in getSheet/doneSheet calls.
 */
class CCompanyTest : public CCompany
{
  public:
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    /**
     * A basic implementation of the waitForPack method from the base class.
     *
     * @return a new problem pack, or an empty smart pointer
     */
    virtual AProblemPack waitForPack ( void ) override;
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    /**
     * A basic implementation of the solvedPack method from the base class.
     *
     * @param[in] pack        a pack to return and validate
     */
    virtual void solvedPack ( AProblemPack pack ) override;
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    /**
     * Check exit status:
     *  - were all packs read?
     *  - were all packs returned?
     * @return true if all packs were read and returned (does not check the results in the returned packs). 
     */
    bool allProcessed ( void ) const;

  private:
    size_t m_GetPos  { 0 };
    size_t m_DonePos { 0 };
};
using ACompanyTest = std::shared_ptr<CCompanyTest>;
//=============================================================================================================================================================
#endif /* SAMPLE_TESTER_H_2983745628345129345 */
