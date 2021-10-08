#include "util/histogram.hh"
#include <iostream>
#include <cassert>

int IQR(std::vector<int> &a)
{
    size_t p75_sz = a.size() * 3 / 4 ;
    size_t p25_sz = a.size() / 4 ;
    //Assume sorted
    //std::sort(a.begin(), a.end());
    int p75 = a[p75_sz];
    int p25 = a[p25_sz];
    return p75 - p25;
}

int calculateBinWidth(std::vector<int> &a)
{
    assert(a.size() > 0);
    float cbrt_size = (std::cbrt(a.size()));
    int h = 2 * IQR(a) / (int) cbrt_size;
    return h ? h : 1;
}

std::map<int, int> binning(std::vector<int> &vec) {
    // Define an map
    std::map<int, int> M;
    int amax = *(max_element(vec.begin(), vec.end()));
    int amin = *(min_element(vec.begin(), vec.end()));
    int binWidth = calculateBinWidth(vec);
    int num_bins = (amax - amin)/binWidth + 1;
    
    //init mmap
    int binEnd = 0;
    int binStart = 0;
    for (int i=0; i < num_bins; i++) {
        binEnd = binStart + binWidth;
        M.insert(std::pair<int,int>(binStart,0));
        binStart = binEnd;
    }

    //populate
    int containingBin = 0;
    for(auto elem : vec ) {
        containingBin = (elem / binWidth) * binWidth;
        M[containingBin]++;
    }

    return M;
}