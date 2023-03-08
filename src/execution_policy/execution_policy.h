#ifndef _EXECUTION_POLICY_H
#define _EXECUTION_POLICY_H
#include "../io_engine/io_engine.h"
namespace pitchstream
{
    class execution_policy {
        public:

        void set_io_engine(io_engine * _ioe) {ioe = _ioe;}
        void set_num_results(int _num_results) {num_results = _num_results;}

        virtual void run() = 0;

        protected:
        io_engine * ioe;
        int num_results;
    };
}
#endif
