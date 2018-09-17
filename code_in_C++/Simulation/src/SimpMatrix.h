#include <vector>
#include <utility>

namespace SimEco{

    class simpMatrix{

        std::vector<unsigned int> num;
        std::vector< std::pair<unsigned int, unsigned int> > pos;

    };

}