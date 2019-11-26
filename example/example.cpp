#include "remo.h"
#include "local_endpoint.h"
#include "remote_endpoint.h"

#include <stdint.h>
#include <iostream>

uint32_t hansli(uint32_t a1, const char* a2, double a3, bool a4) {
    std::cout << "called hansli with " << a1 << " " << a2 << " " << a3 << " " << a4 << std::endl;
    return 333;
}

double fritzli(uint32_t a1, int32_t* o1, double a2, double* o2) {
    std::cout << "called fritzli with " << a1 << ' ' << *o1 << ' ' << a2 << ' ' << *o2 << std::endl;
    *o1 = -*o1;
    *o2 = -*o2;
    return *o2 * 2;
}


int main(int, char**)
{
    try {
        remo::LocalEndpoint endpoint;
        remo::RemoteEndpoint* remote = endpoint.connect(".");

        endpoint.bind("fritzli", &fritzli);

        endpoint.bind("lambda", [](int x) { std::cout << "geil " << x << std::endl; return x+1; });
        auto gugus = remote->call<int>("lambda", (int)5); // needed to print rest???
        std::cout << "lambda returned ret=" << gugus << std::endl;


        uint32_t a1 = 100;
        int32_t o1 = 200;
        double a2 = 300.1;
        double o2 = 400.5;

        double ret = remote->call<double>("fritzli", a1, &o1, a2, &o2);
        std::cout << "fritzli returned ret=" << ret << ", o1=" << o1 << ", o2=" << o2 << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}