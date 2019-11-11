#include "remo.h"
#include "reader.h"
#include "writer.h"
#include "error.h"
#include "function.h"

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


std::any bar(uint32_t* a) {
    return a;
}

int main(int, char**)
{

    uint32_t foo = 100000;
    std::any a = bar(&foo);
    
    foo = 20000;

    std::cout << *std::any_cast<uint32_t*>(a) << std::endl;


    try {
        remo::Controller ctrl;

        remo::bound_function bound_fritzli("fritzli", &fritzli);
        std::cout << "bound function " << bound_fritzli.to_string() << std::endl;

        remo::Packet packet;
        remo::BinaryWriter writer(packet);
        uint32_t a1 = 100;
        int32_t o1 = 200;
        double a2 = 300.1;
        double o2 = 400.5;
        writer.write_call("fritzli", a1, &o1, a2, &o2);

        std::cout << packet << std::endl;
        std::cout << "wire: " << packet.to_hex() << std::endl;        

        remo::BinaryReader reader(packet);
        reader.read_call();

        remo::TypedValue result = bound_fritzli.call(reader.get_args());

        remo::Packet reply;
        remo::BinaryWriter reply_writer(reply);

        remo::ArgList args = reader.get_args(); // TODO const? separate variable;
        reply_writer.write_result(result, args);

        std::cout << reply << std::endl;
        std::cout << "wire: " << reply.to_hex() << std::endl;        

        remo::BinaryReader reply_reader(reply);
        
        uint32_t a1_2{};
        int32_t o1_2{};
        double a2_2{};
        double o2_2{};
        double ret = reply_reader.read_result<double>(a1_2, &o1_2, a2_2, &o2_2);

        std::cout << "ret=" << ret << ", o1=" << o1_2 << ", o2=" << o2_2 << std::endl;

        /*
        BinaryWriter writer;
        writer.write_value((uint32_t)1234);
        writer.write_value((uint32_t)0xAABBCCDD);
        writer.write_value(0.0);
        writer.write_value(1.0);
        writer.write_value(-1.0);
        writer.write_value(1000.0);
        writer.write_value(0.1);
        writer.write_value("gugus");
        writer.write_value((char*)"foo");
        writer.write_value((const char*)"bar");
        */

        //uint8_t buffer [8] = {0x66};
        //writer.write_value(buffer);

        ret = ctrl.call<double>("fritzli", a1, &o1, a2, &o2);
        std::cout << "ret=" << ret << ", o1=" << o1_2 << ", o2=" << o2_2 << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}