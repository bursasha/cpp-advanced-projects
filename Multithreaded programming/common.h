// The classes in this header define the common interface between your implementation and 
// the testing environment. Exactly the same implementation is present in the progtest's 
// testing environment. You are not supposed to modify any declaration in this file, 
// any change is likely to break the compilation.
#ifndef COMMON_H_98273465234756283645234625356
#define COMMON_H_98273465234756283645234625356

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include <map>

//=============================================================================================================================================================
/**
 * A simple representation of a time interval + the payment offered for the rent.
 */
class CInterval
{
  public:
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
	CInterval ( int from, int to, int payment ) : m_From ( from ), m_To ( to ), m_Payment ( payment ) { }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    int m_From; int m_To; int m_Payment;
};
//=============================================================================================================================================================
/**
 * An encapsulation of the rental problem. The problem is defined by a set of intervals (m_Intervals) and the number of 
 * items we are able to rent (m_Count). We need to find the optimal way to rent the items we have such that the 
 * sum of payments is as high as possible. The highest results is then stored in m_MaxProfit. 
 */
class CProblem
{
  public:
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
	CProblem ( int count, std::initializer_list<CInterval> intervals ) : m_Count ( count ), m_Intervals { intervals } { }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    virtual ~CProblem ( void ) noexcept = default;
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    CProblem & add ( const CInterval & interval ) { m_Intervals . push_back ( interval ); return *this; }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    int m_Count;
    int m_MaxProfit = 0;
    std::vector<CInterval> m_Intervals;
};
using AProblem = std::shared_ptr<CProblem>;
//=============================================================================================================================================================
/**
 * A batch of rent problems, the solver is expected to iterate over the stored rent problems (m_Problems) and solve the 
 * profit for each.
 */
class CProblemPack
{
  public:
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    virtual ~CProblemPack ( void ) noexcept = default;
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    void add ( AProblem problem ) { m_Problems . push_back ( std::move ( problem ) ); }
    //---------------------------------------------------------------------------------------------------------------------------------------------------------
    std::vector<AProblem> m_Problems;
};
using AProblemPack = std::shared_ptr<CProblemPack>;
//=============================================================================================================================================================
/**
 * CCompany: deliver problem packs to solve & receive solved problem packs.
 *
 */
class CCompany 
{
  public:
    virtual ~CCompany ( void ) noexcept = default;
    virtual AProblemPack waitForPack ( void ) = 0;
    virtual void solvedPack ( AProblemPack pack ) = 0;
};
using ACompany = std::shared_ptr<CCompany>;
//=============================================================================================================================================================
#endif /* COMMON_H_98273465234756283645234625356 */
