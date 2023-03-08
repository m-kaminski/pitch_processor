#ifndef _EXECUTION_POLICY_ST_H
#define _EXECUTION_POLICY_ST_H
#include "execution_policy.h"
namespace pitchstream
{
    class execution_policy_single_threaded : public execution_policy {
        public:


        virtual void run();

    };
}
#endif
