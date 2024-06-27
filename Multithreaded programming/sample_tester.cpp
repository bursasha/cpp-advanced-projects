#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <thread>
#include <stdexcept>
#include "sample_tester.h"
using namespace std;

//=============================================================================================================================================================
std::initializer_list<AProblem> g_Problems =
{
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 394, 501, 80 }, { 911, 956, 34 }, { 768, 822, 56 }, { 477, 568, 37 }, { 513, 637, 91 }, { 635, 735, 15 }, { 606, 632, 25 }, { 137, 246, 17 }, { 400, 438, 12 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 512, 625, 62 }, { 296, 387, 53 }, { 493, 620, 30 }, { 771, 851, 77 }, { 400, 518, 29 }, { 352, 461, 92 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 525, 559, 20 }, { 663, 781, 36 }, { 64, 91, 46 }, { 63, 113, 97 }, { 902, 1016, 28 }, { 539, 603, 76 }, { 512, 607, 54 }, { 39, 109, 93 }, { 930, 1030, 29 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 354, 451, 18 }, { 440, 557, 83 }, { 330, 379, 89 }, { 350, 447, 95 }, { 588, 682, 86 }, { 439, 561, 41 }, { 814, 910, 91 }, { 482, 529, 95 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 881, 973, 44 }, { 619, 673, 79 }, { 307, 378, 24 }, { 187, 241, 56 }, { 416, 458, 90 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 495, 599, 98 }, { 935, 1031, 39 }, { 749, 812, 30 }, { 232, 318, 25 }, { 152, 253, 14 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 745, 777, 95 }, { 52, 131, 19 }, { 240, 348, 73 }, { 656, 782, 64 }, { 759, 793, 15 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 69, 115, 47 }, { 819, 904, 76 }, { 51, 92, 99 }, { 204, 322, 14 }, { 997, 1027, 87 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 923, 1010, 19 }, { 163, 229, 91 }, { 819, 881, 56 }, { 579, 651, 69 }, { 99, 179, 76 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 576, 693, 75 }, { 628, 656, 75 }, { 833, 955, 87 }, { 831, 958, 74 }, { 903, 1031, 67 }, { 497, 539, 83 }, { 888, 921, 65 }, { 248, 339, 24 }, { 700, 758, 34 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 633, 681, 65 }, { 510, 637, 29 }, { 546, 646, 13 }, { 471, 558, 94 }, { 450, 510, 85 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 344, 431, 83 }, { 233, 328, 49 }, { 481, 538, 71 }, { 182, 272, 6 }, { 413, 511, 68 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 184, 272, 63 }, { 730, 789, 74 }, { 202, 323, 69 }, { 653, 705, 54 }, { 87, 139, 87 }, { 686, 720, 12 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 593, 687, 30 }, { 775, 830, 34 }, { 189, 317, 2 }, { 827, 886, 20 }, { 436, 561, 92 }, { 764, 862, 13 }, { 685, 750, 77 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 861, 907, 79 }, { 548, 604, 90 }, { 909, 1025, 50 }, { 576, 618, 28 }, { 864, 940, 47 }, { 848, 925, 30 }, { 180, 276, 73 }, { 139, 227, 50 }, { 838, 939, 19 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 121, 160, 37 }, { 324, 446, 91 }, { 622, 734, 82 }, { 496, 556, 40 }, { 658, 746, 27 }, { 151, 183, 12 }, { 647, 710, 30 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 427, 550, 59 }, { 265, 359, 76 }, { 487, 528, 88 }, { 625, 704, 22 }, { 557, 626, 83 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 326, 427, 64 }, { 984, 1044, 89 }, { 136, 204, 2 }, { 783, 889, 30 }, { 114, 229, 72 }, { 49, 121, 98 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 473, 588, 11 }, { 99, 164, 31 }, { 657, 766, 14 }, { 51, 81, 46 }, { 780, 877, 45 }, { 119, 205, 58 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 361, 417, 89 }, { 476, 518, 61 }, { 525, 614, 60 }, { 233, 345, 8 }, { 98, 219, 18 }, { 481, 529, 83 }, { 290, 352, 88 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 659, 687, 27 }, { 778, 868, 83 }, { 308, 356, 21 }, { 612, 648, 68 }, { 782, 882, 21 }, { 401, 459, 44 }, { 230, 295, 54 }, { 154, 237, 3 }, { 380, 445, 31 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 649, 731, 92 }, { 685, 795, 70 }, { 311, 403, 2 }, { 532, 645, 62 }, { 642, 721, 41 }, { 362, 462, 80 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 32, 63, 69 }, { 187, 276, 70 }, { 567, 592, 2 }, { 305, 357, 66 }, { 857, 901, 35 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 653, 710, 88 }, { 186, 227, 51 }, { 828, 923, 90 }, { 191, 257, 71 }, { 868, 950, 74 }, { 932, 981, 92 }, { 551, 674, 50 }, { 552, 675, 80 }, { 814, 901, 66 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 324, 440, 59 }, { 637, 741, 77 }, { 794, 846, 61 }, { 470, 512, 79 }, { 865, 981, 67 }, { 412, 501, 60 }, { 645, 726, 16 }, { 579, 607, 70 }, { 518, 630, 52 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 510, 540, 81 }, { 384, 475, 46 }, { 143, 211, 26 }, { 406, 432, 72 }, { 573, 683, 59 }, { 446, 521, 99 }, { 58, 90, 64 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 219, 310, 92 }, { 737, 810, 44 }, { 850, 975, 94 }, { 899, 1004, 34 }, { 536, 584, 48 }, { 949, 1022, 88 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 458, 564, 77 }, { 904, 956, 76 }, { 963, 1022, 41 }, { 560, 643, 62 }, { 191, 266, 37 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 210, 298, 86 }, { 109, 173, 21 }, { 646, 733, 68 }, { 596, 627, 56 }, { 563, 613, 3 }, { 343, 368, 92 }, { 601, 706, 88 }, { 933, 976, 45 }, { 487, 595, 64 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 292, 409, 37 }, { 899, 1002, 48 }, { 272, 396, 13 }, { 865, 955, 72 }, { 924, 968, 29 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 626, 669, 14 }, { 227, 351, 3 }, { 160, 197, 47 }, { 648, 769, 11 }, { 614, 646, 40 }, { 496, 566, 30 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 566, 611, 5 }, { 431, 541, 75 }, { 356, 485, 5 }, { 523, 569, 66 }, { 699, 758, 89 }, { 646, 706, 6 }, { 766, 875, 70 }, { 681, 800, 32 }, { 752, 808, 81 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 53, 88, 17 }, { 244, 283, 59 }, { 58, 176, 94 }, { 56, 178, 47 }, { 256, 342, 18 }, { 584, 659, 81 }, { 926, 1006, 59 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 264, 355, 54 }, { 16, 138, 36 }, { 205, 284, 41 }, { 307, 403, 65 }, { 443, 496, 70 }, { 332, 379, 76 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 16, 129, 85 }, { 600, 658, 67 }, { 526, 640, 26 }, { 256, 288, 52 }, { 889, 978, 54 }, { 821, 946, 74 }, { 343, 405, 6 }, { 23, 48, 49 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 820, 898, 47 }, { 78, 123, 49 }, { 923, 952, 10 }, { 244, 343, 61 }, { 92, 217, 87 }, { 166, 240, 76 }, { 777, 802, 58 }, { 736, 839, 92 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 946, 981, 28 }, { 239, 349, 11 }, { 746, 800, 18 }, { 937, 1041, 11 }, { 981, 1094, 35 }, { 692, 764, 44 }, { 654, 713, 60 }, { 129, 162, 39 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 114, 231, 59 }, { 210, 305, 53 }, { 312, 436, 77 }, { 122, 151, 52 }, { 399, 446, 46 }, { 160, 217, 44 }, { 5, 98, 14 }, { 461, 494, 78 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 910, 1026, 8 }, { 47, 127, 19 }, { 927, 963, 40 }, { 596, 688, 70 }, { 539, 606, 82 }, { 577, 698, 23 }, { 789, 853, 39 }, { 97, 206, 39 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 849, 961, 72 }, { 635, 714, 63 }, { 502, 587, 67 }, { 29, 133, 60 }, { 139, 179, 21 }, { 777, 890, 74 }, { 184, 279, 32 }, { 105, 223, 12 }, { 479, 532, 21 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 947, 995, 51 }, { 778, 901, 15 }, { 294, 377, 65 }, { 873, 922, 68 }, { 629, 741, 81 }, { 773, 800, 59 }, { 617, 722, 77 }, { 284, 317, 88 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 359, 430, 39 }, { 647, 682, 33 }, { 869, 957, 12 }, { 805, 908, 41 }, { 366, 432, 28 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 901, 971, 88 }, { 674, 747, 48 }, { 292, 340, 26 }, { 576, 632, 14 }, { 749, 824, 49 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 133, 188, 20 }, { 2, 121, 30 }, { 808, 901, 69 }, { 175, 204, 96 }, { 775, 811, 86 }, { 207, 336, 54 }, { 667, 740, 83 }, { 892, 991, 41 }, { 193, 305, 17 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 347, 427, 46 }, { 640, 740, 47 }, { 541, 566, 28 }, { 191, 288, 45 }, { 236, 329, 23 }, { 349, 428, 43 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 94, 203, 88 }, { 986, 1065, 29 }, { 180, 242, 45 }, { 853, 949, 79 }, { 386, 425, 43 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 967, 1003, 87 }, { 159, 268, 32 }, { 395, 467, 54 }, { 745, 871, 95 }, { 88, 115, 7 }, { 897, 1016, 5 }, { 419, 463, 23 }, { 778, 868, 9 } } ),
  std::make_shared<CProblem> ( 2, std::initializer_list<CInterval> { { 459, 547, 83 }, { 563, 609, 80 }, { 672, 704, 96 }, { 475, 540, 37 }, { 930, 1051, 12 }, { 900, 1016, 20 }, { 921, 1043, 10 } } ),
  std::make_shared<CProblem> ( 1, std::initializer_list<CInterval> { { 508, 533, 20 }, { 287, 377, 27 }, { 748, 776, 72 }, { 350, 466, 29 }, { 552, 647, 95 }, { 624, 715, 44 }, { 8, 137, 37 }, { 925, 960, 27 }, { 801, 856, 20 } } ),
  std::make_shared<CProblem> ( 3, std::initializer_list<CInterval> { { 6, 104, 89 }, { 11, 129, 19 }, { 639, 679, 92 }, { 675, 791, 29 }, { 547, 588, 83 }, { 222, 258, 46 } } )
};

std::vector<int> g_Results { 278, 231, 413, 580, 293, 206, 251, 323, 235, 519, 257, 203, 284, 255, 417, 250, 247, 355, 205, 407, 352, 306, 242, 341, 541, 447, 278, 293, 432, 170, 131, 239, 293, 288, 413, 301, 235, 274, 320, 422, 504, 153, 225, 496, 232, 284, 312, 328, 300, 358};
//=============================================================================================================================================================
AProblemPack CCompanyTest::waitForPack ( void )
{
  size_t idx = m_GetPos;

  if ( idx == g_Problems . size () ) return AProblemPack ();
  if ( idx > g_Problems . size () ) throw std::invalid_argument ( "waitForPack: called too many times" );

  AProblemPack res = std::make_shared<CProblemPack> ();
  size_t cnt = std::min<size_t> ( rand () % 4 + 1, g_Problems . size () - m_GetPos );
  while ( cnt -- ) res -> add ( g_Problems . begin () [m_GetPos ++] );

  return res;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void CCompanyTest::solvedPack ( AProblemPack pack )
{
  size_t problem_index = 0;

  for ( auto problem : pack -> m_Problems )
  {
    size_t idx = m_DonePos ++;
  
    if ( idx >= g_Problems . size () ) throw std::invalid_argument ( "solvedPack: called too many times" );
    
    if ( g_Problems . begin () [idx] != problem ) throw std::invalid_argument ( "solvedPack: order not preserved" );

    if ( problem -> m_MaxProfit != g_Results[idx] ) { printf ("profit = %d, p_index = %zu\n", problem -> m_MaxProfit, problem_index); throw std::invalid_argument ( "solvedPack: invalid result" ); }
	printf ("\nmax profit = %d\n\n", problem->m_MaxProfit);
	problem_index++;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
bool CCompanyTest::allProcessed ( void ) const { printf ("\nGETPOS = %zu, DONEPOS = %zu, PROBLEMSSIZE = %zu\n\n", m_GetPos, m_DonePos, g_Problems.size ()); return m_GetPos == g_Problems . size () && m_DonePos == g_Problems . size (); }
//=============================================================================================================================================================
